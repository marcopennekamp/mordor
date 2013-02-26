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

MDR_DECL void mdrCtxPushI32 (mdrContext* ctx, mdr_i32 value);
MDR_DECL void mdrCtxPushI64 (mdrContext* ctx, mdr_i64 value);
MDR_DECL void mdrCtxPushU32 (mdrContext* ctx, mdr_u32 value);
MDR_DECL void mdrCtxPushU64 (mdrContext* ctx, mdr_u64 value);
MDR_DECL void mdrCtxPushF32 (mdrContext* ctx, mdr_f32 value);
MDR_DECL void mdrCtxPushF64 (mdrContext* ctx, mdr_f64 value);


#ifdef	__cplusplus
}
#endif

#endif  /* MDR_API_CONTEXT_H_ */
