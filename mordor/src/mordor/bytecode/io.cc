#include <coin/utils/Stream.h>

#include <internal/bytecode/io.h>
#include <internal/bytecode/BytecodeFunction.h>

using namespace coin;


namespace mordor {

BytecodeFunction* LoadBytecodeFunction (coin::Stream* stream) {
    BytecodeFunction* function = new BytecodeFunction ();

    /* General stuff. */
    mordor_u8 exist_flags;
    stream->ReadU8 (exist_flags);
    stream->ReadU8 (function->parameter_count);
    stream->ReadU8 (function->return_type);

    /* Read options. */
    stream->ReadU16 (function->variable_table_size);
    stream->ReadU16 (function->pointer_table_size);
    stream->ReadU16 (function->maximum_stack_size);
    stream->ReadU16 (function->operation_count);

    /* Read function name table. */
    if (exist_flags & BytecodeFunction::EXISTS_FUNCTION_NAME_TABLE) {
        mordor_u16 size;
        stream->ReadU16 (size);
        function->function_name_table.Create (size);
        for (mordor_u16 i = 0; i < size; ++i) {
            stream->ReadString (function->function_name_table[i]);
        }
    }

    /* Set code size and read code. */
    function->AllocateCode (stream->Size () - stream->Position ());
    stream->Read (function->code, function->code_size);

    return function;
}

}
