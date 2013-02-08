// TODO(Marco): This slowly became quite a mess. It should be modularized a little bit more.

#include <map>
#include <string>

#include <coin/utils/Stream.h>
#include <coin/utils/directory.h>

#include <mordor/bytecode/BytecodeOperation.h>
#include <mordor/bytecode/Type.h>

#include "main.h"


using namespace std;
using namespace coin;


namespace mordor {
namespace assembler {

namespace {

const char* kTypeNames [] = {
    "i32",
    "u32",
    "f32",
    "i64",
    "u64",
    "f64",
    "ptr",
    "void"
};
const size_t kTypeNamesLength = 8;

const mdrType kTypes [] = {
    MDR_TYPE_I32,
    MDR_TYPE_U32,
    MDR_TYPE_F32,
    MDR_TYPE_I64,
    MDR_TYPE_U64,
    MDR_TYPE_F64,
    MDR_TYPE_PTR,
    MDR_TYPE_VOID
};


enum OperationType {
    kOp_jmp = 0x00,
    kOp_return,
    kOp_load,
    kOp_store,

    kOp_pop,

    kOp_inc,
    kOp_add,
    kOp_sub,
    kOp_mul,
    kOp_div,
    kOp_rem,
    kOp_neg,
    kOp_shl,
    kOp_shr,
    kOp_and,
    kOp_or,
    kOp_xor,
    kOp_not,

    kOp_call,

    kOp_INVALID
};

const char* kOperationTypeNames [] = {
    "jmp",
    "return",
    "load",
    "store",

    "pop",

    "inc",
    "add",
    "sub",
    "mul",
    "div",
    "rem",
    "neg",
    "shl",
    "shr",
    "and",
    "or",
    "xor",
    "not",

    "call"
};
const size_t kOperationTypeNamesLength = 19;

struct Operation {
    mdrBytecodeOperationType type;
    mdr_u16 param0; /* High or number. */
    mdr_u16 param1; /* Low. */
};


struct Variable {
    mdrType type;
    string name;
    mdr_u16 index;
};

struct Label {
    mdr_u16 index;
    mdr_u32 position;
};


/* Holds all the data that is saved when compiling a function. */
struct FunctionCompileData {
    string name;
    mdrType return_type;
    mdr_u16 variable_table_next_index;
    mdr_u16 pointer_table_next_index;
    mdr_u16 current_stack_size;
    mdr_u16 max_stack_size;
    mdr_u16 labels_next_index; /* Labels get an intermediate index so they can be used before being declared at a determined position. */
    map<string, Label*> labels;
    map<string, Variable*> variables;
    vector<Variable*> parameter_list;
    vector<Operation> operations;

    FunctionCompileData () {
        return_type = MDR_TYPE_VOID;
        variable_table_next_index = 0;
        pointer_table_next_index = 0;
        current_stack_size = 0;
        max_stack_size = 0;
        labels_next_index = 0;
    }

    void stack_size (mdr_s16 diff) {
        current_stack_size += diff;
        if (current_stack_size > max_stack_size) {
            max_stack_size = current_stack_size;
        }
    }

