#ifndef MORDOR_RUNTIME_CORE_H_
#define	MORDOR_RUNTIME_CORE_H_

#include <mordor/def.h>

#include <mordor/runtime/Context.h>
#include <mordor/runtime/Function.h>

#ifdef	__cplusplus
extern "C" {
#endif


MDR_DECL void mdrExecute (mdrContext* ctx, mdrFunction* func, mdr_u32 caller_stack_top);


#ifdef	__cplusplus
}
#endif

#endif  /* MORDOR_RUNTIME_CORE_H_ */