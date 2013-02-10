#ifndef MORDOR_ASSEMBLER_TOKEN_H_
#define MORDOR_ASSEMBLER_TOKEN_H_

#include <mordor/def/Mordor.h>


namespace mordor {
namespace assembler {

const int TOKEN_LITERAL = 0x00;
const int TOKEN_STRING = 0x01;
const int TOKEN_INT_32 = 0x02;
const int TOKEN_INT_64 = 0x03;
const int TOKEN_UINT_32 = 0x04;
const int TOKEN_UINT_64 = 0x05;
const int TOKEN_FLOAT_32 = 0x06;
const int TOKEN_FLOAT_64 = 0x07;
const int TOKEN_CHARACTER = 0x08;

struct Token {
    int tag;
    union {
        char*   string;
        char    character;
        mdr_s32 nInt32;
        mdr_s64 nInt64;
        mdr_u32 nUInt32;
        mdr_u64 nUInt64;
        mdr_f32 nFloat32;
        mdr_f64 nFloat64;
    };

    ~Token () {
        if (tag == TOKEN_STRING) {
            delete[] string;
        }
    }
};

}
}

#endif  /* MORDOR_ASSEMBLER_TOKEN_H_ */