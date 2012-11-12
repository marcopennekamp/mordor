#ifndef MORDOR_INTERNAL_BYTECODE_BYTECODEFUNCTION_H_
#define MORDOR_INTERNAL_BYTECODE_BYTECODEFUNCTION_H_

#include <mordor/def.h>


namespace mordor {

class BytecodeFunction {
  public:
    mordor_u32 variable_table_size;
    mordor_u32 pointer_table_size;
    mordor_u32 maximum_stack_size;
    mordor_u32 operation_count;

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


#endif /* MORDOR_INTERNAL_BYTECODE_BYTECODEFUNCTION_H_ */
