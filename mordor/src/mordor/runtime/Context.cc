#include <mordor/runtime/Context.h>


namespace mdr {

Context::Context (Environment* environment) {
    environment_ = environment;
    stack_.Create (1024 * 16);
    native_call_stack_.data.Create (4 * 8);
    native_call_stack_.size = 0;
}

}