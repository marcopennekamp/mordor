#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <coin/utils/time.h>

#include <mordor/runtime/core.h>
#include <mordor/runtime/Operation.h>

#include <internal/runtime/Context.h>
#include <internal/runtime/Function.h>
#include <internal/runtime/Program.h>
#include <internal/runtime/Library.h>

using namespace mordor;



/* ASM externs. */

extern "C" extern mdr_u32 CallNativeFunctionU32 (Library::func, mdr_u64* register_stack, mdr_u32 stack_size, mdr_u64* stack);
extern "C" extern mdr_u64 CallNativeFunctionU64 (Library::func, mdr_u64* register_stack, mdr_u32 stack_size, mdr_u64* stack);
extern "C" extern mdr_f32 CallNativeFunctionF32 (Library::func, mdr_u64* register_stack, mdr_u32 stack_size, mdr_u64* stack);
extern "C" extern mdr_f64 CallNativeFunctionF64 (Library::func, mdr_u64* register_stack, mdr_u32 stack_size, mdr_u64* stack);




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


mdr_u64 __stdcall test (mdr_u64 a, mdr_u64 b, mdr_u64 c, mdr_u64 d) {
    return 42;
}

mdr_s32 __stdcall test2 (mdr_s32 a, mdr_s32 b, mdr_s32 c, mdr_s32 d) {
    printf ("beep\n");
    fflush (stdout);
    return a + b + c + d;
}

mdr_f32 __stdcall test3 (mdr_f32 a, mdr_f32 b, mdr_f32 c, mdr_f32 d) {
    printf ("%f, %f, %f, %f\n", a, b, c, d);
    return a + b + c + d;
}

mdr_u64 __stdcall test4 () {
    return 42;
}

mdr_u64 __stdcall test5 (mdr_u64 a, mdr_u64 b, mdr_u64 c, mdr_u64 d, mdr_u64 e, mdr_u64 f) {
    return a + b + c + d + e + f;
}


