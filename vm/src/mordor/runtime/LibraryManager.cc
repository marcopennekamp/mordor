#include <internal/runtime/LibraryManager.h>
#include <internal/runtime/Library.h>

using namespace std;


namespace mordor {

void LibraryManager::AddRuntimeLibrary (Library* library) {
    library_map_[library->path ()] = library;
}

void LibraryManager::RemoveRuntimeLibrary (string& path) {
    library_map_.erase (path);
}

}