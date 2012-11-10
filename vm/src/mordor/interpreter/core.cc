#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mordor/interpreter/core.h>
#include <mordor/runtime/Operation.h>

#include <internal/runtime/Context.h>
#include <internal/runtime/Function.h>
#include <internal/runtime/Program.h>

using namespace mordor;




/* Operation definition macros. */

#define OP_START(NAME)  case OP_ ## NAME :
#define OP_END          goto LExecEnd;
#define OP_RESTART      goto LExec;
#define OP_RETURN       goto LReturn;




/* Class parameter parsing. */

#define OPC_P(P0)               mordor_u16 P0; \
                                  extract_P (op, P0);
#define OPC_PP(P0, P1)          mordor_u16 P0, P1; \
                                  extract_PP (op, P0, P1);
#define OPC_PPP(P0, P1, P2)     mordor_u16 P0, P1, P2; \
                                  extract_PPP (op, P0, P1, P2);

#define OPC_W(P0)               mordor_u32 P0; \
                                  extract_W (op, P0);
#define OPC_WP(P0, P1)          mordor_u32 P0; mordor_u16 P1; \
                                  extract_WP (op, P0, P1);
#define OPC_PW(P0, P1)          mordor_u16 P0; mordor_u32 P1; \
                                  extract_PW (op, P0, P1);

#define OPC_L(P0)               mordor_u64 P0; \
                                  extract_L (op, P0);




/* Class extraction functions. */

namespace {

inline void extract_P (Operation op, mordor_u16& param0) {
    param0 = op >> 16 & 0xFFFF;
}

inline void extract_PP (Operation op, mordor_u16& param0, mordor_u16& param1) {
    extract_P (op, param0);
    param1 = op >> 32 & 0xFFFF;
}

inline void extract_PPP (Operation op, mordor_u16& param0, mordor_u16& param1, mordor_u16& param2) {
    extract_PP (op, param0, param1);
    param2 = op >> 48 & 0xFFFF;
}

inline void extract_W (Operation op, mordor_u32& param0) {
    param0 = op >> 16 & 0xFFFFFFFF;
}

inline void extract_WP (Operation op, mordor_u32& param0, mordor_u16& param1) {
    extract_W (op, param0);
    param1 = op >> 48 & 0xFFFF;
}

inline void extract_PW (Operation op, mordor_u16& param0, mordor_u32& param1) {
    extract_P (op, param0);
    param1 = op >> 32 & 0xFFFFFFFF;
}

inline void extract_L (Operation op, mordor_u64& param0) {
    param0 = op >> 16 & 0xFFFFFFFFFFFF;
}

inline void extract_M (Operation op, mordor_u8& param0, mordor_u8& param1, mordor_u8& param2, mordor_u8& param3, mordor_u8& param4, mordor_u8& param5) {
    /* TODO(Marco): Probably needs some optimization here. If possible, that is. */
    param0 = op >> 16 & 0xFF;
    param1 = op >> 24 & 0xFF;
    param2 = op >> 32 & 0xFF;
    param3 = op >> 40 & 0xFF;
    param4 = op >> 48 & 0xFF;
    param5 = op >> 56;
}




/* Fetch functions. */

inline mordor_u64* fetch_Long (mordor_u8* stack, mordor_u16 id) {
    return (mordor_u64*) (stack + id * sizeof (mordor_u32));
}

}




