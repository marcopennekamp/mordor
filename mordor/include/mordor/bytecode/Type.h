#ifndef MORDOR_BYTECODE_TYPE_H_
#define MORDOR_BYTECODE_TYPE_H_

#include <mordor/def.h>

#ifdef	__cplusplus
extern "C" {
#endif


typedef mdr_u8  mdrType;


#define MDR_TYPE_VOID   0x00
#define MDR_TYPE_I8     0x01
#define MDR_TYPE_I16    0x02
#define MDR_TYPE_I32    0x03
#define MDR_TYPE_I64    0x04
#define MDR_TYPE_U8     0x05
#define MDR_TYPE_U16    0x06
#define MDR_TYPE_U32    0x07
#define MDR_TYPE_U64    0x08
#define MDR_TYPE_F32    0x09
#define MDR_TYPE_F64    0x0A
#define MDR_TYPE_PTR    0x0B


/*
 * 64, 32, 16 or 8 as size. 
 * Does not work with the ptr type.
 */
MDR_DECL mdr_bool mdrTypeHasSize (mdrType type, mdr_u8 size);
MDR_DECL mdr_u8 mdrTypeGetSize (mdrType type);
MDR_DECL void mdrTypeAddSize (mdrType& type);


#ifdef	__cplusplus
}
#endif

#endif  /* MORDOR_BYTECODE_TYPE_H_ */