#ifndef MORDOR_INTERNAL_RUNTIME_ENVIRONMENT_H_
#define	MORDOR_INTERNAL_RUNTIME_ENVIRONMENT_H_

#include <mordor/def.h>


namespace mordor {

class Environment {
  public: 
    void LoadLibrary (const char* path);
};

}

#endif  /* MORDOR_INTERNAL_RUNTIME_ENVIRONMENT_H_ */