
# Include the SELinux policy files
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/sepolicy/firefox.te:$(TARGET_COPY_OUT_VENDOR)/etc/selinux/precompiled_sepolicy/firefox.te \
    $(LOCAL_PATH)/sepolicy/file_contexts:$(TARGET_COPY_OUT_VENDOR)/etc/selinux/precompiled_sepolicy/file_contexts


# Include hash_generator and verifier binaries
PRODUCT_PACKAGES += \
    hash_generator \
    verifier

# Include init script to create /data/integrity and manage verifier
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/integrity/init.integrity.rc:system/etc/init/init.integrity.rc

# Include SELinux policies
BOARD_SEPOLICY_DIRS += \
    device/lenovo/Tab_P11/selinux

