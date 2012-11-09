#ifndef MORDOR_INTERNAL_RUNTIME_LIBRARY_H_
#define MORDOR_INTERNAL_RUNTIME_LIBRARY_H_

#include <mordor/def.h>

#include <string>


#if (MORDOR_OS == windows)
#include <windows.h>
namespace mordor {

typedef HMODULE lib;

}
#else
namespace mordor {

typedef void* lib;

}
#endif


namespace mordor {

class Library {
  private:
    std::string path_;
    lib library_;

  public:
    Library (std::string& path, lib library);

    inline lib library () { return library_; }
    inline std::string path () { return path_; }
};

}

#endif  /* MORDOR_INTERNAL_RUNTIME_LIBRARY_H_ */