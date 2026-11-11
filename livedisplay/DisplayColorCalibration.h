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

#ifndef VENDOR_LINEAGE_LIVEDISPLAY_V2_0_PXA1908_DISPLAYCOLORCALIBRATION_H
#define VENDOR_LINEAGE_LIVEDISPLAY_V2_0_PXA1908_DISPLAYCOLORCALIBRATION_H

#include <vendor/lineage/livedisplay/2.0/IDisplayColorCalibration.h>

namespace vendor {
namespace lineage {
namespace livedisplay {
namespace V2_0 {
namespace pxa1908 {

using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;

// Colour calibration through the hardware composer: hwcomposer.mrvl (HWCTint.cpp) reads the
// per-channel gains from sys.livedisplay.rgb ("r g b", 32768 = 1.0) and applies them in its
// 2D-engine composition, so no GPU colour matrix is needed.
class DisplayColorCalibration : public IDisplayColorCalibration {
  public:
    Return<int32_t> getMaxValue() override;
    Return<int32_t> getMinValue() override;
    Return<void> getCalibration(getCalibration_cb _hidl_cb) override;
    Return<bool> setCalibration(const hidl_vec<int32_t>& rgb) override;
};

}  // namespace pxa1908
}  // namespace V2_0
}  // namespace livedisplay
}  // namespace lineage
}  // namespace vendor

#endif
