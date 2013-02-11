#ifndef MDR_RUNTIME_NATIVEFUNCTION_H_
#define MDR_RUNTIME_NATIVEFUNCTION_H_

#include <mdr/def/Mdr.h>
#include <mdr/def/Type.h>


namespace mdr {

class NativeFunction {
public:
    static const mdr_u32 kInvalidId = 0xFFFFFFFF;
    typedef void (MDRI_CALL *function_t) ();

private:
    function_t function_;
    mdrType return_type_;
    mdr_u8 parameter_count_;

public:
    NativeFunction (function_t function_, mdrType return_type, mdr_u8 parameter_count);

    inline function_t function () const { return function_; }
    inline void function (function_t function) { function_ = function; }

    inline mdrType return_type () const { return return_type_; }
    inline void return_type (mdrType return_type) { return_type_ = return_type; }

    inline mdr_u8 parameter_count () const { return parameter_count_; }
    inline void parameter_count (mdr_u8 parameter_count) { parameter_count_ = parameter_count; }
};

}


#endif  /* MDR_RUNTIME_NATIVEFUNCTION_H_ */