#ifndef MDR_RUNTIME_FUNCTION_H_
#define	MDR_RUNTIME_FUNCTION_H_

#include <coin/utils/Array.h>

#include <mdr/def/Operation.h>
#include <mdr/def/Type.h>


namespace mdr {

class Function {
public:
    static const mdr_u32 kInvalidId = 0xFFFFFFFF;

    /*
     * Information for later compilation, since the compiler needs the return type and
     *   parameter count for any function.
     */
    struct CompilationInformation {
        mdrType return_type_;
        mdr_u8 parameter_count_;
    };

    typedef coin::Array<mdr_u64> ConstantTable;

private:
    mdr_u32 stack_size_;
    mdrOperation* operations_;

    /*
     * A constant table for 64 bit values.
     */
    ConstantTable constant_table_;

    CompilationInformation cpinfo_;

public:
    Function (CompilationInformation& cpinfo);
    ~Function ();

    void Allocate (size_t operations_size, size_t constant_table_size);

    inline mdr_u32 stack_size () const { return stack_size_; }
    inline void stack_size (mdr_u32 stack_size) { stack_size_ = stack_size; }

    inline mdrOperation* operations () { return operations_; }
    inline mdr_u8* constant_table () { return (mdr_u8*) constant_table_.array (); }
    inline ConstantTable& constant_table_object () { return constant_table_; }

    inline CompilationInformation& cpinfo () { return cpinfo_; }
};

}


#endif  /* MDR_RUNTIME_FUNCTION_H_ */

