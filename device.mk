
PLATFORM_PATH := device/motorola/troika

# define hardware platform
PRODUCT_PLATFORM := exynos9610

# Enable update engine sideloading by including the static version of the
# boot_control HAL and its dependencies.
PRODUCT_STATIC_BOOT_CONTROL_HAL := \
    bootctrl.exynos9610 \
    libz \
    libcutils

# Boot control HAL
PRODUCT_PACKAGES += \
    bootctrl.exynos9610



