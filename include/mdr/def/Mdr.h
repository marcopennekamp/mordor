#ifndef MDR_DEF_MDR_H_
#define MDR_DEF_MDR_H_

#ifdef	__cplusplus
extern "C" {
#endif


/*
 * OS target.
 */
#define MDR_OS   windows

/*
 * DLL Exports.
 */
#ifdef MDR_STATIC
  #define MDR_DECL 
#else  
  #ifdef MDR_EXPORT
    #define MDR_DECL __declspec (dllexport)
  #else
    #define MDR_DECL __declspec (dllimport)
  #endif
#endif

/* 
 * This is the calling convention used for functions that are called by mordor.
 * Note: Uses the standard x64 calling convention, but specifies __stdcall to have a convention.
 */
#define MDR_FFI_CALL __stdcall

/* 
 * Used to export or import functions that are called by mordor.
 */
#ifdef MDRI_EXPORT
#define MDR_FFI_DECL __declspec (dllexport)
#else
#define MDR_FFI_DECL __declspec (dllimport)
#endif

/*
 * General typedefs.
 */
typedef char                        mdr_i8;
typedef unsigned char               mdr_u8;
typedef short int                   mdr_i16;
typedef unsigned short int          mdr_u16;
typedef int                         mdr_i32;
typedef unsigned int                mdr_u32;
typedef long long int               mdr_i64;
typedef unsigned long long int      mdr_u64;

typedef float                       mdr_f32;
typedef double                      mdr_f64;

typedef mdr_u8                      mdr_bool;

/*
 * Constants.
 */
#define mdr_false   (0)
#define mdr_true    (1)


#ifdef	__cplusplus
}
#endif

#endif  /* MDR_DEF_MDR_H_ */
