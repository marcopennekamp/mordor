#ifndef MORDOR_API_PROGRAM_H_
#define	MORDOR_API_PROGRAM_H_

#include <mordor/def/Mordor.h>

#ifdef	__cplusplus
extern "C" {
#endif


typedef void    mdrProgram;

MDR_DECL mdrProgram* mdrProgCreate ();
MDR_DECL void mdrProgDestroy (mdrProgram* prog);


#ifdef	__cplusplus
}
#endif

#endif  /* MORDOR_RUNTIME_PROGRAM_H_ */
