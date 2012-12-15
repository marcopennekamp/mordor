#ifndef MORDOR_INTERNAL_UTILS_POINTER_H_
#define	MORDOR_INTERNAL_UTILS_POINTER_H_

#include <boost/scoped_ptr.hpp>


namespace mordor {

template<typename T>
typedef boost::scoped_ptr<T> ptr;

}


#endif  /* MORDOR_INTERNAL_UTILS_POINTER_H_ */