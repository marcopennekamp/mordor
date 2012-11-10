#include <stddef.h>

#include <mordor/def.h>

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



    /* The Bytecode function cache has to be invalidated. */
    delete bytecode_function_cache_;
}


Function* Program::GetFunctionFromCache (mordor_u32 index) {
    return function_cache_[index];
}

}
