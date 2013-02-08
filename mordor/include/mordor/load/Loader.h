#ifndef MORDOR_BYTECODE_LOADER_H_
#define MORDOR_BYTECODE_LOADER_H_

#include <vector>

#include <coin/utils/Stream.h>

#include <mordor/def/Mordor.h>


namespace mdr {

class BytecodeFunction;
class NativeFunction;
class LibraryManager;
class Environment;
class Function;
class Program;

class Loader {

private:
    Environment* environment_;

    BytecodeFunction* LoadBytecodeFunction (coin::Stream* stream);
    void LoadNativeFunctions (coin::Stream* stream);

    Function* CompileBytecodeFunction (const BytecodeFunction* function, Program* program);


public:
    Loader (Environment* environment);
    ~Loader ();
};


}


#endif /* MORDOR_BYTECODE_LOADER_H_ */
