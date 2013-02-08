#ifndef MORDOR_INTERNAL_RUNTIME_ENVIRONMENT_H_
#define	MORDOR_INTERNAL_RUNTIME_ENVIRONMENT_H_

#include <string>
#include <vector>

#include <zlib/unzip.h>

#include <mordor/def.h>

#include <internal/runtime/Program.h>
#include <internal/runtime/LibraryManager.h>
#include <internal/runtime/Library.h>


namespace mdr {

class BytecodeFunction;

class Environment {
private:
    std::vector<Program*> programs_;

    LibraryManager library_manager_;


public:
    ~Environment ();

    Program* LoadProgram (const char* path);

    void Initialize ();

    Function* FindFunction (const std::string& name);
    BytecodeFunction* FindBytecodeFunction (const std::string& name);

    LibraryManager& library_manager () { return library_manager_; }

private:
    bool _EvaluateProgramConfig (unzFile archive);
};

}


#endif  /* MORDOR_INTERNAL_RUNTIME_ENVIRONMENT_H_ */