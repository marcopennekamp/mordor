#include <mdr/api/Function.h>
#include <mdr/runtime/Function.h>

using namespace mdr;


MDR_DECL size_t mdrFuncGetParamCount (mdrFunction* func) {
    return ((Function*) func)->cpinfo ()->parameters_.size ();
}

MDR_DECL mdrType* mdrFuncGetParamList  (mdrFunction* func) {
    return ((Function*) func)->cpinfo ()->parameters_.array ();
}