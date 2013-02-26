#include <mdr/api/Environment.h>
#include <mdr/runtime/Environment.h>

using namespace std;
using namespace mdr;


MDR_DECL mdrEnvironment* mdrEnvCreate () {
    return (mdrEnvironment*) new Environment ();
}

MDR_DECL void mdrEnvDestroy (mdrEnvironment* env) {
    delete (Environment*) env;
}

MDR_DECL void mdrEnvLoadProgram (mdrEnvironment* env, const char* path) {
    ((Environment*) env)->LoadProgram (path);
}

MDR_DECL mdrFunction* mdrEnvGetFunction (mdrEnvironment* env, const char* name) {
    return ((Environment*) env)->GetFunction (name);
}