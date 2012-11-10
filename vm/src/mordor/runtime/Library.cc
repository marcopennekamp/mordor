#include <internal/runtime/Library.h>

using namespace std;


namespace mordor {

Library::Library (string& path, lib library) {
    path_ = path; /* Copy path. */
    library_ = library;
}

void Library::AddFunction (std::string& name, func function) {
    functions_[name] = function;
}

}