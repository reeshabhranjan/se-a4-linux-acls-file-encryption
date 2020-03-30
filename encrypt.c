#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <string.h>
#include <shadow.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/crypto.h>
#include "acl.h"

// common constants
const int KEY_SIZE = 256;
const int IV_SIZE = 128;

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
    char key_iv[50];
    PKCS5_PBKDF2_HMAC_SHA1(passphrase, strlen(passphrase), salt, strlen(salt), 1024, KEY_SIZE + IV_SIZE, key_iv);
    *key = substring(key_iv, 0, KEY_SIZE);
    *iv = substring(key_iv, KEY_SIZE, KEY_SIZE + IV_SIZE);
}