#ifndef MORDOR_INTERNAL_UTILS_ZIP_H_
#define	MORDOR_INTERNAL_UTILS_ZIP_H_

#include <zlib/unzip.h>

#include <mordor/def.h>


namespace mdr {

namespace zip {

/* Both arrays have to be deleted with 'delete[]' by the caller. */
void GetFileData (unzFile archive, const void** file_data, mdr_u32* file_size, mdr_bool append_zero, const mdr_s8** file_name);

}

}


#endif  /* MORDOR_INTERNAL_UTILS_ZIP_H_ */