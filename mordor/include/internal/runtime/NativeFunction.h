#ifndef MORDOR_INTERNAL_RUNTIME_NATIVEFUNCTION_H_
#define MORDOR_INTERNAL_RUNTIME_NATIVEFUNCTION_H_

#include <mordor/def.h>
#include <mordor/bytecode/Type.h>


namespace mdr {

class NativeFunction {
public:
    static const mdr_u32 INVALID_ID = 0xFFFFFFFF;
    typedef void (MDRI_CALL *function_t) ();

private:
    function_t function_;
    mdrType return_type_;
    mdr_u8 parameter_count_;
    bool is_in_runtime_library_;

public:
    NativeFunction (function_t function_, mdrType return_type, mdr_u8 parameter_count, bool is_in_runtime_library);

    inline function_t function () const { return function_; }
    inline void function (function_t function) { function_ = function; }

    inline mdrType return_type () const { return return_type_; }
    inline void return_type (mdrType return_type) { return_type_ = return_type; }

    inline mdr_u8 parameter_count () const { return parameter_count_; }
    inline void parameter_count (mdr_u8 parameter_count) { parameter_count_ = parameter_count; }

    inline bool is_in_runtime_library () const { return is_in_runtime_library_; }
    inline void is_in_runtime_library (bool flag) { is_in_runtime_library_ = flag; }
};

}


#endif  /* MORDOR_INTERNAL_RUNTIME_NATIVEFUNCTION_H_ */