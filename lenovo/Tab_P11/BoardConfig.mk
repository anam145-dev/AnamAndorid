# ==== SELinux Configurations ====
# Enable SEPolicy directories for custom Firefox rules and additional SEPolicy paths
BOARD_SEPOLICY_DIRS += \
    device/lenovo/Tab_P11/sepolicy \
    external/sepolicy \
    device/generic/goldfish/sepolicy/common

PRODUCT_PACKAGES += \
    hash_generator \
    verifier

