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

    /* TODO(Marco): Read parameter types. */
    stream->ReadU8 (function->parameter_count);
    stream->ReadU8 (function->return_type);

    /* Read function name table. */
    {
        mordor_u16 size;
        stream->ReadU16 (size);
        function->function_name_table_size = size;
        function->function_name_table.Create (size);
        for (size_t i = 0; i < size; ++i) {
            stream->ReadString (function->function_name_table[i]);
        }
    }

    /* Calculate code size. */
    function->code_size = stream->Size () - stream->Position ();

    /* Read code. */
    function->code = new mordor_u8 [function->code_size];
    stream->Read (function->code, function->code_size);

    return function;
}

}
