/*
 * Shim for the stock (Lollipop) libril.so on Nougat.
 *
 * Lollipop's Parcel::writeString16 took a `const uint16_t*`; Nougat's takes
 * `const char16_t*`. Same layout, different mangled name, so forward the old
 * symbol to the new one. Loaded via LINKER_FORCED_SHIM_LIBS for libril.so.
 */
#include <binder/Parcel.h>

extern "C" android::status_t _ZN7android6Parcel13writeString16EPKtj(
        android::Parcel* parcel, const uint16_t* str, size_t len)
{
    return parcel->writeString16(reinterpret_cast<const char16_t*>(str), len);
}
