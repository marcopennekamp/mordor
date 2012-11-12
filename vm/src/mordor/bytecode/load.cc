#include <coin/utils/Stream.h>

#include <internal/bytecode/load.h>
#include <internal/bytecode/BytecodeFunction.h>

using namespace coin;


namespace mordor {

BytecodeFunction* LoadBytecodeFunction (coin::Stream* stream) {
    BytecodeFunction* function = new BytecodeFunction ();

    /* Read options. */
    stream->ReadU32 (function->variable_table_size);
    stream->ReadU32 (function->pointer_table_size);
    stream->ReadU32 (function->maximum_stack_size);
    stream->ReadU32 (function->operation_count);

    /* Calculate code size. */
    function->code_size = stream->Size () - stream->Position ();

    /* Read code. */
    function->code = new mordor_u8 [function->code_size];
    stream->Read (function->code, function->code_size);

    return function;
}

}
