#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <coin/utils/time.h>

#include <mdr/def/Operation.h>
#include <mdr/runtime/Context.h>
#include <mdr/runtime/Function.h>
#include <mdr/runtime/NativeFunction.h>
#include <mdr/runtime/Environment.h>

using namespace mdr;


/* ASM externs. */

extern "C" {
    extern mdr_u32 CallNativeFunctionU32 (NativeFunction::function_t, mdr_u64* register_stack, mdr_u32 stack_size, mdr_u64* stack);
    extern mdr_u64 CallNativeFunctionU64 (NativeFunction::function_t, mdr_u64* register_stack, mdr_u32 stack_size, mdr_u64* stack);
    extern mdr_f32 CallNativeFunctionF32 (NativeFunction::function_t, mdr_u64* register_stack, mdr_u32 stack_size, mdr_u64* stack);
    extern mdr_f64 CallNativeFunctionF64 (NativeFunction::function_t, mdr_u64* register_stack, mdr_u32 stack_size, mdr_u64* stack);
}


/* Operation definition macros. */

#define _OP_START(NAME)  case OP_ ## NAME :
#define _OP_END          goto LExecEnd;
#define _OP_RESTART      goto LExec;
#define _OP_RETURN       goto LReturn;


/* Class parameter parsing. */

#define _OPC_P(P0)               mdr_u16 P0; \
                                  extract_P (op, P0);
#define _OPC_PP(P0, P1)          mdr_u16 P0, P1; \
                                  extract_PP (op, P0, P1);
#define _OPC_PPP(P0, P1, P2)     mdr_u16 P0, P1, P2; \
                                  extract_PPP (op, P0, P1, P2);

#define _OPC_W(P0)               mdr_u32 P0; \
                                  extract_W (op, P0);
#define _OPC_WP(P0, P1)          mdr_u32 P0; mdr_u16 P1; \
                                  extract_WP (op, P0, P1);
#define _OPC_PW(P0, P1)          mdr_u16 P0; mdr_u32 P1; \
                                  extract_PW (op, P0, P1);

#define _OPC_L(P0)               mdr_u64 P0; \
                                  extract_L (op, P0);

/* */

