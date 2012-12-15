#include <vector>
#include <map>

#include <boost/scoped_ptr.hpp>
#include <internal/utils/Array.h>

#include <coin/utils/time.h>

#include <mordor/bytecode/BytecodeOperation.h>
#include <mordor/bytecode/VariableType.h>

#include <internal/bytecode/compile.h>
#include <internal/bytecode/BytecodeFunction.h>
#include <internal/runtime/Function.h>
#include <internal/runtime/Environment.h>
#include <internal/runtime/Program.h>

using namespace std;
using namespace boost;


#define _START(NAME)    case BCOP_ ## NAME :
#define _CASE(NAME)     case BCOP_ ## NAME : 
#define _END            break;

/* extracts 10 bit parameter, increments op by one. */
#define _U10(P0)        mdr_u16 P0; get_u10 (bc_op, P0);
#define _S10(P0)        mdr_s16 P0; get_s10 (bc_op, P0);


namespace mordor {

namespace {

const mdr_u32 kPointerSize = sizeof (void*);

struct StackEntry {
    mdrVariableType type;
    bool constant;
    mdr_u16 id; /* Variable address or if constant == true, constant table id. */

    void Set (const mdrVariableType _type, const bool _constant, const mdr_u16 _id) {
        type = _type;
        constant = _constant;
        id = _id;
    }
};


inline mdr_u8 get_opcode (const mdrBytecodeOperation* ptr) {
    return (*ptr & 0xFC) >> 2;
}

inline void get_u10 (const mdrBytecodeOperation* ptr, mdr_u16& param0) {
    mdr_u16 v0 = *ptr & 0x03;
    mdr_u16 v1 = *(ptr + 1) & 0xFF;
    param0 = v0 << 8 | v1;
}

inline void get_s10 (const mdrBytecodeOperation* ptr, mdr_s16& param0) {
    mdr_u16 low = *(ptr + 1) & 0xFF;
    mdr_u16 sign = *ptr & 0x02;

    if (sign > 0) { /* negative number. */
        mdr_u16 high = *ptr & 0x01;
        param0 = -(high << 8 | low);
    }else { /* positive number. */
        mdr_u16 high = *ptr & 0x01;
        param0 = high << 8 | low;
    }
}


inline void get_type_info (const mdrVariableType type, mdr_u32& size, bool& is_pointer) {
    if ((type & MDR_VARTYPE_P) > 0) {
        size = kPointerSize;
        is_pointer = true;
    }else {
        size = ((type & MDR_VARTYPE_IS_LONG) > 0) ? 8 : 4;
        is_pointer = false;
    }
}

inline mdr_u32 get_type_size (const mdrVariableType type) {
    mdr_u32 size;
    bool is_pointer;
    get_type_info (type, size, is_pointer);
    return size;
}

inline bool needs_l_operation (const mdrVariableType type) {
    return get_type_size (type) == 8;
}

/**
 * Returns an address on the stack for the given variable or pointer.
 * May or may not reserve stack space.
 */
inline mdr_u16 get_stack_address_for_element (const mdr_u32 element, const mdrVariableType type, Array<mdr_u16>& element_to_stack, mdr_u32& stack_top) {
    mdr_u16 address = element_to_stack[element];

    /* Reserve address if needed. */
    if (address == 0xFFFF) {
        /* Compute element size. */
        mdr_u32 element_size;
        bool is_pointer;
        get_type_info (type, element_size, is_pointer);

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

inline void load_element (const mdr_u16 element, const mdrVariableType type, Array<mdr_u16>& element_to_stack, StackEntry*& bc_stack_top, mdr_u32& stack_top) {
    /* Resolve address. */
    mdr_u16 address = get_stack_address_for_element (element, type, element_to_stack, stack_top);
    bc_stack_top->Set (type, false, address);
    ++bc_stack_top;
}




/* Operation building. */

inline void add_operation (const mdrOperation op, vector<mdrOperation>& operation_buffer) {
    operation_buffer.push_back (op);
}

inline mdrOperation build_operation_I (const mdr_u16 opcode) {
    return (mdrOperation) opcode;
}

inline mdrOperation build_operation_P (const mdr_u16 opcode, const mdr_u16 param0) {
    return build_operation_I (opcode) | (((mdrOperation) param0) << 16);
}

inline mdrOperation build_operation_PP (const mdr_u16 opcode, const mdr_u16 param0, const mdr_u16 param1) {
    return build_operation_I (opcode) | (((mdrOperation) param0) << 16) | (((mdrOperation) param1) << 32);
}

inline mdrOperation build_operation_W (const mdr_u16 opcode, const mdr_u32 param0) {
    return build_operation_I (opcode) | (((mdrOperation) param0) << 16);
}

inline mdrOperation build_operation_PW (const mdr_u16 opcode, const mdr_u16 param0, const mdr_u32 param1) {
    return build_operation_I (opcode) | (((mdrOperation) param0) << 16) | (((mdrOperation) param1) << 32);
}


}


Function* CompileBytecodeFunction (const BytecodeFunction* func, Environment* environment, Program* program) {
    mdr_u64 time = coin::TimeNanoseconds ();

    // TODO(Marco): Idea: Keep track of tmp stack variables and invalidate them

    /* Holds the compiled code. */
    vector<mdrOperation> operation_buffer;
    operation_buffer.reserve (func->operation_count);

    /* The top of the internal stack. */
    mdr_u32 stack_top = 0;

    /* Maps bytecode variable ids to stack addresses. */
    Array<mdr_u16> variable_to_stack (func->variable_table_size);
    variable_to_stack.SetMemory (0xFF); /* Results in 0xFFFF. */

    /* Maps bytecode pointer ids to stack addresses. */
    Array<mdr_u16> pointer_to_stack (func->pointer_table_size);
    pointer_to_stack.SetMemory (0xFF); /* Results in 0xFFFF. */

    /* The typed Stack that is used to deduce types of variables. */
    Array<StackEntry> bc_stack (func->maximum_stack_size);
    StackEntry* bc_stack_top = bc_stack.array ();

    /* Local variables to keep the current bytecode operation.*/
    mdrBytecodeOperation* bc_op = func->code;
    mdr_u32 bc_op_count = 0;

    /* The jump id map maps possible jump targets.
        0xFFFFFFFF -> Location not jumped to.
        0xFFFFFFFE -> Location jumped to, but position not yet resolved.

       Future use: When multiple bc operations are packed, they may not be involved in a jump instruction.
    */
    Array<mdr_u32> jump_id_map (func->operation_count);
    jump_id_map.SetMemory (0xFF); /* Results in 0xFFFFFFFF. */

    /* Build jump id map. */
    while (true) {
        mdr_u8 bc_opcode = get_opcode (bc_op);

        switch (bc_opcode) {
            _START (END) {
                goto LJumpIdMapLoopEnd;
            }

            _START (JMP) {
                _S10 (offset)
                mdr_s64 to = ((mdr_s64) bc_op_count) + offset;
                // printf ("Found JMP on %u by %i to %lli.\n",  bc_op_count, offset, to);
                jump_id_map[to] = 0xFFFFFFFE;
            } _END

            default: _END
        }

        bc_op += BCOP_SIZE[bc_opcode];
        ++bc_op_count;
    }


  LJumpIdMapLoopEnd:
    /* Reset bytecode operation local variables. */
    mdr_u32 bc_op_number = bc_op_count;
    bc_op = func->code;
    bc_op_count = 0;

    /* Compile all the code! */
    while (true) {
        /* Set jump id. */
        mdr_u32* jump_id = jump_id_map.array () + bc_op_count;
        bool is_jumped_to = *jump_id == 0xFFFFFFFE;
        if (is_jumped_to || *jump_id != 0xFFFFFFFF) *jump_id = (mdr_u32) operation_buffer.size ();

        /* Compile bc op. */
        mdr_u8 bc_opcode = get_opcode (bc_op);
        switch (bc_opcode) {
            _START (END) {
                goto LCompileLoopEnd;
            }

            _START (JMP) {
                _S10 (offset)
                mdrOperation op = build_operation_I (OP_kJMP);
                op = op | ((((mdrOperation) (((mdr_s64) bc_op_count) + offset)) << 16) & 0xFFFFFFFF); /* Will be >= 0, so shift left is okay. */
                add_operation (op, operation_buffer);
            } _END

            _START (RETURN) {
                StackEntry* entry = bc_stack_top - 1;
                if (entry->constant) {
                    /* Set variable etc. */
                    add_operation (build_operation_PW (OP_kMOV, stack_top, entry->id), operation_buffer);
                    add_operation (build_operation_P (OP_RET, stack_top), operation_buffer);
                    stack_top += 4; /* TODO(Marco)/Maybe: Map this to the actual constant! */
                }else {
                    mdrOperationType opcode = (needs_l_operation (entry->type)) ? OP_RETl : OP_RET;
                    add_operation (build_operation_P (opcode, entry->id), operation_buffer);
                }
                --bc_stack_top;
            } _END

            _START (RETURN_VOID) { /* This should only be used when it is not the last operation. Otherwise superfluous. */
                add_operation (build_operation_I (OP_RET_VOID), operation_buffer);
            } _END

            _START (iLOAD) {
                _U10 (variable)
                load_element (variable, MDR_VARTYPE_I, variable_to_stack, bc_stack_top, stack_top);
            } _END

            _START (lLOAD) {
                _U10 (variable)
                load_element (variable, MDR_VARTYPE_I | MDR_VARTYPE_IS_LONG, variable_to_stack, bc_stack_top, stack_top);
            } _END

            _START (uiLOAD) {
                _U10 (variable)
                load_element (variable, MDR_VARTYPE_U, variable_to_stack, bc_stack_top, stack_top);
            } _END

            _START (ulLOAD) {
                _U10 (variable)
                load_element (variable, MDR_VARTYPE_U | MDR_VARTYPE_IS_LONG, variable_to_stack, bc_stack_top, stack_top);
            } _END

            _START (fLOAD) {
                _U10 (variable)
                load_element (variable, MDR_VARTYPE_F, variable_to_stack, bc_stack_top, stack_top);
            } _END

            _START (flLOAD) {
                _U10 (variable)
                load_element (variable, MDR_VARTYPE_F | MDR_VARTYPE_IS_LONG, variable_to_stack, bc_stack_top, stack_top);
            } _END

            _START (pLOAD) {
                _U10 (pointer)
                load_element (pointer, MDR_VARTYPE_P, pointer_to_stack, bc_stack_top, stack_top);
            } _END

            _START (CONST) {
                _U10 (constant)
                bc_stack_top->Set (MDR_VARTYPE_U, true, constant);
                ++bc_stack_top;
            } _END

            _START (CALL) {
                _U10 (function_name_id)
                /* Get function name and local id. */
                BytecodeFunction* callee = NULL;
                string& name = func->function_name_table[function_name_id];
                mdr_u32 id = program->GetFunctionId (name);

                if (id != Program::INVALID_FUNCTION_ID) {
                    callee = program->bytecode_function_cache ()[id];
                }else { 
                    /* Not found in normal functions, get from function resolve cache. */
                    id = program->GetFunctionIdFromResolveCache (name);

                    /* Get the BytecodeFunction elsewhere. */
                    callee = environment->FindBytecodeFunction (name);

                    // TODO(Marco): Cache BytecodeFunction!
                }

                if (callee == NULL) {
                    printf ("Error: Called function '%s' could not be found! Missing dependency?", name.c_str ());
                    return NULL;
                }

                /* Push variables in lowest to highest stack order. */
                StackEntry* end = bc_stack_top;
                StackEntry* it = bc_stack_top - callee->parameter_count;
                mdr_u32 offset = 0;
                for (; it != end; ++it) {
                    mdr_u32 size = get_type_size (it->type);
                    mdrOperationType opcode = (size == 8) ? OP_PUSHl : OP_PUSH;
                    if (it->constant) {
                        // TODO(Marco): Push constant properly.
                        add_operation (build_operation_PW (OP_kMOV, stack_top, it->id), operation_buffer);
                        add_operation (build_operation_PP (opcode, stack_top, offset), operation_buffer);
                        stack_top += size;
                        offset += size;
                    }else { /* Push variable. */
                        add_operation (build_operation_PP (opcode, it->id, offset), operation_buffer);
                        offset += size;
                    }
                }
                bc_stack_top -= callee->parameter_count; /* Reverse stack. */
                
                /* Add CALL instruction. */
                add_operation (build_operation_W (OP_CALL, id), operation_buffer);

                /* If function returns something, put that on the stack and create a bc stack entry! */
                if (callee->return_type != MDR_VARTYPE_V) {
                    mdr_u32 return_size = get_type_size (callee->return_type);
                    mdrOperationType opcode = (return_size == 8) ? OP_RETMOVl : OP_RETMOV;
                    add_operation (build_operation_PP (opcode, stack_top, 0), operation_buffer);
                    bc_stack_top->Set (callee->return_type, false, stack_top);
                    ++bc_stack_top;
                    stack_top += return_size;
                }
            } _END

            _START (ADD) {
                // TODO(Marco): Implement "real" add.
                StackEntry* var0 = --bc_stack_top;
                StackEntry* var1 = bc_stack_top - 1;
                add_operation (build_operation_PP (OP_MOV, stack_top, var0->id), operation_buffer);
                add_operation (build_operation_PP (OP_ADD, stack_top, var1->id), operation_buffer);
                var1->Set (MDR_VARTYPE_I, false, stack_top);
                stack_top += 4;
            } _END
        }

        bc_op += BCOP_SIZE[bc_opcode];
        ++bc_op_count;
    }


  LCompileLoopEnd:
    /* Add END operation that is required both for interpreter and further compilation. */
    add_operation (build_operation_I (OP_END), operation_buffer); 

    /* Resolve all jumps. */
    int i = 0;
    while (true) {
        mdrOperation op = operation_buffer[i];

        switch (op & 0xFFFF) {
          case OP_END:
            goto LJumpResolveLoopEnd;   

          case OP_kJMP:
            mdr_u32 position = jump_id_map[(op >> 16) & 0xFFFFFFFF];
            if (position == 0xFFFFFFFF) {
                printf ("Error: Jump location invalid!\n");
                return NULL;
            }else if (position == 0xFFFFFFFE) {
                printf ("Error: Possible jump location not set!\n");
                return NULL;
            }else {
                // printf ("New Jump location found! %i jumps to %u! by %i\n", i, position, (mordor_s32) ((((mordor_s64) position) - i) * sizeof(Operation)));
                operation_buffer[i] = build_operation_W (OP_kJMP, (mdr_s32) ((((mdr_s64) position) - (mdr_s64) i) * sizeof(mdrOperation)));
            }
            break;
        }
        ++i;
    }

  LJumpResolveLoopEnd:
    /* Create function. */
    Function* function = new Function ();
    function->stack_size = stack_top;
    function->program = program;
    size_t size = operation_buffer.size ();
    //printf ("Function size is %u!\n", size);
    function->operations = new mdrOperation[operation_buffer.size ()];
    memcpy (function->operations, &operation_buffer[0], size * sizeof (mdrOperation));

    time = coin::TimeNanoseconds () - time;
    printf ("Compilation took %lluns!\n", time);
    
    return function;
}

}
