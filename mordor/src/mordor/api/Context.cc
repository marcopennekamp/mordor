#include <mordor/api/Context.h>
#include <mordor/runtime/Context.h>

using namespace mdr;


extern "C" {

MDR_DECL mdrContext* mdrCtxCreate (mdrEnvironment* env) {
    return (mdrContext*) new Context ((Environment*) env);
}

MDR_DECL void mdrCtxDestroy (mdrContext* ctx) {
    delete (Context*) ctx;
}

MDR_DECL void mdrCtxExecute (mdrContext* ctx, mdrFunction* func) {
    ((Context*) ctx)->Execute ((Function*) func, 0);
}

MDR_DECL mdr_u8* mdrCtxGetStack (mdrContext* ctx) {
    return ((Context*) ctx)->stack ().array ();
}

MDR_DECL mdr_u8* mdrCtxGetReturnValueAddress (mdrContext* ctx) {
    return ((Context*) ctx)->return_value_address ();
}

}