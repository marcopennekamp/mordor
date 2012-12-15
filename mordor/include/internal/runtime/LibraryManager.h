#ifndef MORDOR_INTERNAL_RUNTIME_LIBRARYMANAGER_H_
#define MORDOR_INTERNAL_RUNTIME_LIBRARYMANAGER_H_

#include <string>
#include <map>

#include <mordor/def.h>


namespace mordor {

class Library;

class LibraryManager {
private:
    std::map<std::string, Library*> library_map_;

public:
    ~LibraryManager ();

    Library* GetRuntimeLibrary (std::string& name);

    void LoadRuntimeLibrary (std::string& name);
    void UnloadRuntimeLibrary (Library* library); /* Deletes the Library object. */

    void AddRuntimeLibrary (Library* library);
    void RemoveRuntimeLibrary (std::string& name);
};

}

#endif  /* MORDOR_INTERNAL_RUNTIME_LIBRARYMANAGER_H_ */