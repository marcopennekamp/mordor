/*
 * Short:   VARTYPE
 *
 */


#ifndef MORDOR_BYTECODE_VARIABLETYPE_H_
#define MORDOR_BYTECODE_VARIABLETYPE_H_

#include <mordor/def.h>

#ifdef	__cplusplus
extern "C" {
#endif


typedef mdr_u8  mdrVariableType;


#define MDR_VARTYPE_V 0x00
#define MDR_VARTYPE_I 0x01
#define MDR_VARTYPE_U 0x02
#define MDR_VARTYPE_F 0x04
#define MDR_VARTYPE_P 0x08
#define MDR_VARTYPE_IS_LONG 0x10


#ifdef	__cplusplus
}
#endif

#endif  /* MORDOR_BYTECODE_VARIABLETYPE_H_ */