#include <mordor/api/Environment.h>

#include <mordor/runtime/Environment.h>

using namespace mdr;


extern "C" {

MDR_DECL mdrEnvironment* mdrEnvCreate () {
    return (mdrEnvironment*) new Environment ();
}

MDR_DECL void mdrEnvDestroy (mdrEnvironment* env) {
    delete (Environment*) env;
}

MDR_DECL void mdrEnvInitialize (mdrEnvironment* env) {
    ((Environment*) env)->Initialize ();
}

MDR_DECL mdrProgram* mdrEnvLoadProgram (mdrEnvironment* env, const char* path) {
    return (mdrProgram*) ((Environment*) env)->LoadProgram (path);
}

MDR_DECL mdrFunction* mdrEnvFindFunction (mdrEnvironment* env, const char* name) {
    return (mdrFunction*) ((Environment*) env)->FindFunction (name);
}

}