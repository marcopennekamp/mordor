#include <internal/runtime/Context.h>



namespace mordor {

Context::Context (const Environment* environment, const Program* program) {
    environment_ = environment;
    program_ = program;
    stack_.Create (1024 * 16);
}

}