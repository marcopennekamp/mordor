#include <mordor/load/BytecodeFunction.h>


namespace mdr {

BytecodeFunction::BytecodeFunction (mdrType return_type, mdr_u8 parameter_count) {
    return_type_ = return_type;
    parameter_count_ = parameter_count;
}

BytecodeFunction::~BytecodeFunction () {
    delete[] code_;
}

void BytecodeFunction::Allocate (mdr_u32 code_size) {
    code_size_ = code_size;
    code_ = new mdr_u8[code_size];
}

}