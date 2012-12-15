#ifndef MORDOR_INTERNAL_BYTECODE_BYTECODEFUNCTION_H_
#define MORDOR_INTERNAL_BYTECODE_BYTECODEFUNCTION_H_

#include <string>

#include <mordor/def.h>

#include <internal/utils/Array.h>
#include <internal/bytecode/VariableType.h>


namespace mordor {

class MORDOR_DECL BytecodeFunction {
public:
    const static mordor_u8 EXISTS_CONSTANT_TABLE                = 0x01;
    const static mordor_u8 EXISTS_GLOBAL_NAME_TABLE             = 0x02;
    const static mordor_u8 EXISTS_STRUCT_NAME_TABLE             = 0x04;
    const static mordor_u8 EXISTS_FIELD_NAME_TABLE              = 0x08;
    const static mordor_u8 EXISTS_FUNCTION_NAME_TABLE           = 0x10;
    const static mordor_u8 EXISTS_INTERFACE_STRUCT_INDEX_TABLE  = 0x20;
    const static mordor_u8 EXISTS_STRUCT_FIELD_INDEX_TABLE      = 0x40;

    VariableType::T return_type;
    mordor_u8 parameter_count;

    mordor_u16 variable_table_size;
    mordor_u16 pointer_table_size;
    mordor_u16 maximum_stack_size;
    mordor_u16 operation_count;

    Array<std::string, mordor_u16> function_name_table;

    // Constant table
    // Global var name table
    // Struct name table
    // Field name table
    // Function name table
    // Interface <-> Struct name table
    // Struct <-> Field name table

    mordor_u32 code_size;
    mordor_u8* code;


public:
    ~BytecodeFunction ();

    /*
     * Allocates the memory for the code. Also sets 'code_size'.
     */
    void AllocateCode (mordor_u32 _code_size);
};

}


#endif /* MORDOR_INTERNAL_BYTECODE_BYTECODEFUNCTION_H_ */
