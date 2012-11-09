#ifndef MORDOR_INTERNAL_RUNTIME_PROGRAM_H_
#define	MORDOR_INTERNAL_RUNTIME_PROGRAM_H_

#include <vector>

#include <mordor/def.h>


namespace mordor {

class Function;

class Program {
  private:
    std::vector<Function*> function_list_;

  public:
    void        AddFunction (Function* function);
    Function*   GetFunction (mordor_u32 index);
};

}





#endif  /* MORDOR_INTERNAL_RUNTIME_PROGRAM_H_ */

