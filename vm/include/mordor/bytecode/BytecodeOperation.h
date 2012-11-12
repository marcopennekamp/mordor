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
    BCOP_iLOADw         = 0x06,
    BCOP_lLOAD          = 0x07,
    BCOP_lLOADw         = 0x08,
    BCOP_uiLOAD         = 0x09,
    BCOP_uiLOADw        = 0x0A,
    BCOP_ulLOAD         = 0x0B,
    BCOP_ulLOADw        = 0x0C,
    BCOP_fLOAD          = 0x0D,
    BCOP_fLOADw         = 0x0E,
    BCOP_flLOAD         = 0x0F,
    BCOP_flLOADw        = 0x10,
    BCOP_pLOAD          = 0x11,
    BCOP_pLOADw         = 0x12,
    BCOP_CONST          = 0x13,
    BCOP_CONST_0        = 0x14,
    BCOP_STORE          = 0x15,
    BCOP_STOREw         = 0x16,
    BCOP_ALOAD          = 0x17,
    BCOP_ASTORE         = 0x18,
    BCOP_MEMLOAD        = 0x19,
    BCOP_MEMSTORE       = 0x1A,
    BCOP_LOAD_GLOBAL    = 0x1B,
    BCOP_STORE_GLOBAL   = 0x1C,
    BCOP_LOAD_FIELD     = 0x1D,
    BCOP_STORE_FIELD    = 0x1E,
    BCOP_POP            = 0x1F,

    BCOP_INC            = 0x20,
    BCOP_INCw           = 0x21,
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
    BCOP_ADRw           = 0x36,
    BCOP_ADR_GLOBAL     = 0x37,
    BCOP_ADR_FIELD      = 0x38,
    BCOP_NEW            = 0x39,
    BCOP_ANEW           = 0x3A,
    BCOP_INEW           = 0x3B,

    BCOP_CALL           = 0x3C,
    BCOP_ICALL          = 0x3D,
    BCOP_VCALL          = 0x3E,
    BCOP_BREAKPOINT     = 0x3F
};
typedef enum mordorBytecodeOperationType BytecodeOperationType;


#include <mordor/mordor_end.h>

#endif  /* MORDOR_BYTECODE_BYTECODEOPERATION_H_ */