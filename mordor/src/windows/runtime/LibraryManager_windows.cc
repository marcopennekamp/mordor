#include <windows.h>
#include <imagehlp.h>

#include <internal/runtime/LibraryManager.h>
#include <internal/runtime/Library.h>

using namespace std;


namespace mdr {

LibraryManager::~LibraryManager () {
    auto it = library_map_.begin ();
    for (; it != library_map_.end (); ++it) {
        FreeLibrary (it->second->library ());
        delete it->second;
    }
}

void LibraryManager::LoadRuntimeLibrary (const string& name) {
    if (GetRuntimeLibrary (name) != NULL) {
        printf ("Info: Runtime Library '%s' already loaded!\n", name.c_str ());
        return;
    }

    string path = name + ".dll";

    /* Load Runtime Library. */
    HMODULE win_lib = LoadLibrary (path.c_str ());
    if (win_lib == NULL) {
        printf ("Error: Runtime Library '%s' could not be loaded!\n", name.c_str ());
        return;
    }
    printf ("Info: Loading Runtime Library '%s'!\n", name.c_str ());
    Library* library = new Library (name, win_lib);

    /* Load Functions declared in the Library. */
    _LOADED_IMAGE loaded_image;
    if (MapAndLoad (path.c_str (), NULL, &loaded_image, true, true)) {
        unsigned long dir_size;
        _IMAGE_EXPORT_DIRECTORY* image_export_directory = (_IMAGE_EXPORT_DIRECTORY*) ImageDirectoryEntryToDataEx (loaded_image.MappedAddress, false, IMAGE_DIRECTORY_ENTRY_EXPORT, &dir_size, NULL);
        if (image_export_directory != NULL) {
            DWORD* name_rvas = (DWORD*) ImageRvaToVa (loaded_image.FileHeader, loaded_image.MappedAddress, image_export_directory->AddressOfNames, NULL);
            for (size_t i = 0; i < image_export_directory->NumberOfNames; ++i) {
                string function_name = (char*) ImageRvaToVa (loaded_image.FileHeader, loaded_image.MappedAddress, name_rvas[i], NULL);
                FARPROC initializer = GetProcAddress (win_lib, function_name.c_str ());
                if (initializer != NULL) {
                    NativeFunction* function = GetNativeFunction (GetNativeFunctionIndex (function_name));

                    /* Function has not been registered and is not needed. */
                    if (function == NULL) {
                        continue;
                    }

                    if (function->function () != NULL) {
                        printf ("Error: Function '%s' loaded from runtime library '%s' has already been loaded!\n", function_name.c_str (), path.c_str ());
                    }else {
                        /* Glue function pointer to NativeFunction. */
                        function->function ((NativeFunction::function_t) initializer);
                    }
                }
            }
        }
        UnMapAndLoad (&loaded_image);
    }

    /* Add Runtime Library to library map. */
    AddRuntimeLibrary (library);
}

void LibraryManager::UnloadRuntimeLibrary (Library* library) {
    FreeLibrary (library->library ());
    RemoveRuntimeLibrary (library->path ());
    delete library;
}

}