extern "C" MDR_DECL void mdrExecute (mdrContext* ctx, mdrFunction* func, mdr_u32 caller_stack_top) {
    /* General stuff. */
    Function* function       = (Function*) func;
    Context* context         = (Context*) ctx;
    mdrOperation* op_pointer = function->operations;
    mdr_u32 stack_top        = caller_stack_top + function->stack_size;

    // TODO: This is a security check! May be slow.
    if (stack_top > context->stack ().size ()) {
        printf ("Error: Requested stack size exceeds Context stack limit.");
        return;
    }

    /* Stacks. */
    mdr_u8* stack = ((mdr_u8*) context->stack ().array () + caller_stack_top);

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
            // printf ("JMP from %p, base %p, by %i (%llX)\n", op_pointer, function->operations, (mordor_s32) offset, op);
            op_pointer = (mdrOperation*) ((mdr_u8*) op_pointer + (mdr_s32) offset);
        _OP_RESTART }

        _OP_START (RET) { _OPC_P (src)
            context->return_value_address () = stack + src;
            _OP_RETURN
        }

        _OP_START (RETl) { _OPC_P (src)
            context->return_value_address () = stack + src;
            _OP_RETURN
        }


    /* MEM. */

        _OP_START (RETMOV) { _OPC_P (dest)
            fetch<mdr_u32> (stack, dest) = *((mdr_u32*) context->return_value_address ());
        _OP_END }

        _OP_START (RETMOVl) { _OPC_P (dest)
            fetch<mdr_u64> (stack, dest) = *((mdr_u64*) context->return_value_address ());
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

        _OP_END }

        _OP_START (fADDl) { _OPC_PP (dest, src)

        _OP_END }

        _OP_START (kADD) { _OPC_PW (dest, value)
            fetch<mdr_u32> (stack, dest) += value;
        _OP_END }

        _OP_START (kADDl) { _OPC_PW (dest, constant_id)

        _OP_END }

        _OP_START (fkADD) { _OPC_PW (dest, value)

        _OP_END }

        _OP_START (fkADDl) { _OPC_PW (dest, constant_id)

        _OP_END }

        _OP_START (SUB) { _OPC_PP (dest, src)
            fetch<mdr_u32> (stack, dest) -= fetch<mdr_u32> (stack, src);
        _OP_END }

        _OP_START (SUBl) { _OPC_PP (dest, src)
            fetch<mdr_u64> (stack, dest) -= fetch<mdr_u64> (stack, src);
        _OP_END }

        _OP_START (fSUB) { _OPC_PP (dest, src)

        _OP_END }

        _OP_START (fSUBl) { _OPC_PP (dest, src)

        _OP_END }

        _OP_START (kSUB) { _OPC_PW (dest, value)
            fetch<mdr_s32> (stack, dest) = value;
        _OP_END }

        _OP_START (kSUBl) { _OPC_PW (dest, constant_id)

        _OP_END }

        _OP_START (fkSUB) { _OPC_PW (dest, value)

        _OP_END }

        _OP_START (fkSUBl) { _OPC_PW (dest, constant_id)

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

        _OP_START (fMUL) {

        _OP_END }

        _OP_START (fMULl) {

        _OP_END }

        _OP_START (skMUL) {

        _OP_END }

        _OP_START (skMULl) {

        _OP_END }

        _OP_START (ukMUL) {

        _OP_END }

        _OP_START (ukMULl) {

        _OP_END }

        _OP_START (fkMUL) {

        _OP_END }

        _OP_START (fkMULl) {

        _OP_END }

        _OP_START (sDIV) {

        _OP_END }

        _OP_START (sDIVl) {

        _OP_END }

        _OP_START (uDIV) {

        _OP_END }

        _OP_START (uDIVl) {

        _OP_END }

        _OP_START (fDIV) {

        _OP_END }

        _OP_START (fDIVl) {

        _OP_END }

        _OP_START (skDIV) {

        _OP_END }

        _OP_START (skDIVl) {

        _OP_END }

        _OP_START (ukDIV) {

        _OP_END }

        _OP_START (ukDIVl) {

        _OP_END }

        _OP_START (fkDIV) {

        _OP_END }

        _OP_START (fkDIVl) {

        _OP_END }

        _OP_START (sREM) {

        _OP_END }

        _OP_START (sREMl) {

        _OP_END }

        _OP_START (uREM) {

        _OP_END }

        _OP_START (uREMl) {

        _OP_END }

        _OP_START (skREM) {

        _OP_END }

        _OP_START (skREMl) {

        _OP_END }

        _OP_START (ukREM) {

        _OP_END }

        _OP_START (ukREMl) {

        _OP_END }

        _OP_START (NEG) {

        _OP_END }

        _OP_START (NEGl) {

        _OP_END }


    /* BITWISE. */

        _OP_START (AND) {

        _OP_END }

        _OP_START (ANDl) {

        _OP_END }

        _OP_START (kAND) {

        _OP_END }

        _OP_START (kANDl) {

        _OP_END }

        _OP_START (OR) {

        _OP_END }

        _OP_START (ORl) {

        _OP_END }

        _OP_START (kOR) {

        _OP_END }

        _OP_START (kORl) {

        _OP_END }

        _OP_START (XOR) {

        _OP_END }

        _OP_START (XORl) {

        _OP_END }

        _OP_START (kXOR) {

        _OP_END }

        _OP_START (kXORl) {

        _OP_END }

        _OP_START (SHL) {

        _OP_END }

        _OP_START (SHLl) {

        _OP_END }

        _OP_START (kSHL) {

        _OP_END }

        _OP_START (kSHLl) {

        _OP_END }

        _OP_START (SHR) {

        _OP_END }

        _OP_START (SHRl) {

        _OP_END }

        _OP_START (kSHR) {

        _OP_END }

        _OP_START (kSHRl) {

        _OP_END }

        _OP_START (ASHR) {

        _OP_END }

        _OP_START (ASHRl) {

        _OP_END }

        _OP_START (kASHR) {

        _OP_END }

        _OP_START (kASHRl) {

        _OP_END }

        _OP_START (NOT) {

        _OP_END }

        _OP_START (NOTl) {

        _OP_END }


    /* CALL. */

        _OP_START (CALL) { _OPC_W (function_id)
            mdrExecute (context, function->program->GetFunctionFromCache (function_id), stack_top);
        _OP_END }

        _OP_START (CALL_NATIVE) {




            context->native_stack_size_ = 0;
        _OP_END }

        _OP_START (PUSH) { _OPC_PP (src, offset)
            fetch<mdr_u32> (context->stack ().array (), stack_top + offset) = fetch<mdr_u32> (stack, src);
        _OP_END }

        _OP_START (PUSHl) { _OPC_PP (src, offset)
            fetch<mdr_u64> (context->stack ().array (), stack_top + offset) = fetch<mdr_u64> (stack, src);
        _OP_END }

        _OP_START (NPUSH_REG) { _OPC_PP (src, offset)
            ((mdr_u32*) (context->native_call_stack_.array ()))[offset] = fetch<mdr_u32> (stack, src);
        _OP_END }

        _OP_START (NPUSH_REGl) { _OPC_PP (src, offset)
            context->native_call_stack_.array ()[offset] = fetch<mdr_u32> (stack, src);
        _OP_END }

        _OP_START (NPUSH_STACK) { _OPC_PP (src, offset)
            fetch<mdr_u32> (context->stack ().array (), stack_top + offset) = fetch<mdr_u32> (stack, src);
            context->native_stack_size_ += 1; /* Stack is 8 byte aligned. */
        _OP_END }

        _OP_START (NPUSH_STACKl) { _OPC_PP (src, offset)
            fetch<mdr_u64> (context->stack ().array (), stack_top + offset) = fetch<mdr_u64> (stack, src);
            context->native_stack_size_ += 1; /* Stack is 8 byte aligned. */
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