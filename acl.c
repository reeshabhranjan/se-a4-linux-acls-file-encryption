#include "acl.h"
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/xattr.h>

void setacl(struct acl_data* data, char* filepath)
{
    setxattr(filepath, "testname", "testvalue", sizeof("testvalue"), XATTR_CREATE);
}

void getacl(char* filepath)
{

}

int file_exists(char* filepath)
{
    int _file_exists = access(filepath, F_OK) + 1;
    return _file_exists;
}