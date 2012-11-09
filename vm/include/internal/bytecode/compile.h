#ifndef MORDOR_INTERNAL_BYTECODE_COMPILE_H
#define MORDOR_INTERNAL_BYTECODE_COMPILE_H


namespace mordor {

class Function;
class BytecodeFunction;

Function* CompileBytecodeFunction (BytecodeFunction* func);

}

#endif /* MORDOR_INTERNAL_BYTECODE_COMPILE_H */
