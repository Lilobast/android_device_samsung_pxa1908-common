/*
 * Copyright (C) 2026 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
