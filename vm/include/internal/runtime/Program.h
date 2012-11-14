#ifndef MORDOR_INTERNAL_RUNTIME_PROGRAM_H_
#define	MORDOR_INTERNAL_RUNTIME_PROGRAM_H_

#include <vector>
#include <map>
#include <string>

#include <mordor/def.h>


namespace mordor {

class Function;
class BytecodeFunction;
class Environment;

class Program {
  private:
    struct InitializerCache {
        std::map<std::string, mordor_u32> function_resolve_cache;
        std::vector<BytecodeFunction*> bytecode_function_cache;
        mordor_u32 next_function_id;
    };

    std::map<std::string, mordor_u32> functions_;
    std::vector<Function*> function_cache_;

    InitializerCache* cache_;

  public:
    static const mordor_u32 INVALID_FUNCTION_ID = 0xFFFFFFFF;

    Program ();

    void        Initialize (Environment* environment);
    void        ResolveSymbols (Environment* environment);
    void        AddBytecodeFunction (std::string& name, BytecodeFunction* function);

    Function*   GetFunctionFromCache (mordor_u32 index);
    mordor_u32  GetFunctionId (std::string& name);

    mordor_u32  GetFunctionIdFromResolveCache (std::string& name); /* Do NOT use this after calling ResolveSymbols! */

    inline std::vector<BytecodeFunction*>& bytecode_function_cache () { return cache_->bytecode_function_cache; }
};

}


#endif  /* MORDOR_INTERNAL_RUNTIME_PROGRAM_H_ */

