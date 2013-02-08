#include <stdlib.h>

#include <mordor/runtime/Function.h>


namespace mdr {

Function::Function (size_t operations_size, size_t constant_table_size) {
    operations_ = new mdrOperation[operations_size];
    if (constant_table_size > 0) {
        constant_table_ = new mdr_u8[constant_table_size];
    }else {
        constant_table_ = NULL; /* Set to null if not needed so it can be determined whether to delete it later. */
    }
}

Function::~Function () {
    delete[] operations_;
    if (constant_table_ != NULL) {
        delete[] constant_table_;
    }
}

}