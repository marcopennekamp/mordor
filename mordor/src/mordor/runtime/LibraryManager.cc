#include <internal/runtime/LibraryManager.h>
#include <internal/runtime/Library.h>

using namespace std;


namespace mdr {

Library* LibraryManager::GetRuntimeLibrary (const std::string& name) {
    auto it = library_map_.find (name);
    if (it == library_map_.end ()) {
        return NULL;
    }

    return it->second;
}

mdr_u32 LibraryManager::GetNativeFunctionIndex (const std::string& name) {
    /* Search for already loaded native function. */
    auto it = native_function_id_map_.find (name);
    if (it != native_function_id_map_.end ()) {
        return it->second;
    }
    
    return NativeFunction::INVALID_ID;
}
    
NativeFunction* LibraryManager::GetNativeFunction (const mdr_u32 index) {
    return native_functions_[index];
}

NativeFunction* LibraryManager::GetNativeFunction (const string& name) {
    mdr_u32 index = GetNativeFunctionIndex (name);

    if (index == NativeFunction::INVALID_ID) {
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

    NativeFunction* function = new NativeFunction (NULL, return_type, parameter_count, false);
    mdr_u32 id = (mdr_u32) native_functions_.size ();
    native_function_id_map_[name] = id;
    native_functions_.push_back (function);
}

void LibraryManager::AddRuntimeLibrary (Library* library) {
    library_map_[library->path ()] = library;
}

void LibraryManager::RemoveRuntimeLibrary (const string& path) {
    library_map_.erase (path);
}

}