#define _CALL_NATIVE(TYPE, FUNC) \
    NativeFunction* function = environment_->GetNativeFunction (function_id); \
    *((mdr_ ## TYPE *) (stack_.array () + stack_top)) = CallNativeFunction ## FUNC (function->function (), (mdr_u64*) native_call_stack_.data.array (), \
    native_call_stack_.size, (mdr_u64*) (stack_.array () + stack_top)); \
    native_call_stack_.size = 0; 

/* Class extraction functions. */

namespace {

inline void extract_P (mdrOperation op, mdr_u16& param0) {
    param0 = op >> 16 & 0xFFFF;
}

inline void extract_PP (mdrOperation op, mdr_u16& param0, mdr_u16& param1) {
    extract_P (op, param0);
    param1 = op >> 32 & 0xFFFF;
}

inline void extract_PPP (mdrOperation op, mdr_u16& param0, mdr_u16& param1, mdr_u16& param2) {
    extract_PP (op, param0, param1);
    param2 = op >> 48 & 0xFFFF;
}

inline void extract_W (mdrOperation op, mdr_u32& param0) {
    param0 = op >> 16 & 0xFFFFFFFF;
}

inline void extract_WP (mdrOperation op, mdr_u32& param0, mdr_u16& param1) {
    extract_W (op, param0);
    param1 = op >> 48 & 0xFFFF;
}

inline void extract_PW (mdrOperation op, mdr_u16& param0, mdr_u32& param1) {
    extract_P (op, param0);
    param1 = op >> 32 & 0xFFFFFFFF;
}

inline void extract_L (mdrOperation op, mdr_u64& param0) {
    param0 = op >> 16 & 0xFFFFFFFFFFFF;
}

inline void extract_M (mdrOperation op, mdr_u8& param0, mdr_u8& param1, mdr_u8& param2, mdr_u8& param3, mdr_u8& param4, mdr_u8& param5) {
    /* TODO(Marco): Probably needs some optimization here. If possible, that is. */
    param0 = op >> 16 & 0xFF;
    param1 = op >> 24 & 0xFF;
    param2 = op >> 32 & 0xFF;
    param3 = op >> 40 & 0xFF;
    param4 = op >> 48 & 0xFF;
    param5 = op >> 56;
}


/*
 * Fetches the address of a stack variable with the type T at offset 'id'.
 */
template <typename T>
inline T& fetch (mdr_u8* stack, mdr_u16 id) {
    return *(T*) (stack + id);
}

}


void Context::Execute (Function* function, mdr_u32 caller_stack_top) {
    /* General stuff. */
    mdrOperation* op_pointer = function->operations ();
    mdr_u32 stack_top        = caller_stack_top + function->stack_size ();

    // TODO: This is a "security check"! May be slow.
    if (stack_top > stack_.size ()) {
        printf ("Error: Requested stack size exceeds Context stack limit.");
        return;
    }

    /* Stacks. */
    mdr_u8* stack = ((mdr_u8*) stack_.array () + caller_stack_top);

    /* Current op. */
    mdr_u64 op;

    /* Executes the next instruction. */
  LExec:
    op = *op_pointer;

    switch (op & 0xFFFF) {

    /* BASIC. */

        _OP_START (END) {
            _OP_RETURN
        }

        _OP_START (kJMP) { _OPC_W (offset)
            op_pointer = (mdrOperation*) ((mdr_u8*) op_pointer + (mdr_s32) offset);
        _OP_RESTART }

        _OP_START (RET) { _OPC_P (src)
            return_value_address_ = stack + src;
            _OP_RETURN
        }

        _OP_START (RETl) { _OPC_P (src)
            return_value_address_ = stack + src;
            _OP_RETURN
        }


    /* MEM. */

        _OP_START (RETMOV) { _OPC_P (dest)
            fetch<mdr_u32> (stack, dest) = *((mdr_u32*) return_value_address ());
        _OP_END }

        _OP_START (RETMOVl) { _OPC_P (dest)
            fetch<mdr_u64> (stack, dest) = *((mdr_u64*) return_value_address ());
        _OP_END }

        _OP_START (MOV) { _OPC_PP (dest, src)
            fetch<mdr_u32> (stack, dest) = fetch<mdr_u32> (stack, src);
        _OP_END }

        _OP_START (MOVl) { _OPC_PP (dest, src)
            fetch<mdr_u64> (stack, dest) = fetch<mdr_u64> (stack, src);
        _OP_END }

        _OP_START (kMOV) { _OPC_PW (dest, value)
            fetch<mdr_u32> (stack, dest) = value;
        _OP_END }

        _OP_START (kMOVl) {

        _OP_END }

        _OP_START (kMOVM) {

        _OP_END }

        _OP_START (kMOVMb) {

        _OP_END }

        _OP_START (kMOVMs) {

        _OP_END }

        _OP_START (kMOVMl) {

        _OP_END }

        _OP_START (MOV_M2V) {

        _OP_END }

        _OP_START (MOV_M2Vb) {

        _OP_END }

        _OP_START (MOV_M2Vs) {

        _OP_END }

        _OP_START (MOV_M2Vl) {

        _OP_END }

        _OP_START (MOV_V2M) {

        _OP_END }

        _OP_START (MOV_V2Mb) {

        _OP_END }

        _OP_START (MOV_V2Ms) {

        _OP_END }

        _OP_START (MOV_V2Ml) {

        _OP_END }

        _OP_START (MOVO_M2V) {

        _OP_END }

        _OP_START (MOVO_M2Vb) {

        _OP_END }

        _OP_START (MOVO_M2Vs) {

        _OP_END }

        _OP_START (MOVO_M2Vl) {

        _OP_END }

        _OP_START (MOVO_V2M) {

        _OP_END }

        _OP_START (MOVO_V2Mb) {

        _OP_END }

        _OP_START (MOVO_V2Ms) {

        _OP_END }

        _OP_START (MOVO_V2Ml) {

        _OP_END }


    /* ARITH. */

        _OP_START (ADD) { _OPC_PP (dest, src)
            fetch<mdr_u32> (stack, dest) += fetch<mdr_u32> (stack, src);
        _OP_END }

        _OP_START (ADDl) { _OPC_PP (dest, src)
            fetch<mdr_u64> (stack, dest) += fetch<mdr_u64> (stack, src);
        _OP_END }

        _OP_START (fADD) { _OPC_PP (dest, src)
            fetch<mdr_f32> (stack, dest) += fetch<mdr_f32> (stack, src);
        _OP_END }

        _OP_START (fADDl) { _OPC_PP (dest, src)
            fetch<mdr_f64> (stack, dest) += fetch<mdr_f64> (stack, src);
        _OP_END }

        _OP_START (kADD) { _OPC_PW (dest, value)
            fetch<mdr_u32> (stack, dest) += value;
        _OP_END }

        _OP_START (kADDl) { _OPC_PW (dest, constant_id)
            fetch<mdr_u64> (stack, dest) += fetch<mdr_u64> (function->constant_table (), constant_id);
        _OP_END }

        _OP_START (fkADD) { _OPC_PW (dest, value)
            fetch<mdr_f32> (stack, dest) += *((mdr_f32*) &value);
        _OP_END }

        _OP_START (fkADDl) { _OPC_PW (dest, constant_id)
            fetch<mdr_f64> (stack, dest) += fetch<mdr_f64> (function->constant_table (), constant_id);
        _OP_END }

        _OP_START (SUB) { _OPC_PP (dest, src)
            fetch<mdr_u32> (stack, dest) -= fetch<mdr_u32> (stack, src);
        _OP_END }

        _OP_START (SUBl) { _OPC_PP (dest, src)
            fetch<mdr_u64> (stack, dest) -= fetch<mdr_u64> (stack, src);
        _OP_END }

        _OP_START (fSUB) { _OPC_PP (dest, src)
            fetch<mdr_f32> (stack, dest) -= fetch<mdr_f32> (stack, src);
        _OP_END }

        _OP_START (fSUBl) { _OPC_PP (dest, src)
            fetch<mdr_f64> (stack, dest) -= fetch<mdr_f64> (stack, src);
        _OP_END }

        _OP_START (kSUB) { _OPC_PW (dest, value)
            fetch<mdr_u32> (stack, dest) -= value;
        _OP_END }

        _OP_START (kSUBl) { _OPC_PW (dest, constant_id)
            fetch<mdr_u64> (stack, dest) -= fetch<mdr_u64> (function->constant_table (), constant_id);
        _OP_END }

        _OP_START (fkSUB) { _OPC_PW (dest, value)
            fetch<mdr_f32> (stack, dest) -= *((mdr_f32*) &value);
        _OP_END }

        _OP_START (fkSUBl) { _OPC_PW (dest, constant_id)
            fetch<mdr_f64> (stack, dest) -= fetch<mdr_f64> (function->constant_table (), constant_id);
        _OP_END }

        _OP_START (sMUL) { _OPC_PP (dest, src)
            fetch<mdr_s32> (stack, dest) *= fetch<mdr_s32> (stack, src);
        _OP_END }

        _OP_START (sMULl) { _OPC_PP (dest, src)
            fetch<mdr_s64> (stack, dest) *= fetch<mdr_s64> (stack, src);
        _OP_END }

        _OP_START (uMUL) { _OPC_PP (dest, src)
            fetch<mdr_u32> (stack, dest) *= fetch<mdr_u32> (stack, src);
        _OP_END }

        _OP_START (uMULl) { _OPC_PP (dest, src)
            fetch<mdr_u64> (stack, dest) *= fetch<mdr_u64> (stack, src);
        _OP_END }

        _OP_START (fMUL) { _OPC_PP (dest, src)
            fetch<mdr_f32> (stack, dest) *= fetch<mdr_f32> (stack, src);
        _OP_END }

        _OP_START (fMULl) { _OPC_PP (dest, src)
            fetch<mdr_f64> (stack, dest) *= fetch<mdr_f64> (stack, src);
        _OP_END }

        _OP_START (skMUL) { _OPC_PW (dest, value)
            fetch<mdr_s32> (stack, dest) *= value;
        _OP_END }

        _OP_START (skMULl) { _OPC_PW (dest, constant_id)
            fetch<mdr_s64> (stack, dest) *= fetch<mdr_s64> (function->constant_table (), constant_id);
        _OP_END }

        _OP_START (ukMUL) { _OPC_PW (dest, value)
            fetch<mdr_u32> (stack, dest) *= value;
        _OP_END }

        _OP_START (ukMULl) { _OPC_PW (dest, constant_id)
            fetch<mdr_u64> (stack, dest) *= fetch<mdr_u64> (function->constant_table (), constant_id);
        _OP_END }

        _OP_START (fkMUL) { _OPC_PW (dest, value)
            fetch<mdr_f32> (stack, dest) *= value;
        _OP_END }

        _OP_START (fkMULl) { _OPC_PW (dest, constant_id)
            fetch<mdr_f64> (stack, dest) *= fetch<mdr_f64> (function->constant_table (), constant_id);
        _OP_END }

        _OP_START (sDIV) { _OPC_PP (dest, src)
            fetch<mdr_s32> (stack, dest) /= fetch<mdr_s32> (stack, src);
        _OP_END }

        _OP_START (sDIVl) { _OPC_PP (dest, src)
            fetch<mdr_s64> (stack, dest) /= fetch<mdr_s64> (stack, src);
        _OP_END }

        _OP_START (uDIV) { _OPC_PP (dest, src)
            fetch<mdr_u32> (stack, dest) /= fetch<mdr_u32> (stack, src);
        _OP_END }

        _OP_START (uDIVl) { _OPC_PP (dest, src)
             fetch<mdr_u64> (stack, dest) /= fetch<mdr_u64> (stack, src);
        _OP_END }

        _OP_START (fDIV) { _OPC_PP (dest, src)
             fetch<mdr_f32> (stack, dest) /= fetch<mdr_f32> (stack, src);
        _OP_END }

        _OP_START (fDIVl) { _OPC_PP (dest, src)
            fetch<mdr_f64> (stack, dest) /= fetch<mdr_f64> (stack, src);
        _OP_END }

        _OP_START (skDIV) { _OPC_PW (dest, value)
            fetch<mdr_s32> (stack, dest) /= value;
        _OP_END }

        _OP_START (skDIVl) { _OPC_PW (dest, constant_id)
            fetch<mdr_s64> (stack, dest) /= fetch<mdr_s64> (function->constant_table (), constant_id);
        _OP_END }

        _OP_START (ukDIV) { _OPC_PW (dest, value)
            fetch<mdr_u32> (stack, dest) /= value;
        _OP_END }

        _OP_START (ukDIVl) { _OPC_PW (dest, constant_id)
            fetch<mdr_u64> (stack, dest) /= fetch<mdr_u64> (function->constant_table (), constant_id);
        _OP_END }

        _OP_START (fkDIV) { _OPC_PW (dest, value)
            fetch<mdr_f32> (stack, dest) /= value;
        _OP_END }

        _OP_START (fkDIVl) { _OPC_PW (dest, constant_id)
            fetch<mdr_f64> (stack, dest) /= fetch<mdr_f64> (function->constant_table (), constant_id);
        _OP_END }

        _OP_START (sREM) { _OPC_PP (dest, src)
            fetch<mdr_s32> (stack, dest) %= fetch<mdr_s32> (stack, src);
        _OP_END }

        _OP_START (sREMl) { _OPC_PP (dest, src)
            fetch<mdr_s64> (stack, dest) %= fetch<mdr_s64> (stack, src);
        _OP_END }

        _OP_START (uREM) { _OPC_PP (dest, src)
            fetch<mdr_u32> (stack, dest) %= fetch<mdr_u32> (stack, src);
        _OP_END }

        _OP_START (uREMl) { _OPC_PP (dest, src)
            fetch<mdr_u64> (stack, dest) %= fetch<mdr_u64> (stack, src);
        _OP_END }

        _OP_START (skREM) { _OPC_PW (dest, value)
            fetch<mdr_s32> (stack, dest) %= value;
        _OP_END }

        _OP_START (skREMl) { _OPC_PW (dest, constant_id)
            fetch<mdr_s64> (stack, dest) %= fetch<mdr_s64> (function->constant_table (), constant_id);
        _OP_END }

        _OP_START (ukREM) { _OPC_PW (dest, value)
            fetch<mdr_u32> (stack, dest) %= value;
        _OP_END }

        _OP_START (ukREMl) { _OPC_PW (dest, constant_id)
            fetch<mdr_u64> (stack, dest) %= fetch<mdr_u64> (function->constant_table (), constant_id);
        _OP_END }

        _OP_START (NEG) { _OPC_P (dest)
            mdr_s32& ptr = fetch<mdr_s32> (stack, dest);
            ptr = -ptr;
        _OP_END }

        _OP_START (NEGl) { _OPC_P (dest)
            mdr_s64& ptr = fetch<mdr_s64> (stack, dest);
            ptr = -ptr;
        _OP_END }


    /* BITWISE. */

        _OP_START (AND) { _OPC_PP (dest, src)
            fetch<mdr_u32> (stack, dest) &= fetch<mdr_u32> (stack, src);
        _OP_END }

        _OP_START (ANDl) { _OPC_PP (dest, src)
            fetch<mdr_u64> (stack, dest) &= fetch<mdr_u64> (stack, src);
        _OP_END }

        _OP_START (kAND) { _OPC_PW (dest, value)
            fetch<mdr_u32> (stack, dest) &= value;
        _OP_END }

        _OP_START (kANDl) { _OPC_PW (dest, constant_id)
            fetch<mdr_u64> (stack, dest) &= fetch<mdr_u64> (function->constant_table (), constant_id);
        _OP_END }

        _OP_START (OR) { _OPC_PP (dest, src)
            fetch<mdr_u32> (stack, dest) |= fetch<mdr_u32> (stack, src);
        _OP_END }

        _OP_START (ORl) { _OPC_PP (dest, src)
            fetch<mdr_u64> (stack, dest) |= fetch<mdr_u64> (stack, src);
        _OP_END }

        _OP_START (kOR) { _OPC_PW (dest, value)
            fetch<mdr_u32> (stack, dest) |= value;
        _OP_END }

        _OP_START (kORl) { _OPC_PW (dest, constant_id)
            fetch<mdr_u64> (stack, dest) |= fetch<mdr_u64> (function->constant_table (), constant_id);
        _OP_END }

        _OP_START (XOR) { _OPC_PP (dest, src)
            fetch<mdr_u32> (stack, dest) ^= fetch<mdr_u32> (stack, src);
        _OP_END }

        _OP_START (XORl) { _OPC_PP (dest, src)
            fetch<mdr_u64> (stack, dest) ^= fetch<mdr_u64> (stack, src);
        _OP_END }

        _OP_START (kXOR) { _OPC_PW (dest, value)
            fetch<mdr_u32> (stack, dest) ^= value;
        _OP_END }

        _OP_START (kXORl) { _OPC_PW (dest, constant_id)
            fetch<mdr_u64> (stack, dest) ^= fetch<mdr_u64> (function->constant_table (), constant_id);
        _OP_END }

        _OP_START (SHL) { _OPC_PP (dest, src)
            fetch<mdr_u32> (stack, dest) <<= fetch<mdr_u32> (stack, src);
        _OP_END }

        _OP_START (SHLl) { _OPC_PP (dest, src)
            fetch<mdr_u64> (stack, dest) <<= fetch<mdr_u32> (stack, src);
        _OP_END }

        _OP_START (kSHL) { _OPC_PW (dest, value)
            fetch<mdr_u32> (stack, dest) <<= value;
        _OP_END }

        _OP_START (kSHLl) { _OPC_PW (dest, value)
             fetch<mdr_u64> (stack, dest) <<= value;
        _OP_END }

        _OP_START (SHR) { _OPC_PP (dest, src)
            fetch<mdr_u32> (stack, dest) >>= fetch<mdr_u32> (stack, src);
        _OP_END }

        _OP_START (SHRl) { _OPC_PP (dest, src)
            fetch<mdr_u64> (stack, dest) >>= fetch<mdr_u32> (stack, src);
        _OP_END }

        _OP_START (kSHR) { _OPC_PW (dest, value)
            fetch<mdr_u32> (stack, dest) >>= value;
        _OP_END }

        _OP_START (kSHRl) { _OPC_PW (dest, value)
            fetch<mdr_u64> (stack, dest) >>= value;
        _OP_END }
        
        _OP_START (ASHL) { _OPC_PP (dest, src)

        _OP_END }

        _OP_START (ASHLl) { _OPC_PP (dest, src)

        _OP_END }

        _OP_START (kASHL) { _OPC_PW (dest, value)

        _OP_END }

        _OP_START (kASHLl) { _OPC_PW (dest, value)

        _OP_END }

        _OP_START (ASHR) { _OPC_PP (dest, src)

        _OP_END }

        _OP_START (ASHRl) { _OPC_PP (dest, src)

        _OP_END }

        _OP_START (kASHR) { _OPC_PW (dest, value)

        _OP_END }

        _OP_START (kASHRl) { _OPC_PW (dest, value)

        _OP_END }

        _OP_START (NOT) { _OPC_P (dest)
            mdr_u32& ptr = fetch<mdr_u32> (stack, dest);
            ptr = ~ptr;
        _OP_END }

        _OP_START (NOTl) { _OPC_P (dest)
            mdr_u64& ptr = fetch<mdr_u64> (stack, dest);
            ptr = ~ptr;
        _OP_END }


    /* LOGICAL. */

        _OP_START (LAND) { _OPC_PP (dest, src)
            mdr_u32& ptr = fetch<mdr_u32> (stack, dest);
            ptr = ptr && fetch<mdr_u32> (stack, src);
        _OP_END }

        _OP_START (kLAND) { _OPC_PP (dest, value)
            mdr_u32& ptr = fetch<mdr_u32> (stack, dest);
            ptr = ptr && value;
        _OP_END }

        _OP_START (LOR) { _OPC_PP (dest, src)
            mdr_u32& ptr = fetch<mdr_u32> (stack, dest);
            ptr = ptr || fetch<mdr_u32> (stack, src);
        _OP_END }

        _OP_START (kLOR) { _OPC_PW (dest, value)
            mdr_u32& ptr = fetch<mdr_u32> (stack, dest);
            ptr = ptr || value;
        _OP_END }

        _OP_START (LXOR) { _OPC_PP (dest, src)
            mdr_u32& ptr = fetch<mdr_u32> (stack, dest);
            ptr = (ptr & 0x1) != (fetch<mdr_u32> (stack, src) & 0x1); /* Eventually the values need to be normalized. */
        _OP_END }

        _OP_START (kLXOR) { _OPC_PP (dest, value)
            mdr_u32& ptr = fetch<mdr_u32> (stack, dest);
            ptr = (ptr & 0x1) != value;
        _OP_END }

        _OP_START (LNOT) { _OPC_P (dest)
            mdr_u32& ptr = fetch<mdr_u32> (stack, dest);
            ptr = !ptr;
        _OP_END }


    /* CALL. */

        _OP_START (CALL) { _OPC_W (function_id)
            Execute (environment_->GetFunction (function_id), stack_top);
        _OP_END }

        _OP_START (CALL_NATIVE_U32) { _OPC_W (function_id)
            _CALL_NATIVE (u32, U32)
        _OP_END }

        _OP_START (CALL_NATIVE_U64) { _OPC_W (function_id)
            _CALL_NATIVE (u64, U64)
        _OP_END }

        _OP_START (CALL_NATIVE_F32) { _OPC_W (function_id)
            _CALL_NATIVE (f32, F32)
        _OP_END }

        _OP_START (CALL_NATIVE_F64) { _OPC_W (function_id)
            _CALL_NATIVE (f64, F64)
        _OP_END }

        _OP_START (PUSH) { _OPC_PP (src, offset)
            fetch<mdr_u32> (stack_.array (), stack_top + offset) = fetch<mdr_u32> (stack, src);
        _OP_END }

        _OP_START (PUSHl) { _OPC_PP (src, offset)
            fetch<mdr_u64> (stack_.array (), stack_top + offset) = fetch<mdr_u64> (stack, src);
        _OP_END }

        _OP_START (NPUSH) { _OPC_PP (src, offset)
            if (offset > 31) {
                /* Push to stack. */
                fetch<mdr_u32> (stack_.array (), stack_top + offset) = fetch<mdr_u32> (stack, src);
                native_call_stack_.size += 1; /* Stack is 8 byte aligned. */
            }else {
                /* Push to "register". */
                *((mdr_u32*) (native_call_stack_.data.array () + offset)) = fetch<mdr_u32> (stack, src);
            }
        _OP_END }

        _OP_START (NPUSHl) { _OPC_PP (src, offset)
            if (offset > 31) {
                /* Push to stack. */
                fetch<mdr_u64> (stack_.array (), stack_top + offset) = fetch<mdr_u64> (stack, src);
                native_call_stack_.size += 1; /* Stack is 8 byte aligned. */
            }else {
                /* Push to "register". */
                *(native_call_stack_.data.array () + offset) = fetch<mdr_u32> (stack, src);
            }
        _OP_END }

        default:
            printf ("Error: An unexpected operation terminated the code: '%llX'!\n", op);
            _OP_RETURN
    }


    /* Increments the instruction pointer after an instruction. */
  LExecEnd:
    ++op_pointer;
    goto LExec;

  LReturn:
    return;
}