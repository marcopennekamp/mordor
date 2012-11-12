#ifndef MORDOR_INTERNAL_RUNTIME_PROGRAM_H_
#define	MORDOR_INTERNAL_RUNTIME_PROGRAM_H_

#include <vector>
#include <map>
#include <string>

#include <mordor/def.h>


namespace mordor {

class Function;
class BytecodeFunction;

class Program {
  private:
    std::map<std::string, mordor_u32> functions_;
    std::vector<Function*> function_cache_;

    std::vector<BytecodeFunction*>* bytecode_function_cache_;

  public:
    static const mordor_u32 INVALID_ID = 0xFFFFFFFF;

    Program ();

    void        Initialize ();

    void        AddBytecodeFunction (std::string& name, BytecodeFunction* function);

    Function*   GetFunctionFromCache (mordor_u32 index);
    mordor_u32  GetFunctionId (std::string& name);
};

}


#endif  /* MORDOR_INTERNAL_RUNTIME_PROGRAM_H_ */

