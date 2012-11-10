#include <internal/utils/zip.h>


namespace mordor {
namespace zip {

void GetFileData (unzFile archive, const void** file_data_ptr, mordor_u32* file_size_ptr, mordor_bool append_zero, const mordor_s8** file_name_ptr) {
    unz_file_info info;
    unzGetCurrentFileInfo (archive, &info, NULL, NULL, NULL, NULL, NULL, NULL);

    /* Get file data if requested. */
    if (file_data_ptr != NULL) {
        mordor_u32 file_size = info.uncompressed_size;
        if (append_zero) file_size += 1;
        mordor_u8* file_data = new mordor_u8[file_size];
        if (append_zero) file_data[file_size - 1] = '\0';
        unzOpenCurrentFile (archive);
        unzReadCurrentFile (archive, (voidp) file_data, info.uncompressed_size);
        unzCloseCurrentFile (archive);
        *file_data_ptr = file_data;
        if (file_size_ptr != NULL) *file_size_ptr = file_size;
    }

    /* Get file name if requested. */
    if (file_name_ptr != NULL) {
        mordor_u32 file_name_size = info.size_filename + 1;
        mordor_s8* file_name = new mordor_s8[file_name_size];
        unzGetCurrentFileInfo (archive, NULL, file_name, file_name_size, NULL, NULL, NULL, NULL);
        file_name[file_name_size - 1] = '\0';
        *file_name_ptr = file_name;
    }
}

}
}