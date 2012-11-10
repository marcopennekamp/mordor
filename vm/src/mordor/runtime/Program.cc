#include <stddef.h>

#include <mordor/def.h>

#include <internal/runtime/Function.h>
#include <internal/runtime/Program.h>

using namespace std;


namespace mordor {

void Program::AddFunction (string& name, Function* function) {
    mordor_u32 id = (mordor_u32) function_cache_.size ();
    function_cache_.push_back (function);
    functions_[name] = id;
}

Function* Program::GetFunctionFromCache (mordor_u32 index) {
    return function_cache_[index];
}

}
