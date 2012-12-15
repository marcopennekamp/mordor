#include <stdio.h>

#include <coin/utils/time.h>

#include <mordor/interpreter/core.h>

#include <internal/runtime/Environment.h>
#include <internal/runtime/Context.h>
#include <internal/bytecode/BytecodeFunction.h>
#include <internal/utils/Array.h>


using namespace mordor;
using namespace std;


int main (int argc, char** argv) {
    if (argc < 2) {
        printf ("Error: You must provide a program that should be executed as well as a function to start the execution.\n");
        return 1;
    }

    coin::TimeInit ();

    /* Load Program in argv[1]. */
    Environment env;
    Program* program = env.LoadProgram (argv[1]);
    env.Initialize ();

    Context context (&env, program);

    /* Get add function. */
    string func_name = "test.add";
    Function* test_function = env.FindFunction (func_name);

    /* Set Parameters and invoke add function. */
    mordor_s64* stack = (mordor_s64*) context.stack ().array ();
    stack[0] = 42;
    stack[1] = 42;

    mordorInterpreterExecute (&context, test_function, 0);

    printf ("The function returned '%u'\n", *((mordor_s64*) context.return_value_address ()));
}