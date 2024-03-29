#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <pwd.h>
#include "acl.h"
#include "security.h"
#include <string.h>
#include "utils.h"
#include "encrypt.h"

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

    // TODO what if the file doesn't exist? (Assumption)
    // TODO sticky bit needed in any file to allow delete?
    int verification_result = fverify(filepath);

    if (!verification_result)
    {
        // TODO replace various perrors with printfs.
        printf("Checksum verification failed. Exiting.\n");
        exit(1);
    }

    int buf_len;
    char* buf = read_from_file_with_num_bytes(filepath, &buf_len);
    for (int i = 0; i < buf_len; i++)
    {
        printf("%c", buf[i]);
    }
    
    // printf("%s", buf);

    seteuid(getuid());
    printf("UID: %d EUID: %d\n", getuid(), geteuid());
    
    return 0;
}