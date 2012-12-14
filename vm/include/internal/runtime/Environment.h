#ifndef MORDOR_INTERNAL_RUNTIME_ENVIRONMENT_H_
#define	MORDOR_INTERNAL_RUNTIME_ENVIRONMENT_H_

#include <string>
#include <vector>

#include <zlib/unzip.h>

#include <mordor/def.h>

#include <internal/runtime/Program.h>
#include <internal/runtime/LibraryManager.h>


namespace mordor {

class BytecodeFunction;

class Environment {
  private:
    std::vector<Program*> programs_;

    LibraryManager library_manager_;


  public:
    Program* LoadProgram (const char* path);

    void Initialize ();

    Function* FindFunction (std::string& name);
    BytecodeFunction* FindBytecodeFunction (std::string& name);

  private:
    bool _EvaluateProgramConfig (unzFile archive);
};

}

#endif  /* MORDOR_INTERNAL_RUNTIME_ENVIRONMENT_H_ */