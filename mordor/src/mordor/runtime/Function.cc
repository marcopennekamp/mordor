#include <stdlib.h>

#include <mordor/runtime/Function.h>

#include <internal/runtime/Function.h>


namespace mdr {

Function::Function (size_t operations_size, size_t constant_table_size) {
    operations = new mdrOperation[operations_size];
    if (constant_table_size > 0) {
        constant_table = new mdr_u8[constant_table_size];
    }else {
        constant_table = NULL; /* Set to null if not needed so it can be determined whether to delete it later. */
    }
}

Function::~Function () {
    delete[] operations;
    if (constant_table != NULL) {
        delete[] constant_table;
    }
}

}