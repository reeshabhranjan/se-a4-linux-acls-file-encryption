#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<error.h>
#include<errno.h>
#include<string.h>

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

    printf("Enter a string (under 100 characters)\n");
    char s[100000];
    fgets(s, 100000, stdin);

    int len_s = strlen(s);

    int fd = open(filepath, O_WRONLY | O_APPEND | O_CREAT);
    write(fd, s, len_s);
    close(fd);
    return 0;
}