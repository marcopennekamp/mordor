#ifndef MDR_DEF_LIBRARY_H_
#define MDR_DEF_LIBRARY_H_

#include <mdr/def/Mdr.h>


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


#endif  /* MDR_DEF_LIBRARY_H_ */