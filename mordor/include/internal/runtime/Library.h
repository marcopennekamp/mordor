#ifndef MORDOR_INTERNAL_RUNTIME_LIBRARY_H_
#define MORDOR_INTERNAL_RUNTIME_LIBRARY_H_

#include <string>
#include <map>

#include <mordor/def.h>

#include <internal/runtime/NativeFunction.h>


#if (MDR_OS == windows)
#include <windows.h>
namespace mdr {

typedef HMODULE lib;

}
#else
namespace mdr {

typedef void* lib;

}
#endif


namespace mdr {

class Library {
private:
    std::string path_;
    lib library_;

public:
    Library (const std::string& path, lib library);

    inline lib library () { return library_; }
    inline std::string path () { return path_; }
};

}


#endif  /* MORDOR_INTERNAL_RUNTIME_LIBRARY_H_ */