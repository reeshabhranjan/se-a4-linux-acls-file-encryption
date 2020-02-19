#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include "acl.h"
#include<grp.h>
#include<pwd.h>
#include<sys/stat.h>
#include<string.h>

int username_exists(char* username)
{
    struct passwd* pwd = (struct passwd*) malloc(sizeof(struct passwd));
    *pwd = *(getpwnam(username));
    return pwd != NULL;
}

int groupname_exists(char* groupname)
{
    struct group* grp = (struct group*) malloc(sizeof(struct group));
    *grp = *(getgrnam(groupname));
    return grp != NULL;
}

int is_file(char* filename)
{
    struct stat st;
    stat(filename, &st);
    return S_ISREG(st.st_mode);
}

int is_directory(char* directory_name)
{
    struct stat st;
    stat(directory_name, &st);
    return S_ISDIR(st.st_mode);
}

int is_valid_permission(int permission)
{
    char* permission_string = int_to_string(permission);
    if (strlen(permission_string) > 3)
    {
        return 0;
    }
    
    int read_permission = permission / 100;
    permission -= 100 * read_permission;
    int write_permission = permission / 10;
    permission -= 10 * write_permission;
    int execute_permission = permission;

    int corrupted_read = read_permission != 1 && read_permission != 0;
    int corrupted_write = write_permission != 1 && write_permission != 0;
    int corrupted_execute = execute_permission != 1 && execute_permission != 0;

    if (corrupted_read || corrupted_write || corrupted_execute)
    {
        return 0;
    }

    return 1;
}

// file_exits already defined in acl.h