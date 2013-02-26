#include <mdr/api/Type.h>


MDR_DECL mdr_bool mdrTypeHasSize (mdrType type, mdr_u8 size) {
    return mdrTypeGetSize (type) == size;
}

MDR_DECL mdr_u8 mdrTypeGetSize (mdrType type) {
    return MDR_TYPE_SIZE[type];
}

MDR_DECL void mdrTypeAddSize (mdrType& type) {
    type += 1;
}