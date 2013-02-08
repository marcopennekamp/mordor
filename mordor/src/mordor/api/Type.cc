#include <mordor/api/Type.h>


MDR_DECL mdr_bool mdrTypeHasSize (mdrType type, mdr_u8 size) {
    switch (size) {
    case 64:
        return type == MDR_TYPE_I64 || type == MDR_TYPE_U64 || type == MDR_TYPE_F64;
    case 32:
        return type == MDR_TYPE_I32 || type == MDR_TYPE_U32 || type == MDR_TYPE_F32;
    case 16:
        return type == MDR_TYPE_I16 || type == MDR_TYPE_U16;
    case 8:
        return type == MDR_TYPE_I8 || type == MDR_TYPE_U8;
    default:
        return false;
    };
}

MDR_DECL mdr_u8 mdrTypeGetSize (mdrType type) {
    switch (type) {
    case MDR_TYPE_VOID: return 0;
    case MDR_TYPE_I8: return 8;
    case MDR_TYPE_I16: return 16;
    case MDR_TYPE_I32: return 32;
    case MDR_TYPE_I64: return 64;
    case MDR_TYPE_U8: return 8;
    case MDR_TYPE_U16: return 16;
    case MDR_TYPE_U32: return 32;
    case MDR_TYPE_U64: return 64;
    case MDR_TYPE_F32: return 32;
    case MDR_TYPE_F64: return 64;
    case MDR_TYPE_PTR: return 0;
    default: return 0;
    }
}

MDR_DECL void mdrTypeAddSize (mdrType& type) {
    type += 1;
}