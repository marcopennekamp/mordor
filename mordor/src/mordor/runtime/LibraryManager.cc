#include <mordor/runtime/LibraryManager.h>
#include <mordor/runtime/Library.h>
#include <mordor/runtime/NativeFunction.h>

using namespace std;


namespace mdr {

mdr_u32 LibraryManager::GetNativeFunctionIndex (const std::string& name) {
    /* Search for already loaded native function. */
    auto it = native_function_id_map_.find (name);
    if (it != native_function_id_map_.end ()) {
        return it->second;
    }
    
    return NativeFunction::kInvalidId;
}
    
NativeFunction* LibraryManager::GetNativeFunction (const mdr_u32 index) {
    return native_functions_[index];
}

NativeFunction* LibraryManager::GetNativeFunction (const string& name) {
    mdr_u32 index = GetNativeFunctionIndex (name);

    if (index == NativeFunction::kInvalidId) {
        return NULL;
    }

    return native_functions_[index];
}

void LibraryManager::AddNativeFunction (const std::string& name, const mdrType return_type, mdr_u8 parameter_count) {
    NativeFunction* existing_function = GetNativeFunction (name);
    if (existing_function != NULL) {
        printf ("Info: Function '%s' already loaded.\n", name.c_str ());
        return;
    }

    NativeFunction* function = new NativeFunction (NULL, return_type, parameter_count);
    mdr_u32 id = (mdr_u32) native_functions_.size ();
    native_function_id_map_[name] = id;
    native_functions_.push_back (function);
}

}