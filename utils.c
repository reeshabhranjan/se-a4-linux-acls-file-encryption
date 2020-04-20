#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

char* concatenate_strings(char* s1, char* s2)
{
    int n1 = strlen(s1);
    int n2 = strlen(s2);
    char* s = malloc(n1 + n2 + 1);
    for (int i = 0; i < n1; i++)
    {
        s[i] = s1[i];
    }

    for (int i = n1; i < n1 + n2; i++)
    {
        s[i - n1] = s2[i];
    }
    
    s[n1 + n2] = '\0';
    return s;
}

void write_to_file(char* filepath, char* buffer)
{
    int fd = open(filepath, O_WRONLY | O_APPEND | O_CREAT);
    write(fd, buffer, strlen(buffer));
    close(fd);
}

void create_file(char* filepath, int owner_id, int group_id, int permissions)
{
    int fd = open(filepath, O_CREAT, permissions);
    chown(filepath, owner_id, group_id);
    close(fd);
}

char* read_from_file(char* filepath)
{
    char* buf = (char*) malloc(100000);

    int fd = open(filepath, O_RDONLY);
    read(fd, buf, 100000);
    close(fd);
    return buf;
}