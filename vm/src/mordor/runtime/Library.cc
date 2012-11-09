#include <internal/runtime/Library.h>

using namespace std;


namespace mordor {

Library::Library (string& path, lib library) {
    path_ = path; /* Copy path. */
    library_ = library;
}

}