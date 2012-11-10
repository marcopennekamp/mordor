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
    Environment env;
    env.LoadProgram ("test.mordor");

    /* Interpreter tests. */
    Operation* operations;
    
    int length = 1e3 + 4;
    operations = (Operation*) calloc (sizeof (Operation) * length, 1);
    operations[0] = 0x0000000000000014;
    operations[1] = 0x0000000100020014;
    for (int i = 2; i < length - 2; ++i) {
        operations[i] = 0x0000000200000030;
    }
    operations[length - 2] = 0x0000000000000004;
    operations[length - 1] = 0x0000000000000000;
    Function* add_test_function = new Function ();
    add_test_function->stack_size = 64;
    add_test_function->operations = operations;
    
    operations = (Operation*) malloc (sizeof (Operation));
    operations[0] = 0x0000000000000000;
    Function* blank_test_function = new Function ();
    blank_test_function->stack_size = 8;
    blank_test_function->operations = operations;

    Program* program = new Program ();

    /* Load main.func. */
    {
        FileStream stream ("test/return.func", StreamMode::read);
        Function* function = (Function*) LoadFunction (&stream);
        string name = "return";
        program->AddFunction (name, function);
    }

    /* Load test.func. */
    {
        FileStream stream ("test/test.func", StreamMode::read);
        string name = "test";
        program->AddFunction (name, (Function*) LoadFunction (&stream));
    }

    Context context;
    context.stack_size = 1024 * 16;
    context.stack = new mordor_u8[context.stack_size];

    mordor_u8* retval_address;
    
    printf ("Invoking a function that adds 1000 times for 1000 times:\n");
    mordor_u64 time = coin::TimeNanoseconds ();
    for (int i = 0; i < 1000; ++i) {
        mordorInterpreterExecute (&context, (ProgramInterface*) program, add_test_function, 0, &retval_address);
    }
    time = coin::TimeNanoseconds () - time;
    printf ("successfully executed in %lluns with '%i'\n\n", time, *((int*) retval_address));

    printf ("Invoking a blank function 1000000 times:\n");
    time = coin::TimeNanoseconds ();
    for (int i = 0; i < 1e6; ++i) {
        mordorInterpreterExecute (&context, (ProgramInterface*) program, blank_test_function, 0, &retval_address);
    }
    time = coin::TimeNanoseconds () - time;
    printf ("successfully executed in %lluns\n", time);

    delete add_test_function;
    free (operations);

    return 0;
}
