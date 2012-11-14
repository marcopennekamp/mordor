#ifndef MORDOR_INTERNAL_BYTECODE_BYTECODEFUNCTION_H_
#define MORDOR_INTERNAL_BYTECODE_BYTECODEFUNCTION_H_

#include <string>

#include <mordor/def.h>

#include <internal/utils/Array.h>


namespace mordor {

class BytecodeFunction {
  public:
    mordor_u32 variable_table_size;
    mordor_u32 pointer_table_size;
    mordor_u32 maximum_stack_size;
    mordor_u32 operation_count;

    mordor_u8 parameter_count;
    mordor_u8 return_type; // TODO(Marco): Use proper Return Type type.

    mordor_u16 function_name_table_size;
    Array<std::string> function_name_table;

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
