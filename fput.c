#include<stdio.h>
#include<stdlib.h>

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
    char s[100];
    fgets(s, 100, stdin);
    FILE* file = fopen(filepath, "a+");
    fputs(s, file);
    fclose(file);
}