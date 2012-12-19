#include <mordor/runtime/Context.h>

#include <internal/runtime/Context.h>

using namespace mordor;


extern "C" {

MDR_DECL mdrContext* mdrCtxCreate (const mdrEnvironment* env) {
    return (mdrContext*) new Context ((const Environment*) env);
}

MDR_DECL void mdrCtxDestroy (mdrContext* ctx) {
    delete (Context*) ctx;
}

MDR_DECL mdr_u8* mdrCtxGetStack (mdrContext* ctx) {
    return ((Context*) ctx)->stack ().array ();
}

MDR_DECL mdr_u8* mdrCtxGetReturnValueAddress (mdrContext* ctx) {
    return ((Context*) ctx)->return_value_address ();
}

}


namespace mordor {

Context::Context (const Environment* environment) {
    environment_ = environment;
    stack_.Create (1024 * 16);
    native_call_stack_.Create (4 * 8);
    native_stack_size_ = 0;
}

}