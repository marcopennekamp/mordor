#ifndef MDR_RUNTIME_ENVIRONMENT_H_
#define	MDR_RUNTIME_ENVIRONMENT_H_

#include <string>
#include <vector>
#include <map>

#include <zlib/unzip.h>

#include <coin/utils/Stream.h>

#include <mdr/def/Mdr.h>
#include <mdr/def/Library.h>
#include <mdr/runtime/Function.h>
#include <mdr/runtime/NativeFunction.h>


namespace mdr {

class BytecodeFunction;
class Program;

class Environment {
public:
    typedef std::map<const std::string, mdr_u32> IdMap;

private:
    std::map<const std::string, Library> libraries_;
    
    IdMap native_function_id_map_;
    std::vector<NativeFunction*> native_functions_;


    /*
     * Holds all Function IDs.
     */
    IdMap function_id_map_;

    /*
     * Saves all functions that are already loaded.
     */
    std::vector<Function*> functions_;


    BytecodeFunction* LoadBytecodeFunction (coin::Stream* stream, const std::string name, Function::CompilationInformation& cpinfo);
    void ReadProgramFile (unzFile archive, std::vector<std::string>& programs, std::vector<std::string>& native_libraries);
    void ReadNativeFile (unzFile archive);

    /* return true on success. */
    bool CompileBytecodeFunction (BytecodeFunction* function);

    void UnloadRuntimeLibraries ();


    void LoadProgramElements (unzFile archive, std::vector<BytecodeFunction*>& bytecode_function_cache);


public:
    ~Environment ();

    void LoadProgram (const std::string& path);
    void LoadRuntimeLibrary (const std::string& path);


    mdr_u32 GetNativeFunctionId (const std::string& name);
    NativeFunction* GetNativeFunction (const mdr_u32 id);
    NativeFunction* GetNativeFunction (const std::string& name);
    void AddNativeFunction (const std::string& name, const mdrType return_type, mdr_u8 parameter_count, NativeFunction::function_t function);


    mdr_u32 GetFunctionId (const std::string& name);
    Function* GetFunction (const mdr_u32 id);
    Function* GetFunction (const std::string& name);

    /*
     * Whether adding was successful.
     */
    bool AddFunction (const std::string& name, Function::CompilationInformation* cpinfo);

};

}


#endif  /* MDR_RUNTIME_ENVIRONMENT_H_ */