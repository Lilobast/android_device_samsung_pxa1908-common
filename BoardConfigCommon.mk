# Copyright (C) 2014 The CyanogenMod Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Inherit from mrvl-common
-include device/samsung/mrvl-common/BoardConfigCommon.mk

COMMON_PATH := device/samsung/pxa1908-common

include $(COMMON_PATH)/board/*.mk

TARGET_SYSTEM_PROP := $(COMMON_PATH)/system.prop

# RIL
# Pie: AOSP libril (hardware/ril). hardware/samsung libril maps Samsung OEM requests/unsols by
# table position with Exynos/Shannon numbering; this Marvell libsec-ril numbers them differently
# (e.g. GET_CELL_BROADCAST_CONFIG 10008 vs 10001). AOSP libril ignores unknown OEM unsols.
# TODO(RIL bring-up): check standard request structs; else a Samsung libril with our tables.
BOARD_PROVIDES_LIBRIL := false
ANDROID_MULTI_SIM := true
ANDROID_SIM_COUNT_2 := true

# Board specific headers
TARGET_SPECIFIC_HEADER_PATH := $(COMMON_PATH)/include

BOARD_PROVIDES_MKBOOTIMG := true
BOARD_MKBOOTIMG_MRVL := true

BOARD_PROVIDES_MKIMAGE := true
BOARD_MKIMAGE_MRVL := true

BOARD_CUSTOM_BOOTIMG_MK := hardware/marvell/bootimage/mkbootimg.mk

BOARD_USE_MARVELL_ION := true

# CMHW
BOARD_HARDWARE_CLASS += $(COMMON_PATH)/cmhw

# Custom RIL class: the N-era SamsungRIL class does not fit the Pie (HIDL) RIL; disabled.
#BOARD_RIL_CLASS := ../../../$(COMMON_PATH)/ril_class

# Fonts
EXTENDED_FONT_FOOTPRINT := true

# Init
TARGET_INIT_VENDOR_LIB := libinit_pxa1908
TARGET_UNIFIED_DEVICE := true

#BOARD_BUILD_SYSTEM_ROOT_IMAGE := true

# Marvell policy first, device policy last (device rules use Marvell types)
# Pie: the Qualcomm-derived common/ and test/ policies of device/marvell/sepolicy are not used;
# only its pxa1908 file labels. Device rules live in $(COMMON_PATH)/sepolicy.
#BOARD_SEPOLICY_DIRS += device/marvell/sepolicy/pxa1908
# Pie bring-up: AOSP policy only, SELinux permissive (androidboot.selinux=permissive in
# BoardConfig.mk). The 14.1 device policy depends on the Qualcomm-derived Marvell policy and
# is being rewritten for Pie (notes/los16_port_plan.md).
#BOARD_SEPOLICY_DIRS += $(COMMON_PATH)/sepolicy
# LiveDisplay tint is applied by hwcomposer.mrvl; SurfaceFlinger covers GLES-only frames
TARGET_HWC_LIVEDISPLAY_TINT := true


# No dexpreopt: with host-compiled (WITH_DEXPREOPT) framework/services code, system_server died with a
# SIGSEGV (fault at 0x78, then a second fault inside the handler, no tombstone) ~1 s after every
# Wi-Fi connect -> boot loop while Wi-Fi is on. Code compiled on the device works.
WITH_DEXPREOPT := false
