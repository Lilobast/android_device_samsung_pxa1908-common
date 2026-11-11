#BOARD_USE_SAMSUNG_CAMERAFORMAT_NV21 := true
#BOARD_USE_SAMSUNG_COLORFORMAT := true

# Marvell OMX HW decoders can't decode into native window buffers on N
# (no enableAndroidNativeBuffers); use ACodec's software-renderer path for them.
TARGET_MRVL_HW_DEC_USE_SW_RENDERER := true
TARGET_MRVL_HW_DEC_ZERO_COPY := true
