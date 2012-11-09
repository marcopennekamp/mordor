#include <windows.h>

#include <internal/runtime/LibraryManager.h>
#include <internal/runtime/Library.h>

using namespace std;


namespace mordor {

void LibraryManager::LoadRuntimeLibrary (string& path) {
    /* Load Runtime Library. */
    HMODULE win_lib = LoadLibrary (path.c_str ());
    if (win_lib == NULL) {
        printf ("Error: Runtime Library '%s' could not be loaded!", path.c_str ());
        return;
    }
    Library* library = new Library (path, win_lib);

    /* Load Functions declared in the Library. */



    /* Add Runtime Library to library map. */
    AddRuntimeLibrary (library);



    /*
    HMODULE test_library = LoadLibrary("testdll.dll");
    if (test_library == NULL) {
        printf ("Error while loading \"test.dll\"!\n");
    } else {
        FARPROC initializer = GetProcAddress (test_library, "test");
        if(initializer == NULL) {
            printf ("Function could not be loaded!");
        } else {
            typedef u64 (*func_type) (void);
            func_type function = (func_type) initializer;
            printf ("Function returned: %llX\n", function ());
        }
        FreeLibrary (test_library);
    }
    */
}

void LibraryManager::UnloadRuntimeLibrary (Library* library) {
    FreeLibrary (library->library ());
    RemoveRuntimeLibrary (library->path ());
}

}