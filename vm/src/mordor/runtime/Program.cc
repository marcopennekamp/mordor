#include <internal/runtime/Program.h>

#include <stddef.h>

#include <mordor/def.h>

#include <internal/runtime/Function.h>



namespace mordor {

void Program::AddFunction (Function* function) {
    mordor_u32 id = function->id;
    if (function_list_.size () <= id) {
        function_list_.resize (id + 1, NULL); /* TODO(Marco): Inefficient! */
    }
    function_list_[id] = function;
}

Function* Program::GetFunction (mordor_u32 index) {
    return function_list_[index];
}

}
