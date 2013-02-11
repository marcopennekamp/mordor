#ifndef MDR_DEF_TYPE_H_
#define MDR_DEF_TYPE_H_

#include <mdr/def/Mdr.h>

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


static const mdr_u32 MDR_TYPE_SIZE [] = {
    0,      /* void */
    8,      /* i8 */
    16,     /* i16 */
    32,     /* i32 */
    64,     /* i64 */
    8,      /* u8 */
    16,     /* u16 */
    32,     /* u32 */
    64,     /* u64 */
    32,     /* f32 */
    64,     /* f64 */
    0       /* ptr / Size is not static. */
};


#ifdef	__cplusplus
}
#endif

#endif  /* MDR_DEF_TYPE_H_ */