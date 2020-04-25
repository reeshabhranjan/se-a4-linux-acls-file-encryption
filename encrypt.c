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
#include <openssl/rand.h>
#include <openssl/pem.h>
#include "acl.h"
#include "utils.h"
#include "security.h"

// TODO modify/improve implementations?

// common constants
const int KEY_SIZE_BITS = 256; // bits
const int IV_SIZE_BITS = 128; // bits
#define SIGNATURE_EXTENSION ".sign"
#define RANDOM_NUM_EXTENSION ".rand"
#define PART_2_SUB_DIRECTORY "part-2/"
#define RSA_FILE_EXTENSION ".pem"

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

void generate_key_iv_from_passphrase_and_salt(char** key, char** iv, char* passphrase, char* salt)
{
    char key_iv[KEY_SIZE_BITS / 8 + IV_SIZE_BITS / 8 + 10]; // 10 as a buffer if anything goes unexpected
    // TODO is it safe to call strlen on salt?
    PKCS5_PBKDF2_HMAC_SHA1(passphrase, strlen(passphrase), salt, strlen(salt), 1024, KEY_SIZE_BITS / 8 + IV_SIZE_BITS / 8, key_iv);
    *key = substring(key_iv, 0, KEY_SIZE_BITS / 8);
    *iv = substring(key_iv, KEY_SIZE_BITS / 8, KEY_SIZE_BITS / 8 + IV_SIZE_BITS / 8);
    printf("[!] encrypt.c: key_generation: key size: %d\n", KEY_SIZE_BITS / 8);   
    printf("[!] encrypt.c: key_generation:  iv size: %d\n", (KEY_SIZE_BITS / 8 + IV_SIZE_BITS / 8 - KEY_SIZE_BITS / 8));   
}

void generate_key_iv(char** key, char** iv)
{
    char* passphrase = get_password(); // TODO what if there is no password? (NULL or something else?)
    char* salt = get_username();
    generate_key_iv_from_passphrase_and_salt(key, iv, passphrase, salt);
}

char* encrypt_string(char* plaintext, char* key, char* iv, int* ciphertext_len_returned)
{
    printf("[!] encrypt_string key_len: %d\n", (int) strlen(key));
    printf("[!] encrypt_string iv_len: %d\n", (int) strlen(iv));
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
    memset(ciphertext, 0, 100000);
    int ciphertext_len = 0;

    result = EVP_EncryptUpdate(context, ciphertext, &ciphertext_len, plaintext, strlen(plaintext));

    if (result != 1)
    {
        perror("Couldn't complete encryption rounds.");
        exit(1);
    }
    *ciphertext_len_returned = 0;
    *ciphertext_len_returned += ciphertext_len;

    result = EVP_EncryptFinal_ex(context, ciphertext + ciphertext_len, &ciphertext_len);

    if (result != 1)
    {
        perror("Couldn't complete the final encryption round.");
        exit(1);
    }

    *ciphertext_len_returned += ciphertext_len;
    EVP_CIPHER_CTX_free(context);
    return ciphertext;
}

