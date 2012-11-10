#include <internal/bytecode/compile.h>
#include <internal/bytecode/BytecodeFunction.h>
#include <internal/runtime/Function.h>


namespace mordor {

Function* CompileBytecodeFunction (BytecodeFunction* func) {
    /* Dummy function. */
    Function* function = new Function ();
    function->stack_size = 8;
    Operation* operations = new Operation[1];
    operations[0] = 0x0000000000000000;

    return function;
}

}
