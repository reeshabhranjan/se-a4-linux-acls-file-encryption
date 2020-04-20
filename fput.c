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
#include "utils.h"
#include "encrypt.h"

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
        printf("The file does not exist. Creating one...\n");
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
    char s[100000];
    fgets(s, 100000, stdin);

    write_to_file(filepath, s, 1);

    // create HMAC
    char* checksum = fsign(s);
    char* checksum_file_name = concatenate_strings(filepath, ".sign");
    if (!file_exists(checksum_file_name))
    {
        // TODO what permissions to give to the checksum file
        create_file(checksum_file_name, getuid(), getgid(), 0644);
    }
    write_to_file(checksum_file_name, checksum, 1);

    seteuid(getuid());
    printf("UID: %d EUID: %d\n", getuid(), geteuid());

    return 0;
}