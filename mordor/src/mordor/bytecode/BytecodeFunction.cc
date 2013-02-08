#include <internal/bytecode/BytecodeFunction.h>


namespace mdr {

BytecodeFunction::~BytecodeFunction () {
    delete[] code;
}


void BytecodeFunction::AllocateCode (mdr_u32 _code_size) {
    code_size = _code_size;
    code = new mdr_u8[_code_size];
}

}