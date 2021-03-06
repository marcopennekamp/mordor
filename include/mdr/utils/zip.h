#ifndef MDR_UTILS_ZIP_H_
#define	MDR_UTILS_ZIP_H_

#include <zlib/unzip.h>

#include <coin/utils/Stream.h>

#include <mdr/def/Mdr.h>


namespace mdr {

namespace zip {

std::string GetFileName (unzFile archive);

/* The array has to be deleted with 'delete[]' by the caller. */
mdr_u8* GetFileData (unzFile archive, const std::string& file_path, mdr_u32* file_size);
mdr_u8* GetFileData (unzFile archive, mdr_u32* file_size);

}

}


#endif  /* MDR_UTILS_ZIP_H_ */