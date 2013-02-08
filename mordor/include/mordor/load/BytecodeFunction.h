#ifndef MORDOR_BYTECODE_BYTECODEFUNCTION_H_
#define MORDOR_BYTECODE_BYTECODEFUNCTION_H_

#include <string>

#include <mordor/def/Mordor.h>
#include <mordor/def/Type.h>

#include <mordor/utils/Array.h>


namespace mdr {

class BytecodeFunction {
public:
    struct Constant {
        mdrType type;
        mdr_u64 value;
    };

    const static mdr_u8 kConstantTableExists    = 0x80;
    const static mdr_u8 kConstantTableWide      = 0x40;
    const static mdr_u8 kNameTableExists        = 0x20;
    const static mdr_u8 kNameTableWide          = 0x10;

    typedef Array<Constant, mdr_u16> ConstantTable;
    typedef Array<std::string, mdr_u16> NameTable;

private:
    mdrType return_type_;
    mdr_u8 parameter_count_;

    mdr_u16 variable_table_size_;
    mdr_u16 pointer_table_size_;
    mdr_u16 maximum_stack_size_;
    mdr_u16 operation_count_;

    ConstantTable constant_table_;
    NameTable name_table_;

    mdr_u32 code_size_;
    mdr_u8* code_;

public:
    BytecodeFunction (mdrType return_type, mdr_u8 parameter_count);
    ~BytecodeFunction ();

    /*
     * Allocates the memory for the code. Also sets 'code_size'.
     */
    void Allocate (mdr_u32 code_size);

    inline mdrType return_type () const { return return_type_; }
    inline mdr_u8 parameter_count () const { return parameter_count_; }

    inline mdr_u16 variable_table_size () const { return variable_table_size_; }
    inline void variable_table_size (mdr_u16 size) { variable_table_size_ = size; }

    inline mdr_u16 pointer_table_size () const { return pointer_table_size_; }
    inline void pointer_table_size (mdr_u16 size) { pointer_table_size_ = size; }

    inline mdr_u16 maximum_stack_size () const { return maximum_stack_size_; }
    inline void maximum_stack_size (mdr_u16 size) { maximum_stack_size_ = size; }

    inline mdr_u16 operation_count () const { return operation_count_; }
    inline void operation_count (mdr_u16 size) { operation_count_ = size; }

    inline ConstantTable& constant_table () { return constant_table_; }
    inline NameTable& name_table () { return name_table_; }

    inline mdr_u32 code_size () const { return code_size_; }
    inline mdr_u8* code () { return code_; }

};

}


#endif /* MORDOR_BYTECODE_BYTECODEFUNCTION_H_ */