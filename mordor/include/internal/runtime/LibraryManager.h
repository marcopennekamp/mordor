#ifndef MORDOR_INTERNAL_RUNTIME_LIBRARYMANAGER_H_
#define MORDOR_INTERNAL_RUNTIME_LIBRARYMANAGER_H_

#include <string>
#include <map>

#include <mordor/def.h>

#include <internal/runtime/Library.h>


namespace mordor {

class LibraryManager {
private:
    std::map<std::string, Library*> library_map_;

public:
    ~LibraryManager ();

    Library* GetRuntimeLibrary (const std::string& name);
    Library::func FindFunction (const std::string& name);

    void LoadRuntimeLibrary (const std::string& name);
    void UnloadRuntimeLibrary (Library* library); /* Deletes the Library object. */

    void AddRuntimeLibrary (Library* library);
    void RemoveRuntimeLibrary (const std::string& name);
};

}

#endif  /* MORDOR_INTERNAL_RUNTIME_LIBRARYMANAGER_H_ */