#ifndef MORDOR_INTERNAL_BYTECODE_COMPILE_H_
#define MORDOR_INTERNAL_BYTECODE_COMPILE_H_


namespace mordor {

class Function;
class BytecodeFunction;

Function* CompileBytecodeFunction (const BytecodeFunction* func);

}

#endif /* MORDOR_INTERNAL_BYTECODE_COMPILE_H_ */
