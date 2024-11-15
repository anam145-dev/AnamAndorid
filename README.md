###lenovo Tab P11 AOSP System Image Build

Lenovo Tab P11 기기를 대상으로 AOSP 기반 시스템 이미지를 빌드하는 과정을 설명한다. 
Firefox를 prebuilt application으로 추가하고, SELinux 정책 및 파일 무결성 검사를 설정하여 보안을 강화한다.

#디렉토리 구조
![image](https://github.com/user-attachments/assets/6b2e2755-a850-4e6d-b174-0df17b4fcf00)



#디렉토리 설명
- sepolicy/: SELinux 정책 파일을 보관.
  - `firefox.te`: Firefox와 관련된 SELinux 정책 정의.
- integrity/: 파일 무결성 검사 설정 관련 파일을 보관.
  - SELinux 정책에서 integrity 폴더 내 파일 실행을 Firefox와 연계.
- Android.bp: 빌드 시스템에서 Tab_P11 디렉토리를 포함하기 위한 설정 파일.



#주요 설정
1. Prebuilt Application
   - Firefox를 prebuilt application으로 설정.
   - Firefox 외의 실행 파일 설치 및 실행 방지 (ADB 제외).

2. SELinux 정책
   - `firefox.te` 파일에서 다음 사항 설정:
     - Firefox만 실행 가능하도록 제한.
     - integrity 폴더 내 파일의 실행을 허용.

3. 파일 무결성 검사
   - integrity 폴더 내에서 무결성 검사 관련 설정 파일 관리.

4. 패키지 관리 제한
   - 패키지 설치를 담당하는 Package Installer 제거.
   - APK 설치 경로를 통한 실행 차단.



#시스템 이미지 빌드
# AOSP 빌드 과정
1. AOSP 소스 다운로드 및 환경 설정.
2. `device/lenovo/Tab_P11` 디렉토리 생성 및 설정 파일 추가.
3. 다음 명령어를 실행하여 시스템 이미지 빌드:
   bash
   source build/envsetup.sh
   lunch aosp_arm64-eng
   make -j$(nproc)
   

 빌드 시 문제 및 해결 방안
- WiFi 문제 발생 가능성  
  WiFi 관련 문제 발생 시 [WiFi 문제 해결](https://www.notion.so/hwseclab/wifi-cae1f4956a444c1caa6dd1eb58f0f25b?pvs=4)을 참고.



 참고 자료
- [Build AOSP's GSI Image](https://www.notion.so/hwseclab/Build-System-Image-9a1add6c3dd440b1aeabe5c2ec521624?pvs=4)
- [WiFi 문제 해결](https://www.notion.so/hwseclab/wifi-cae1f4956a444c1caa6dd1eb58f0f25b?pvs=4)
