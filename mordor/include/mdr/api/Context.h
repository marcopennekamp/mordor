#ifndef MDR_API_CONTEXT_H_
#define	MDR_API_CONTEXT_H_

#include <mdr/def/Mdr.h>
#include <mdr/api/Environment.h>

#ifdef	__cplusplus
extern "C" {
#endif


typedef void    mdrContext;

MDR_DECL mdrContext* mdrCtxCreate (mdrEnvironment* env);
MDR_DECL void mdrCtxDestroy (mdrContext* ctx);

MDR_DECL void mdrCtxExecute (mdrContext* ctx, mdrFunction* func);

MDR_DECL mdr_u8* mdrCtxGetStack (mdrContext* ctx);
MDR_DECL mdr_u8* mdrCtxGetReturnValueAddress (mdrContext* ctx);


#ifdef	__cplusplus
}
#endif

#endif  /* MDR_API_CONTEXT_H_ */
