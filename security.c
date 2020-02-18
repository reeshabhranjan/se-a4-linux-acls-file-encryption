#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include "acl.h"
#include<grp.h>
#include<pwd.h>

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

// file_exits already defined in acl.h