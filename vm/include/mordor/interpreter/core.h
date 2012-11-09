#ifndef MORDOR_INTERPRETER_INTERPRETER_H
#define	MORDOR_INTERPRETER_INTERPRETER_H

#include <mordor/runtime/ContextInterface.h>
#include <mordor/runtime/ProgramInterface.h>
#include <mordor/runtime/FunctionInterface.h>


#include <mordor/mordor_begin.h>


void mordorInterpreterExecute (ContextInterface* context_interface, ProgramInterface* program, FunctionInterface* function, mordor_u32 caller_stack_top, mordor_u8** return_address);


#include <mordor/mordor_end.h>

#endif

