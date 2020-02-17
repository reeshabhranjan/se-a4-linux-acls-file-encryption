#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>

int main(int argc, char* argv[])
{
    if (argc <= 1)
    {
        printf("Enter a directory name.\n");
        exit(1);
    }

    if (argc > 2)
    {
        perror("Enter only a directory name with no spaces (or escaped).\n");
        exit(1);
    }

    char* dir_name = argv[1];

    // TODO set permissions inherited from the parent directory
    int default_permissions = 777;
    int status = mkdir(dir_name, default_permissions);
    if (status == -1)
    {
        perror("Cannot create directory.");
        exit(1);
    }
}