#ifndef MORDOR_INTERNAL_BYTECODE_BYTECODEFUNCTION_H_
#define MORDOR_INTERNAL_BYTECODE_BYTECODEFUNCTION_H_

#include <string>

#include <mordor/def.h>
#include <mordor/bytecode/Type.h>

#include <internal/utils/Array.h>


namespace mdr {

class BytecodeFunction {
public:
    struct Constant {
        mdrType type;
        mdr_u64 value;
    };

    const static mdr_u8 CONSTANT_TABLE_EXISTS   = 0x80;
    const static mdr_u8 CONSTANT_TABLE_WIDE     = 0x40;
    const static mdr_u8 NAME_TABLE_EXISTS       = 0x20;
    const static mdr_u8 NAME_TABLE_WIDE         = 0x10;

    mdrType return_type;
    mdr_u8 parameter_count;

    mdr_u16 variable_table_size;
    mdr_u16 pointer_table_size;
    mdr_u16 maximum_stack_size;
    mdr_u16 operation_count;

    Array<Constant, mdr_u16> constant_table;
    Array<std::string, mdr_u16> name_table;


    mdr_u32 code_size;
    mdr_u8* code;


public:
    ~BytecodeFunction ();

    /*
     * Allocates the memory for the code. Also sets 'code_size'.
     */
    void AllocateCode (mdr_u32 _code_size);
};

}


#endif /* MORDOR_INTERNAL_BYTECODE_BYTECODEFUNCTION_H_ */
