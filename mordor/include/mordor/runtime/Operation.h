#ifndef MORDOR_RUNTIME_OPERATION_H_
#define	MORDOR_RUNTIME_OPERATION_H_

#include <mordor/def.h>

#ifdef	__cplusplus
extern "C" {
#endif


typedef mdr_u64       mdrOperation;


enum mdrOperationType {
    /* BASIC. */
    OP_END          = 0x00,
    OP_kJMP         = 0x01,
    OP_RET_VOID     = 0x0A,
    OP_RET          = 0x0B,
    OP_RETl         = 0x0C,

    /* MEM. */
    OP_RETMOV       = 0x10,
    OP_RETMOVl      = 0x11,

    OP_MOV          = 0x12,
    OP_MOVl         = 0x13,
    OP_kMOV         = 0x14,
    OP_kMOVl        = 0x15,

    OP_kMOVM        = 0x16,
    OP_kMOVMb       = 0x17,
    OP_kMOVMs       = 0x18,
    OP_kMOVMl       = 0x19,

    OP_MOV_M2V      = 0x1A,
    OP_MOV_M2Vb     = 0x1B,
    OP_MOV_M2Vs     = 0x1C,
    OP_MOV_M2Vl     = 0x1D,
    OP_MOV_V2M      = 0x1E,
    OP_MOV_V2Mb     = 0x1F,
    OP_MOV_V2Ms     = 0x20,
    OP_MOV_V2Ml     = 0x21,

    OP_MOVO_M2V     = 0x22,
    OP_MOVO_M2Vb    = 0x23,
    OP_MOVO_M2Vs    = 0x24,
    OP_MOVO_M2Vl    = 0x25,
    OP_MOVO_V2M     = 0x26,
    OP_MOVO_V2Mb    = 0x27,
    OP_MOVO_V2Ms    = 0x28,
    OP_MOVO_V2Ml    = 0x29,

    /* ARITH. */
    OP_ADD          = 0x30,
    OP_ADDl         = 0x31,
    OP_fADD         = 0x32,
    OP_fADDl        = 0x33,
    OP_kADD         = 0x34,
    OP_kADDl        = 0x35,
    OP_fkADD        = 0x36,
    OP_fkADDl       = 0x37,

    OP_SUB          = 0x38,
    OP_SUBl         = 0x39,
    OP_fSUB         = 0x3A,
    OP_fSUBl        = 0x3B,
    OP_kSUB         = 0x3C,
    OP_kSUBl        = 0x3D,
    OP_fkSUB        = 0x3E,
    OP_fkSUBl       = 0x3F,

    OP_sMUL         = 0x40,
    OP_sMULl        = 0x41,
    OP_uMUL         = 0x42,
    OP_uMULl        = 0x43,
    OP_fMUL         = 0x44,
    OP_fMULl        = 0x45,
    OP_skMUL        = 0x46,
    OP_skMULl       = 0x47,
    OP_ukMUL        = 0x48,
    OP_ukMULl       = 0x49,
    OP_fkMUL        = 0x4A,
    OP_fkMULl       = 0x4B,

    OP_sDIV         = 0x4C,
    OP_sDIVl        = 0x4D,
    OP_uDIV         = 0x4E,
    OP_uDIVl        = 0x4F,
    OP_fDIV         = 0x50,
    OP_fDIVl        = 0x51,
    OP_skDIV        = 0x52,
    OP_skDIVl       = 0x53,
    OP_ukDIV        = 0x54,
    OP_ukDIVl       = 0x55,
    OP_fkDIV        = 0x56,
    OP_fkDIVl       = 0x57,

    OP_sREM         = 0x58,
    OP_sREMl        = 0x59,
    OP_uREM         = 0x5A,
    OP_uREMl        = 0x5B,
    OP_skREM        = 0x5C,
    OP_skREMl       = 0x5D,
    OP_ukREM        = 0x5E,
    OP_ukREMl       = 0x5F,

    OP_NEG          = 0x60,
    OP_NEGl         = 0x61,

    /* BITWISE. */
    OP_AND          = 0x70,
    OP_ANDl         = 0x71,
    OP_kAND         = 0x72,
    OP_kANDl        = 0x73,

    OP_OR           = 0x74,
    OP_ORl          = 0x75,
    OP_kOR          = 0x76,
    OP_kORl         = 0x77,

    OP_XOR          = 0x78,
    OP_XORl         = 0x79,
    OP_kXOR         = 0x7A,
    OP_kXORl        = 0x7B,

    OP_SHL          = 0x7C,
    OP_SHLl         = 0x7D,
    OP_kSHL         = 0x7E,
    OP_kSHLl        = 0x7F,

    OP_SHR          = 0x80,
    OP_SHRl         = 0x81,
    OP_kSHR         = 0x82,
    OP_kSHRl        = 0x83,

    OP_ASHR         = 0x84,
    OP_ASHRl        = 0x85,
    OP_kASHR        = 0x86,
    OP_kASHRl       = 0x87,

    OP_NOT          = 0x88,
    OP_NOTl         = 0x89,

    /* CALL.*/
    OP_CALL         = 0xA0,
    OP_CALL_NATIVE  = 0xA1,
    OP_PUSH         = 0xB0,
    OP_PUSHl        = 0xB1
};


#ifdef	__cplusplus
}
#endif

#endif  /* MORDOR_RUNTIME_OPERATION_H_ */

