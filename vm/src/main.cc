#include <stdio.h>

#include <string>

#include <coin/utils/time.h>

#include <mdr/api/Environment.h>
#include <mdr/api/Context.h>
#include <mdr/api/Function.h>

using namespace std;


namespace {

bool PushFunctionParams (mdrFunction* func, mdrContext* ctx, int argc, char** argv) {
    size_t parameter_count = mdrFuncGetParamCount (func);
    if (parameter_count > argc - 3) {
        printf ("Error: Not enough parameters provided to call the main function!\n");
        return false;
    }

    /* Convert and push all parameters. */
    mdrType* param_types = mdrFuncGetParamList (func);
    for (size_t i = 0; i < parameter_count; ++i) {
        char* str = argv[i + 3];
        switch (param_types[i]) {
        case MDR_TYPE_I32:
            mdrCtxPushI32 (ctx, (mdr_i32) atoi (str));
            break;
        case MDR_TYPE_I64:
            mdrCtxPushI64 (ctx, (mdr_i64) _strtoi64 (str, NULL, 10));
            break;
        case MDR_TYPE_U32:
            mdrCtxPushU32 (ctx, (mdr_u32) atoi (str));
            break;
        case MDR_TYPE_U64:
            mdrCtxPushU64 (ctx, (mdr_u64) _strtoui64 (str, NULL, 10));
            break;
        case MDR_TYPE_F32:
            mdrCtxPushF32 (ctx, (mdr_f32) strtod (str, NULL));
            break;
        case MDR_TYPE_F64:
            mdrCtxPushF64 (ctx, (mdr_f64) strtod (str, NULL));
            break;
        default:
            printf ("Error: Parameter type of parameter '%u' is not supported for command line arguments (yet)!\n", i);
            return false;
        }
    }

    return true;
}

}

int main (int argc, char** argv) {
    if (argc < 3) {
        printf ("Error: You must provide a program that should be executed as well as a function to start the execution!\n");
        return 1;
    }

    coin::TimeInit ();

    /* Load Program in argv[1]. */
    mdrEnvironment* env = mdrEnvCreate ();
    mdrEnvLoadProgram (env, argv[1]);
    mdrContext* ctx = mdrCtxCreate (env);

    /* Get function. */
    mdrFunction* main_function = mdrEnvGetFunction (env, argv[2]);

    if (!PushFunctionParams (main_function, ctx, argc, argv)) {
        return 1;
    }

    mdrCtxPushU64 (ctx, 16);
    mdrCtxPushU64 (ctx, 6);

    mdr_u64 time = coin::TimeNanoseconds ();
    mdrCtxExecute (ctx, main_function);
    printf ("Execution took %lluns\n", coin::TimeNanoseconds () - time);
    printf ("The function returned '%llu'\n", *((mdr_u64*) mdrCtxGetReturnValueAddress (ctx)));

    mdrCtxDestroy (ctx);
    mdrEnvDestroy (env);
}