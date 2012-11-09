#ifndef MORDOR_INTERNAL_RUNTIME_LIBRARYMANAGER_H_
#define MORDOR_INTERNAL_RUNTIME_LIBRARYMANAGER_H_

#include <mordor/def.h>

#include <string>
#include <map>


namespace mordor {

class Library;

class LibraryManager {
  private:
    std::map<std::string, Library*> library_map_;

  public:
    ~LibraryManager ();

    void AddRuntimeLibrary (Library* library);
    void RemoveRuntimeLibrary (std::string& path);

    void LoadRuntimeLibrary (std::string& path);
    void UnloadRuntimeLibrary (Library* library);
};

}

#endif  /* MORDOR_INTERNAL_RUNTIME_LIBRARYMANAGER_H_ */