#ifndef MORDOR_INTERNAL_RUNTIME_FUNCTION_H_
#define	MORDOR_INTERNAL_RUNTIME_FUNCTION_H_

#include <mordor/runtime/Operation.h>


namespace mordor {

class MORDOR_DECL Function {
public:

    mordor_u32  stack_size;

    Operation*  operations;
};

}

#endif  /* MORDOR_INTERNAL_RUNTIME_FUNCTION_H_ */

