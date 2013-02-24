#ifndef MDR_RUNTIME_BYTECODEFUNCTION_H_
#define MDR_RUNTIME_BYTECODEFUNCTION_H_

#include <string>

#include <coin/utils/Array.h>

#include <mdr/def/Mdr.h>
#include <mdr/def/Type.h>
#include <mdr/runtime/Function.h>


namespace mdr {

class BytecodeFunction {
public:
    struct Constant {
        mdrType type;
        union {
            mdr_i8  _i8;
            mdr_i16 _i16;
            mdr_i32 _i32;
            mdr_i64 _i64;
            mdr_u8  _u8;
            mdr_u16 _u16;
            mdr_u32 _u32;
            mdr_u64 _u64;
            mdr_f32 _f32;
            mdr_f64 _f64;
        } value;
        
    };

    const static mdr_u8 kConstantTableExists    = 0x80;
    const static mdr_u8 kConstantTableWide      = 0x40;
    const static mdr_u8 kNameTableExists        = 0x20;
    const static mdr_u8 kNameTableWide          = 0x10;

    typedef coin::Array<Constant, mdr_u16> ConstantTable;
    typedef coin::Array<std::string, mdr_u16> NameTable;

private:
    std::string name_;

    /* Return type and parameter count are directly saved in the IR function type. */

    mdr_u16 variable_table_size_;
    mdr_u16 pointer_table_size_;
    mdr_u16 maximum_stack_size_;
    mdr_u16 operation_count_;

    ConstantTable constant_table_;
    NameTable name_table_;

    mdr_u32 code_size_;
    mdr_u8* code_;

public:
    BytecodeFunction (const std::string& name);
    ~BytecodeFunction ();

    /*
     * Allocates the memory for the code. Also sets 'code_size'.
     */
    void Allocate (mdr_u32 code_size);

    inline const std::string& name () const { return name_; }

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


#endif /* MDR_RUNTIME_BYTECODEFUNCTION_H_ */
