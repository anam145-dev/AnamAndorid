#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <zip.h>
#include <unistd.h>
#include <sys/stat.h>

#define APK_PATH "/apps/packages/Firefox/Firefox.apk"
#define OUTPUT_DIR "extracted_files"
#define HASH_STORAGE "/data/integrity/hash_storage"
#define PRIVATE_KEY_PATH "private_key.pem"
#define MAX_PATH 1024

void calculate_sha256(const char *filepath, unsigned char *output);
void store_encrypted_hash(const char *filepath, const char *hash_storage, RSA *rsa);
void extract_file_from_apk(const char *apk_path, const char *file_to_extract, const char *output_path);
void extract_all_so_files_from_apk(const char *apk_path, const char *output_dir);

int main() {
    // Load private key
    FILE *key_file = fopen(PRIVATE_KEY_PATH, "r");
    if (!key_file) {
        perror("Error opening private key file");
        return EXIT_FAILURE;
    }
    RSA *rsa = PEM_read_RSAPrivateKey(key_file, NULL, NULL, NULL);
    fclose(key_file);
    if (!rsa) {
        ERR_print_errors_fp(stderr);
        return EXIT_FAILURE;
    }

    // Ensure the output and hash storage directories exist
    if (access(OUTPUT_DIR, F_OK) == -1) {
        if (mkdir(OUTPUT_DIR, 0700) != 0) {
            perror("Error creating output directory");
            return EXIT_FAILURE;
        }
    }

    if (access(HASH_STORAGE, F_OK) == -1) {
        if (mkdir(HASH_STORAGE, 0700) != 0) {
            perror("Error creating hash storage directory");
            return EXIT_FAILURE;
        }
    }

    // Define target files
    const char *files_to_extract[] = {
        "classes.dex",
        "AndroidManifest.xml"
    };
    const char *output_files[] = {
        OUTPUT_DIR "/classes.dex",
        OUTPUT_DIR "/AndroidManifest.xml"
    };

    // Extract and process target files
    for (int i = 0; i < 2; i++) {
        extract_file_from_apk(APK_PATH, files_to_extract[i], output_files[i]);
        store_encrypted_hash(output_files[i], HASH_STORAGE, rsa);
    }

    // Extract all .so files from lib/x86_64/ and store their encrypted hashes
    extract_all_so_files_from_apk(APK_PATH, OUTPUT_DIR);
    DIR *dir = opendir(OUTPUT_DIR);
    if (!dir) {
        perror("Error opening output directory");
        return EXIT_FAILURE;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".so") != NULL) {
            char filepath[MAX_PATH];
            snprintf(filepath, sizeof(filepath), "%s/%s", OUTPUT_DIR, entry->d_name);
            store_encrypted_hash(filepath, HASH_STORAGE, rsa);
        }
    }

    closedir(dir);
    RSA_free(rsa);

    printf("Hashes generated, encrypted, and stored successfully.\n");
    return 0;
}

// Function to calculate SHA-256 hash of a file
void calculate_sha256(const char *filepath, unsigned char *output) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    char buffer[1024];
    FILE *file = fopen(filepath, "rb");
    if (!file) {
        perror("Error opening file for hashing");
        exit(EXIT_FAILURE);
    }

    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), file))) {
        SHA256_Update(&sha256, buffer, bytes);
    }
    SHA256_Final(hash, &sha256);
    fclose(file);

    memcpy(output, hash, SHA256_DIGEST_LENGTH);
}

// Function to encrypt and store hash in hash storage
void store_encrypted_hash(const char *filepath, const char *hash_storage, RSA *rsa) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    unsigned char encrypted_hash[RSA_size(rsa)];
    calculate_sha256(filepath, hash);

    int encrypted_length = RSA_private_encrypt(SHA256_DIGEST_LENGTH, hash, encrypted_hash, rsa, RSA_PKCS1_PADDING);
    if (encrypted_length == -1) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    // Save the encrypted hash to the hash storage directory
    char hash_path[MAX_PATH];
    snprintf(hash_path, sizeof(hash_path), "%s/%s.hash", hash_storage, strrchr(filepath, '/') + 1);

    FILE *hash_file = fopen(hash_path, "wb");
    if (!hash_file) {
        perror("Error opening hash storage file");
        exit(EXIT_FAILURE);
    }

    fwrite(encrypted_hash, 1, encrypted_length, hash_file);
    fclose(hash_file);

    printf("Encrypted hash stored for %s\n", filepath);
}

// Function to extract a specific file from an APK
void extract_file_from_apk(const char *apk_path, const char *file_to_extract, const char *output_path) {
    // (Implementation similar to the one above)
}

// Function to extract all .so files from lib/x86_64/
void extract_all_so_files_from_apk(const char *apk_path, const char *output_dir) {
    // (Implementation similar to the one above)
}

