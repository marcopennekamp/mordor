#include <vector>
#include <map>

#include <coin/utils/time.h>
#include <coin/utils/Array.h>

#include <mdr/def/BytecodeOperation.h>
#include <mdr/api/Type.h>
#include <mdr/runtime/BytecodeFunction.h>
#include <mdr/runtime/Function.h>
#include <mdr/runtime/Environment.h>
#include <mdr/runtime/NativeFunction.h>
#include <mdr/runtime/Function.h>

using namespace std;


#define _START(NAME)    case BCOP_ ## NAME :
#define _CASE(NAME)     case BCOP_ ## NAME : 
#define _END            break;

/* extracts 10 bit parameter, increments op by one. */
#define _U10(P0)        mdr_u16 P0; get_u10 (bc_op, P0);
#define _S10(P0)        mdr_s16 P0; get_s10 (bc_op, P0);


namespace mdr {

namespace {

const mdr_u32 kPointerSize = sizeof (void*);

struct StackEntry {
    mdrType type_;
    bool is_constant_;

    /* Variable address or if constant == true, constant. */
    union {
        mdr_u16 id_;
        BytecodeFunction::Constant constant_;
    };

private:
    void _Set (const mdrType type, const bool is_constant) {
        type_ = type;
        is_constant_ = is_constant;
    }

public:
    void Set (const mdrType type, const mdr_u16 id) {
        _Set (type, false);
        id_ = id;
    }

