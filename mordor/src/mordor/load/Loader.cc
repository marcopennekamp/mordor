#include <coin/utils/Stream.h>

#include <mordor/api/Type.h>
#include <mordor/load/Loader.h>
#include <mordor/load/BytecodeFunction.h>
#include <mordor/runtime/NativeFunction.h>
#include <mordor/runtime/Environment.h>

using namespace std;
using namespace coin;


namespace mdr {

Environment* Loader::CreateEnvironment (const std::string program_path) {
    Environment* environment = new Environment ();
    
    
    
    /* Collect Program dependencies. Read native files and cache function names. */
    vector<string> programs;
    


    /* Load all native libraries and resolve native functions. */



    /* Compile all programs. */



    /* Resolve mordor function locations. */


}

BytecodeFunction* Loader::LoadBytecodeFunction (coin::Stream* stream) {
    mdr_u8 flags;
    stream->ReadU8 (flags);

    mdrType return_type = flags & 0x0F;
    mdr_u8 parameter_count;
    stream->ReadU8 (parameter_count);

    BytecodeFunction* function = new BytecodeFunction (return_type, parameter_count);

    mdr_u32 options; /* Consult the bytecode function file format! */
    stream->ReadU32 (options);
    function->maximum_stack_size (options & 0x3FF);
    function->pointer_table_size ((options & 0x1FFC00) >> 10);
    function->variable_table_size ((options & 0xFFE00000) >> 21);

    mdr_u16 operation_count;
    stream->ReadU16 (operation_count);
    function->operation_count (operation_count);
    
    /* Read constant table. */
    if (flags & BytecodeFunction::kConstantTableExists) {
        mdr_u16 size;
        if (flags & BytecodeFunction::kConstantTableWide) {
            stream->ReadU16 (size);
        }else {
            mdr_u8 _size;
            stream->ReadU8 (_size);
            size = _size;
        }

        function->constant_table ().Create (size);
        for (mdr_u16 i = 0; i < size; ++i) {
            mdrType type;
            stream->ReadU8 (type);
            BytecodeFunction::Constant& constant = function->constant_table ()[i];
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
    if (flags & BytecodeFunction::kNameTableExists) {
        mdr_u16 size;
        if (flags & BytecodeFunction::kNameTableWide) {
            stream->ReadU16 (size);
        }else {
            mdr_u8 _size;
            stream->ReadU8 (_size);
            size = _size;
        }

        function->name_table ().Create (size);
        for (mdr_u16 i = 0; i < size; ++i) {
            stream->ReadString (function->name_table ()[i]);
        }
    }

    /* Set code size and read code. */
    function->Allocate (stream->Size () - stream->Position ());
    stream->Read (function->code (), function->code_size ());

    return function;
}


void Loader::LoadNativeFunctions (coin::Stream* stream, LibraryManager& library_manager) {
    mdr_u32 count;
    stream->ReadU32 (count);

    for (size_t i = 0; i < count; ++i) {
        string name;
        mdrType return_type;
        mdr_u8 parameter_count;
        stream->ReadString (name);
        stream->ReadU8 (return_type);
        stream->ReadU8 (parameter_count);

        NativeFunction* function = new NativeFunction (NULL, return_type, parameter_count);

        
    }
}



}
