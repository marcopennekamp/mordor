#include <stddef.h>

#include <mordor/def.h>
#include <mordor/runtime/Program.h>

#include <internal/bytecode/compile.h>
#include <internal/bytecode/BytecodeFunction.h>
#include <internal/runtime/Environment.h>
#include <internal/runtime/Function.h>
#include <internal/runtime/Program.h>

using namespace std;


namespace mdr {

Program::Program () {
    cache_ = new InitializerCache ();
}

Program::~Program () {
    for (size_t i = 0; i < owned_functions_size_; ++i) {
        delete function_cache_[i];
    }
}


void Program::AddBytecodeFunction (const string& name, BytecodeFunction* function) {
    mdr_u32 id = (mdr_u32) cache_->bytecode_function_cache.size ();
    cache_->bytecode_function_cache.push_back (function);
    functions_[name] = id;
}


void Program::Initialize (Environment* environment) {
    /* Initialize native functions. */
    {

    }

    /* Compile Bytecode functions. */
    {
        owned_functions_size_ = (mdr_u32) cache_->bytecode_function_cache.size ();
        cache_->next_function_id = owned_functions_size_; /* Set the next available ID to the value after the last already known function. */
        function_cache_.resize (owned_functions_size_);
        for (size_t i = 0; i < owned_functions_size_; ++i) {
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


Function* Program::GetFunctionFromCache (mdr_u32 index) {
    return function_cache_[index];
}

mdr_u32 Program::GetFunctionId (const string& name) {
    auto it = functions_.find (name);
    if (it != functions_.end ()) {
        return it->second;
    }
    return INVALID_FUNCTION_ID;
}

mdr_u32 Program::GetFunctionIdFromResolveCache (const std::string& name) {
    auto it = cache_->function_resolve_cache.find (name);
    if (it != cache_->function_resolve_cache.end ()) { /* Already cached. */
        return it->second;
    }

    /* Otherwise: Add to cache. */
    mdr_u32 id = cache_->next_function_id++;
    cache_->function_resolve_cache[name] = id;
    
    return id;
}

}
