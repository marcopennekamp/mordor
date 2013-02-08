#ifndef MORDOR_INTERNAL_RUNTIME_LIBRARYMANAGER_H_
#define MORDOR_INTERNAL_RUNTIME_LIBRARYMANAGER_H_

#include <string>
#include <vector>
#include <map>

#include <mordor/def.h>

#include <internal/runtime/Library.h>


namespace mdr {

class LibraryManager {
private:
    std::map<std::string, Library*> library_map_;
    
    std::map<const std::string, mdr_u32> native_function_id_map_;
    std::vector<NativeFunction*> native_functions_;

public:
    ~LibraryManager ();

    Library* GetRuntimeLibrary (const std::string& name);

    /* 
     * If the name has not been registered yet, it searches the runtime libraries for the function.
     * Should the function not be found, a NativeFunction object is created regardless. 
     * This allows for two possibilities:
     *  - The NativeFunction can be given an alternative function pointer.
     *  - Another runtime library can be loaded.
     */
    mdr_u32 GetNativeFunctionIndex (const std::string& name);
    
    NativeFunction* GetNativeFunction (const mdr_u32 index);
    NativeFunction* GetNativeFunction (const std::string& name);

    void AddNativeFunction (const std::string& name, const mdrType return_type, mdr_u8 parameter_count);

    void LoadRuntimeLibrary (const std::string& name);
    void UnloadRuntimeLibrary (Library* library); /* Deletes the Library object. */

    void AddRuntimeLibrary (Library* library);
    void RemoveRuntimeLibrary (const std::string& name);
};

}


#endif  /* MORDOR_INTERNAL_RUNTIME_LIBRARYMANAGER_H_ */