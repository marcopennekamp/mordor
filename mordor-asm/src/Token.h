#ifndef MORDOR_ASSEMBLER_TOKEN_H_
#define MORDOR_ASSEMBLER_TOKEN_H_

#include <mdr/def/Mdr.h>
#include <mdr/runtime/BytecodeFunction.h>


namespace mdr {
namespace assembler {

const int TOKEN_LITERAL = 0x00;
const int TOKEN_STRING = 0x01;
const int TOKEN_CONSTANT = 0x02;
const int TOKEN_CHARACTER = 0x03;

struct Token {
    int tag;
    union {
        char*                       string;
        char                        character;
        BytecodeFunction::Constant  constant;
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