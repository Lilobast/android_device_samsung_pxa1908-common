# Pie: HIDL wrappers around the legacy (libhardware) HAL modules.
# Binderized services, except the graphics mapper and renderscript (passthrough only).
# Listed in manifest.xml (DEVICE_MANIFEST_FILE).

# Graphics
PRODUCT_PACKAGES += \
    android.hardware.graphics.allocator@2.0-impl \
    android.hardware.graphics.mapper@2.0-impl \
    android.hardware.graphics.composer@2.1-impl \
    android.hardware.memtrack@1.0-impl \
    android.hardware.memtrack@1.0-service \
    android.hardware.renderscript@1.0-impl

# Audio
PRODUCT_PACKAGES += \
    android.hardware.audio@2.0-impl \
    android.hardware.audio@2.0-service \
    android.hardware.audio.effect@2.0-impl

# Bluetooth
PRODUCT_PACKAGES += \
    android.hardware.bluetooth@1.0-impl \
    android.hardware.bluetooth@1.0-service

# Camera (HAL1 through the legacy provider). No provider service: CameraService runs in
# mediaserver (NO_CAMERA_SERVER, as on 14.1, so the HAL shares the process with the OMX encoder
# for recording) and loads the legacy provider passthrough.
PRODUCT_PACKAGES += \
    android.hardware.camera.provider@2.4-impl \
    camera.device@1.0-impl

# DRM
PRODUCT_PACKAGES += \
    android.hardware.drm@1.0-impl \
    android.hardware.drm@1.0-service

# GNSS
PRODUCT_PACKAGES += \
    android.hardware.gnss@1.0-impl \
    android.hardware.gnss@1.0-service

# Keymaster (keystore.mrvl is keymaster 0.2; the 3.0 wrapper supports it)
PRODUCT_PACKAGES += \
    android.hardware.keymaster@3.0-impl \
    android.hardware.keymaster@3.0-service

# Lights, power, vibrator, sensors, USB
PRODUCT_PACKAGES += \
    android.hardware.light@2.0-impl \
    android.hardware.light@2.0-service \
    android.hardware.power@1.0-impl \
    android.hardware.power@1.0-service \
    android.hardware.vibrator@1.0-impl \
    android.hardware.vibrator@1.0-service \
    android.hardware.sensors@1.0-impl \
    android.hardware.sensors@1.0-service \
    android.hardware.usb@1.0-service

# NFC (Broadcom BCM20795): LineageOS wrapper around nfc_nci.bcm2079x, which is the open-source
# halimpl (hardware/broadcom/nfc, ro.hardware.nfc_nci.bcm2079x=default). The Lollipop Samsung
# blob (nfc_nci.bcm2079x.PXA19xx) and Pie's libnfc-nci disagreed (unexpected NCI responses).
PRODUCT_PACKAGES += \
    android.hardware.nfc@1.0-impl-bcm \
    android.hardware.nfc@1.0-service \
    nfc_nci.bcm2079x.default

# Wi-Fi: no vendor HAL (BOARD_WLAN_DEVICE unset -> libwifi-hal fallback, which fails IWifi
# start). Without IWifi in the manifest the framework runs wificond + supplicant only; the
# sd8777 firmware is loaded at boot by the kernel/MarvellWirelessDaemon.
PRODUCT_PACKAGES += \
    wificond

# LiveDisplay (colour calibration through the HW composer, sys.livedisplay.rgb)
PRODUCT_PACKAGES += \
    vendor.lineage.livedisplay@2.0-service.pxa1908
