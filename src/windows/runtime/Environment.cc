#include <windows.h>
#include <imagehlp.h>

#include <mdr/runtime/Environment.h>
#include <mdr/runtime/NativeFunction.h>

using namespace std;


namespace mdr {

void Environment::LoadRuntimeLibrary (const std::string& _path) {
    string path = _path + ".dll";

    /* Make sure the library has not been loaded. */
    auto it = libraries_.find (path);
    if (it != libraries_.end ()) {
        printf ("Info: Library '%s' already loaded!", path.c_str ());
        return;
    }

    /* Load Runtime Library. */
    HMODULE library = LoadLibrary (path.c_str ());
    if (library == NULL) {
        printf ("Error: Runtime Library '%s' could not be loaded!\n", path.c_str ());
        return;
    }
    printf ("Info: Loading Runtime Library '%s'!\n", path.c_str ());

    /* Load Functions declared in the Library. */
    _LOADED_IMAGE loaded_image;
    if (MapAndLoad (path.c_str (), NULL, &loaded_image, true, true)) {
        unsigned long dir_size;
        _IMAGE_EXPORT_DIRECTORY* image_export_directory = (_IMAGE_EXPORT_DIRECTORY*) ImageDirectoryEntryToDataEx (loaded_image.MappedAddress, false, IMAGE_DIRECTORY_ENTRY_EXPORT, &dir_size, NULL);
        if (image_export_directory != NULL) {
            DWORD* name_rvas = (DWORD*) ImageRvaToVa (loaded_image.FileHeader, loaded_image.MappedAddress, image_export_directory->AddressOfNames, NULL);
            for (size_t i = 0; i < image_export_directory->NumberOfNames; ++i) {
                string function_name = (char*) ImageRvaToVa (loaded_image.FileHeader, loaded_image.MappedAddress, name_rvas[i], NULL);
                FARPROC initializer = GetProcAddress (library, function_name.c_str ());
                if (initializer != NULL) {
                    NativeFunction* function = GetNativeFunction (function_name);

                    /* Function has not been registered and is not needed. */
                    if (function == NULL) {
                        printf ("Function '%s' not registered.\n", function_name.c_str ());
                        continue;
                    }

                    if (function->function () != NULL) {
                        printf ("Warning: Function '%s' loaded from runtime library '%s' has already been loaded!\n", function_name.c_str (), path.c_str ());
                    }else {
                        /* Glue function pointer to NativeFunction. */
                        function->function ((NativeFunction::function_t) initializer);
                    }
                }
            }
        }

        UnMapAndLoad (&loaded_image);
    }

    libraries_[path] = library;
}

void Environment::UnloadRuntimeLibraries () {
    auto it = libraries_.begin ();
    for (; it != libraries_.end (); ++it) {
        FreeLibrary (it->second);
    }
}

}