#include <internal/bytecode/BytecodeFunction.h>


namespace mordor {

BytecodeFunction::~BytecodeFunction () {
    delete[] code;
}


void BytecodeFunction::AllocateCode (mordor_u32 _code_size) {
    code_size = _code_size;
    code = new mordor_u8[_code_size];
}

}