extern "C" void mordorInterpreterExecute (ContextInterface* context_interface, ProgramInterface* program, FunctionInterface* function_interface, mordor_u32 caller_stack_top, mordor_u8** return_address) {
    /* General stuff. */
    Function*   function    = (Function*) function_interface;
    Context*    context = (Context*) context_interface;
    Operation*  op_pointer  = function->operations;
    mordor_u32  stack_top   = caller_stack_top + function->stack_size;
    mordor_u8*  retval_address;

    // TODO: This is a security check!
    if (stack_top > context->stack_size) {
        printf ("Error: Requested stack size exceeds Context stack limit.");
        return;
    }

    /* Stacks. */
    mordor_u8*  stack       = ((mordor_u8*) context->stack + caller_stack_top);
    mordor_u32* stack_u32   = (mordor_u32*) stack;
    mordor_f32* stack_f32   = (mordor_f32*) stack;

    /* Current op. */
    mordor_u64 op;


    /* Executes the next instruction. */
  LExec:
    op = *op_pointer;

    switch (op & 0xFFFF) {

    /* BASIC. */

        OP_START (kJMP) { OPC_W (offset)
            op_pointer = (Operation*) (((mordor_u8*) op_pointer) + ((mordor_s32) offset));
        OP_RESTART }

        OP_START (CALL) { OPC_W (function_id)
            mordorInterpreterExecute (context, program, ((Program*) program)->GetFunctionFromCache (function_id), stack_top, &retval_address);
        OP_END }

        OP_START (CALL_NATIVE) {

        OP_END }

        OP_START (RET) { OPC_P (src)
            *return_address = stack + src * sizeof (mordor_u32);
        OP_RETURN }

        OP_START (RETl) { OPC_P (src)
            *return_address = stack + src * sizeof (mordor_u32);
        OP_RETURN }


    /* MEM. */

        OP_START (REGMOV) {

        OP_END }

        OP_START (REGMOVl) {

        OP_END }

        OP_START (MOV) { OPC_PP (dest, src)
            stack_u32[dest] = stack_u32[src];
        OP_END }

        OP_START (MOVl) {

        OP_END }

        OP_START (kMOV) { OPC_PW (dest, src)
            stack_u32[dest] = src;
        OP_END }

        OP_START (kMOVl) {

        OP_END }

        OP_START (kMOVM) {

        OP_END }

        OP_START (kMOVMb) {

        OP_END }

        OP_START (kMOVMs) {

        OP_END }

        OP_START (kMOVMl) {

        OP_END }

        OP_START (MOV_M2V) {

        OP_END }

        OP_START (MOV_M2Vb) {

        OP_END }

        OP_START (MOV_M2Vs) {

        OP_END }

        OP_START (MOV_M2Vl) {

        OP_END }

        OP_START (MOV_V2M) {

        OP_END }

        OP_START (MOV_V2Mb) {

        OP_END }

        OP_START (MOV_V2Ms) {

        OP_END }

        OP_START (MOV_V2Ml) {

        OP_END }

        OP_START (MOVO_M2V) {

        OP_END }

        OP_START (MOVO_M2Vb) {

        OP_END }

        OP_START (MOVO_M2Vs) {

        OP_END }

        OP_START (MOVO_M2Vl) {

        OP_END }

        OP_START (MOVO_V2M) {

        OP_END }

        OP_START (MOVO_V2Mb) {

        OP_END }

        OP_START (MOVO_V2Ms) {

        OP_END }

        OP_START (MOVO_V2Ml) {

        OP_END }


    /* ARITH. */

        OP_START (ADD) { OPC_PP (dest, src)
            stack_u32[dest] += stack_u32[src];
        OP_END }

        OP_START (ADDl) { OPC_PP (dest, src)
            mordor_u64* dest_addr = fetch_Long (stack, dest);
            mordor_u64  src_value = *fetch_Long (stack, src);
            *dest_addr += src_value;
        OP_END }

        OP_START (fADD) {

        OP_END }

        OP_START (fADDl) {

        OP_END }

        OP_START (kADD) {

        OP_END }

        OP_START (kADDl) {

        OP_END }

        OP_START (fkADD) {

        OP_END }

        OP_START (fkADDl) {

        OP_END }

        OP_START (SUB) { OPC_PP (dest, src)
            stack_u32[dest] -= stack_u32[src];
        OP_END }

        OP_START (SUBl) {

        OP_END }

        OP_START (fSUB) {

        OP_END }

        OP_START (fSUBl) {

        OP_END }

        OP_START (kSUB) {

        OP_END }

        OP_START (kSUBl) {

        OP_END }

        OP_START (fkSUB) {

        OP_END }

        OP_START (fkSUBl) {

        OP_END }

        OP_START (sMUL) { OPC_PP (dest, src)
            mordor_s32* dest_addr = (mordor_s32*) stack_u32 + dest;
            mordor_s32 src_val = *((mordor_s32*) stack_u32 + src);
            *dest_addr *= src_val;
        OP_END }

        OP_START (sMULl) {

        OP_END }

        OP_START (uMUL) { OPC_PP (dest, src)
            stack_u32[dest] *= stack_u32[src];
        OP_END }

        OP_START (uMULl) {

        OP_END }

        OP_START (fMUL) {

        OP_END }

        OP_START (fMULl) {

        OP_END }

        OP_START (skMUL) {

        OP_END }

        OP_START (skMULl) {

        OP_END }

        OP_START (ukMUL) {

        OP_END }

        OP_START (ukMULl) {

        OP_END }

        OP_START (fkMUL) {

        OP_END }

        OP_START (fkMULl) {

        OP_END }

        OP_START (sDIV) {

        OP_END }

        OP_START (sDIVl) {

        OP_END }

        OP_START (uDIV) {

        OP_END }

        OP_START (uDIVl) {

        OP_END }

        OP_START (fDIV) {

        OP_END }

        OP_START (fDIVl) {

        OP_END }

        OP_START (skDIV) {

        OP_END }

        OP_START (skDIVl) {

        OP_END }

        OP_START (ukDIV) {

        OP_END }

        OP_START (ukDIVl) {

        OP_END }

        OP_START (fkDIV) {

        OP_END }

        OP_START (fkDIVl) {

        OP_END }

        OP_START (sREM) {

        OP_END }

        OP_START (sREMl) {

        OP_END }

        OP_START (uREM) {

        OP_END }

        OP_START (uREMl) {

        OP_END }

        OP_START (skREM) {

        OP_END }

        OP_START (skREMl) {

        OP_END }

        OP_START (ukREM) {

        OP_END }

        OP_START (ukREMl) {

        OP_END }

        OP_START (NEG) {

        OP_END }

        OP_START (NEGl) {

        OP_END }


    /* BITWISE. */

        OP_START (AND) {

        OP_END }

        OP_START (ANDl) {

        OP_END }

        OP_START (kAND) {

        OP_END }

        OP_START (kANDl) {

        OP_END }

        OP_START (OR) {

        OP_END }

        OP_START (ORl) {

        OP_END }

        OP_START (kOR) {

        OP_END }

        OP_START (kORl) {

        OP_END }

        OP_START (XOR) {

        OP_END }

        OP_START (XORl) {

        OP_END }

        OP_START (kXOR) {

        OP_END }

        OP_START (kXORl) {

        OP_END }

        OP_START (SHL) {

        OP_END }

        OP_START (SHLl) {

        OP_END }

        OP_START (kSHL) {

        OP_END }

        OP_START (kSHLl) {

        OP_END }

        OP_START (SHR) {

        OP_END }

        OP_START (SHRl) {

        OP_END }

        OP_START (kSHR) {

        OP_END }

        OP_START (kSHRl) {

        OP_END }

        OP_START (ASHR) {

        OP_END }

        OP_START (ASHRl) {

        OP_END }

        OP_START (kASHR) {

        OP_END }

        OP_START (kASHRl) {

        OP_END }

        OP_START (NOT) {

        OP_END }

        OP_START (NOTl) {

        OP_END }

        default: OP_RETURN
    }


    /* Increments the instruction pointer after an instruction. */
  LExecEnd:
    ++op_pointer;
    goto LExec;

  LReturn:
    return;
}
