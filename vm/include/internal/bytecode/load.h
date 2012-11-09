#ifndef MORDOR_INTERNAL_BYTECODE_LOAD_H
#define MORDOR_INTERNAL_BYTECODE_LOAD_H


namespace mordor {

class Stream;
class BytecodeFunction;

BytecodeFunction* LoadBytecodeFunction (Stream* stream);

}

#endif /* MORDOR_INTERNAL_BYTECODE_LOAD_H */
