#ifndef MORDOR_INTERNAL_RUNTIME_FUNCTION_H_
#define	MORDOR_INTERNAL_RUNTIME_FUNCTION_H_

#include <mordor/runtime/Operation.h>

#include <internal/runtime/Program.h>


namespace mdr {

class Function {
public:
    Program* program;

    mdr_u32  stack_size;
    mdrOperation* operations;

    /*
     * A constant table for 64 bit values.
     */
    mdr_u8* constant_table;

    Function (size_t operations_size, size_t constant_table_size);
    ~Function ();
};

}


#endif  /* MORDOR_INTERNAL_RUNTIME_FUNCTION_H_ */

