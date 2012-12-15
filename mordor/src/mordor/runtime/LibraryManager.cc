#include <internal/runtime/LibraryManager.h>
#include <internal/runtime/Library.h>

using namespace std;


namespace mordor {

Library* LibraryManager::GetRuntimeLibrary (std::string& name) {
    auto iterator = library_map_.find (name);
    if (iterator == library_map_.end ()) {
        return NULL;
    }
    return iterator->second;
}

void LibraryManager::AddRuntimeLibrary (Library* library) {
    library_map_[library->path ()] = library;
}

void LibraryManager::RemoveRuntimeLibrary (string& path) {
    library_map_.erase (path);
}

}