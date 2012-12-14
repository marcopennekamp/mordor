#ifndef MORDOR_INTERNAL_BYTECODE_IO_H_
#define MORDOR_INTERNAL_BYTECODE_IO_H_

#include <coin/utils/Stream.h>


namespace mordor {

class BytecodeFunction;

BytecodeFunction* LoadBytecodeFunction (coin::Stream* stream);

}

#endif /* MORDOR_INTERNAL_BYTECODE_IO_H_ */
