/* 
 * Module:  Context
 * Short:   Ctx
 *
 */


#ifndef MORDOR_RUNTIME_CONTEXT_H_
#define	MORDOR_RUNTIME_CONTEXT_H_

#include <mordor/def.h>

#include <mordor/runtime/Environment.h>

#ifdef	__cplusplus
extern "C" {
#endif


typedef void    mdrContext;


MDR_DECL mdrContext* mdrCtxCreate (mdrEnvironment* env);
MDR_DECL void mdrCtxDestroy (mdrContext* ctx);

MDR_DECL mdr_u8* mdrCtxGetStack (mdrContext* ctx);
MDR_DECL mdr_u8* mdrCtxGetReturnValueAddress (mdrContext* ctx);


#ifdef	__cplusplus
}
#endif

#endif  /* MORDOR_RUNTIME_CONTEXT_H_ */
