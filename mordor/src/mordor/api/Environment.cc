#include <mordor/api/Environment.h>

#include <mordor/runtime/Environment.h>

using namespace std;
using namespace mdr;


extern "C" {

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

}