#include <mdr/api/Context.h>
#include <mdr/runtime/Context.h>

using namespace mdr;

/* Casts mdrContext to Context. */
#define CCTX        Context* const context = (Context*) ctx
#define PUSH_ADDR   (context->stack ().array () + context->push_stack_top ())


MDR_DECL mdrContext* mdrCtxCreate (mdrEnvironment* env) {
    return (mdrContext*) new Context ((Environment*) env);
}

MDR_DECL void mdrCtxDestroy (mdrContext* ctx) { CCTX;
    delete context;
}

MDR_DECL void mdrCtxExecute (mdrContext* ctx, mdrFunction* func) { CCTX;
    context->Execute ((Function*) func, 0);
    context->push_stack_top (0); /* Reset the C push stack pointer for the next call. */
}

MDR_DECL mdr_u8* mdrCtxGetStack (mdrContext* ctx) { CCTX;
    return context->stack ().array ();
}

MDR_DECL mdr_u8* mdrCtxGetReturnValueAddress (mdrContext* ctx) { CCTX;
    return context->return_value_address ();
}

MDR_DECL void mdrCtxPushI32 (mdrContext* ctx, mdr_i32 value) { CCTX;
    *((mdr_i32*) PUSH_ADDR) = value;
    context->Push (4);
}

MDR_DECL void mdrCtxPushI64 (mdrContext* ctx, mdr_i64 value) { CCTX;
    *((mdr_i64*) PUSH_ADDR) = value;
    context->Push (8);
}

MDR_DECL void mdrCtxPushU32 (mdrContext* ctx, mdr_u32 value) { CCTX;
    *((mdr_u32*) PUSH_ADDR) = value;
    context->Push (4);
}

MDR_DECL void mdrCtxPushU64 (mdrContext* ctx, mdr_u64 value) { CCTX;
    *((mdr_u64*) PUSH_ADDR) = value;
    context->Push (8);
}

MDR_DECL void mdrCtxPushF32 (mdrContext* ctx, mdr_f32 value) { CCTX;
    *((mdr_f32*) PUSH_ADDR) = value;
    context->Push (4);
}

MDR_DECL void mdrCtxPushF64 (mdrContext* ctx, mdr_f64 value) { CCTX;
    *((mdr_f64*) PUSH_ADDR) = value;
    context->Push (8);
}
