#include <stdlib.h>

#include <mordor/runtime/Function.h>


namespace mdr {

Function::Function (CompilationInformation& cpinfo) {
    cpinfo_ = cpinfo;

    /* Set to null if not needed or allocated so it can be determined whether to delete it later. */
    operations_ = NULL;
    constant_table_ = NULL;
}

void Function::Allocate (size_t operations_size, size_t constant_table_size) {
    operations_ = new mdrOperation[operations_size];
    if (constant_table_size > 0) {
        constant_table_ = new mdr_u8[constant_table_size];
    }
}

Function::~Function () {
    if (operations_ != NULL) {
        delete[] operations_;
    }

    if (constant_table_ != NULL) {
        delete[] constant_table_;
    }
}

}