#ifndef MORDOR_DEF_H_
#define MORDOR_DEF_H_

/*
 * OS target.
 */
#define MORDOR_OS windows


/*
 * DLL Exports.
 */
#ifdef MORDOR_EXPORT
#define MORDOR_DECL __declspec (dllexport)
#else
#define MORDOR_DECL __declspec (dllimport)
#endif


/*
 * General typedefs.
 */
typedef char mordor_s8;
typedef unsigned char mordor_u8;
typedef short int mordor_s16;
typedef unsigned short int mordor_u16;
typedef int mordor_s32;
typedef unsigned int mordor_u32;
typedef long long int mordor_s64;
typedef unsigned long long int mordor_u64;

typedef float mordor_f32;
typedef double mordor_f64;

typedef mordor_u8 mordor_bool;


/*
 * Constants.
 */
#define mordor_false 0
#define mordor_true 1


/*
 * Internal string representation.
 * ASCII representation, because of memory limit reasons.
 */
typedef mordor_s8 mordor_char;

struct mordorString {
    mordor_u16 length;
    mordor_char* data;
};
typedef struct mordorString mordor_string;

#endif  /* MORDOR_DEF_H_ */
