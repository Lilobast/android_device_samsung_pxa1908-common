# Charger
BOARD_BATTERY_DEVICE_NAME := "battery"
BOARD_CHARGING_CMDLINE_NAME := "androidboot.mode"
BOARD_CHARGING_CMDLINE_VALUE := "charger"
BOARD_CHARGER_ENABLE_SUSPEND := true
BOARD_CHARGER_SHOW_PERCENTAGE := true

# Charger
BOARD_CHARGING_MODE_BOOTING_LPM := true

# mmpfb returns EINVAL for the FBIOPUT_VSCREENINFO page flip of minui (healthd charger);
# pan to the drawn buffer instead.
BOARD_RECOVERY_NEEDS_FBIOPAN_DISPLAY := true
