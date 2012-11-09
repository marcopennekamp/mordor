#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <windows.h>

#include <coin/utils/time.h>
#include <coin/utils/Stream.h>

#include <mordor/interpreter/core.h>
#include <mordor/runtime/loader.h>

#include <internal/runtime/Context.h>
#include <internal/runtime/Function.h>
#include <internal/runtime/Program.h>

using namespace coin;
using namespace mordor;


int main () {
    coin::TimeInit ();

    /* Load TestDLL. */
    HMODULE test_library = LoadLibrary("testdll.dll");
    if (test_library == NULL) {
        printf ("Error while loading \"test.dll\"!\n");
    } else {
        FARPROC initializer = GetProcAddress (test_library, "test");
        if(initializer == NULL) {
            printf ("Function could not be loaded!");
        } else {
            typedef void* (*func_type) (void);
            func_type function = (func_type) initializer;
            printf ("Function returned: %llX\n", (u64) function ());
        }
        FreeLibrary (test_library);
    }

    /* Linux:
    cpu_set_t cpu_set;
    CPU_ZERO(&cpu_set);
    CPU_SET_S (0, sizeof (cpu_set_t), &cpu_set); /* run on ONE CPU. */
    /* sched_setaffinity (0, sizeof (cpu_set_t), &cpu_set);
    */

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
        Stream stream ("test/return.func", StreamMode::read);
        Function* function = (Function*) LoadFunction (&stream);
        program->AddFunction (function);
    }

    /* Load test.func. */
    {
        Stream stream ("test/test.func", StreamMode::read);
        program->AddFunction ((Function*) LoadFunction (&stream));
    }

    Context context;
    context.stack = new mordor_u8[1024 * 16];
    context.stack_size = 1024 * 16;

    mordor_u8* retval_address;
    
    printf ("Invoking a function that adds 1000 times for 1000 times:\n");
    mordor_u64 time = coin::TimeNanoseconds ();
    for (int i = 0; i < 1000; ++i) {
        mordorInterpreterExecute (&context, (ProgramInterface*) program, add_test_function, 0, &retval_address);
    }
    time = coin::TimeNanoseconds () - time;
    printf ("successfully executed in %lluns\n\n", time);

    printf ("Invoking a blank function 1000000 times:\n");
    time = coin::TimeNanoseconds ();
    for (int i = 0; i < 1e6; ++i) {
        mordorInterpreterExecute (&context, (ProgramInterface*) program, blank_test_function, 0, &retval_address);
    }
    time = coin::TimeNanoseconds () - time;
    printf ("successfully executed in %lluns\n", time);

    free (add_test_function);
    free (operations);

    return 0;
}
