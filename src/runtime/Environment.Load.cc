#include <algorithm>

#include <coin/utils/Stream.h>
#include <coin/utils/time.h>

#include <mdr/api/Type.h>
#include <mdr/utils/zip.h>
#include <mdr/runtime/Environment.h>
#include <mdr/runtime/BytecodeFunction.h>
#include <mdr/runtime/NativeFunction.h>

using namespace std;
using namespace coin;


namespace mdr {

void Environment::LoadProgram (const string& path) {
    /* Collect Program dependencies. Read native files and cache native functions. */
    vector<string> programs;
    vector<string> native_libraries;
    programs.push_back (path);
    
    /* Load program archive. */
    unzFile program_archive = unzOpen (path.c_str ());
    if (program_archive == NULL) {
        printf ("Error: Program '%s' could not be loaded: Program archive not found!\n", path);
        return;
    }

    /* Read program information recursively. */
    ReadProgramFile (program_archive, programs, native_libraries);
    ReadNativeFile (program_archive);

    unzClose (program_archive);

    /* Load all native libraries and resolve native functions. */
    for (size_t i = 0; i < native_libraries.size (); ++i) {
        LoadRuntimeLibrary (native_libraries[i]);
    }

    /* Load all programs and register all elements. */
    vector<BytecodeFunction*> bytecode_function_cache;
    mdr_u32 function_cache_offset = (mdr_u32) functions_.size ();
    for (size_t i = 0; i < programs.size (); ++i) {
        unzFile archive = unzOpen (programs[i].c_str ());
        if (archive == NULL) {
            printf ("Error: Program '%s' could not be loaded: Program archive not found!\n", programs[i].c_str ());
            return;
        }
        LoadProgramElements (archive, bytecode_function_cache);
        unzClose (archive);
    }

    /* Compile all functions. */
    for (size_t i = 0; i < bytecode_function_cache.size (); ++i) {
        if (!CompileBytecodeFunction (bytecode_function_cache[i])) {
            printf ("Error: An error occured during compilation. Please destroy the Environment!");
            // TODO(Marco): Cleanup.
            break;
        }
    }

    /* Cleanup. */
    for (size_t i = 0; i < bytecode_function_cache.size (); ++i) {
        delete bytecode_function_cache[i];
    }
}

void Environment::LoadProgramElements (unzFile archive, vector<BytecodeFunction*>& bytecode_function_cache) {
    /* Load all '.func' files. */
    bool found = (unzGoToFirstFile (archive) == UNZ_OK);
    for (; found; found = (unzGoToNextFile (archive) == UNZ_OK)) {
        mdr_u32 file_size;

        mdr_u64 time = coin::TimeNanoseconds ();
        mdr_u8* file_data = zip::GetFileData (archive, &file_size);
        string file_name = zip::GetFileName (archive);
        printf ("Loading the file took %lluns.\n", coin::TimeNanoseconds () - time);

        if (file_data == NULL) {
            continue;
        }

        /* Check whether file_name has the extension ".func". */
        size_t index = file_name.find_last_of ('.');
        if (index == string::npos) continue;
        if (file_name.substr (index, file_name.size ()) == ".func") {
            /* Correct name ('/' to '.'). */
            string name = file_name.substr (0, index);
            replace (name.begin (), name.end (), '/', '.');

            // printf ("Loading function '%s'.\n", name.c_str ());

            /* Load and add BytecodeFunction and Function. */
            Function::CompilationInformation* cpinfo = new Function::CompilationInformation ();
            BufferStream stream (file_data, file_size, coin::StreamMode::read);
            BytecodeFunction* bytecode_function = LoadBytecodeFunction (&stream, name, *cpinfo);

            if (!AddFunction (name, cpinfo)) {
                printf ("Error: Function '%s' is already declared!\n", name.c_str ());
                delete bytecode_function;
                continue;
            }
            bytecode_function_cache.push_back (bytecode_function);
        }
    }

    printf ("\n");
}

BytecodeFunction* Environment::LoadBytecodeFunction (coin::Stream* stream, const std::string name, Function::CompilationInformation& cpinfo) {
    mdr_u8 flags;
    stream->ReadU8 (flags);

    cpinfo.return_type_ = flags & 0x0F;

    BytecodeFunction* function = new BytecodeFunction (name);

    mdr_u32 options; /* Consult the bytecode function file format! */
    stream->ReadU32 (options);
    function->maximum_stack_size (options & 0x3FF);
    function->pointer_table_size ((options & 0x1FFC00) >> 10);
    function->variable_table_size ((options & 0xFFE00000) >> 21);

    mdr_u16 operation_count;
    stream->ReadU16 (operation_count);
    function->operation_count (operation_count);

    /* Read parameter types. */
    mdr_u8 parameter_count;
    stream->ReadU8 (parameter_count);
    if (parameter_count > 0) {
        cpinfo.parameters_.Create (parameter_count);
        for (size_t i = 0; i < parameter_count; ++i) {
            mdr_u8 type_bits;
            stream->ReadU8 (type_bits);
            cpinfo.parameters_[i] = type_bits >> 4;
            i++;
            if (i < parameter_count) {
                cpinfo.parameters_[i] = type_bits & 0x0F;
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
            constant.value._u64 = 0;
            switch (mdrTypeGetSize (type)) {
            case 1: {
                mdr_u8 value;
                stream->ReadU8 (value);
                constant.value._u8 = value;
                break;
            }

            case 2: {
                mdr_u16 value;
                stream->ReadU16 (value);
                constant.value._u16 = value;
                break;
            }

            case 4: {
                mdr_u32 value;
                stream->ReadU32 (value);
                constant.value._u32 = value;
                break;
            }

            case 8: {
                mdr_u64 value;
                stream->ReadU64 (value);
                constant.value._u64 = value;
                break;
            }

            default:
                printf ("Error: Invalid constant size!\n");
                return NULL;
            }
        }
    }

    /* Set code size and read code. */
    function->Allocate (stream->Size () - stream->Position ());
    stream->Read (function->code (), function->code_size ());

    return function;
}

void Environment::ReadProgramFile (unzFile archive, vector<string>& programs, vector<string>& native_libraries) {
    mdr_u32 file_size;
    mdr_u8* file_data = zip::GetFileData (archive, "program", &file_size);
    BufferStream stream (file_data, file_size, StreamMode::read);

    /* Read program dependencies. */
    mdr_u16 dependency_count;
    stream.ReadU16 (dependency_count);

    for (size_t i = 0; i < dependency_count; ++i) {
        string path;
        stream.ReadString (path);

        /* Check whether program dependency is already in 'programs'. */
        bool in_list = false;
        size_t size = programs.size ();
        for (size_t j = 0; j < size; ++j) {
            if (programs[j] == path) {
                in_list = true;
                break;
            }
        }

        if (!in_list) {
            programs.push_back (path);

            /* Read program dependencies of dependency and native dependencies! */
            unzFile dependency = unzOpen (path.c_str ());
            if (dependency == NULL) {
                printf ("Error: Program dependency '%s' could not be loaded: Program archive not found!\n", path);
                // TODO(Marco): Exit VM!
                return;
            }

            ReadProgramFile (dependency, programs, native_libraries);
            ReadNativeFile (dependency);

            unzClose (dependency);
        }
    }

    /* Read native library dependencies. */
    mdr_u16 native_library_count;
    stream.ReadU16 (native_library_count);

    for (size_t i = 0; i < native_library_count; ++i) {
        string path;
        stream.ReadString (path);

        /* Check whether library dependency is already registered. */
        bool in_list = false;
        size_t size = native_libraries.size ();
        for (size_t j = 0; j < size; ++j) {
            if (native_libraries[j] == path) {
                in_list = true;
                break;
            }
        }

        if (!in_list) {
            native_libraries.push_back (path);
        }
    }

    delete[] file_data;
}

void Environment::ReadNativeFile (unzFile archive) {
    mdr_u32 file_size;
    mdr_u8* file_data = zip::GetFileData (archive, "native", &file_size);
    BufferStream stream (file_data, file_size, StreamMode::read);

    mdr_u32 count;
    stream.ReadU32 (count);

    for (size_t i = 0; i < count; ++i) {
        string name;
        mdrType return_type;
        mdr_u8 parameter_count;
        stream.ReadString (name);
        stream.ReadU8 (return_type);
        stream.ReadU8 (parameter_count);

        AddNativeFunction (name, return_type, parameter_count, NULL);
    }

    delete[] file_data;
}

}