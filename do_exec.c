#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/wait.h>

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