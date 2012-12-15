#include <stdio.h>

#include <string>

#include <coin/utils/time.h>

#include <mordor/runtime/core.h>
#include <mordor/runtime/Environment.h>

using namespace std;


int main (int argc, char** argv) {
    if (argc < 2) {
        printf ("Error: You must provide a program that should be executed as well as a function to start the execution.\n");
        return 1;
    }

    coin::TimeInit ();

    /* Load Program in argv[1]. */
    mdrEnvironment* env = mdrEnvCreate ();
    mdrProgram* prog = mdrEnvLoadProgram (env, argv[1]);
    mdrEnvInitialize (env);

    mdrContext* ctx = mdrCtxCreate (env);

    /* Get add function. */
    mdrFunction* test_function = mdrEnvFindFunction (env, "test.add");

    /* Set Parameters and invoke add function. */
    mdr_s64* stack = (mdr_s64*) mdrCtxGetStack (ctx);
    stack[0] = 42;
    stack[1] = 42;

    mdrExecute (ctx, test_function, 0);

    printf ("The function returned '%u'\n", *((mdr_s64*) mdrCtxGetReturnValueAddress (ctx)));

    mdrEnvDestroy (env);
    mdrCtxDestroy (ctx);
}