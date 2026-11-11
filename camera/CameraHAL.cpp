/*
 * Copyright (C) 2012 The Android Open Source Project
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

#include <errno.h>
#include <cstdlib>
#include <string.h>
#include <hardware/camera_common.h>
#include <hardware/hardware.h>

#include <pthread.h>

//#define LOG_NDEBUG 0

#define ATRACE_TAG (ATRACE_TAG_CAMERA | ATRACE_TAG_HAL)
#include <cutils/trace.h>
#include <cutils/properties.h>

#define LOG_TAG "CameraReversing"
#include <cutils/log.h>

class __DEBUG_CLASS__
{
    const char* _func;
    public:
        __DEBUG_CLASS__(int line, const char* func):_func(func)
        {
            ALOGD("Entering %s at line %d", _func, line);
        }

        ~__DEBUG_CLASS__()
        {
            ALOGD("Exiting %s", _func);
        }
};

#define log_func_entry         __DEBUG_CLASS__ __debug_obj__(__LINE__, __func__)
#define log_func_line(fmt,...) ALOGD("%s at %d: " fmt, __func__, __LINE__, ## __VA_ARGS__)

#include "stockcamera.h"
#include "CameraHAL.h"

#include <dlfcn.h>

/*
 * This file serves as the entry point to the HAL.  It contains the module
 * structure and functions used by the framework to load and interface to this
 * HAL, as well as the handles to the individual camera devices.
 */

namespace default_camera_hal {

libstockcamera camera_mrvl;
pthread_mutex_t camera_mutex = PTHREAD_MUTEX_INITIALIZER;

extern "C" {

static int camera_get_number_of_cameras()
{
    // The stock HAL re-probes the sensors on every call; a second probe on N
    // reports a wrong count and crashes, so ask it once and cache the answer.
    static int cached = -1;
    pthread_mutex_lock(&camera_mutex);
    if (cached < 0 && camera_mrvl.camera_get_number_of_cameras) {
        cached = camera_mrvl.camera_get_number_of_cameras();
        ALOGI("%s: stock HAL reports %d camera(s)", __func__, cached);
    }
    int res = cached < 0 ? 0 : cached;
    pthread_mutex_unlock(&camera_mutex);
    return res;
}

static int camera_get_camera_info(int id, struct camera_info* info)
{
    if (!camera_mrvl.camera_get_camera_info || !info)
        return -EINVAL;
    // The stock (API 1.0) HAL only fills facing/orientation; clear the rest for N.
    memset(info, 0, sizeof(*info));
    int res = camera_mrvl.camera_get_camera_info(id, info);
    info->device_version = CAMERA_DEVICE_API_VERSION_1_0;
    ALOGI("%s: id %d facing %d orientation %d -> %d", __func__, id, info->facing,
          info->orientation, res);
    return res;
}

static int camera_device_open(const hw_module_t* mod, const char* name, hw_device_t** dev)
{
    (void)mod;
    if (!camera_mrvl.camera_open_dev || !camera_mrvl.HMI)
        return -EINVAL;
    // Hand the stock HAL its own module so its internal checks match.
    int res = camera_mrvl.camera_open_dev(&camera_mrvl.HMI->common, name, dev);
    ALOGI("%s: camera %s -> %d", __func__, name, res);
    return res;
}

static hw_module_methods_t gCameraModuleMethods = {
    open : camera_device_open
};

camera_module_t HAL_MODULE_INFO_SYM __attribute__ ((visibility("default"))) = {
    common : {
        tag                : HARDWARE_MODULE_TAG,
        module_api_version : CAMERA_MODULE_API_VERSION_1_0,
        hal_api_version    : HARDWARE_HAL_API_VERSION,
        id                 : CAMERA_HARDWARE_MODULE_ID,
        name               : "Default Camera HAL",
        author             : "Nemirtingas",
        methods            : &gCameraModuleMethods,
        dso                : NULL,
        reserved           : {0},
    },
    get_number_of_cameras : camera_get_number_of_cameras,
    get_camera_info       : camera_get_camera_info,
    set_callbacks         : NULL,
    get_vendor_tag_ops    : NULL,
    open_legacy           : NULL,
    set_torch_mode        : NULL,
    init                  : NULL,
    reserved              : {0},
};
} // extern "C"

} // namespace default_camera_hal
