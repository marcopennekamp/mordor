#include <stddef.h>

#include <mordor/def.h>

#include <internal/bytecode/compile.h>
#include <internal/bytecode/BytecodeFunction.h>
#include <internal/runtime/Function.h>
#include <internal/runtime/Program.h>

using namespace std;


namespace mordor {

Program::Program () {
    bytecode_function_cache_ = new vector<BytecodeFunction*> ();
}


void Program::AddBytecodeFunction (string& name, BytecodeFunction* function) {
    mordor_u32 id = (mordor_u32) bytecode_function_cache_->size ();
    bytecode_function_cache_->push_back (function);
    functions_[name] = id;
}


void Program::Initialize () {
    /* Compile Bytecode functions. */
    size_t size = bytecode_function_cache_->size ();
    function_cache_.reserve (size);
    for (size_t i = 0; i < size; ++i) {
        BytecodeFunction* bc_func = (*bytecode_function_cache_)[i];
        Function* function = CompileBytecodeFunction (bc_func);
        delete bc_func; /* Deletes the BytecodeFunction. */
        if (function == NULL) {
            printf ("Error occured while compiling!");
            continue;
        }
        function_cache_.push_back (function);
    }

    /* The Bytecode function cache has to be invalidated. */
    delete bytecode_function_cache_;
    bytecode_function_cache_ = NULL;
}


Function* Program::GetFunctionFromCache (mordor_u32 index) {
    return function_cache_[index];
}

mordor_u32 Program::GetFunctionId (string& name) {
    auto it = functions_.find (name);
    if (it != functions_.end ()) {
        return it->second;
    }
    return INVALID_ID;
}

}
