#include <mdr/runtime/Context.h>


namespace mdr {

Context::Context (Environment* environment) {
    environment_ = environment;
    stack_.Create (1024 * 16);
    native_call_stack_.data.Create (4 * 8);
    native_call_stack_.size = 0;
    push_stack_top_ = 0;
}

void Context::Push (mdr_u16 offset) {
    push_stack_top_ += offset;
}

}