#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <pwd.h>
#include "acl.h"
#include "security.h"
#include <string.h>
#include "encrypt.h"
#include "utils.h"

int main(int argc, char* argv[])
{
    if (argc <= 1)
    {
        perror("Enter a file to read from.\n");
        exit(1);
    }

    if (argc > 2)
    {
        perror("Please enter only a file name with no spaces (or escaped)\n");
        exit(1);
    }

    char* filepath = argv[1];

    if (!file_exists(filepath))
    {
        perror("The file does not exist.");
        exit(1);
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

    if (!validate(caller_name, filepath, 100))
    {
        perror("You do not have sufficient permissions!\n");
        exit(1);
    }

    printf("UID: %d EUID: %d\n", getuid(), geteuid());

    int verification_result = verify_hmac_trapdoor(filepath);

    if (!verification_result)
    {
        printf("Checksum verification failed. Exiting.\n");
        exit(1);
    }

    char* buf = (char*) malloc(100000);
    memset(buf, 0, 100000);

    int len_buf;
    buf = read_from_file_with_num_bytes(filepath, &len_buf);

    printf("Attempting decryption...\n"); // TODO decrypt only if the true owner
    char* plaintext = decrypt_string_trapdoor(buf, len_buf);
    free(buf);
    buf = NULL;
    buf = plaintext;    

    printf("%s", buf);

    seteuid(getuid());
    printf("UID: %d EUID: %d\n", getuid(), geteuid());
    
    return 0;
}