#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <coin/utils/time.h>
#include <coin/utils/Stream.h>

#include <mordor/interpreter/core.h>
#include <mordor/runtime/loader.h>

#include <internal/runtime/Context.h>
#include <internal/runtime/Function.h>
#include <internal/runtime/Program.h>
#include <internal/runtime/LibraryManager.h>
#include <internal/runtime/Library.h>
#include <internal/runtime/Environment.h>

using namespace coin;
using namespace mordor;
using namespace std;


int main () {
    coin::TimeInit ();

    /* Load Program 'test'. */
    mordor_u64 time = coin::TimeNanoseconds ();
    Environment env;
    Program* main_prog = env.LoadProgram ("test.mordor");
    env.Initialize ();
    time = coin::TimeNanoseconds () - time;
    printf ("Initialization took %lluns.\n", time);

    string func_name = "main";
    Function* main_func = env.FindFunction (func_name);

    /* Interpreter tests. */
    Operation* operations;
    
    int length = 1000 + 6;
    operations = (Operation*) calloc (sizeof (Operation) * length, 1);
    operations[0] = 0x0000000000000014; /* low. */
    operations[1] = 0x0000000000040014; /* high. */
    operations[2] = 0x0000000100080014; /* low. */
    operations[3] = 0x00000000000C0014; /* high. */
    for (int i = 4; i < length - 2; ++i) {
        operations[i] = 0x0000000800000031;
    }
    operations[length - 2] = 0x000000000000000C;
    operations[length - 1] = 0x0000000000000000;
    Function* add_test_function = new Function ();
    add_test_function->stack_size = 64;
    add_test_function->operations = operations;
    
    operations = (Operation*) malloc (sizeof (Operation));
    operations[0] = 0x0000000000000000;
    Function* blank_test_function = new Function ();
    blank_test_function->stack_size = 8;
    blank_test_function->operations = operations;

    Context context (&env, main_prog);

    printf ("Invoking a function that adds 1000 times for 1000 times:\n");
    time = coin::TimeNanoseconds ();
    for (int i = 0; i < 1000; ++i) {
        mordorInterpreterExecute (&context, add_test_function, 0);
    }
    time = coin::TimeNanoseconds () - time;
    printf ("successfully executed in %lluns with '%lli'\n\n", time, *((mordor_u64*) context.return_value_address ()));

    printf ("Invoking a blank function 1000000 times:\n");
    time = coin::TimeNanoseconds ();
    for (int i = 0; i < 1000000; ++i) {
        mordorInterpreterExecute (&context, blank_test_function, 0);
    }
    time = coin::TimeNanoseconds () - time;
    printf ("successfully executed in %lluns\n\n", time);

    printf ("Invoking the main function supplied:\n");
    mordorInterpreterExecute (&context, main_func, 0);
    printf ("It returned '%u'!\n", *((mordor_u32*) context.return_value_address ()));

    delete add_test_function;
    free (operations);

    return 0;
}
