#ifndef MORDOR_INTERPRETER_CORE_H_
#define	MORDOR_INTERPRETER_CORE_H_

#include <mordor/runtime/ContextInterface.h>
#include <mordor/runtime/ProgramInterface.h>
#include <mordor/runtime/FunctionInterface.h>


#include <mordor/mordor_begin.h>


MORDOR_DECL void mordorInterpreterExecute (ContextInterface* context_interface, FunctionInterface* function, mordor_u32 caller_stack_top);


#include <mordor/mordor_end.h>

#endif  /* MORDOR_INTERPRETER_CORE_H_ */
