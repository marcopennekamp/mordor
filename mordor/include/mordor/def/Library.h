#ifndef MORDOR_DEF_LIBRARY_H_
#define MORDOR_DEF_LIBRARY_H_

#include <mordor/def/Mordor.h>


#if (MDR_OS == windows)
#include <windows.h>
namespace mdr {

typedef HMODULE Library;

}
#else
namespace mdr {

typedef void* Library;

}
#endif


#endif  /* MORDOR_DEF_LIBRARY_H_ */