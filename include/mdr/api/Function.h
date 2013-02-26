#ifndef MDR_API_FUNCTION_H_
#define	MDR_API_FUNCTION_H_

#include <mdr/def/Mdr.h>

#include <mdr/def/Type.h>

#ifdef	__cplusplus
extern "C" {
#endif


typedef void    mdrFunction;

// MDR_DECL mdrFunction* mdrFuncCreate ();
// MDR_DECL void mdrFuncDestroy (mdrFunction* func);

MDR_DECL size_t mdrFuncGetParamCount (mdrFunction* func);
MDR_DECL mdrType* mdrFuncGetParamList  (mdrFunction* func);


#ifdef	__cplusplus
}
#endif

#endif  /* MDR_API_FUNCTION_H_ */
