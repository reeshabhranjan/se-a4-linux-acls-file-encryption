#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include "acl.h"
#include<grp.h>
#include<pwd.h>
#include<sys/stat.h>

int username_exists(char* username)
{
    struct passwd* pwd = getpwnam(username);
    return pwd != NULL;
}

int groupname_exists(char* groupname)
{
    struct group* grp = getgrnam(groupname);
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

// file_exits already defined in acl.h