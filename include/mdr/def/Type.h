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
    1,      /* i8 */
    2,      /* i16 */
    4,      /* i32 */
    8,      /* i64 */
    1,      /* u8 */
    2,      /* u16 */
    4,      /* u32 */
    8,      /* u64 */
    4,      /* f32 */
    8,      /* f64 */
    0       /* ptr / Size is not static. */
};


#ifdef	__cplusplus
}
#endif

#endif  /* MDR_DEF_TYPE_H_ */