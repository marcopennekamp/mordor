#ifndef MORDOR_RUNTIME_LIBRARY_H_
#define MORDOR_RUNTIME_LIBRARY_H_

#include <string>
#include <map>

#include <mordor/def/Mordor.h>


#if (MDR_OS == windows)
#include <windows.h>
namespace mdr {

typedef HMODULE library_t;

}
#else
namespace mdr {

typedef void* library_t;

}
#endif


namespace mdr {

class Library {
private:
    library_t library_;

public:
    Library (library_t library);

    inline library_t library () { return library_; }
};

}


#endif  /* MORDOR_INTERNAL_RUNTIME_LIBRARY_H_ */