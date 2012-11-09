#ifndef MORDOR_INTERNAL_BYTECODE_BYTECODEFUNCTION_H
#define MORDOR_INTERNAL_BYTECODE_BYTECODEFUNCTION_H

#include <mordor/def.h>


namespace mordor {

class BytecodeFunction {
  public:
    mordor_u32 variable_table_size;
    mordor_u32 pointer_table_size;

    // Constant table
    // Global var name table
    // Struct name table
    // Field name table
    // Function name table
    // Interface <-> Struct name table
    // Struct <-> Field name table


    mordor_u32 code_size;
    mordor_u8* code;
};

}


#endif /* MORDOR_INTERNAL_BYTECODE_BYTECODEFUNCTION_H */
