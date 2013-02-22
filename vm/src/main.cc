#include <stdio.h>

#include <string>

#include <coin/utils/time.h>

#include <mdr/api/Environment.h>
#include <mdr/api/Context.h>
#include <mdr/api/Function.h>

using namespace std;


int main (int argc, char** argv) {
    if (argc < 2) {
        printf ("Error: You must provide a program that should be executed as well as a function to start the execution.\n");
        return 1;
    }

    coin::TimeInit ();

    /* Load Program in argv[1]. */
    mdrEnvironment* env = mdrEnvCreate ();
    mdrEnvLoadProgram (env, argv[1]);
    mdrContext* ctx = mdrCtxCreate (env);

    /* Get add function. */
    mdrFunction* test_function = mdrEnvGetFunction (env, "test.add_int");
    mdrFunction* call = mdrEnvGetFunction (env, "test.call");
    mdrFunction* call_native = mdrEnvGetFunction (env, "test.call_native");

    /* Set Parameters and invoke add function. */
    mdr_s32* stack = (mdr_s32*) mdrCtxGetStack (ctx);
    stack[0] = 42;
    stack[1] = 42;

    mdr_u64 time = coin::TimeNanoseconds ();

    //mdrCtxExecute (ctx, test_function);

    printf ("Execution took %lluns\n", coin::TimeNanoseconds () - time);
    printf ("The function returned '%i'\n", *((mdr_s32*) mdrCtxGetReturnValueAddress (ctx)));

    stack = (mdr_s32*) mdrCtxGetStack (ctx);
    stack[0] = 53252;
    stack[1] = 23242;

    //mdrCtxExecute (ctx, call);
    mdrCtxExecute (ctx, call_native);
    printf ("The function returned '%i'\n", *((mdr_s32*) mdrCtxGetReturnValueAddress (ctx)));

    mdrCtxDestroy (ctx);
    mdrEnvDestroy (env);
}