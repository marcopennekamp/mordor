#include <zlib/unzip.h>

#include <libconfig.h>

#include <internal/runtime/Environment.h>
#include <internal/utils/zip.h>

using namespace std;


namespace mordor {

bool Environment::_EvaluateProgramConfig (unzFile archive) {
    /* Locate 'program' file. */
    int found = unzLocateFile (archive, "program", true);
    if (found != UNZ_OK) {
        return false;
    }

    /* Load 'program' file. */
    mordor_char* file_data = NULL;
    zip::GetFileData (archive, (const void**) &file_data, mordor_true, NULL);
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

    config_destroy (config);
    delete config;
    delete[] file_data;

    return true;
}


Program* Environment::LoadProgram (const char* path) {
    /* Open archive. */
    unzFile archive = unzOpen (path);
    if (archive == NULL) {
        printf ("Program '%s' could not be loaded: Program archive not found!", path);
        return NULL;
    }

    /* Evaluate 'program' file. */
    if (!_EvaluateProgramConfig (archive)) {
        printf ("Program '%s' could not be loaded: Evaluating 'program' file failed!\n", path);
        return NULL;
    }

    unzClose (archive);

    return NULL;
}


void Environment::InitializePrograms () {

}


Function* Environment::FindFunction (std::string& name) {

}

}