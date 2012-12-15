/* 
 * The mordor prefix is mdr.
 *
 */


#ifndef MORDOR_DEF_H_
#define MORDOR_DEF_H_

#ifdef	__cplusplus
extern "C" {
#endif


/*
 * OS target.
 */
#define MDR_OS windows


/*
 * DLL Exports.
 */
#ifdef MDR_EXPORT
#define MDR_DECL __declspec (dllexport)
#else
#define MDR_DECL __declspec (dllimport)
#endif


/*
 * General typedefs.
 */
typedef char                        mdr_s8;
typedef unsigned char               mdr_u8;
typedef short int                   mdr_s16;
typedef unsigned short int          mdr_u16;
typedef int                         mdr_s32;
typedef unsigned int                mdr_u32;
typedef long long int               mdr_s64;
typedef unsigned long long int      mdr_u64;

typedef float                       mdr_f32;
typedef double                      mdr_f64;

typedef mdr_u8                      mdr_bool;


/*
 * Constants.
 */
#define mdr_false 0
#define mdr_true 1


#ifdef	__cplusplus
}
#endif

#endif  /* MORDOR_DEF_H_ */
