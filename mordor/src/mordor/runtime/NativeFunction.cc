#include <internal/runtime/NativeFunction.h>


namespace mdr {

NativeFunction::NativeFunction (function_t function, mdrType return_type, mdr_u8 parameter_count, bool is_in_runtime_library) {
    function_ = function;
    return_type_ = return_type;
    parameter_count_ = parameter_count;
    is_in_runtime_library_ = is_in_runtime_library;
}

}