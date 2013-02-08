#ifndef MORDOR_RUNTIME_CONTEXT_H_
#define	MORDOR_RUNTIME_CONTEXT_H_

#include <mordor/def/Mordor.h>
#include <mordor/utils/Array.h>


namespace mdr {

class Environment;
class Program;
class Function;

class Context {
public:
    /*
     * Holds the register values for the x64 calling convention.
     * This is needed because executing the interpreter itself pollutes rcx, rdx and possibly r8 and r9,
     *   so they have to be added right before calling with very low level code.
     *
     * 32bit values have to be put in the low part of the memory, which equals the
     *   array indices 0, 2, 4 or 6 with a 32bit pointer in mind.
     */
    struct NativeCallStack {
        Array<mdr_u8> data;
        mdr_u32 size;
    };

private:
    Environment* environment_;

    Array<mdr_u8> stack_;
    mdr_u8* return_value_address_;
    NativeCallStack native_call_stack_;

public:
    Context (Environment* environment);

    void Execute (Function* function, mdr_u32 caller_stack_top);

    inline Environment* environment () { return environment_; }

    inline Array<mdr_u8>& stack () { return stack_; }
    inline mdr_u8*& return_value_address () { return return_value_address_; } // TODO(Marco): Why the reference here?
    inline NativeCallStack& native_call_stack () { return native_call_stack_; }
};

}


#endif  /* MORDOR_RUNTIME_CONTEXT_H_ */