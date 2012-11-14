#include <stddef.h>

#include <mordor/def.h>

#include <internal/bytecode/compile.h>
#include <internal/bytecode/BytecodeFunction.h>
#include <internal/runtime/Environment.h>
#include <internal/runtime/Function.h>
#include <internal/runtime/Program.h>

using namespace std;


namespace mordor {

Program::Program () {
    cache_ = new InitializerCache ();
}


void Program::AddBytecodeFunction (string& name, BytecodeFunction* function) {
    mordor_u32 id = (mordor_u32) cache_->bytecode_function_cache.size ();
    cache_->bytecode_function_cache.push_back (function);
    functions_[name] = id;
}


void Program::Initialize (Environment* environment) {
    /* Compile Bytecode functions. */
    {
        size_t size = cache_->bytecode_function_cache.size ();
        cache_->next_function_id = (mordor_u32) size;
        function_cache_.resize (size);
        for (size_t i = 0; i < size; ++i) {
            BytecodeFunction* bc_func = cache_->bytecode_function_cache[i];
            Function* function = CompileBytecodeFunction (bc_func, environment, this);
            if (function == NULL) {
                printf ("Error occured while compiling!\n");
                continue;
            }
            function_cache_[i] = function;
        }
    }

    /* The BytecodeFunctions have to be deleted. */
    {
        size_t size = cache_->bytecode_function_cache.size ();
        for (size_t i = 0; i < size; ++i) {
            delete cache_->bytecode_function_cache[i]; 
        }
    }
}


void Program::ResolveSymbols (Environment* environment) {
    /* Resolve pending functions in the function resolve cache. */
    {
        function_cache_.resize (function_cache_.size () + cache_->function_resolve_cache.size ());
        auto it = cache_->function_resolve_cache.begin ();
        for (; it != cache_->function_resolve_cache.end (); ++it) {
            Function* function = environment->FindFunction ((string&) it->first);
            if (function == NULL) {
                printf ("Function '%s' could not be found!\n", it->first.c_str ());
                continue;
            }
            function_cache_[it->second] = function;
        }
    }

    /* Delete and invalidate cache. */
    delete cache_;
    cache_ = NULL;
}


Function* Program::GetFunctionFromCache (mordor_u32 index) {
    return function_cache_[index];
}

mordor_u32 Program::GetFunctionId (string& name) {
    auto it = functions_.find (name);
    if (it != functions_.end ()) {
        return it->second;
    }
    return INVALID_FUNCTION_ID;
}

mordor_u32 Program::GetFunctionIdFromResolveCache (std::string& name) {
    auto it = cache_->function_resolve_cache.find (name);
    if (it != cache_->function_resolve_cache.end ()) { /* Already cached. */
        return it->second;
    }

    /* Otherwise: Cache. */
    mordor_u32 id = cache_->next_function_id++;
    cache_->function_resolve_cache[name] = id;
    
    return id;
}

}
