#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<pwd.h>
#include "acl.h"
#include "security.h"

int main(int argc, char* argv[])
{
    if (argc <= 1)
    {
        perror("Please enter the full name of the executable.");
        exit(1);
    }

    int saved_euid = geteuid();
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

    struct stat st;
    stat(filepath, &st);

    struct acl_data* acl = getacl(filepath);
    char* owner_name = acl -> owner;
    struct passwd* pwd = (struct passwd*) malloc(sizeof(struct passwd));
    *pwd = *(getpwnam(owner_name));

    int owner_uid = pwd -> pw_uid;

    seteuid(owner_uid);
    printf("Changed euid to %d\n", owner_uid);

    printf("UID: %d EUID: %d\n", getuid(), geteuid());

    if (!validate(owner_name, filepath, 1))
    {
        perror("You do not have sufficient permissions (as owner)!");
        seteuid(getuid());
        exit(1);
    }


    if (fork() == 0)
    {
        execv(filepath, argv + 1);
    }

    else
    {
        wait(NULL);
        seteuid(getuid());
        printf("Restored euid to %d\n", getuid());
    }

    printf("UID: %d EUID: %d\n", getuid(), geteuid());

    return 0;
}