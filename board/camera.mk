# Camera
TARGET_HAS_LEGACY_CAMERA_HAL1 := true
TARGET_PROVIDES_CAMERA_HAL := true
USE_DEVICE_SPECIFIC_CAMERA := true

#Faceproc needs this to load
TARGET_NEEDS_PLATFORM_TEXT_RELOCATIONS := true
BOARD_USE_SAMSUNG_CAMERAFORMAT_NV21 := true

# Marvell ION support in MemoryHeapBase(/dev/ion) for the stock camera HAL
BOARD_USES_MRVL_ION_MEMORYHEAP := true
