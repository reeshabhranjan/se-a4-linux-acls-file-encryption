#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <error.h>
#include <errno.h>
#include <string.h>
#include <pwd.h>
#include "acl.h"
#include "security.h"
#include "encrypt.h"
#include "utils.h"

int main(int argc, char* argv[])
{

    if (argc <= 1)
    {
        perror("Enter a filepath!\n");
        exit(1);
    }

    if (argc > 2)
    {
        perror("Please enter only filepath without spaces (or escaped)\n");
        exit(1);
    }

    char* filepath = argv[1];

    if (!file_exists(filepath))
    {
        printf("The file does not exist. Creating one now...\n");
        create_file(filepath, getuid(), getgid(), 0644);
    }

    if (!is_file(filepath))
    {
        perror("The file is not a regular file.");
        exit(1);
    }

    int caller_uid = getuid();
    struct passwd* pwd = (struct passwd*) malloc(sizeof(struct passwd));
    *pwd = *(getpwuid(caller_uid));
    char* caller_name = (char*) malloc(strlen(pwd -> pw_name) + 1);
    strcpy(caller_name, pwd -> pw_name);

    if (!validate(caller_name, filepath, 10))
    {
        perror("You do not have sufficient permission.");
        exit(1);
    }

    printf("UID: %d EUID: %d\n", getuid(), geteuid());

    printf("Enter a string (under 100000 characters)\n");
    char* s = (char*) malloc(100000);
    memset(s, 0, 100000);
    fgets(s, 100000, stdin);

    printf("Encrypting...\n");
    char* key;
    char* iv;

    generate_key_iv(&key, &iv);
    int ciphertext_len;
    char* ciphertext = encrypt_string(s, key, iv, &ciphertext_len);
    free(s);
    s = NULL;
    s = ciphertext;

    write_to_file_with_len(filepath, s, ciphertext_len, 1);
    // // printf("[!] fput_encrypt: wrote ciphertext: %s\n", s);
    // printf_custom("[!] fput_encrypt: wrote ciphertext", s, ciphertext_len);

    // create HMAC
    int checksum_len;
    char* checksum = fsign(s, ciphertext_len, &checksum_len);
    char* checksum_file_name = concatenate_strings(filepath, ".sign");
    if (!file_exists(checksum_file_name))
    {
        // TODO what permissions to give to the checksum file
        create_file(checksum_file_name, getuid(), getgid(), 0600);
    }
    if (!validate(caller_name, checksum_file_name, 110))
    {
        printf("You are missing one of read or write permissions, hence cannot create checksum file.\n");
        exit(1);
    }
    write_to_file_with_len(checksum_file_name, checksum, checksum_len, 1);
    // printf("[!] fput_encrypt.c: checksum length written: %d\n", checksum_len);
    // printf("[!] fput_encrypt.c: checksum written: %s\n", checksum);

    seteuid(getuid());
    printf("UID: %d EUID: %d\n", getuid(), geteuid());

    return 0;
}