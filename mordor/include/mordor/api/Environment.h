#ifndef MORDOR_API_ENVIRONMENT_H_
#define	MORDOR_API_ENVIRONMENT_H_

#include <mordor/def/Mordor.h>

#include <mordor/api/Function.h>

#ifdef	__cplusplus
extern "C" {
#endif


typedef void mdrEnvironment;

MDR_DECL mdrEnvironment* mdrEnvCreate ();
MDR_DECL void mdrEnvDestroy (mdrEnvironment* env);

MDR_DECL void mdrEnvLoadProgram (mdrEnvironment* env, const char* path);
MDR_DECL mdrFunction* mdrEnvGetFunction (mdrEnvironment* env, const char* name);


#ifdef	__cplusplus
}
#endif

#endif  /* MORDOR_API_ENVIRONMENT_H_ */
