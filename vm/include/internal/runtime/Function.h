#ifndef MORDOR_INTERNAL_RUNTIME_FUNCTION_H_
#define	MORDOR_INTERNAL_RUNTIME_FUNCTION_H_

#include <mordor/runtime/Operation.h>


namespace mordor {

class Function {
  public:

    mordor_u32  id;

    mordor_u32  stack_size;

    Operation*  operations;
};

}

#endif  /* MORDOR_INTERNAL_RUNTIME_FUNCTION_H_ */

