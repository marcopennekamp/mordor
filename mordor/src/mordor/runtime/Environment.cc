#include <algorithm>

#include <zlib/unzip.h>

#include <libconfig.h>

#include <coin/utils/Stream.h>

#include <mordor/runtime/Environment.h>
#include <mordor/runtime/Program.h>

#include <mordor/load/BytecodeFunction.h>
#include <mordor/load/Loader.h>
#include <internal/bytecode/io.h>
#include <internal/runtime/Environment.h>
#include <internal/utils/zip.h>

using namespace std;


namespace mdr {

Environment::~Environment () {
    /* Delete Programs. */
    size_t size = programs_.size ();
    for (size_t i = 0; i < size; ++i) {
        mdrProgDestroy ((mdrProgram*) programs_[i]);
    }
}

bool Environment::_EvaluateProgramConfig (unzFile archive) {
    /* Locate 'program' file. */
    int found = unzLocateFile (archive, "program", true);
    if (found != UNZ_OK) {
        return false;
    }

    /* Load 'program' file. */
    char* file_data = NULL;
    zip::GetFileData (archive, (const void**) &file_data, NULL, mdr_true, NULL);
    if (file_data == NULL) {
        return false;
    }

    /* Load config. */
    config_t* config = new config_t ();
    config_init (config);
    config_read_string (config, file_data);

    /*
    const mordor_char* program_name;
    config_lookup_string (config, "name", &program_name);

    printf ("Program name: %s\n", (char*) program_name);
    */

    /* Load specified runtime libraries. */
    config_setting_t* setting = config_lookup (config, "libraries");
    if (setting != NULL) { /* Setting is optional. */
        size_t length = config_setting_length (setting);
        for (int i = 0; i < length; ++i) {
            string name = config_setting_get_string_elem (setting, i);
            library_manager_.LoadRuntimeLibrary (name);
        }
    }

    delete[] file_data;
    config_destroy (config);
    delete config;

    return true;
}


Program* Environment::LoadProgram (const char* path) {
    /* Open archive. */
    unzFile archive = unzOpen (path);
    if (archive == NULL) {
        printf ("Program '%s' could not be loaded: Program archive not found!\n", path);
        return NULL;
    }

    /* Evaluate 'program' file. */
    if (!_EvaluateProgramConfig (archive)) {
        printf ("Program '%s' could not be loaded: Evaluating 'program' file failed!\n", path);
        return NULL;
    }

    /* Create Program. */
    Program* program = (Program*) mdrProgCreate ();

    /* Load all '.func' files. */
    bool found = (unzGoToFirstFile (archive) == UNZ_OK);
    for (; found; found = (unzGoToNextFile (archive) == UNZ_OK)) {
        mdr_u8* file_data = NULL;
        mdr_u32 file_size;
        string file_name;
        {
            const mdr_s8* file_name_array = NULL;
            zip::GetFileData (archive, (const void**) &file_data, &file_size, mdr_false, &file_name_array);
            if (file_data == NULL || file_name_array == NULL) {
                if (file_data != NULL) delete[] file_data;
                if (file_name_array != NULL) delete file_name_array;
                continue;
            }
            file_name = file_name_array;
            delete[] file_name_array;
        }

        /* Check whether file_name has the extension ".func". */
        size_t index = file_name.find_last_of ('.');
        if (index == string::npos) continue;
        if (file_name.substr (index, file_name.size ()) == ".func") {
            /* Correct name ('/' to '.'). */
            string name = file_name.substr (0, index);
            replace (name.begin (), name.end (), '/', '.');

            /* Read and add BytecodeFunction. */
            coin::BufferStream stream (file_data, file_size, coin::StreamMode::read);
            BytecodeFunction* function = LoadBytecodeFunction (&stream);
            program->AddBytecodeFunction (name, function);
        }
    }

    unzClose (archive);

    /* Add Program. */
    programs_.push_back (program);

    return program;
}


void Environment::Initialize () {
    size_t size = programs_.size ();

    /* Initialize Programs. */
    for (size_t i = 0; i < size; ++i) {
        programs_[i]->Initialize (this);
    }

    /* Resolve their symbols. Must be called when everything has been compiled. */
    for (size_t i = 0; i < size; ++i) {
        programs_[i]->ResolveSymbols (this);
    }
}

// TODO(Marco): Duplicate code!

Function* Environment::FindFunction (const std::string& name) {
    size_t size = programs_.size ();
    for (size_t i = 0; i < size; ++i) {
        mdr_u32 id = programs_[i]->GetFunctionId (name);
        if (id != Program::INVALID_FUNCTION_ID) {
            return programs_[i]->GetFunctionFromCache (id);
        }
    }
    return NULL;
}

BytecodeFunction* Environment::FindBytecodeFunction (const std::string& name) {
    size_t size = programs_.size ();
    for (size_t i = 0; i < size; ++i) {
        mdr_u32 id = programs_[i]->GetFunctionId (name);
        if (id != Program::INVALID_FUNCTION_ID) {
            return programs_[i]->bytecode_function_cache ()[id];
        }
    }
    return NULL;
}

}