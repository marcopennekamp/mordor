#ifndef MORDOR_INTERNAL_RUNTIME_CONTEXT_H_
#define	MORDOR_INTERNAL_RUNTIME_CONTEXT_H_

#include <mordor/def.h>


namespace mordor {

class Context {
  public:
    void* stack;
    mordor_u32 stack_size;

};

}

#endif  /* MORDOR_INTERNAL_RUNTIME_CONTEXT_H_ */