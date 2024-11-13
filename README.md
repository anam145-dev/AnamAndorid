https://www.notion.so/hwseclab/Build-System-Image-9a1add6c3dd440b1aeabe5c2ec521624?pvs=4 참고
(build AOSP's GSI Image, add prebuilt application, delete package installer 등) 

AOSP 폴더 내에 device/lenovo/Tab_P11 폴더 생성
이후 Tab_P11 폴더 내에 sepolicy, integrity 폴더 및 Android.bp 등의 파일 생성

sepolicy 폴더: selinux 정책 설정 관련 파일 보관
integrity 폴더: 파일 무결성 검사 관련 파일 보관(integrity 폴더 안에 있는 파일에 대한 실행 설정 sepolicy 내의 firefox.te로 설정)

prebuilt application을 firefox로 설정했으며 firefox 이외의 실행파일 설치 및 실행 방지(adb 제외)

aosp로 system.img 빌드 시 와이파이 문제 발생 가능: https://www.notion.so/hwseclab/wifi-cae1f4956a444c1caa6dd1eb58f0f25b?pvs=4
