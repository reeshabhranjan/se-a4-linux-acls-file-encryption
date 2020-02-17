#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/wait.h>
#include<pwd.h>
#include "acl.h"

int main(int argc, char* argv[])
{
    if (argc <= 1)
    {
        perror("Please enter the full name of the executable.");
        exit(1);
    }

    int saved_euid = geteuid();
    char* filepath = argv[1];
    struct stat st;
    stat(filepath, &st);

    int owner_uid = st.st_uid;

    seteuid(owner_uid);
    printf("Changed euid to %d\n", owner_uid);
    struct passwd* pwd = getpwuid(owner_uid);
    char* owner_name = pwd -> pw_name;

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

    return 0;
}