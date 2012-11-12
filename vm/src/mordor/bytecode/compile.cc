#include <vector>
#include <map>

#include <boost/scoped_ptr.hpp>
#include <internal/utils/Array.h>

#include <coin/utils/time.h>

#include <mordor/bytecode/BytecodeOperation.h>

#include <internal/bytecode/compile.h>
#include <internal/bytecode/BytecodeFunction.h>
#include <internal/runtime/Function.h>

using namespace std;
using namespace boost;


#define _START(NAME)    case BCOP_ ## NAME :
#define _CASE(NAME)     case BCOP_ ## NAME : 
#define _END            break;

/* extracts 10 bit parameter, increments op by one. */
#define _U10(P0)        mordor_u16 P0; get_u10 (bc_op, P0); ++bc_op;
#define _S10(P0)        mordor_s16 P0; get_s10 (bc_op, P0); ++bc_op;


namespace mordor {

namespace {

const mordor_u32 kPointerSize = sizeof (void*);

typedef mordor_u8 VariableType;
const mordor_u8 TYPE_I = 0x01;
const mordor_u8 TYPE_U = 0x02;
const mordor_u8 TYPE_F = 0x04;
const mordor_u8 TYPE_P = 0x08;
const mordor_u8 TYPE_IS_LONG = 0x10;

struct StackEntry {
    VariableType type;
    bool constant;
    mordor_u16 id; /* Variable address or if constant == true, constant table id. */

