#include <stdio.h>

#include <string>

#include <coin/utils/time.h>

#include <mdr/api/Environment.h>
#include <mdr/api/Context.h>
#include <mdr/api/Function.h>

using namespace std;


int main (int argc, char** argv) {
    if (argc < 3) {
        printf ("Error: You must provide a program that should be executed as well as a function to start the execution.\n");
        return 1;
    }

    coin::TimeInit ();

    /* Load Program in argv[1]. */
    mdrEnvironment* env = mdrEnvCreate ();
    mdrEnvLoadProgram (env, argv[1]);
    mdrContext* ctx = mdrCtxCreate (env);

    /* Get function. */
    mdrFunction* main_function = mdrEnvGetFunction (env, argv[2]);

    mdr_u64 time = coin::TimeNanoseconds ();
    mdrCtxExecute (ctx, main_function);
    printf ("Execution took %lluns\n", coin::TimeNanoseconds () - time);
    printf ("The function returned '%llu'\n", *((mdr_u64*) mdrCtxGetReturnValueAddress (ctx)));

    mdrCtxDestroy (ctx);
    mdrEnvDestroy (env);
}