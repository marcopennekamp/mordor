#ifndef MORDOR_INTERNAL_RUNTIME_CONTEXT_H_
#define	MORDOR_INTERNAL_RUNTIME_CONTEXT_H_

#include <mordor/def.h>

#include <internal/utils/Array.h>


namespace mdr {

class Environment;
class Program;

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
    Array<mdr_u8> native_call_stack_;

    mdr_u32 native_stack_size_;

private:
    Environment* environment_;
    Array<mdr_u8> stack_;
    mdr_u8* return_value_address_;

public:
    Context (Environment* environment);

    inline Environment* environment () { return environment_; }
    inline Array<mdr_u8>& stack () { return stack_; }
    inline mdr_u8*& return_value_address () { return return_value_address_; }
};

}


#endif  /* MORDOR_INTERNAL_RUNTIME_CONTEXT_H_ */