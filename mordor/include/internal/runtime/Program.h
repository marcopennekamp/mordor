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
        /* 
         * Maps Function names to indices in the function_cache. 
         * Used to reference Functions before they are actually compiled.
         */
        std::map<std::string, mdr_u32> function_resolve_cache;

        /* 
         * Saves all BytecodeFunctions. 
         */
        std::vector<BytecodeFunction*> bytecode_function_cache;

        mdr_u32 next_function_id;
    };

    /*
     * Used to compile and link (linking with other Programs) the Program.
     */
    InitializerCache* cache_;

    /*
     * Holds all Functions owned by this Program.
     */
    std::map<std::string, mdr_u32> functions_;

    /*
     * Holds all Functions that are ever referenced in the Program's code. 
     * This includes functions from other Programs.
     */
    std::vector<Function*> function_cache_;


public:
    static const mdr_u32 INVALID_FUNCTION_ID = 0xFFFFFFFF;

    Program ();

    void Initialize (Environment* environment);
    void ResolveSymbols (Environment* environment);
    void AddBytecodeFunction (const std::string& name, BytecodeFunction* function);

    Function* GetFunctionFromCache (mdr_u32 index);
    mdr_u32 GetFunctionId (const std::string& name);

    mdr_u32 GetFunctionIdFromResolveCache (const std::string& name); /* Do NOT use this after calling ResolveSymbols! */

    inline std::vector<BytecodeFunction*>& bytecode_function_cache () { return cache_->bytecode_function_cache; }
};

}


#endif  /* MORDOR_INTERNAL_RUNTIME_PROGRAM_H_ */

