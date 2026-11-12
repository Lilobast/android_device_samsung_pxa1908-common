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

# Camera (HAL1 through the legacy provider).
# The provider service is left out until the camera is ported: on Pie it crash-looped and
# re-initialised the sensor and torch on every restart.
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

# NFC (Broadcom BCM20795: LineageOS wrapper around nfc_nci.bcm2079x) — disabled for now: Pie's
# libnfc-nci and the Lollipop HAL disagree (nfc_ncif_update_window "Unexpected call", NCI command
# timeouts) and com.android.nfc aborted on every timeout. Re-enable together with pxa1908-common.mk.
#PRODUCT_PACKAGES += \
#    android.hardware.nfc@1.0-impl-bcm \
#    android.hardware.nfc@1.0-service

# Wi-Fi (HIDL wifi HAL with the generic libwifi-hal; supplicant/hostapd over HIDL)
PRODUCT_PACKAGES += \
    android.hardware.wifi@1.0-service \
    wificond

# LiveDisplay (colour calibration through the HW composer, sys.livedisplay.rgb)
PRODUCT_PACKAGES += \
    vendor.lineage.livedisplay@2.0-service.pxa1908
