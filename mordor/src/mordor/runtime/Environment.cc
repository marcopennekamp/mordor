#include <algorithm>

#include <zlib/unzip.h>

#include <libconfig.h>

#include <coin/utils/Stream.h>

#include <mordor/def/Type.h>
#include <mordor/runtime/Environment.h>
#include <mordor/runtime/NativeFunction.h>

using namespace std;


namespace mdr {

namespace {

mdr_u32 GetIdFromMap (Environment::IdMap map, const string& name, const mdr_u32 kInvalidId) {
    auto it = map.find (name);
    if (it != map.end ()) {
        return it->second;
    }
    
    return kInvalidId;
}

template<typename T>
T* GetValueFromVector (vector<T*> list, const mdr_u32 id) {
    return list[id];
}

template<typename T>
T* GetValueFromVector (vector<T*> list, Environment::IdMap map, const string& name, const mdr_u32 kInvalidId) {
    mdr_u32 id = GetIdFromMap (map, name, kInvalidId);

    if (id == kInvalidId) {
        return NULL;
    }

    return list[id];
}

}

Environment::~Environment () {
    UnloadRuntimeLibraries ();

    /* Delete Functions. */
    for (size_t i = 0; i < functions_.size (); ++i) {
        delete functions_[i];
    }
}


mdr_u32 Environment::GetNativeFunctionId (const std::string& name) {
    return GetIdFromMap (native_function_id_map_, name, NativeFunction::kInvalidId);
}

NativeFunction* Environment::GetNativeFunction (const mdr_u32 id) {
    return GetValueFromVector (native_functions_, id);
}

NativeFunction* Environment::GetNativeFunction (const std::string& name) {
    return GetValueFromVector (native_functions_, native_function_id_map_, name, NativeFunction::kInvalidId);
}

void Environment::AddNativeFunction (const std::string& name, const mdrType return_type, mdr_u8 parameter_count, NativeFunction::function_t function_ptr) {
    NativeFunction* existing_function = GetNativeFunction (name);
    if (existing_function != NULL) {
        printf ("Info: Native function '%s' already declared.\n", name.c_str ());
        return;
    }

    NativeFunction* function = new NativeFunction (function_ptr, return_type, parameter_count);
    mdr_u32 id = (mdr_u32) native_functions_.size ();
    native_function_id_map_[name] = id;
    native_functions_.push_back (function);
}


mdr_u32 Environment::GetFunctionId (const std::string& name) {
    return GetIdFromMap (function_id_map_, name, Function::kInvalidId);
}

Function* Environment::GetFunction (const mdr_u32 id) {
    return GetValueFromVector (functions_, id);
}

Function* Environment::GetFunction (const std::string& name) {
    return GetValueFromVector (functions_, function_id_map_, name, Function::kInvalidId);
}

bool Environment::AddFunction (const std::string& name, Function::CompilationInformation& cpinfo) {
    Function* existing_function = GetFunction (name);
    if (existing_function != NULL) {
        printf ("Error: Function '%s' already declared!\n", name.c_str ());
        return false;
    }

    Function* function = new Function (cpinfo);
    mdr_u32 id = (mdr_u32) functions_.size ();
    function_id_map_[name] = id;
    functions_.push_back (function);
    return true;
}


}