# 경로: device/lenovo/Tab_P11/integrity/remote_file_integrity.py
import hashlib
import requests
import json
import os

# 서버에서 hash_manifest.json 파일 다운로드
def download_manifest():
    url = "https://server.com/path/to/hash_manifest.json"  # 서버에 있는 hash_manifest.json 파일 URL
    response = requests.get(url)
    if response.status_code == 200:
        with open("/data/integrity/hash_manifest.json", "w") as f:
            f.write(response.text)
    return response.json()

# 파일의 SHA-256 해시 계산
def calculate_hash(file_path):
    sha256 = hashlib.sha256()
    with open(file_path, "rb") as f:
        while chunk := f.read(8192):
            sha256.update(chunk)
    return sha256.hexdigest()

# 원격 서버 파일의 무결성 확인
def verify_remote_files():
    # hash_manifest.json 파일 로드
    with open("/data/integrity/hash_manifest.json", "r") as f:
        manifest = json.load(f)

    for file_path, expected_hash in manifest.items():
        # 로컬에 다운로드한 파일 위치
        local_file_path = f"/data/downloads/{os.path.basename(file_path)}"
        
        # 파일 존재 확인 및 해시값 비교
        if os.path.exists(local_file_path):
            actual_hash = calculate_hash(local_file_path)
            if actual_hash != expected_hash:
                print(f"{local_file_path} is compromised!")
            else:
                print(f"{local_file_path} is intact.")
        else:
            print(f"{local_file_path} does not exist for verification.")

# hash_manifest.json을 다운로드하고 무결성 검사 실행
download_manifest()
verify_remote_files()

