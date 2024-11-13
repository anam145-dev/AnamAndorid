# hash_calculator.py
import hashlib
import os
import json

firefox_dir = "/system/app/firefox"  # Firefox 파일들이 설치된 경로
hash_store_path = "/data/integrity/hash_store.json"  # 해시 저장 경로

def calculate_hash(file_path):
    sha256 = hashlib.sha256()
    with open(file_path, "rb") as f:
        while chunk := f.read(8192):
            sha256.update(chunk)
    return sha256.hexdigest()

def store_hashes():
    hashes = {}
    for root, dirs, files in os.walk(firefox_dir):
        for file in files:
            if file.endswith((".dex", ".so")):
                file_path = os.path.join(root, file)
                file_hash = calculate_hash(file_path)
                hashes[file_path] = file_hash
    with open(hash_store_path, "w") as f:
        json.dump(hashes, f)

store_hashes()

