#include <mdr/utils/zip.h>


namespace mdr {
namespace zip {

std::string GetFileName (unzFile archive) {
    unz_file_info info;
    unzGetCurrentFileInfo (archive, &info, NULL, NULL, NULL, NULL, NULL, NULL);

    mdr_u32 file_name_size = info.size_filename;
    mdr_s8* file_name = new mdr_s8[file_name_size];
    unzGetCurrentFileInfo (archive, NULL, file_name, file_name_size, NULL, NULL, NULL, NULL);

    std::string str (file_name, file_name_size);
    delete[] file_name;

    return str;
}

mdr_u8* GetFileData (unzFile archive, const std::string& file_path, mdr_u32* file_size) {
    int found = unzLocateFile (archive, file_path.c_str (), true);
    if (found != UNZ_OK) {
        return NULL;
    }

    return GetFileData (archive, file_size);
}

mdr_u8* GetFileData (unzFile archive, mdr_u32* file_size) {
    unz_file_info info;
    unzGetCurrentFileInfo (archive, &info, NULL, NULL, NULL, NULL, NULL, NULL);

    mdr_u32 size = info.uncompressed_size;
    mdr_u8* file_data = new mdr_u8[size];
    unzOpenCurrentFile (archive);
    unzReadCurrentFile (archive, (voidp) file_data, info.uncompressed_size);
    unzCloseCurrentFile (archive);

    if (file_size != NULL) *file_size = size;
    return file_data;
}

}
}