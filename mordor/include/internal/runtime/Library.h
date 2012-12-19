#ifndef MORDOR_INTERNAL_RUNTIME_LIBRARY_H_
#define MORDOR_INTERNAL_RUNTIME_LIBRARY_H_

#include <string>
#include <map>

#include <mordor/def.h>


#if (MDR_OS == windows)
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
public:
    typedef void (_stdcall *func) ();

private:
    std::string path_;
    std::map<std::string, func> functions_;
    lib library_;

public:
    Library (const std::string& path, lib library);

    void AddFunction (const std::string& name, func function);

    inline lib library () { return library_; }
    inline std::string path () { return path_; }
};

}

#endif  /* MORDOR_INTERNAL_RUNTIME_LIBRARY_H_ */