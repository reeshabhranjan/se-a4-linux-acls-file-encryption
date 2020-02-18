#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<pwd.h>
#include "acl.h"
#include "security.h"

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
    struct passwd* pwd = getpwuid(caller_uid);
    char* caller_name = pwd -> pw_name;

    if (!validate(caller_name, filepath, 100))
    {
        perror("You do not have sufficient permissions!\n");
        exit(1);
    }

    char buf[10000];

    int fd = open(filepath, O_RDONLY);
    read(fd, buf, 10000);

    printf("%s", buf);
    
    return 0;
}