#ifndef MORDOR_INTERNAL_BYTECODE_COMPILE_H_
#define MORDOR_INTERNAL_BYTECODE_COMPILE_H_


namespace mordor {

class Function;
class BytecodeFunction;
class Environment;
class Program;

Function* CompileBytecodeFunction (const BytecodeFunction* func, Environment* environment, Program* program);

}

#endif /* MORDOR_INTERNAL_BYTECODE_COMPILE_H_ */