    ~FunctionCompileData () {
        // TODO(Marco): Delete all variables and labels.
    }
};


mdrType getTypeFromString (const char* str) {
    mdrType type = MDR_TYPE_VOID;
    for (int i = 0; i < kTypeNamesLength; ++i) {
        if (strcmp (str, kTypeNames[i]) == 0) {
            type = kTypes[i];
        }
    }
    return type;
}

OperationType getOperationTypeFromString (const char* str) {
    OperationType op = kOp_INVALID;
    for (int i = 0; i < kOperationTypeNamesLength; ++i) {
        if (strcmp (str, kOperationTypeNames[i]) == 0) {
            op = (OperationType) i;
        }
    }
    return op;
}

void setVariableIndex (Variable& variable, FunctionCompileData& compile_data) {
    if (variable.type == MDR_TYPE_PTR) { /* Pointer. */
        variable.index = compile_data.pointer_table_next_index++;
    }else { /* "Normal" variable. */
        variable.index = compile_data.variable_table_next_index;
        compile_data.variable_table_next_index += (mdrTypeGetSize (variable.type) == 64) ? 2 : 1;
    }
}

/* Adds the label if necessary and returns it. */
Label* getLabel (string& name, FunctionCompileData& compile_data) {
    auto& labels = compile_data.labels;
    auto it = labels.find (name);
    if (it == labels.end ()) {
        Label* label = new Label ();
        label->index = compile_data.labels_next_index;
        ++compile_data.labels_next_index;
        label->position = -1;
        it = labels.insert (labels.begin (), pair<string, Label*> (name, label));
    }
    return it->second;
}

Token* nextToken (size_t index, vector<Token*>& tokens) {
    ++index;
    if (tokens.size () <= index) return NULL; /* Return null when there is no next token. */

    Token* token = tokens[index];
    if (token->tag == TOKEN_CHARACTER && token->character == '\n') return NULL; /* If the token is a newline token, return null as well. */

    return token;
}

void writeOperationPP (Operation& operation, Stream& out) {
    // TODO(Marco): Implement.
}

/* Note: Since the size of the number is 10bit, it is not saved in two's complement but a negated version of the positive number if negative. */
void writeOperationSignedP (Operation& operation, Stream& out) {
    mdr_s16 param0s = operation.param0;

    bool is_negative = false;
    if (param0s < 0) {
        operation.param0 = -param0s;
        is_negative = true;
    }

    out.WriteU8 (operation.type << 2 | ((is_negative) ? 0x02 : 0x00) /* sign. */ | ((operation.param0 >> 8) & 0x01));
    out.WriteU8 (operation.param0 & 0xFF);
}

void writeOperationP (Operation& operation, Stream& out) {
    out.WriteU8 (operation.type << 2 | (((operation.param0 >> 8) & 0x03)));
    out.WriteU8 (operation.param0 & 0xFF);
}

void writeOperationS (Operation& operation, Stream& out) {
    out.WriteU8 (operation.type << 2);
}

void writeOperation (Operation& operation, Stream& out) {
    /* This function pointer list defines a function for every bytecode operation to be written. */
    typedef void (*Callback) (Operation&, Stream&);
    Callback simple = &writeOperationS;
    Callback param0 = &writeOperationP;
    Callback param0_signed = &writeOperationSignedP;
    Callback param1 = &writeOperationPP; /* Includes param0 and param1. */
    Callback param1_signed = NULL; // TODO(Marco): Implement.

    Callback callbacks [] = {
        simple, /* END */
        param0_signed, /* JMP */
        param1_signed, /* JMPw */
        simple, /* RETURN */
        simple, /* RETURN */

        param0, /* iLOAD */
        param0, /* lLOAD */
        param0, /* uiLOAD */
        param0, /* ulLOAD */
        param0, /* fLOAD */
        param0, /* flLOAD */
        param0, /* pLOAD */
        param0, /* STORE */
        param0, /* CONST */

        NULL,   /* Free slot. */
        NULL,   /* Free slot. */

        param0, /* ALOAD */
        param0, /* ASTORE */
        param0, /* MEMLOAD */
        param0, /* MEMSTORE */
        param0, /* LOAD_GLOBAL */
        param0, /* STORE_GLOBAL */
        param0, /* LOAD_FIELD */
        param0, /* STORE_FIELD */

        simple, /* POP */

        NULL,   /* Free slot. */
        NULL,   /* Free slot. */
        NULL,   /* Free slot. */
        NULL,   /* Free slot. */
        NULL,   /* Free slot. */
        NULL,   /* Free slot. */
        NULL,   /* Free slot. */

        param0, /* INC_VAR */
        simple, /* INC */
        simple, /* ADD */
        simple, /* SUB */
        simple, /* MUL */
        simple, /* DIV */
        simple, /* REM */
        simple, /* NEG */
        simple, /* SHL */
        simple, /* SHR */
        simple, /* AND */
        simple, /* OR */
        simple, /* XOR */
        simple, /* NOT */

        param0_signed, /* CEQ_0 */
        param0_signed, /* CNE_0 */
        param0_signed, /* CEQ */
        param0_signed, /* CNE */
        param0_signed, /* CGT */
        param0_signed, /* CGE */

        param0, /* CAST */

        param0, /* ADR */
        param0, /* ADR_GLOBAL */
        param0, /* ADR_FIELD */

        NULL,   /* Free slot. */

        param0, /* NEW */
        param0, /* ANEW */
        NULL,   /* INEW */ // TODO(Marco): Create operation write class for two params

        param0, /* CALL */
        param0, /* ICALL */
        simple, /* VCALL */

        simple  /* BREAKPOINT */
    };

    callbacks[operation.type] (operation, out);
}


/** Replaces dots '.' with slashes '/'. */
void toFolderString (string& str) {
    size_t size = str.length ();
    for (size_t i = 0; i < size; ++i) {
        if (str[i] == '.') {
            str[i] = '/';
        }
    }
}

size_t parseOperation (OperationType op, size_t index, vector<Token*>& tokens, FunctionCompileData& compile_data) {
    size_t tokens_size = tokens.size ();
    Operation operation;

    switch (op) {
        case kOp_jmp: {
            Token* next_token = nextToken (index++, tokens);
            if (next_token != NULL && next_token->tag == TOKEN_LITERAL) {
                string name (next_token->string);
                Label* label = getLabel (name, compile_data);
                operation.type = BCOP_JMP;
                operation.param0 = label->index;
            }else {
                printf ("Error: 'jmp' must be followed by a literal naming the label to jump to.\n");
                return -1;
            }
        }
        break;

        case kOp_return: { /* "return" | "return", "void" */
            Token* next_token = nextToken (index, tokens);
            if (next_token == NULL) { /* Normal return. */
                operation.type = BCOP_RETURN;
            }else if (next_token->tag == TOKEN_LITERAL && strcmp (next_token->string, "void") == 0) { /* Return void. */
                ++index;
                operation.type = BCOP_RETURN_VOID;
            }
        }
        break;

        case kOp_load: {
            Token* next_token = nextToken (index++, tokens);
            if (next_token == NULL) {
                printf ("Error: There must be a variable or value after 'load'.\n");
                return -1;
            }

            if (next_token->tag == TOKEN_LITERAL) { /* Variable. */
                Variable* variable = compile_data.variables[next_token->string];
                if (variable == NULL) {
                    printf ("Error: Variable '%s' is not declared.\n", next_token->string);
                    return -1;
                }
                mdrType type = variable->type; 
                
                /* Deduce operation type from variable type, Sherlock. */
                if (type != MDR_TYPE_VOID) {
                    if (type == MDR_TYPE_PTR) {
                        operation.type = BCOP_pLOAD;
                    }else if (mdrTypeGetSize (type) == 64) { /* Not long. */
                        switch (type) {
                            case MDR_TYPE_I32:
                                operation.type = BCOP_iLOAD;
                                break;
                            case MDR_TYPE_U32:
                                operation.type = BCOP_uiLOAD;
                                break;
                            case MDR_TYPE_F32:
                                operation.type = BCOP_fLOAD;
                                break;
                        }
                    }else { /* Long. */
                        switch (type) {
                            case MDR_TYPE_I64:
                                operation.type = BCOP_lLOAD;
                                break;
                            case MDR_TYPE_U64:
                                operation.type = BCOP_ulLOAD;
                                break;
                            case MDR_TYPE_F64:
                                operation.type = BCOP_flLOAD;
                                break;
                        }
                    }

                    operation.param0 = variable->index;
                }
            }else { /* Constant. */
                printf ("Error: Constants are not supported yet.\n");
                return -1;
            }

            compile_data.stack_size (1);
        }
        break;

        case kOp_add: {
            operation.type = BCOP_ADD;
            compile_data.stack_size (-2);
        }
        break;

        case kOp_sub: {
            operation.type = BCOP_SUB;
            compile_data.stack_size (-2);
        }
        break;

        default:
            printf ("Error: Unsupported Operation!\n");
            return -1;
    }

    compile_data.operations.push_back (operation);

    return index + 1; /* Skip newline. */
}

size_t parseFunction (const string& root, vector<Token*>& tokens, size_t index) {
    size_t size = tokens.size ();

    FunctionCompileData compile_data;

    /* Parse header. */
    {
        Token* name_token = nextToken (index++, tokens);
        if (name_token == NULL || name_token->tag != TOKEN_LITERAL) {
            printf ("Error: No function name was specified!\n");
            return -1;
        }
        compile_data.name = name_token->string;
    }

    /* Parse parameters and return type. */
    while (index < size) {
        Token* type_token = nextToken (index++, tokens);
        if (type_token == NULL || type_token->tag != TOKEN_LITERAL) {
            break;
        }

        Token* name_token = nextToken (index++, tokens);
        if (name_token != NULL && name_token->tag == TOKEN_LITERAL) {
            /* Token combination is a parameter. */
            Variable* parameter = new Variable ();
            parameter->type = getTypeFromString (type_token->string);
            parameter->name = name_token->string;
            setVariableIndex (*parameter, compile_data);
            compile_data.parameter_list.push_back (parameter);
            compile_data.variables[parameter->name] = parameter;
        }else {
            /* Token combination is the return type. */
            compile_data.return_type = getTypeFromString (type_token->string);
            break;
        }
    }

    printf ("Parsing function '%s' with the return type '%X'.\n", compile_data.name, compile_data.return_type);
    for (int i = 0; i < compile_data.parameter_list.size ();  ++i) {
        Variable* parameter = compile_data.parameter_list[i];
        printf ("\tParameter '%s' with the type '%X'.\n", parameter->name, parameter->type);
    }

    /* Parse OperationTypes and variable declarations. */
    while (index < size) {
        Token* literal_token = nextToken (index++, tokens); /* A newline token HAS to be skipped before! */
        if (literal_token == NULL || literal_token->tag != TOKEN_LITERAL) {
            break;
        }

        Token* next_token = nextToken (index, tokens);
        if (next_token != NULL) {
            if (next_token->tag == TOKEN_CHARACTER && next_token->character == ':') { /* Label. */
                string label_name (literal_token->string);
                Label* label = getLabel (label_name, compile_data);
                if (label->position != -1) {
                    printf ("Error: Label '%s' already exists!\n", literal_token->string);
                    return -1;
                }
                label->position = (mdr_u32) compile_data.operations.size ();
                index += 2;
            }else {
                mdrType parsed_type = getTypeFromString (literal_token->string);
                if (parsed_type == MDR_TYPE_VOID) { /* Operation. */
                    index = parseOperation (getOperationTypeFromString (literal_token->string), index, tokens, compile_data);
                    if (index == -1) { /* Error occured. */
                        printf ("Op: '%s'\n", literal_token->string);
                        return -1; 
                    }
                }else if (next_token->tag == TOKEN_LITERAL) { /* Variable declaration. */
                    Variable* variable = new Variable ();
                    variable->type = parsed_type;
                    variable->name = next_token->string;
                    setVariableIndex (*variable, compile_data);
                    compile_data.variables[variable->name] = variable;
                    index += 2;
                }else {
                    printf ("Error: Unknown Token combination on line X\n"); // TODO(Marco): Print line!
                    return -1;
                }
            }
        }else { /* Can be a single operation or an invalid line. */
            index = parseOperation (getOperationTypeFromString (literal_token->string), index, tokens, compile_data);
            if (index == -1) { /* Error occured. */
                printf ("Op: '%s'\n", literal_token->string);
                return -1; 
            }
        }
    }

    /* Resolve label positions. */
    size_t operation_count = compile_data.operations.size ();
    for (size_t i = 0; i < operation_count; ++i) {
        Operation& operation = compile_data.operations[i];

        if (operation.type == BCOP_JMP) {
            /* Iterate all labels to find the appropriate one. */
            mdr_u16 index = operation.param0;
            Label* label = NULL;
            string name;
            auto end = compile_data.labels.end ();
            auto begin = compile_data.labels.begin ();
            for (; begin != end; begin++) {
                if (begin->second->index == index) {
                    label = begin->second;
                    name = begin->first;
                    break;
                }
            }

            if (label != NULL) {
                mdr_u16 jump_distance = (mdr_u16) (((mdr_s32) label->position) - i);
                if (jump_distance == 0) {
                    printf ("Error: Can not jump to the label '%s', because it is at the location of this operation.\n", name.c_str ());
                    return -1;
                }
                operation.param0 = jump_distance;
            }else {
                printf ("Error: A label does not exist. This should not have happened!\n");
                return -1;
            }
        }

    }

    /* Turn RETURN_VOID at the end into END or add an END operation. */
    Operation& last_operation = compile_data.operations[compile_data.operations.size () - 1];
    if (last_operation.type == BCOP_RETURN_VOID) {
        last_operation.type = BCOP_END;
    }else {
        Operation op;
        op.type = BCOP_END;
        compile_data.operations.push_back (op);
    }

    /* Write function. */
    string file_path = compile_data.name;
    toFolderString (file_path);
    file_path = root + "/" + file_path + ".func";
    Directory::MakeDirectories (file_path, false);

    FileStream stream (file_path.c_str (), StreamMode::write);

    /* Write function info. */
    stream.WriteU8 (0x00 /* Exist flags. */);
    stream.WriteU8  ((mdr_u8) compile_data.parameter_list.size ());
    stream.WriteU8  (compile_data.return_type);
    stream.WriteU16 (compile_data.variable_table_next_index);
    stream.WriteU16 (compile_data.pointer_table_next_index);
    stream.WriteU16 (compile_data.max_stack_size);
    stream.WriteU16 ((mdr_u16) compile_data.operations.size ());

    /* Write Operations. */
    for (size_t i = 0, size = compile_data.operations.size (); i < size; ++i) {
        writeOperation (compile_data.operations[i], stream);
    }

    return index;
}

}




void Compile (const char* target_path, vector<Token*>& tokens) {
    size_t size = tokens.size ();
    for (size_t i = 0; i < size;) {
        Token* token = tokens[i];

        if (token->tag == TOKEN_CHARACTER) {
            switch (token->character) {
              case '!':
                i = parseFunction (target_path, tokens, i);
                continue;
            }
        }

        ++i;
    }
}

}
}