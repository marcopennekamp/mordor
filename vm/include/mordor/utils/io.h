#ifndef MORDOR_UTILS_IO_H_
#define MORDOR_UTILS_IO_H_

#include <stdio.h>

#include <mordor/mordor_begin.h>


/*
 * These functions write the specified type to the output stream.
 */
void mordorWriteS8 (FILE* stream, mordor_s8 value);
void mordorWriteU8 (FILE* stream, mordor_u8 value);
void mordorWriteS16 (FILE* stream, mordor_s16 value);
void mordorWriteU16 (FILE* stream, mordor_u16 value);
void mordorWriteS32 (FILE* stream, mordor_s32 value);
void mordorWriteU32 (FILE* stream, mordor_u32 value);
void mordorWriteS64 (FILE* stream, mordor_s64 value);
void mordorWriteU64 (FILE* stream, mordor_u64 value);

/*
 * These functions read the specified type from the input stream.
 */
mordor_s8 mordorReadS8 (FILE* stream);
mordor_u8 mordorReadU8 (FILE* stream);
mordor_s16 mordorReadS16 (FILE* stream);
mordor_u16 mordorReadU16 (FILE* stream);
mordor_s32 mordorReadS32 (FILE* stream);
mordor_u32 mordorReadU32 (FILE* stream);
mordor_s64 mordorReadS64 (FILE* stream);
mordor_u64 mordorReadU64 (FILE* stream);


#include <mordor/mordor_end.h>

#endif
