import hashlib
import subprocess
import zipfile
import json  # JSON 모듈 추가

# 설정된 경로들
firefox_apk_path = "/packages/apps/Firefox/Firefox.apk"  # Firefox APK의 원본 경로
hash_store_encrypted = "/data/integrity/encrypted_hash_store.enc"
hash_store_decrypted = "/data/integrity/hash_store.json"
allow_file = "/data/integrity/firefox_allowed"  # 무결성 검사 통과 시 생성할 파일

def calculate_hash(file_data):
    """SHA-256 해시값 계산"""
    sha256 = hashlib.sha256()
    sha256.update(file_data)
    return sha256.hexdigest()

def decrypt_hash_store():
    """암호화된 해시 저장 파일 복호화"""
    subprocess.run(["openssl", "enc", "-d", "-aes-256-cbc", "-in", hash_store_encrypted, "-out", hash_store_decrypted, "-k", "<your_password>"])

def extract_and_hash():
    """ APK 파일에서 .dex 및 .so 파일을 추출하고 해시를 계산하여 저장 """
    hashes = {}
    with zipfile.ZipFile(firefox_apk_path, 'r') as apk:
        for file in apk.namelist():
            if file.endswith((".dex", ".so")):
                with apk.open(file) as f:
                    file_data = f.read()
                    file_hash = calculate_hash(file_data)
                    hashes[file] = file_hash
    # 해시값을 JSON 파일로 저장
    with open(hash_store_decrypted, "w") as f:
        json.dump(hashes, f)

def verify_integrity():
    """ 무결성 검사 수행 및 adb 로그 메시지 출력 """
    decrypt_hash_store()  # 암호화된 해시 파일 복호화
    with open(hash_store_decrypted, "r") as f:
        stored_hashes = json.load(f)

    all_files_intact = True
    with zipfile.ZipFile(firefox_apk_path, 'r') as apk:
        for file, stored_hash in stored_hashes.items():
            with apk.open(file) as f:
                file_data = f.read()
                current_hash = calculate_hash(file_data)
                if current_hash != stored_hash:
                    print(f"{file} has been modified!")
                    subprocess.run(["log", "-t", "IntegrityCheck", f"{file} has been modified!"])
                    all_files_intact = False
                else:
                    print(f"{file} is unchanged.")

    if all_files_intact:
        with open(allow_file, "w") as f:
            f.write("integrity check passed")
        subprocess.run(["log", "-t", "IntegrityCheck", "Integrity check passed. Firefox is allowed to run."])
    else:
        subprocess.run(["log", "-t", "IntegrityCheck", "Integrity check failed. Firefox is blocked."])

# 무결성 검사 실행
extract_and_hash()
verify_integrity()

