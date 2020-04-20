#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <string.h>
#include <shadow.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/crypto.h>
#include <openssl/conf.h>
#include <openssl/err.h>
#include "acl.h"
#include "utils.h"

// TODO modify/improve implementations?

// common constants
const int KEY_SIZE_BITS = 256; // bits
const int IV_SIZE_BITS = 128; // bits

// functions for encryption-decryption and sign-verify

char* get_password()
{
    int caller_uid = getuid();
    struct passwd* caller_pwd = getpwuid(caller_uid);
    struct spwd* caller_spwd = getspnam(caller_pwd -> pw_name);
    char* passphrase = caller_spwd -> sp_pwdp;
    return passphrase;
}

char* get_username()
{
    int caller_uid = getuid();
    struct passwd* caller_pwd = getpwuid(caller_uid);
    char* username = caller_pwd -> pw_name;
    return username;
}

void generate_key_iv(char** key, char** iv)
{
    char* passphrase = get_password(); // TODO what if there is no password? (NULL or something else?)
    char* salt = get_username();
    char key_iv[KEY_SIZE_BITS / 8 + IV_SIZE_BITS / 8 + 10]; // 10 as a buffer if anything goes unexpected
    PKCS5_PBKDF2_HMAC_SHA1(passphrase, strlen(passphrase), salt, strlen(salt), 1024, KEY_SIZE_BITS / 8 + IV_SIZE_BITS / 8, key_iv);
    *key = substring(key_iv, 0, KEY_SIZE_BITS / 8);
    *iv = substring(key_iv, KEY_SIZE_BITS / 8, KEY_SIZE_BITS / 8 + IV_SIZE_BITS / 8);
}

char* encrypt_string(char* plaintext, char* key, char* iv)
{
    EVP_CIPHER_CTX *context;
    context = EVP_CIPHER_CTX_new();
    
    if (context == NULL)
    {
        perror("Cannot initialise context.");
        exit(1);
    }

    int result = EVP_EncryptInit_ex(context, EVP_aes_256_cbc(), NULL, key, iv);
    if (result != 1)
    {
        perror("Cannot initialise operation.");
        exit(1);
    }

    char* ciphertext = (char*) malloc(100000);
    int ciphertext_len = 0;

    result = EVP_EncryptUpdate(context, ciphertext, &ciphertext_len, plaintext, strlen(plaintext));

    if (result != 1)
    {
        perror("Couldn't complete encryption rounds.");
        exit(1);
    }

    result = EVP_EncryptFinal_ex(context, ciphertext + ciphertext_len, &ciphertext_len);

    if (result != 1)
    {
        perror("Couldn't complete the final encryption round.");
        exit(1);
    }

    EVP_CIPHER_CTX_free(context);

    return ciphertext;
}

char* decrypt_string(char* ciphertext, char* key, char* iv)
{
    EVP_CIPHER_CTX *context;
    context = EVP_CIPHER_CTX_new(); // TODO can the context be shared between encr and decr?
    
    if (context == NULL)
    {
        perror("Cannot initialise context.");
        exit(1);
    }

    int result = EVP_DecryptInit_ex(context, EVP_aes_256_cbc(), NULL, key, iv);

    if (result != 1)
    {
        perror("Cannot initialise operation.");
        exit(1);
    }

    char* plaintext = (char*) malloc(100000);
    int plaintext_len = 0;

    result = EVP_DecryptUpdate(context, plaintext, &plaintext_len, ciphertext, strlen(ciphertext));

    if (result != 1)
    {
        perror("Cannot decrypt.");
        exit(1);
    }

    result = EVP_DecryptFinal_ex(context, plaintext + plaintext_len, &plaintext_len);

    if (result != 1)
    {
        perror("Cannot decrypt final step.");
        exit(1);
    }

    EVP_CIPHER_CTX_free(context);

    return plaintext;
}

void fsign(char* filepath, char* key, char* iv)
{
    // TODO error handling
    char* buffer = read_from_file(filepath);
    EVP_MD_CTX* context;
    context = EVP_MD_CTX_create();
    if (context == NULL)
    {
        perror("Cannot create context.");
        exit(1);
    }
    
    EVP_MD* message_digest = EVP_get_digestbyname("SHA512");
    if (message_digest == NULL)
    {
        perror("Cannot retrieve message_digest.");
        exit(1);
    }

    int result = EVP_DigestInit_ex(context, message_digest, NULL);

    if (result != 1)
    {
        perror("Cannot initialize digest.");
        exit(1);
    }

    
}