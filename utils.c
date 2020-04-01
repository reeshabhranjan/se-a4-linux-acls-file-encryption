#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

void write_to_file(char* filename, char* buffer)
{

}

void create_file(char* filepath, int owner_id, int group_id, int permissions)
{
    int fd = open(filepath, O_CREAT, permissions);
    chown(filepath, owner_id, group_id);
    close(fd);
}