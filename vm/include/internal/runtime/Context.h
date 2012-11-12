#ifndef MORDOR_INTERNAL_RUNTIME_CONTEXT_H_
#define	MORDOR_INTERNAL_RUNTIME_CONTEXT_H_

#include <mordor/def.h>

#include <internal/utils/Array.h>


namespace mordor {

class Environment;
class Program;

class Context {
  private:
    const Environment* environment_;
    const Program* program_;
    Array<mordor_u8> stack_;
    mordor_u8* return_value_address_;

  public:
    Context (const Environment* environment, const Program* program);

    inline const Environment* environment () { return environment_; }
    inline const Program* program () { return program_; }
    inline Array<mordor_u8>& stack () { return stack_; }
    inline mordor_u8*& return_value_address () { return return_value_address_; }
};

}


#endif  /* MORDOR_INTERNAL_RUNTIME_CONTEXT_H_ */