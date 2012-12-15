#ifndef MORDOR_INTERNAL_UTILS_ZIP_H_
#define	MORDOR_INTERNAL_UTILS_ZIP_H_

#include <zlib/unzip.h>

#include <mordor/def.h>


namespace mordor {
namespace zip {

/* Both arrays have to be deleted with 'delete[]' by the caller. */
MORDOR_DECL void GetFileData (unzFile archive, const void** file_data, mordor_u32* file_size, mordor_bool append_zero, const mordor_s8** file_name);

}
}

#endif  /* MORDOR_INTERNAL_UTILS_ZIP_H_ */