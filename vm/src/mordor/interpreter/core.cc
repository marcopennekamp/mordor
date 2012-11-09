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
    /* TODO(Marco): Probably needs some optimization here. */
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

    goto LExec;

    /* Increments the instruction pointer after an instruction. */
  LExecEnd:
    ++op_pointer;
    goto LExec;

  /* Executes the next instruction. */
  LExec:
    op = *op_pointer;

    switch (op & 0xFFFF) {

    /* BASIC. */

        OP_START (kJMP) { OPC_W (offset)
            op_pointer = (Operation*) (((mordor_u8*) op_pointer) + ((mordor_s32) offset));
        OP_RESTART }

        OP_START (CALL) { OPC_W (function_id)
            mordorInterpreterExecute (context, program, ((Program*) program)->GetFunction (function_id), stack_top, &retval_address);
        OP_END }

        __OP_CALL_NATIVE : {

        }

        OP_START (RET) { OPC_P (src)
            *return_address = stack + src * sizeof (mordor_u32);
        }

        OP_START (RETl) { OPC_P (src)
            *return_address = stack + src * sizeof (mordor_u32);
        }


    /* MEM. */

        __OP_REGMOV : {

        }

        __OP_REGMOVl : {

        }

        OP_START (MOV) { OPC_PP (dest, src)
            stack_u32[dest] = stack_u32[src];
        }

        __OP_MOVl : {

        }

        OP_START (kMOV) { OPC_PW (dest, src)
            stack_u32[dest] = src;
        OP_END }

        __OP_kMOVl : {

        }

        __OP_kMOVM : {

        }

        __OP_kMOVMb : {

        }

        __OP_kMOVMs : {

        }

        __OP_kMOVMl : {

        }

        __OP_MOV_M2V : {

        }

        __OP_MOV_M2Vb : {

        }

        __OP_MOV_M2Vs : {

        }

        __OP_MOV_M2Vl : {

        }

        __OP_MOV_V2M : {

        }

        __OP_MOV_V2Mb : {

        }

        __OP_MOV_V2Ms : {

        }

        __OP_MOV_V2Ml : {

        }

        __OP_MOVO_M2V : {

        }

        __OP_MOVO_M2Vb : {

        }

        __OP_MOVO_M2Vs : {

        }

        __OP_MOVO_M2Vl : {

        }

        __OP_MOVO_V2M : {

        }

        __OP_MOVO_V2Mb : {

        }

        __OP_MOVO_V2Ms : {

        }

        __OP_MOVO_V2Ml : {

        }


    /* ARITH. */

        OP_START (ADD) { OPC_PP (dest, src)
            stack_u32[dest] += stack_u32[src];
        OP_END }

        OP_START (ADDl) { OPC_PP (dest, src)
            mordor_u64* dest_addr = fetch_Long (stack, dest);
            mordor_u64  src_value = *fetch_Long (stack, src);
            *dest_addr += src_value;
        OP_END }

        __OP_fADD : {

        }

        __OP_fADDl : {

        }

        __OP_kADD : {

        }

        __OP_kADDl : {

        }

        __OP_fkADD : {

        }

        __OP_fkADDl : {

        }

        OP_START (SUB) { OPC_PP (dest, src)
            stack_u32[dest] -= stack_u32[src];
        }

        __OP_SUBl : {

        }

        __OP_fSUB : {

        }

        __OP_fSUBl : {

        }

        __OP_kSUB : {

        }

        __OP_kSUBl : {

        }

        __OP_fkSUB : {

        }

        __OP_fkSUBl : {

        }

        OP_START (sMUL) { OPC_PP (dest, src)
            mordor_s32* dest_addr = (mordor_s32*) stack_u32 + dest;
            mordor_s32 src_val = *((mordor_s32*) stack_u32 + src);
            *dest_addr *= src_val;
        }

        __OP_sMULl : {

        }

        OP_START (uMUL) { OPC_PP (dest, src)
            stack_u32[dest] *= stack_u32[src];
        }

        __OP_uMULl : {

        }

        __OP_fMUL : {

        }

        __OP_fMULl : {

        }

        __OP_skMUL : {

        }

        __OP_skMULl : {

        }

        __OP_ukMUL : {

        }

        __OP_ukMULl : {

        }

        __OP_fkMUL : {

        }

        __OP_fkMULl : {

        }

        __OP_sDIV : {

        }

        __OP_sDIVl : {

        }

        __OP_uDIV : {

        }

        __OP_uDIVl : {

        }

        __OP_fDIV : {

        }

        __OP_fDIVl : {

        }

        __OP_skDIV : {

        }

        __OP_skDIVl : {

        }

        __OP_ukDIV : {

        }

        __OP_ukDIVl : {

        }

        __OP_fkDIV : {

        }

        __OP_fkDIVl : {

        }

        __OP_sREM : {

        }

        __OP_sREMl : {

        }

        __OP_uREM : {

        }

        __OP_uREMl : {

        }

        __OP_skREM : {

        }

        __OP_skREMl : {

        }

        __OP_ukREM : {

        }

        __OP_ukREMl : {

        }

        __OP_NEG : {

        }

        __OP_NEGl : {

        }


    /* BITWISE. */

        __OP_AND : {

        }

        __OP_ANDl : {

        }

        __OP_kAND : {

        }

        __OP_kANDl : {

        }

        __OP_OR : {

        }

        __OP_ORl : {

        }

        __OP_kOR : {

        }

        __OP_kORl : {

        }

        __OP_XOR : {

        }

        __OP_XORl : {

        }

        __OP_kXOR : {

        }

        __OP_kXORl : {

        }

        __OP_SHL : {

        }

        __OP_SHLl : {

        }

        __OP_kSHL : {

        }

        __OP_kSHLl : {

        }

        __OP_SHR : {

        }

        __OP_SHRl : {

        }

        __OP_kSHR : {

        }

        __OP_kSHRl : {

        }

        __OP_ASHR : {

        }

        __OP_ASHRl : {

        }

        __OP_kASHR : {

        }

        __OP_kASHRl : {

        }

        __OP_NOT : {

        }

        __OP_NOTl : {

        }

        default:
            return;
    }

}
