#ifndef MDR_API_TYPE_H_
#define MDR_API_TYPE_H_

#include <mdr/def/Mdr.h>
#include <mdr/def/Type.h>

#ifdef	__cplusplus
extern "C" {
#endif


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

#endif  /* MDR_API_TYPE_H_ */