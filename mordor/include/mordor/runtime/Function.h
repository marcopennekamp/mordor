#ifndef MORDOR_RUNTIME_FUNCTION_H_
#define	MORDOR_RUNTIME_FUNCTION_H_

#include <mordor/def/Operation.h>
#include <mordor/runtime/Program.h>


namespace mdr {

class Function {
public:
    static const mdr_u32 kInvalidId = 0xFFFFFFFF;

private:
    Program* program_;

    mdr_u32  stack_size_;
    mdrOperation* operations_;

    /*
     * A constant table for 64 bit values.
     */
    mdr_u8* constant_table_;

public:
    Function (size_t operations_size, size_t constant_table_size);
    ~Function ();

    inline Program* program () { return program_; }

    inline mdr_u32 stack_size () const { return stack_size_; }
    inline mdrOperation* operations () { return operations_; }
    inline mdr_u8* constant_table () { return constant_table_; }
};

}


#endif  /* MORDOR_RUNTIME_FUNCTION_H_ */