char* decrypt_string(char* ciphertext, char* key, char* iv, int ciphertext_len)
{
    // TODO add null byte after ciphertext?
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
    memset(plaintext, 0, 100000);
    int plaintext_len = 0;

    result = EVP_DecryptUpdate(context, plaintext, &plaintext_len, ciphertext, ciphertext_len);

    if (result != 1)
    {
        perror("Cannot decrypt.");
        exit(1);
    }

    result = EVP_DecryptFinal_ex(context, plaintext + plaintext_len, &plaintext_len);

    if (result != 1)
    {
        perror("Cannot decrypt final step.");
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    EVP_CIPHER_CTX_free(context);

    return plaintext;
}

char* fsign(char* buffer, int buffer_len, int* checksum_len_return)
{
    // TODO error handling
    // TODO check for read and write permissions
    EVP_MD_CTX* context;
    context = EVP_MD_CTX_create();
    if (context == NULL)
    {
        perror("Cannot create context.");
        exit(1);
    }
    
    // TODO make key size unique
    const EVP_MD* message_digest = EVP_get_digestbyname("SHA512");
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

    char* key;
    char* iv;
    generate_key_iv(&key, &iv);
    EVP_PKEY* evp_key = EVP_PKEY_new_mac_key(EVP_PKEY_HMAC, NULL, key, strlen(key));

    result = EVP_DigestSignInit(context, NULL, message_digest, NULL, evp_key);

    if (result != 1)
    {
        perror("Cannot initialise digest-sign.");
        exit(1);
    }

    result = EVP_DigestSignUpdate(context, buffer, buffer_len);
    printf("[!] encrypt.c fsign: length of buffer: %d\n", buffer_len);

    if (result != 1)
    {
        perror("Cannot execute update step of digest-sign.");
        exit(1);
    }
    
    char* checksum;
    size_t checksumlen;
    // this initial call is necessary to know the checksum length
    result = EVP_DigestSignFinal(context, NULL, &checksumlen);
    if (result != 1 || checksumlen <= 0)
    {
        perror("Cannot calculate the checksum length.");
        exit(1);
    }

    checksum = OPENSSL_malloc(checksumlen);

    if (checksum == NULL)
    {
        perror("Couldn't initialise checksum variable.");
        exit(1);
    }

    result = EVP_DigestSignFinal(context, checksum, &checksumlen);

    if (result == -1)
    {
        perror("Couldn't retrieve the checksum from the last step of HMAC.");
        exit(1);
    }

    *checksum_len_return = checksumlen;

    return checksum;
}

int fverify(char* filepath)
{
    // TODO array of bytes v/s array of chars
    // TODO check for read permissions
    char* filepath_checksum = concatenate_strings(filepath, SIGNATURE_EXTENSION);
    int checksum_len_file;
    char* string_checksum_file = read_from_file_with_num_bytes(filepath_checksum, &checksum_len_file);
    int ciphertext_len;
    char* content = read_from_file_with_num_bytes(filepath, &ciphertext_len);
    // printf("[!] encrypt.c fverify: ciphertext read: %s\n", content);
    printf_custom("[!] encrypt.c fverify: ciphertext read", content, ciphertext_len);
    int checksum_len_derived;
    char* string_checksum_derived = fsign(content, ciphertext_len, &checksum_len_derived);

    printf("[!] encryp.c verification: file check sum:    %s\n", string_checksum_file);
    printf("[!] encryp.c verification: derived check sum: %s\n", string_checksum_derived);

    printf("[!] encrypt.c checksum length retrieved: %d\n", checksum_len_file);

    if (checksum_len_derived != checksum_len_file)
    {
        printf("The checksum length doesnt match for %s\n", filepath_checksum);
        exit(1);
    }

    int same = 1;

    for (int i = 0; i < checksum_len_derived; i++)
    {
        if (string_checksum_file[i] != string_checksum_derived[i])
        {
            same = 0;
            break;
        }
    }
    return same;
}

char* gen_rand(int length)
{
    char* random_string = (char*) malloc(length + 1);
    for (int i = 0; i < length; i++)
    {
        random_string[i] = '0' + (rand()%10);
    }
    
    random_string[length] = '\0';
    return random_string;
}

char* encrypt_string_trapdoor(char* buffer, int* ciphertext_len_return)
{
    int n = strlen(buffer);
    char* filepath_random = concatenate_strings(PART_2_SUB_DIRECTORY, get_username());
    char* dummy = filepath_random;
    filepath_random = concatenate_strings(filepath_random, RANDOM_NUM_EXTENSION);
    free(dummy);
    char* key;
    char* iv;
    generate_key_iv(&key, &iv);
    key = "xxxxxxxxxxxxxxxx";
    iv = "xxxxxxxxxxxxxxxx";
    // key = "key123";
    // iv = "iv123";
    char* dummy_remove;
    if (!file_exists(filepath_random))
    {
        create_file(filepath_random, getuid(), getgid(), 0644); // TODO permission needs to be set accordingly?
        // TODO setacl
        char* string_random_number = gen_rand(64);
        printf("[!] encrypt.c: random_number generated: %s\n", string_random_number);
        int ciphertext_len;
        char* string_random_number_encrypted = encrypt_string(string_random_number, key, iv, &ciphertext_len);
        printf("[!] encrypt.c: created encrypted random file with bytes: %d\n", ciphertext_len);
        printf("[!] encypt.c: key used for encrypting: %s", key);
        printf("[!] encypt.c: iv used for encrypting: %s", iv);
        write_to_file_with_len(filepath_random, string_random_number_encrypted, ciphertext_len, 1);
        dummy_remove = string_random_number_encrypted;
    }
    int ciphertext_len_random;
    char* string_random_number_encrypted = read_from_file_with_num_bytes(filepath_random, &ciphertext_len_random);
    string_random_number_encrypted = dummy_remove;
    printf("[!] encrypt.c ciphertext_len_random: %d\n", ciphertext_len_random);
    printf("[!] encypt.c: key used for decrypting: %s", key);
    printf("[!] encypt.c: iv used for decrypting: %s", iv);
    char* string_random_number = decrypt_string(string_random_number_encrypted, key, iv, ciphertext_len_random);

    // TODO make sure you are not changing the key and iv variables
    // pointing at some field of pwd struct, like the above char* key and char* iv

    // now generate an encryption key using random number
    // as the passphrase and username as salt
    char* key_trapdoor;
    char* iv_trapdoor;
    generate_key_iv_from_passphrase_and_salt(&key_trapdoor, &iv_trapdoor, string_random_number, get_username());

    int ciphertext_len;
    char* string_encrypted_trapdoor = encrypt_string(buffer, key_trapdoor, iv_trapdoor, &ciphertext_len);
    *ciphertext_len_return = ciphertext_len;

    return string_encrypted_trapdoor;
}

EVP_PKEY* read_rsa_private_key_from_file()
{
    // TODO replace the path with home directory
    char* filepath_rsa_credentials = concatenate_strings(PART_2_SUB_DIRECTORY, get_username());
    char* dummy = filepath_rsa_credentials;
    filepath_rsa_credentials = concatenate_strings(filepath_rsa_credentials, RSA_FILE_EXTENSION);
    free(dummy);

    if (!file_exists(filepath_rsa_credentials))
    {
        // TODO assume that the current user will always have a file with his/her name
        // and the related permissions?
        printf("There is no RSA public-private key pair file correspnding to current user\n");
        exit(1);
    }

    FILE* rsa_pem_file = fopen(filepath_rsa_credentials, "r");
    EVP_PKEY* rsa_private_key = PEM_read_PrivateKey(rsa_pem_file, &rsa_private_key, NULL, NULL);

    if (rsa_private_key == NULL)
    {
        printf("Cannot read RSA private key\n");
        exit(1);
    }

    fclose(rsa_pem_file);

    return rsa_private_key;
}

EVP_PKEY* read_rsa_public_key_from_file()
{
    // TODO replace the path with home directory
    char* filepath_rsa_credentials = concatenate_strings(PART_2_SUB_DIRECTORY, get_username());
    char* dummy = filepath_rsa_credentials;
    filepath_rsa_credentials = concatenate_strings(filepath_rsa_credentials, RSA_FILE_EXTENSION);
    free(dummy);

    if (!file_exists(filepath_rsa_credentials))
    {
        // TODO assume that the current user will always have a file with his/her name
        // and the related permissions?
        printf("There is no RSA public-private key pair file correspnding to current user\n");
        exit(1);
    }

    FILE* rsa_pem_file = fopen(filepath_rsa_credentials, "r");
    EVP_PKEY* rsa_public_key = PEM_read_PUBKEY(rsa_pem_file, &rsa_public_key, NULL, NULL);

    if (rsa_public_key == NULL)
    {
        printf("Cannot read RSA private key\n");
        exit(1);
    }

    fclose(rsa_pem_file);
    return rsa_public_key;
}

char* create_hmac_trapdoor(char* buffer)
{
    EVP_PKEY* private_key = read_rsa_private_key_from_file();
    EVP_MD_CTX* context = EVP_MD_CTX_create();
    
    if (context == NULL)
    {
        printf("Cannot create hmac-trapdoor's context.\n");
        exit(1);
    }

    int result = EVP_DigestSignInit(context, NULL, EVP_sha512(), NULL, private_key);
    if (result != 1)
    {
        printf("create_hmac_trapdoor: EVP_DigestSignInit failed.\n");
        exit(1);
    }

    result = EVP_DigestSignUpdate(context, buffer, strlen(buffer));

    if (result != 1)
    {
        printf("create_hmac_trapdoor: EVP_DigestSignUpdate failed.\n");
        exit(1);
    }

    // this call is needed to check the length of the signature
    size_t sig_len;

    result = EVP_DigestSignFinal(context, NULL, &sig_len);

    if (result != 1 || sig_len < 0)
    {
        printf("create_hmac_trapdoor: EVP_DigestSignFinal (pre-call) failed.\n");
        exit(1);
    }

    char* string_signature = OPENSSL_malloc(sig_len);

    if (string_signature == NULL)
    {
        printf("create_hmac_trapdoor: OPENSSL_malloc failed.\n");
        exit(1);
    }

    result = EVP_DigestSignFinal(context, string_signature, &sig_len);

    // TODO call EVP_MD_CTX_destroy(context) everywhere
    // TODO check every free() call such that it doesn't free
    // a pointer passed into the functin
    return string_signature;
}

int verify_hmac_trapdoor(char* filepath)
{
    // recalculate the signature from data file
    if (!file_exists(filepath))
    {
        printf("The input file doesn't exist.\n");
        exit(1);
    }

    int message_len;
    char* string_message = read_from_file_with_num_bytes(filepath, &message_len);

    // start verification

    char* filepath_signature = concatenate_strings(filepath, SIGNATURE_EXTENSION);

    if (!file_exists(filepath_signature))
    {
        printf("verify_hmac_trapdoor: cannot find the required signature file\n");
        exit(1);
    }

    int signature_len;
    char* string_signature = read_from_file_with_num_bytes(filepath_signature, &signature_len);

    EVP_PKEY* public_key = read_rsa_public_key_from_file();

    EVP_MD_CTX* context = EVP_MD_CTX_create();

    if (context == NULL)
    {
        printf("verify_hmac_trapdoor: cannot create context\n");
        exit(1);
    }

    int result = EVP_DigestVerifyInit(context, NULL, EVP_sha512(), NULL, public_key);

    if (result != 1)
    {
        printf("verify_hmac_trapdoor: EVP_DigestVerifyInit failed\n");
        exit(1);
    }

    result = EVP_DigestVerifyUpdate(context, string_message, message_len);

    if (result != 1)
    {
        printf("verify_hmac_trapdoor: EVP_DigestVerifyUpdate failed.\n");
        exit(1);
    }

    // TODO fix this
    result = EVP_DigestVerifyFinal(context, string_signature, signature_len);

    return result;
}

char* decrypt_string_trapdoor(char* ciphertext, int ciphertext_len)
{
    char* filepath_random_number = concatenate_strings(PART_2_SUB_DIRECTORY, get_username());
    char* dummy = filepath_random_number;
    filepath_random_number = concatenate_strings(filepath_random_number, RANDOM_NUM_EXTENSION);
    free(dummy);

    if (!file_exists(filepath_random_number))
    {
        printf("Random number file does not exist.\n");
        exit(1);
    }

    char* key;
    char* iv;
    generate_key_iv(&key, &iv);
    key = "xxxxxxxxxxxxxxxx";
    iv = "xxxxxxxxxxxxxxxx";

    int ciphertext_len_random;
    char* string_random_encrypted = read_from_file_with_num_bytes(filepath_random_number, &ciphertext_len_random);
    char* string_random = decrypt_string(string_random_encrypted, key, iv, ciphertext_len_random);


    char* key_trapdoor;
    char* iv_trapdoor;
    generate_key_iv_from_passphrase_and_salt(&key, &iv, string_random, get_username());

    char* plaintext = decrypt_string(ciphertext, key_trapdoor, iv_trapdoor, ciphertext_len);

    return plaintext;
}