#ifndef MORDOR_INTERNAL_RUNTIME_ENVIRONMENT_H_
#define	MORDOR_INTERNAL_RUNTIME_ENVIRONMENT_H_

#include <string>
#include <vector>

#include <zlib/unzip.h>

#include <mordor/def.h>

#include <internal/runtime/Program.h>
#include <internal/runtime/LibraryManager.h>
#include <internal/runtime/Library.h>


namespace mordor {

class BytecodeFunction;

class Environment {
private:
    std::vector<Program*> programs_;

    std::map<const std::string, mdr_u32> native_function_id_map_;
    std::vector<Library::func> native_functions_;

    LibraryManager library_manager_;


public:
    static const mdr_u32 INVALID_NATIVE_FUNCTION_ID = 0xFFFFFFFF;

    ~Environment ();

    Program* LoadProgram (const char* path);

    void Initialize ();

    mdr_u32 GetNativeFunctionIndex (const std::string& name);

    Function* FindFunction (const std::string& name);
    BytecodeFunction* FindBytecodeFunction (const std::string& name);

private:
    bool _EvaluateProgramConfig (unzFile archive);
};

}

#endif  /* MORDOR_INTERNAL_RUNTIME_ENVIRONMENT_H_ */