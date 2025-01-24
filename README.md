<<<<<<< HEAD
# lenovo Tab P11 AOSP System Image Build

Lenovo Tab P11 기기를 대상으로 AOSP 기반 시스템 이미지를 빌드하는 과정을 설명한다. 
Firefox를 prebuilt application으로 추가하고, SELinux 정책 및 파일 무결성 검사를 설정하여 보안을 강화한다.

## 디렉토리 구조


## 디렉토리 설명
- sepolicy/: SELinux 정책 파일을 보관.
  - `firefox.te`: Firefox와 관련된 SELinux 정책 정의.
- integrity/: 파일 무결성 검사 설정 관련 파일을 보관.
  - SELinux 정책에서 integrity 폴더 내 파일 실행을 Firefox와 연계.
- Android.bp: 빌드 시스템에서 Tab_P11 디렉토리를 포함하기 위한 설정 파일.



## 주요 설정
### 1. Prebuilt Application
   - Firefox를 prebuilt application으로 설정.
   - Firefox 외의 실행 파일 설치 및 실행 방지 (ADB 제외).

### 2. SELinux 정책
   - `firefox.te` 파일에서 다음 사항 설정:
     - Firefox만 실행 가능하도록 제한.
     - integrity 폴더 내 파일의 실행을 허용.

### 3. 파일 무결성 검사
   - integrity 폴더 내에서 무결성 검사 관련 설정 파일 관리.

### 4. 패키지 관리 제한
   - 패키지 설치를 담당하는 Package Installer 제거.
   - APK 설치 경로를 통한 실행 차단.



## 시스템 이미지 빌드
### AOSP 빌드 과정
1. AOSP 소스 다운로드 및 환경 설정.
2. `device/lenovo/Tab_P11` 디렉토리 생성 및 설정 파일 추가.
3. 다음 명령어를 실행하여 시스템 이미지 빌드:
   bash
   source build/envsetup.sh
   lunch aosp_arm64-eng
   make -j$(nproc)
   

## 빌드 시 문제 및 해결 방안
- WiFi 문제 발생 가능성  
  WiFi 관련 문제 발생 시 [WiFi 문제 해결](https://www.notion.so/hwseclab/wifi-cae1f4956a444c1caa6dd1eb58f0f25b?pvs=4)을 참고.


## 참고 자료
- [Build AOSP's GSI Image](https://www.notion.so/hwseclab/Build-System-Image-9a1add6c3dd440b1aeabe5c2ec521624?pvs=4)
- [WiFi 문제 해결](https://www.notion.so/hwseclab/wifi-cae1f4956a444c1caa6dd1eb58f0f25b?pvs=4)
=======
# File Integrity Check for Android

This repository contains the implementation of a File Integrity Check mechanism designed to enhance the security of Android devices by preventing malicious APKs from exploiting vulnerable entry points. This approach uses a two-layer defense mechanism: file integrity checks followed by SELinux policy enforcement and Package Manager removal.

---

## Overview

The File Integrity Check system comprises two main components: hash_generator and verifier. These components work together to prevent the installation and execution of vulnerable `.dex` and `.so` files or modifications to pre-built application files such as `AndroidManifest.xml`.

### Defense Layers:
1. First Line of Defense: File Integrity Check
   - Prevents installation of malicious files (.dex, .so) by verifying their integrity.
   - Ensures pre-built application files (e.g., `AndroidManifest.xml`) are not tampered with.

2. Second Line of Defense: SELinux Policy Enforcement and Package Manager Removal
   - SELinux policies restrict executable permissions to only pre-approved applications.
   - The Package Manager is removed to block the installation of additional APKs.

---

## Components

### 1. hash_generator
- Purpose: 
  - Generates hash values for the following pre-built application files during the AOSP build process:
    - `classes.dex`
    - `.so` libraries
    - `AndroidManifest.xml`
  - Encrypts the SHA-256 hash values using an RSA private key.
  - Stores the encrypted hash values in the `/data/integrity/hash_storage` directory.

- Execution: 
  - The program runs during the AOSP build process and ensures that the generated hash values are securely stored for later verification.

---

### 2. verifier
- Purpose: 
  - Validates the integrity of files before executing pre-built applications (e.g., Firefox).
  - Performs two types of checks:
    - Local Verification: Compares the current hash values of the files with the pre-generated hash values from the `hash_generator`.
    - Remote Verification: Ensures that files received from remote servers originate from trusted sources before allowing their use.

- Execution: 
  - The program runs on the Android device after flashing the system image (`system.img`).
  - It must be executed before launching pre-built applications to ensure their integrity.

---

## File Structure

The following is the directory structure for the File Integrity Check system:

device/lenovo/Tab_P11/ ├── Android.bp ├── BoardConfig.mk ├── device.mk ├── integrity │ ├── hash_generator.c │ ├── init.integrity.rc │ └── verifier.c └── sepolicy ├── file_contexts └── firefox.te

ruby
코드 복사

- `hash_generator.c`: Source code for hash generation during the build process.
- `verifier.c`: Source code for file verification during application execution.
- SELinux Policies:
  - Policies ensure that:
    1. All executable files (except Firefox) are blocked by default.
    2. A dedicated SELinux domain is created for Firefox, allowing its execution.
    3. `hash_generator` and `verifier` are allowed to execute and interact with the `/data/integrity` directory.
  - Policies also enable `verifier` to validate remote server files securely.

---

## SELinux Policy Highlights

The SELinux policies defined in `firefox.te` ensure the security and functionality of the File Integrity Check mechanism:

1. Firefox Application Restrictions:
   - Firefox can only access its data directory (`/data/data/org.mozilla.firefox`) and execute its files.
   - Non-Firefox APK installation or execution is blocked globally.

2. Integrity Checker Permissions:
   - `hash_generator` and `verifier` are allowed to:
     - Read Firefox-related files and write to `/data/integrity`.
     - Access public key storage for verification purposes.
     - Verify remote files received over the network.

3. Blocking Non-Firefox Executables:
   - Prevent execution or modification of APKs or executables from untrusted apps globally.
   - Restrict access to temporary directories and cached files.

4. Domain Transitions:
   - Ensures proper transitions for `hash_generator` and `verifier` to the defined SELinux domain.

---

## Build and Execution

### Building the System
1. Add the `hash_generator` and `verifier` source files to the AOSP source tree under `/device/lenovo/Tab_P11/integrity/`.
2. Ensure the `Android.bp`, `BoardConfig.mk`, and `device.mk` files include the necessary build configurations for:
   - `hash_generator`
   - `verifier`
   - SELinux policy files

3. Build the system:
   source build/envsetup.sh
   lunch <target>
   make
   
## Flashing the System Image
1. Flash the built `system.img` file to the Android device:
   fastboot flash system out/target/product/<target>/system.img
2. Reboot the device to apply the changes:
	fastboot reboot
	
## Security Features

### SELinux Configuration
1. Blocks the execution of non-Firefox APKs and restricts access to critical system files and directories.
2. Ensures that hash_generator and verifier can run securely and perform their intended tasks.

### Integrity Verification
1. Prevents tampering with important files such as .dex, .so, and AndroidManifest.xml.
2. Verifies the origin of remote files to prevent malicious replacements or unauthorized modifications
2. Verifies the origin of remote files to prevent malicious replacements or unauthorized modifications
>>>>>>> 3b70015 (commit message)
