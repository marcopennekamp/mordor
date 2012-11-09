#include <coin/utils/Stream.h>

#include <internal/bytecode/load.h>
#include <internal/bytecode/BytecodeFunction.h>

using namespace coin;


namespace mordor {

BytecodeFunction* LoadBytecodeFunction (coin::Stream* stream) {
    BytecodeFunction* function = new BytecodeFunction ();

    /* Read variable and pointer table size. */
    stream->ReadBuffer ((mordor_u8*) &function->variable_table_size, sizeof (function->variable_table_size));
    stream->ReadBuffer ((mordor_u8*) &function->pointer_table_size, sizeof (function->pointer_table_size));

    /* Calculate code size. */
    function->code_size = stream->FileSize () - stream->Position ();

    /* Read code. */
    function->code = new mordor_u8 [function->code_size];
    stream->ReadBuffer (function->code, function->code_size);

    return function;
}

}
