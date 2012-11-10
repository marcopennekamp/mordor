#ifndef MORDOR_RUNTIME_LOADER_H_
#define	MORDOR_RUNTIME_LOADER_H_

#include <coin/utils/StreamInterface.h>

#include <mordor/runtime/FunctionInterface.h>


#include <mordor/mordor_begin.h>


FunctionInterface*  LoadFunction (StreamInterface* stream);


#include <mordor/mordor_end.h>

#endif  /* MORDOR_RUNTIME_LOADER_H_ */

