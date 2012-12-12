#ifndef MORDOR_BYTECODE_BYTECODEOPERATION_H_
#define	MORDOR_BYTECODE_BYTECODEOPERATION_H_

#include <mordor/mordor_begin.h>


typedef mordor_u8               mordorBytecodeOperation;
typedef mordorBytecodeOperation BytecodeOperation;

enum mordorBytecodeOperationType {
    BCOP_END            = 0x00,
    BCOP_JMP            = 0x01,
    BCOP_JMPw           = 0x02,
    BCOP_RETURN         = 0x03,
    BCOP_RETURN_VOID    = 0x04,

    BCOP_iLOAD          = 0x05,
    BCOP_lLOAD          = 0x06,
    BCOP_uiLOAD         = 0x07,
    BCOP_ulLOAD         = 0x08,
    BCOP_fLOAD          = 0x09,
    BCOP_flLOAD         = 0x0A,
    BCOP_pLOAD          = 0x0B,
    BCOP_STORE          = 0x0C,
    BCOP_CONST          = 0x0D,

    /* 0E - 0F free slot. */

    BCOP_ALOAD          = 0x10,
    BCOP_ASTORE         = 0x11,
    BCOP_MEMLOAD        = 0x12,
    BCOP_MEMSTORE       = 0x13,
    BCOP_LOAD_GLOBAL    = 0x14,
    BCOP_STORE_GLOBAL   = 0x15,
    BCOP_LOAD_FIELD     = 0x16,
    BCOP_STORE_FIELD    = 0x17,
    BCOP_POP            = 0x18,

    /* 19 - 1F free slot. */

    BCOP_INC_VAR        = 0x20,
    BCOP_INC            = 0x21,
    BCOP_ADD            = 0x22,
    BCOP_SUB            = 0x23,
    BCOP_MUL            = 0x24,
    BCOP_DIV            = 0x25,
    BCOP_REM            = 0x26,
    BCOP_NEG            = 0x27,
    BCOP_SHL            = 0x28,
    BCOP_SHR            = 0x29,
    BCOP_AND            = 0x2A,
    BCOP_OR             = 0x2B,
    BCOP_XOR            = 0x2C,
    BCOP_NOT            = 0x2D,

    BCOP_CEQ_0          = 0x2E,
    BCOP_CNE_0          = 0x2F,
    BCOP_CEQ            = 0x30,
    BCOP_CNE            = 0x31,
    BCOP_CGT            = 0x32,
    BCOP_CGE            = 0x33,

    BCOP_CAST           = 0x34,

    BCOP_ADR            = 0x35,
    BCOP_ADR_GLOBAL     = 0x36,
    BCOP_ADR_FIELD      = 0x37,

    /* 38 free slot. */

    BCOP_NEW            = 0x39,
    BCOP_ANEW           = 0x3A,
    BCOP_INEW           = 0x3B,

    BCOP_CALL           = 0x3C,
    BCOP_ICALL          = 0x3D,
    BCOP_VCALL          = 0x3E,

    BCOP_BREAKPOINT     = 0x3F
};
typedef enum mordorBytecodeOperationType BytecodeOperationType;

const size_t BCOP_LENGTH = 0x40;


#include <mordor/mordor_end.h>

#endif  /* MORDOR_BYTECODE_BYTECODEOPERATION_H_ */