#include <coin/utils/Stream.h>

#include <internal/bytecode/load.h>
#include <internal/bytecode/BytecodeFunction.h>

using namespace coin;


namespace mordor {

BytecodeFunction* LoadBytecodeFunction (coin::Stream* stream) {
    BytecodeFunction* function = new BytecodeFunction ();

    /* Read options. */
    stream->Read ((u8*) &function->variable_table_size, sizeof (function->variable_table_size));
    stream->Read ((u8*) &function->pointer_table_size, sizeof (function->pointer_table_size));
    stream->Read ((u8*) &function->maximum_stack_size, sizeof (function->maximum_stack_size));
    stream->Read ((u8*) &function->operation_count, sizeof (function->operation_count));

    /* Calculate code size. */
    function->code_size = stream->Size () - stream->Position ();

    /* Read code. */
    function->code = new mordor_u8 [function->code_size];
    stream->Read (function->code, function->code_size);

    return function;
}

}
