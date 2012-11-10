#ifndef MORDOR_INTERNAL_RUNTIME_PROGRAM_H_
#define	MORDOR_INTERNAL_RUNTIME_PROGRAM_H_

#include <vector>
#include <map>
#include <string>

#include <mordor/def.h>


namespace mordor {

class Function;

class Program {
  private:
    std::map<std::string, mordor_u32> functions_;
    std::vector<Function*> function_cache_;

  public:
    void        AddFunction (std::string& name, Function* function);
    Function*   GetFunctionFromCache (mordor_u32 index);
    mordor_u32  GetFunctionId (std::string& name);
};

}


#endif  /* MORDOR_INTERNAL_RUNTIME_PROGRAM_H_ */

