#ifndef MORDOR_INTERNAL_BYTECODE_LOAD_H
#define MORDOR_INTERNAL_BYTECODE_LOAD_H

#include <coin/utils/Stream.h>


namespace mordor {

class BytecodeFunction;

BytecodeFunction* LoadBytecodeFunction (coin::Stream* stream);

}

#endif /* MORDOR_INTERNAL_BYTECODE_LOAD_H */
