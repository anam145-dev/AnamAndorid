#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <dirent.h>
#include <unistd.h>

#define HASH_STORAGE "/data/integrity/hash_storage"
#define PUBLIC_KEY_PATH "/data/integrity/public_key_storage/public_key.pem"
#define REMOTE_FILE_SIGNATURE "remote_file.sig"
#define REMOTE_FILE_PATH "downloaded_file"
#define MAX_PATH 1024

void calculate_sha256(const char *filepath, unsigned char *output);
int verify_local_file(const char *filepath, const char *hash_storage, RSA *rsa);
int verify_remote_file(const char *file_path, const char *signature_path, RSA *rsa);

int main() {
    // Load the public key
    FILE *key_file = fopen(PUBLIC_KEY_PATH, "r");
    if (!key_file) {
        perror("Error opening public key file");
        return EXIT_FAILURE;
    }
    RSA *rsa = PEM_read_RSA_PUBKEY(key_file, NULL, NULL, NULL);
    fclose(key_file);
    if (!rsa) {
        ERR_print_errors_fp(stderr);
        return EXIT_FAILURE;
    }

    // Local File Verification
    printf("Verifying local files...\n");
    const char *local_files[] = {
        "/apps/packages/Firefox/classes.dex",
        "/apps/packages/Firefox/lib/x86_64/libfirefox.so",
        "/apps/packages/Firefox/AndroidManifest.xml"
    };

    for (int i = 0; i < sizeof(local_files) / sizeof(local_files[0]); i++) {
        if (!verify_local_file(local_files[i], HASH_STORAGE, rsa)) {
            fprintf(stderr, "Local file verification failed for %s\n", local_files[i]);
            RSA_free(rsa);
            return EXIT_FAILURE;
        }
    }
    printf("All local files verified successfully.\n");

    // Remote File Verification
    printf("Verifying remote files...\n");
    if (!verify_remote_file(REMOTE_FILE_PATH, REMOTE_FILE_SIGNATURE, rsa)) {
        fprintf(stderr, "Remote file verification failed for %s\n", REMOTE_FILE_PATH);
        RSA_free(rsa);
        return EXIT_FAILURE;
    }
    printf("Remote file verified successfully.\n");

    RSA_free(rsa);
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

// Function to verify a local file against stored hashes
int verify_local_file(const char *filepath, const char *hash_storage, RSA *rsa) {
    unsigned char computed_hash[SHA256_DIGEST_LENGTH];
    calculate_sha256(filepath, computed_hash);

    // Load the encrypted hash from hash storage
    char hash_path[MAX_PATH];
    snprintf(hash_path, sizeof(hash_path), "%s/%s.hash", hash_storage, strrchr(filepath, '/') + 1);

    FILE *hash_file = fopen(hash_path, "rb");
    if (!hash_file) {
        perror("Error opening hash storage file");
        return 0;
    }

    unsigned char encrypted_hash[RSA_size(rsa)];
    size_t encrypted_length = fread(encrypted_hash, 1, sizeof(encrypted_hash), hash_file);
    fclose(hash_file);

    // Decrypt the hash
    unsigned char decrypted_hash[SHA256_DIGEST_LENGTH];
    int decrypted_length = RSA_public_decrypt(encrypted_length, encrypted_hash, decrypted_hash, rsa, RSA_PKCS1_PADDING);
    if (decrypted_length == -1) {
        ERR_print_errors_fp(stderr);
        return 0;
    }

    // Compare the computed hash with the decrypted hash
    if (memcmp(computed_hash, decrypted_hash, SHA256_DIGEST_LENGTH) != 0) {
        fprintf(stderr, "Hash mismatch for %s\n", filepath);
        return 0;
    }

    printf("Local file verified: %s\n", filepath);
    return 1;
}

// Function to verify a remote file's integrity and authenticity
int verify_remote_file(const char *file_path, const char *signature_path, RSA *rsa) {
    unsigned char computed_hash[SHA256_DIGEST_LENGTH];
    calculate_sha256(file_path, computed_hash);

    // Load the signature
    FILE *sig_file = fopen(signature_path, "rb");
    if (!sig_file) {
        perror("Error opening signature file");
        return 0;
    }

    fseek(sig_file, 0, SEEK_END);
    size_t sig_len = ftell(sig_file);
    fseek(sig_file, 0, SEEK_SET);

    unsigned char *signature = malloc(sig_len);
    fread(signature, 1, sig_len, sig_file);
    fclose(sig_file);

    // Verify the signature
    int result = RSA_verify(NID_sha256, computed_hash, SHA256_DIGEST_LENGTH, signature, sig_len, rsa);
    free(signature);

    if (!result) {
        ERR_print_errors_fp(stderr);
        return 0;
    }

    printf("Remote file verified: %s\n", file_path);
    return 1;
}

