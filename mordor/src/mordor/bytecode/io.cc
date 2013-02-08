#include <coin/utils/Stream.h>

#include <internal/bytecode/io.h>
#include <internal/bytecode/BytecodeFunction.h>
#include <internal/runtime/NativeFunction.h>

using namespace std;
using namespace coin;


namespace mdr {

BytecodeFunction* LoadBytecodeFunction (coin::Stream* stream) {
    BytecodeFunction* function = new BytecodeFunction ();

    mdr_u8 flags;
    stream->ReadU8 (flags);
    function->return_type = flags & 0x0F;

    stream->ReadU8 (function->parameter_count);

    mdr_u32 options; /* Consult the bytecode function file format! */
    stream->ReadU32 (options);
    function->maximum_stack_size = options & 0x3FF;
    function->pointer_table_size = (options & 0x1FFC00) >> 10;
    function->variable_table_size = (options & 0xFFE00000) >> 21;

    stream->ReadU16 (function->operation_count);

    /* Read constant table. */
    if (flags & BytecodeFunction::CONSTANT_TABLE_EXISTS) {
        mdr_u16 size;
        if (flags & BytecodeFunction::CONSTANT_TABLE_WIDE) {
            stream->ReadU16 (size);
        }else {
            mdr_u8 _size;
            stream->ReadU8 (_size);
            size = _size;
        }

        function->constant_table.Create (size);
        for (mdr_u16 i = 0; i < size; ++i) {
            mdrType type;
            stream->ReadU8 (type);
            BytecodeFunction::Constant& constant = function->constant_table[i];
            constant.type = type;
            switch (mdrTypeGetSize (type)) {
            case 8: {
                mdr_u8 value;
                stream->ReadU8 (value);
                constant.value = value;
                break;
            }

            case 16: {
                mdr_u16 value;
                stream->ReadU16 (value);
                constant.value = value;
                break;
            }

            case 32: {
                mdr_u32 value;
                stream->ReadU32 (value);
                constant.value = value;
                break;
            }

            case 64: {
                mdr_u64 value;
                stream->ReadU64 (value);
                constant.value = value;
                break;
            }

            default:
                printf ("Error: Invalid constant size!");
                return NULL;
            }
        }
    }

    /* Read name table. */
    if (flags & BytecodeFunction::NAME_TABLE_EXISTS) {
        mdr_u16 size;
        if (flags & BytecodeFunction::NAME_TABLE_WIDE) {
            stream->ReadU16 (size);
        }else {
            mdr_u8 _size;
            stream->ReadU8 (_size);
            size = _size;
        }

        function->name_table.Create (size);
        for (mdr_u16 i = 0; i < size; ++i) {
            stream->ReadString (function->name_table[i]);
        }
    }

    /* Set code size and read code. */
    function->AllocateCode (stream->Size () - stream->Position ());
    stream->Read (function->code, function->code_size);

    return function;
}


void LoadNativeFunctions (coin::Stream* stream, LibraryManager& library_manager) {
    mdr_u32 count;
    stream->ReadU32 (count);

    for (size_t i = 0; i < count; ++i) {
        string name;
        mdrType return_type;
        mdr_u8 parameter_count;
        stream->ReadString (name);
        stream->ReadU8 (return_type);
        stream->ReadU8 (parameter_count);

        NativeFunction* function = new NativeFunction (NULL, return_type, parameter_count, false);

        
    }
}



}
