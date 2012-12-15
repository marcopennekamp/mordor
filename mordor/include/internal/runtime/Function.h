#ifndef MORDOR_INTERNAL_RUNTIME_FUNCTION_H_
#define	MORDOR_INTERNAL_RUNTIME_FUNCTION_H_

#include <mordor/runtime/Operation.h>


namespace mordor {

class Function {
public:
    mdr_u32  stack_size;
    mdrOperation*  operations;
    Program* program;
};

}

#endif  /* MORDOR_INTERNAL_RUNTIME_FUNCTION_H_ */

