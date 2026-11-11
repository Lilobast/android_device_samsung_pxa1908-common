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

#include <algorithm>
#include <string>
#include <vector>

#include <android-base/parseint.h>
#include <android-base/properties.h>
#include <android-base/strings.h>

#include "DisplayColorCalibration.h"

using android::base::GetProperty;
using android::base::ParseInt;
using android::base::SetProperty;
using android::base::Split;
using android::base::Trim;

namespace vendor {
namespace lineage {
namespace livedisplay {
namespace V2_0 {
namespace pxa1908 {

static constexpr const char* kProp = "sys.livedisplay.rgb";
static constexpr int32_t kMin = 255;
static constexpr int32_t kMax = 32768;

Return<int32_t> DisplayColorCalibration::getMaxValue() {
    return kMax;
}

Return<int32_t> DisplayColorCalibration::getMinValue() {
    return kMin;
}

Return<void> DisplayColorCalibration::getCalibration(getCalibration_cb _hidl_cb) {
    std::vector<int32_t> rgb;
    for (const std::string& c : Split(Trim(GetProperty(kProp, "")), " ")) {
        int32_t v;
        if (ParseInt(c, &v)) rgb.push_back(v);
    }
    if (rgb.size() != 3) rgb = {kMax, kMax, kMax};
    _hidl_cb(rgb);
    return Void();
}

Return<bool> DisplayColorCalibration::setCalibration(const hidl_vec<int32_t>& rgb) {
    if (rgb.size() != 3) return false;
    std::string contents;
    for (size_t i = 0; i < 3; i++) {
        if (i) contents += " ";
        contents += std::to_string(std::max(kMin, std::min(kMax, rgb[i])));
    }
    return SetProperty(kProp, contents);
}

}  // namespace pxa1908
}  // namespace V2_0
}  // namespace livedisplay
}  // namespace lineage
}  // namespace vendor
