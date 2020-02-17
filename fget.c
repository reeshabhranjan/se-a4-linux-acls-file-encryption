#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>

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

    char buf[10000];

    int fd = open(filepath, O_RDONLY);
    read(fd, buf, 10000);

    printf("%s", buf);
    
    return 0;
}