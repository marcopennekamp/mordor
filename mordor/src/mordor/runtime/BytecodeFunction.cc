#include <mordor/runtime/BytecodeFunction.h>


namespace mdr {

BytecodeFunction::BytecodeFunction (const std::string& name) {
    name_ = name;
}

BytecodeFunction::~BytecodeFunction () {
    delete[] code_;
}

void BytecodeFunction::Allocate (mdr_u32 code_size) {
    code_size_ = code_size;
    code_ = new mdr_u8[code_size];
}

}