    void Set (const BytecodeFunction::Constant& constant) {
        _Set (constant.type, true);
        constant_ = constant;
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


inline void get_type_info (const mdrType type, mdr_u32& size, bool& is_pointer) {
    if (type == MDR_TYPE_PTR) {
        size = kPointerSize;
        is_pointer = true;
    }else {
        size = mdrTypeGetSize (type);
        is_pointer = false;
    }
}

inline mdr_u32 get_type_size (const mdrType type) {
    mdr_u32 size;
    bool is_pointer;
    get_type_info (type, size, is_pointer);
    return size;
}

inline bool needs_l_operation (const mdrType type) {
    return get_type_size (type) == 8;
}

/**
 * Returns an address on the stack for the given variable or pointer.
 * May or may not reserve stack space.
 */
inline mdr_u16 get_stack_address_for_element (const mdr_u32 element, const mdrType type, coin::Array<mdr_u16>& element_to_stack, mdr_u32& stack_top) {
    mdr_u16 address = element_to_stack[element];

    /* Reserve address if needed. */
    if (address == 0xFFFF) {
        /* Compute element size. */
        mdr_u32 element_size;
        bool is_pointer;
        get_type_info (type, element_size, is_pointer);

        /* Check whether local stack is not exceeded. */
        if (stack_top + element_size >= 0xFFFF) {
            printf ("Error: Maximum stack size exceeded!\n");
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

inline void load_element (const mdr_u16 element, const mdrType type, coin::Array<mdr_u16>& element_to_stack, StackEntry*& bc_stack_top, mdr_u32& stack_top) {
    /* Resolve address. */
    mdr_u16 address = get_stack_address_for_element (element, type, element_to_stack, stack_top);
    bc_stack_top->Set (type, address);
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

inline mdr_u32 add_constant_u64 (vector<BytecodeFunction::Constant>& constants_u64, BytecodeFunction::Constant& constant) {
    mdr_u32 id = (mdr_u32) constants_u64.size () * 8;
    constants_u64.push_back (constant);
    return id;
}


inline void build_call_parameters (const mdr_u32 parameter_count, const mdrOperationType kOpcode, const mdrOperationType kOpcodel, 
                                    StackEntry*& bc_stack_top, mdr_u32& stack_top, vector<mdrOperation>& operation_buffer,
                                    vector<BytecodeFunction::Constant>& constants_u64) {
    StackEntry* end = bc_stack_top;
    StackEntry* it = bc_stack_top - parameter_count;
    mdr_u32 offset = 0;
    for (; it != end; ++it) {
        mdr_u32 size = get_type_size (it->type_);
        mdrOperationType opcode = (size == 8) ? kOpcodel : kOpcode;
        if (kOpcode == OP_NPUSH) size = 8; /* Native stack is 8 byte aligned. */

        if (it->is_constant_) {
            if (size == 4) {
                add_operation (build_operation_PW (OP_kMOV, stack_top, it->constant_.value._u32), operation_buffer);
            }else {
                add_operation (build_operation_PW (OP_kMOVl, stack_top, add_constant_u64 (constants_u64, it->constant_)), operation_buffer);
            }
            add_operation (build_operation_PP (opcode, offset, stack_top), operation_buffer);
            stack_top += size;
        }else { /* Push variable. */
            add_operation (build_operation_PP (opcode, it->id_, offset), operation_buffer);
        }
        offset += size;
    }
    bc_stack_top -= parameter_count; /* Reverse stack. */
}


inline void fetch_return_value (const mdrType return_type, StackEntry*& bc_stack_top, mdr_u32& stack_top, vector<mdrOperation>& operation_buffer) {
    mdr_u32 return_size = get_type_size (return_type);
    mdrOperationType opcode = (return_size == 8) ? OP_RETMOVl : OP_RETMOV;
    add_operation (build_operation_PP (opcode, stack_top, 0), operation_buffer);
    bc_stack_top->Set (return_type, stack_top);
    ++bc_stack_top;
    stack_top += return_size;
}


inline void build_function_constant_table (Function::ConstantTable& constant_table, vector<BytecodeFunction::Constant>& constants_u64) {
    size_t size = constants_u64.size ();
    for (size_t i = 0; i < size; ++i) {
        BytecodeFunction::Constant& constant = constants_u64[i];
        constant_table[i] = constant.value._u64;
    }
}

}


bool Environment::CompileBytecodeFunction (BytecodeFunction* func) {
    printf ("Compiling function '%s'.\n", func->name ().c_str ());

    mdr_u64 time = coin::TimeNanoseconds ();

    // TODO(Marco): Idea: Keep track of tmp stack variables and invalidate them.

    /* Holds the compiled code. */
    vector<mdrOperation> operation_buffer;
    operation_buffer.reserve (func->operation_count ());

    /* The top of the internal stack. */
    mdr_u32 stack_top = 0;

    /* Maps bytecode variable ids to stack addresses. */
    coin::Array<mdr_u16> variable_to_stack (func->variable_table_size ());
    variable_to_stack.SetMemory (0xFF); /* Results in 0xFFFF. */

    /* Maps bytecode pointer ids to stack addresses. */
    coin::Array<mdr_u16> pointer_to_stack (func->pointer_table_size ());
    pointer_to_stack.SetMemory (0xFF); /* Results in 0xFFFF. */

    /* The typed Stack that is used to deduce types of variables. */
    coin::Array<StackEntry> bc_stack (func->maximum_stack_size ());
    StackEntry* bc_stack_top = bc_stack.array ();

    /* Local variables to keep the current bytecode operation.*/
    mdrBytecodeOperation* bc_op = func->code ();
    mdr_u32 bc_op_count = 0; /* Op count for jump translation. */

    /* The constants for the u64 constant table of the IR function. */
    vector<BytecodeFunction::Constant> constants_u64;

    /* The jump id map maps possible jump targets.
        0xFFFFFFFF -> Location not jumped to.
        0xFFFFFFFE -> Location jumped to, but position not yet resolved.

       Future use: When multiple bc operations are packed, they may not be involved in a jump instruction.
    */
    coin::Array<mdr_u32> jump_id_map (func->operation_count ());
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
    bc_op = func->code ();
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
                if (entry->is_constant_) {
                    mdr_u8 type_size = mdrTypeGetSize (entry->type_);
                    mdrOperationType ret_opcode;
                    if (type_size == 4) {
                        add_operation (build_operation_PW (OP_kMOV, stack_top, entry->constant_.value._u32), operation_buffer);
                        ret_opcode = OP_RET;
                    }else {
                        add_operation (build_operation_PW (OP_kMOVl, stack_top, add_constant_u64 (constants_u64, entry->constant_)), operation_buffer);
                        ret_opcode = OP_RETl;
                    }
                    add_operation (build_operation_P (ret_opcode, stack_top), operation_buffer);
                    stack_top += type_size; /* TODO(Marco)/Maybe: Map this to the actual constant! */
                }else {
                    mdrOperationType opcode = (needs_l_operation (entry->type_)) ? OP_RETl : OP_RET;
                    add_operation (build_operation_P (opcode, entry->id_), operation_buffer);
                }
                --bc_stack_top;
            } _END

            _START (RETURN_VOID) { /* This should only be used when it is not the last operation. Otherwise superfluous. */
                add_operation (build_operation_I (OP_RET_VOID), operation_buffer);
            } _END

            _START (iLOAD) {
                _U10 (variable)
                load_element (variable, MDR_TYPE_I32, variable_to_stack, bc_stack_top, stack_top);
            } _END

            _START (lLOAD) {
                _U10 (variable)
                load_element (variable, MDR_TYPE_I64, variable_to_stack, bc_stack_top, stack_top);
            } _END

            _START (uiLOAD) {
                _U10 (variable)
                load_element (variable, MDR_TYPE_U32, variable_to_stack, bc_stack_top, stack_top);
            } _END

            _START (ulLOAD) {
                _U10 (variable)
                load_element (variable, MDR_TYPE_U64, variable_to_stack, bc_stack_top, stack_top);
            } _END

            _START (fLOAD) {
                _U10 (variable)
                load_element (variable, MDR_TYPE_F32, variable_to_stack, bc_stack_top, stack_top);
            } _END

            _START (flLOAD) {
                _U10 (variable)
                load_element (variable, MDR_TYPE_F64, variable_to_stack, bc_stack_top, stack_top);
            } _END

            _START (pLOAD) {
                _U10 (pointer)
                load_element (pointer, MDR_TYPE_PTR, pointer_to_stack, bc_stack_top, stack_top);
            } _END

            _START (kLOAD) {
                _U10 (constant_id)
                BytecodeFunction::Constant& constant = func->constant_table ().array ()[constant_id];
                bc_stack_top->Set (constant);
                ++bc_stack_top;
            } _END

            _START (CALL) {
                _U10 (function_name_id)
                /* Get function name and local id. */
                string& name = func->name_table ()[function_name_id];
                mdr_u32 id = GetFunctionId (name);
                if (id == Function::kInvalidId) {
                    printf ("Error: Called function '%s' could not be found! Missing dependency?", name.c_str ());
                    return false;
                }

                Function* callee = GetFunction (id);

                /* Push variables in lowest to highest stack order. */
                build_call_parameters (callee->cpinfo ().parameter_count_, OP_PUSH, OP_PUSHl, bc_stack_top, stack_top, operation_buffer, constants_u64);

                /* Add CALL instruction. */
                add_operation (build_operation_W (OP_CALL, id), operation_buffer);

                /* If function returns something, put that on the stack and create a bc stack entry! */
                if (callee->cpinfo ().return_type_ != MDR_TYPE_VOID) {
                    fetch_return_value (callee->cpinfo ().return_type_, bc_stack_top, stack_top, operation_buffer);
                }
            } _END

            _START (NCALL) {
                _U10 (function_name_id)
                mdr_u32 function_id = GetNativeFunctionId (func->name_table ()[function_name_id]);
                NativeFunction* function = GetNativeFunction (function_id);
                
                mdrOperationType op = OP_END;
                switch (function->return_type ()) {
                    case MDR_TYPE_VOID:
                    case MDR_TYPE_U32:
                        op = OP_CALL_NATIVE_U32;
                        break;

                    default:
                        printf ("Error: Unexpected type when calling native function!\n");
                        break;
                }

                if (op != OP_END) {
                    // TODO(Marco): Check for NULL?

                    /* Similar to CALL. */
                    build_call_parameters (function->parameter_count (), OP_NPUSH, OP_NPUSHl, bc_stack_top, stack_top, operation_buffer, constants_u64);
                    add_operation (build_operation_W (op, function_id), operation_buffer);
                    if (function->return_type () != MDR_TYPE_VOID) {
                        fetch_return_value (function->return_type (), bc_stack_top, stack_top, operation_buffer);
                    }
                }
            } _END

            _START (ADD) {
                StackEntry* var1 = --bc_stack_top;
                StackEntry* var0 = bc_stack_top - 1;

                if (var0->type_ != var1->type_) {
                    printf ("Error: Types for add must be the same!");
                    return false;
                }

                /* Put constant on stack. No need to evaluate this. (var0 becomes the new constant). */
                if (var0->is_constant_ && var1->is_constant_) {
                    switch (var0->type_) {
                    case MDR_TYPE_I32:
                    case MDR_TYPE_U32:
                        var0->constant_.value._u32 += var1->constant_.value._u32;
                        break;

                    case MDR_TYPE_I64:
                    case MDR_TYPE_U64:
                        var0->constant_.value._u64 += var1->constant_.value._u64;
                        break;

                    case MDR_TYPE_F32:
                        var0->constant_.value._f32 += var1->constant_.value._f32;
                        break;

                    case MDR_TYPE_F64:
                        var0->constant_.value._f64 += var1->constant_.value._f64;
                        break;
                    }
                }else if (var0->is_constant_ || var1->is_constant_) { /* Add constant to stack value. */
                    BytecodeFunction::Constant constant;
                    mdr_u16 id;
                    if (var0->is_constant_) {
                        constant = var0->constant_;
                        id = var1->id_;
                    }else {
                        constant = var1->constant_;
                        id = var0->id_;
                    }
                    
                    switch (var0->type_) {
                    case MDR_TYPE_I32:
                    case MDR_TYPE_U32:
                        add_operation (build_operation_PW (OP_kADD, id, constant.value._u32), operation_buffer);
                        break;

                    case MDR_TYPE_I64:
                    case MDR_TYPE_U64: {
                        add_operation (build_operation_PW (OP_kADDl, id, add_constant_u64 (constants_u64, constant)), operation_buffer);
                    } break;

                    default:
                        printf ("Error: Type '%u' not supported by add.", var0->type_);
                        return false;
                    }

                    /* var0 is the result now. */
                    var0->is_constant_ = false;
                    var0->id_ = id;
                    // stack_top += mdrTypeGetSize (var0->type_);
                }else {
                    mdr_u16 add_opcode;
                    switch (var0->type_) {
                    case MDR_TYPE_I32:
                    case MDR_TYPE_U32:
                        add_opcode = OP_ADD;
                        break;

                    case MDR_TYPE_I64:
                    case MDR_TYPE_U64:
                        add_opcode = OP_ADDl;
                        break;

                    default:
                        printf ("Error: Type '%u' not supported by add.", var0->type_);
                        return false;
                    }

                    mdr_u8 type_size = mdrTypeGetSize (var0->type_);
                    mdr_u16 mov_opcode = (type_size == 8) ? OP_MOVl : OP_MOV;

                    add_operation (build_operation_PP (mov_opcode, stack_top, var0->id_), operation_buffer);
                    add_operation (build_operation_PP (add_opcode, stack_top, var1->id_), operation_buffer);
                    var0->Set (var0->type_, stack_top);
                    stack_top += type_size;
                }
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
                return false;
            }else if (position == 0xFFFFFFFE) {
                printf ("Error: Possible jump location not set!\n");
                return false;
            }else {
                // printf ("New Jump location found! %i jumps to %u! by %i\n", i, position, (mordor_s32) ((((mordor_s64) position) - i) * sizeof(Operation)));
                operation_buffer[i] = build_operation_W (OP_kJMP, (mdr_s32) ((((mdr_s64) position) - (mdr_s64) i) * sizeof(mdrOperation)));
            }
            break;
        }

        ++i;
    }

  LJumpResolveLoopEnd:
    /* Allocate function. */
    Function* function = GetFunction (func->name ());
    mdr_u32 size = (mdr_u32) operation_buffer.size ();
    function->Allocate (size, constants_u64.size ());
    function->stack_size (stack_top);
    memcpy (function->operations (), &operation_buffer[0], size * sizeof (mdrOperation));

    build_function_constant_table (function->constant_table_object (), constants_u64);

    time = coin::TimeNanoseconds () - time;

    for (size_t i = 0; i < operation_buffer.size (); ++i) {
        printf ("%llX\n", operation_buffer[i]);
    }

    printf ("Compilation took %lluns!\n", time);
    printf ("\n");

    return true;
}

}