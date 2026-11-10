LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := uishim.cpp
LOCAL_SHARED_LIBRARIES := libui
LOCAL_MODULE := libui_shim
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := utilsshim.cpp
LOCAL_SHARED_LIBRARIES := libutils
LOCAL_MODULE := libutils_shim
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)

# Lollipop Parcel API for the stock libril.so (see rilshim.cpp)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := rilshim.cpp
LOCAL_SHARED_LIBRARIES := libbinder libutils
LOCAL_MODULE := libril_shim
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
