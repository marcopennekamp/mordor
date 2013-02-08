#include <internal/runtime/Library.h>

using namespace std;


namespace mdr {

Library::Library (const string& path, lib library) {
    path_ = path; /* Copy path. */
    library_ = library;
}

}