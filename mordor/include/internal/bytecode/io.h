#ifndef MORDOR_INTERNAL_BYTECODE_IO_H_
#define MORDOR_INTERNAL_BYTECODE_IO_H_

#include <vector>

#include <coin/utils/Stream.h>

#include <mordor/def.h>

#include <internal/runtime/LibraryManager.h>


namespace mdr {

class BytecodeFunction;
class NativeFunction;

BytecodeFunction* LoadBytecodeFunction (coin::Stream* stream);
void LoadNativeFunctions (coin::Stream* stream, LibraryManager& library_manager);

}


#endif /* MORDOR_INTERNAL_BYTECODE_IO_H_ */