    void Set (const VariableType _type, const bool _constant, const mordor_u16 _id) {
        type = _type;
        constant = _constant;
        id = _id;
    }
};


inline mordor_u8 get_opcode (const BytecodeOperation* ptr) {
    return (*ptr & 0xFC) >> 2;
}

inline void get_u10 (const BytecodeOperation* ptr, mordor_u16& param0) {
    mordor_u16 v0 = *ptr & 0x03;
    mordor_u16 v1 = *(ptr + 1) & 0xFF;
    param0 = v0 << 8 | v1;
}

inline void get_s10 (const BytecodeOperation* ptr, mordor_s16& param0) {
    mordor_u16 low = *(ptr + 1) & 0xFF;
    mordor_u16 sign = *ptr & 0x02;

    if (sign > 0) { /* negative number. */
        mordor_u16 high = *ptr & 0x01;
        param0 = -(high << 8 | low);
    }else { /* positive number. */
        mordor_u16 high = *ptr & 0x03;
        param0 = high << 8 | low;
    }
}


/**
 * Returns an address on the stack for the given variable or pointer.
 * May or may not reserve stack space.
 */
inline mordor_u16 get_stack_address_for_element (const mordor_u32 element, const VariableType type, Array<mordor_u16>& element_to_stack, mordor_u32& stack_top) {
    mordor_u16 address = element_to_stack[element];

    /* Reserve address if needed. */
    if (address == 0xFFFF) {
        /* Compute element size. */
        mordor_u32 element_size;
        bool is_pointer = false;
        if ((type & TYPE_P) > 0) {
            element_size = kPointerSize;
            is_pointer = true;
        }else {
            element_size = ((type & TYPE_IS_LONG) > 0) ? 8 : 4;
        }
        printf ("Element size is %u", element_size);

        /* Check whether local stack is not exceeded. */
        if (stack_top + element_size >= 0xFFFF) {
            printf ("Maximum stack size exceeded!\n");
            return 0;
        }

        /* Reserve addresses. */
        address = stack_top;
        if (!is_pointer && element_size == 8) { /* Long variables. */
            element_to_stack[element + 1] = address + 4;
        }
        element_to_stack[element] = address;
        stack_top += element_size;
    }

    return address & 0xFFFF;
}

inline void load_element (const mordor_u16 element, const VariableType type, Array<mordor_u16>& element_to_stack, StackEntry*& bc_stack_top, mordor_u32& stack_top) {
    /* Resolve address. */
    mordor_u16 address = get_stack_address_for_element (element, type, element_to_stack, stack_top);
    ++bc_stack_top;
    bc_stack_top->Set (type, false, address);
}




/* Operation building. */

inline void add_operation (const Operation op, vector<Operation>& operation_buffer) {
    operation_buffer.push_back (op);
}

inline Operation build_operation_I (const mordor_u16 opcode) {
    return (Operation) opcode;
}

inline Operation build_operation_P (const mordor_u16 opcode, const mordor_u16 param0) {
    return build_operation_I (opcode) | (((Operation) param0) << 16);
}

inline Operation build_operation_W (const mordor_u16 opcode, const mordor_u32 param0) {
    return build_operation_I (opcode) | (((Operation) param0) << 16);
}

inline Operation build_operation_PW (const mordor_u16 opcode, const mordor_u16 param0, const mordor_u32 param1) {
    return build_operation_I (opcode) | (((Operation) param0) << 16) | (((Operation) param1) << 32);
}


}


Function* CompileBytecodeFunction (const BytecodeFunction* func) {
    mordor_u64 time = coin::TimeNanoseconds ();

    printf ("code size: %u\n", func->code_size);
    printf ("var table size: %u\n", func->variable_table_size);
    printf ("ptr table size: %u\n", func->pointer_table_size);
    printf ("max stack size: %u\n", func->maximum_stack_size);
    printf ("operation count: %u\n", func->operation_count);
    
    /* Holds the compiled code. */
    vector<Operation> operation_buffer;
    operation_buffer.reserve (func->operation_count);

    /* The top of the internal stack. */
    mordor_u32 stack_top = 0;

    /* Maps bytecode variable ids to stack addresses. */
    Array<mordor_u16> variable_to_stack (func->variable_table_size);
    variable_to_stack.SetMemory (0xFF); /* Results in 0xFFFF. */

    /* Maps bytecode pointer ids to stack addresses. */
    Array<mordor_u16> pointer_to_stack (func->pointer_table_size);
    pointer_to_stack.SetMemory (0xFF); /* Results in 0xFFFF. */

    /* The typed Stack that is used to deduce types of variables. */
    Array<StackEntry> bc_stack (func->maximum_stack_size);
    StackEntry* bc_stack_top = bc_stack.array ();

    /* Local variables to keep the current bytecode operation.*/
    BytecodeOperation* bc_op = func->code;
    mordor_u32 bc_op_count = 0;

    /* The jump id map maps possible jump targets.
        0xFFFFFFFF -> Location not jumped to.
        0xFFFFFFFE -> Location jumped to, but position not yet resolved.

       Future use: When multiple bc operations are packed, they may not be involved in a jump instruction.
    */
    Array<mordor_u32> jump_id_map (func->operation_count);
    jump_id_map.SetMemory (0xFF); /* Results in 0xFFFFFFFF. */


    /* Build jump id map. */
    while (true) {
        mordor_u8 bc_opcode = get_opcode (bc_op);
        switch (bc_opcode) {
            _START (END) {
                goto LJumpIdMapLoopEnd;
            }

            _START (JMP) {
                _S10 (offset)
                mordor_s64 to = ((mordor_s64) bc_op_count) + offset;
                //printf ("Found JMP on %u by %i to %lli.\n",  bc_op_count, offset, to);
                jump_id_map[to] = 0xFFFFFFFE;
            } _END

            default: _END
        }

        ++bc_op;
        ++bc_op_count;
    }


  LJumpIdMapLoopEnd:
    /* Reset bytecode operation local variables. */
    mordor_u32 bc_op_number = bc_op_count;
    bc_op = func->code;
    bc_op_count = 0;

    /* Compile all the code! */
    while (true) {
        /* Set jump id. */
        mordor_u32* jump_id = jump_id_map.array () + bc_op_count;
        bool is_jumped_to = *jump_id == 0xFFFFFFFE;
        if (is_jumped_to || *jump_id != 0xFFFFFFFF) *jump_id = (mordor_u32) operation_buffer.size ();

        /* Compile bc op. */
        mordor_u8 bc_opcode = get_opcode (bc_op);
        switch (bc_opcode) {
            _START (END) {
                goto LCompileLoopEnd;
            }

            _START (JMP) {
                _S10 (offset)
                Operation op = build_operation_I (OP_kJMP);
                op = op | ((((Operation) (((mordor_s64) bc_op_count) + offset)) << 16) & 0xFFFFFFFF); /* Will be >= 0, so shift left is okay. */
                add_operation (op, operation_buffer);
            } _END

            _START (RETURN) {
                if (bc_stack_top->constant) {
                    /* Set variable etc. */
                    add_operation (build_operation_PW (OP_kMOV, stack_top, bc_stack_top->id), operation_buffer);
                    add_operation (build_operation_P (OP_RET, stack_top), operation_buffer);
                    stack_top += 4; /* TODO(Marco)/Maybe: Map this to the actual constant! */
                }else {
                    if ((bc_stack_top->type & TYPE_IS_LONG) > 0) {
                        add_operation (build_operation_P (OP_RETl, bc_stack_top->id), operation_buffer);
                    }else {
                        add_operation (build_operation_P (OP_RET, bc_stack_top->id), operation_buffer);
                    }
                }
                bc_stack_top--;
            } _END

            _START (RETURN_VOID) { /* This should only be used when it is not the last operation. Otherwise superfluous. */
                add_operation (build_operation_I (OP_RET_VOID), operation_buffer);
            } _END

            _START (iLOAD) {
                _U10 (variable)
                load_element (variable, TYPE_I, variable_to_stack, bc_stack_top, stack_top);
            } _END

            _START (lLOAD) {
                _U10 (variable)
                load_element (variable, TYPE_I | TYPE_IS_LONG, variable_to_stack, bc_stack_top, stack_top);
            } _END

            _START (uiLOAD) {
                _U10 (variable)
                load_element (variable, TYPE_U, variable_to_stack, bc_stack_top, stack_top);
            } _END

            _START (ulLOAD) {
                _U10 (variable)
                load_element (variable, TYPE_U | TYPE_IS_LONG, variable_to_stack, bc_stack_top, stack_top);
            } _END

            _START (fLOAD) {
                _U10 (variable)
                load_element (variable, TYPE_F, variable_to_stack, bc_stack_top, stack_top);
            } _END

            _START (flLOAD) {
                _U10 (variable)
                load_element (variable, TYPE_F | TYPE_IS_LONG, variable_to_stack, bc_stack_top, stack_top);
            } _END

            _START (pLOAD) {
                _U10 (pointer)
                load_element (pointer, TYPE_P, pointer_to_stack, bc_stack_top, stack_top);
            } _END

            _START (CONST) {
                _U10 (constant)
                ++bc_stack_top;
                bc_stack_top->Set (TYPE_U, true, constant);
            }
        }

        ++bc_op;
        ++bc_op_count;
    }


  LCompileLoopEnd:
    /* Add END operation that is required both for interpreter and further compilation. */
    add_operation (build_operation_I (OP_END), operation_buffer); 

    /* Resolve all jumps. */
    int i = 0;
    while (true) {
        Operation op = operation_buffer[i];

        switch (op & 0xFFFF) {
          case OP_END:
            goto LJumpResolveLoopEnd;   

          case OP_kJMP:
            mordor_u32 position = jump_id_map[(op >> 16) & 0xFFFFFFFF];
            if (position == 0xFFFFFFFF) {
                printf ("Error: Jump location invalid!\n");
                return NULL;
            }else if (position == 0xFFFFFFFE) {
                printf ("Error: Possible jump location not set!\n");
                return NULL;
            }else {
                printf ("New Jump location found! %i jumps to %u! by %i\n", i, position, (mordor_s32) ((((mordor_s64) position) - i) * sizeof(Operation)));
                operation_buffer[i] = build_operation_W (OP_kJMP, (mordor_s32) ((((mordor_s64) position) - (mordor_s64) i) * sizeof(Operation)));
                printf ("Op: %llX\n", op);
            }
            break;
        }
        ++i;
    }

  LJumpResolveLoopEnd:
    /* Create function. */
    Function* function = new Function ();
    function->stack_size = stack_top;
    size_t size = operation_buffer.size ();
    //printf ("Function size is %u!\n", size);
    function->operations = new Operation[operation_buffer.size ()];
    memcpy (function->operations, &operation_buffer[0], size * sizeof (Operation));

    time = coin::TimeNanoseconds () - time;
    printf ("Compilation took %lluns!\n", time);

    printf ("\n\n");
    for (int i = 0; i < size; ++i) {
        printf ("%llX\n", function->operations[i]);
    }
    printf ("\n\n");

    return function;
}

}
