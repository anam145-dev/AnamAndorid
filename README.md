# Lenovo Tab P11 AOSP System Image Build

This document explains the process of building an AOSP-based system image for the Lenovo Tab P11 device. It includes adding Firefox as a prebuilt application, enforcing SELinux policies, and implementing file integrity checks to enhance security.

---

## Directory Structure

### Directory Descriptions
- `sepolicy/`: Contains SELinux policy files.
  - `firefox.te`: Defines SELinux policies related to Firefox.
- `integrity/`: Stores files related to file integrity checks.
  - SELinux policies are configured to allow Firefox to execute files within the `integrity/` folder.
- `Android.bp`: Configuration file for including the `Tab_P11` directory in the build system.

---

## Key Configurations

### 1. Prebuilt Application
- Firefox is set as a prebuilt application.
- The installation and execution of other applications (excluding ADB) are restricted.

### 2. SELinux Policies
- The `firefox.te` file includes the following configurations:
  - Restricts execution permissions to Firefox only.
  - Allows execution of files within the `integrity/` directory.

### 3. File Integrity Check
- Manages file integrity check configurations in the `integrity/` directory.

### 4. Package Management Restrictions
- The Package Installer is removed to prevent manual APK installations.
- Execution of applications installed via unauthorized APK paths is blocked.

---

## Building the System Image

### AOSP Build Process
1. Download the AOSP source code and set up the build environment.
2. Create the `device/lenovo/Tab_P11` directory and add necessary configuration files.
3. Run the following commands to build the system image:
   ```bash
   source build/envsetup.sh
   lunch aosp_arm64-eng
   make -j$(nproc)
   ```

---

## Troubleshooting

### Potential WiFi Issues  
If WiFi-related problems occur, refer to the [WiFi Issue Resolution Guide](https://www.notion.so/hwseclab/wifi-cae1f4956a444c1caa6dd1eb58f0f25b?pvs=4).

---

## References
- [Building AOSP GSI Image](https://www.notion.so/hwseclab/Build-System-Image-9a1add6c3dd440b1aeabe5c2ec521624?pvs=4)
- [WiFi Issue Resolution Guide](https://www.notion.so/hwseclab/wifi-cae1f4956a444c1caa6dd1eb58f0f25b?pvs=4)

---

# File Integrity Check for Android

This repository contains the implementation of a file integrity check mechanism designed to enhance Android security by preventing malicious APKs from exploiting vulnerabilities. The approach consists of a two-layer defense mechanism: file integrity checks followed by SELinux policy enforcement and the removal of the Package Manager.

---

## Overview

The file integrity check system consists of two main components: `hash_generator` and `verifier`. These components work together to prevent the installation and execution of tampered `.dex` and `.so` files, as well as unauthorized modifications to prebuilt application files such as `AndroidManifest.xml`.

### Defense Mechanisms:

#### 1. First Line of Defense: File Integrity Check
- Verifies the integrity of files (`.dex`, `.so`) before they are executed.
- Ensures prebuilt application files (e.g., `AndroidManifest.xml`) remain unaltered.

#### 2. Second Line of Defense: SELinux Policy Enforcement & Package Manager Removal
- Restricts execution permissions to approved applications using SELinux policies.
- Removes the Package Manager to prevent APK installations.

---

## Components

### 1. `hash_generator`

#### Purpose
- Generates hash values for the following prebuilt application files during the AOSP build process:
  - `classes.dex`
  - `.so` libraries
  - `AndroidManifest.xml`
- Encrypts SHA-256 hash values using an RSA private key.
- Stores the encrypted hashes in `/data/misc/integrity/firefox/`.

#### Execution
- Runs during the AOSP build process, ensuring secure storage of hash values for verification.

### 2. `verifier`

#### Purpose
- Validates file integrity before executing prebuilt applications (e.g., Firefox).
- Performs two verification methods:
  - **Local Verification:** Compares current file hashes with pre-generated hashes from `hash_generator`.
  - **Remote Verification:** Ensures received files are from trusted sources before allowing execution.

#### Execution
- Runs on the Android device after flashing the system image (`system.img`).
- Must be executed before launching prebuilt applications to verify their integrity.

---

## File Structure

```
device/lenovo/Tab_P11/
├── Android.bp
├── BoardConfig.mk
├── device.mk
├── integrity
│   ├── hash_generator.c
│   ├── init.integrity.rc
│   └── verifier.c
└── sepolicy
    ├── file_contexts
    └── firefox.te
```

---

## SELinux Policy Highlights

### 1. Firefox Execution Restrictions
- Firefox can only access its own data directory (`/data/misc/integrity/firefox`).
- Non-Firefox APK installations and executions are globally blocked.

### 2. Integrity Checker Permissions
- `hash_generator` and `verifier` are granted:
  - Read access to Firefox-related files.
  - Write permissions for `/data/misc/integrity/firefox `.
  - Access to public key storage for verification.
  - Secure remote file verification capabilities.

### 3. Blocking Unauthorized Executables
- Prevents execution or modification of unauthorized APKs and executables.
- Restricts access to temporary directories and cache files.

### 4. Domain Transitions
- Ensures proper domain transitions for `hash_generator` and `verifier` in SELinux.

---

## Build & Execution

### System Build
1. Add `hash_generator` and `verifier` source files to the AOSP source tree at `/device/lenovo/Tab_P11/integrity/`.
2. Modify `Android.bp`, `BoardConfig.mk`, and `device.mk` to include:
   - `hash_generator`
   - `verifier`
   - SELinux policy files
3. Build the system:
   ```bash
   source build/envsetup.sh
   lunch <target>
   make
   ```

### Flashing the System Image
1. Flash the built `system.img` to the Android device:
   ```bash
   fastboot flash system out/target/product/<target>/system.img
   ```
2. Reboot the device to apply changes:
   ```bash
   fastboot reboot
   ```

---

## Security Features

### SELinux Configuration
- Restricts execution of non-Firefox applications.
- Protects critical system files and directories.
- Ensures `hash_generator` and `verifier` function securely.

### Integrity Verification
- Prevents tampering with `.dex`, `.so`, and `AndroidManifest.xml` files.
- Validates remote file origins to prevent unauthorized modifications.

