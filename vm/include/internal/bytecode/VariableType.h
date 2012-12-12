#ifndef MORDOR_INTERNAL_BYTECODE_VARIABLETYPE_H_
#define MORDOR_INTERNAL_BYTECODE_VARIABLETYPE_H_

#include <mordor/def.h>


namespace mordor {
namespace VariableType {

    typedef mordor_u8 T;
    const mordor_u8 VOID = 0x00;
    const mordor_u8 I = 0x01;
    const mordor_u8 U = 0x02;
    const mordor_u8 F = 0x04;
    const mordor_u8 P = 0x08;
    const mordor_u8 IS_LONG = 0x10;

}
}

#endif  /* MORDOR_INTERNAL_BYTECODE_VARIABLETYPE_H_ */