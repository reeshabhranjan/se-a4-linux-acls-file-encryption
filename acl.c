#include "acl.h"
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/xattr.h>

void setacl(struct acl_data* data, char* filepath)
{
    setxattr(filepath, "user.testname", "testvalue", sizeof("testvalue"), XATTR_CREATE);
}

void getacl(char* filepath)
{
    int n = getxattr(filepath, "user.testname", NULL, 0);
    printf("%d\n", n);
    char buf[n];
    getxattr(filepath, "user.testname", buf, sizeof(buf));
    printf("value extracted: %s\n", buf);
}

int file_exists(char* filepath)
{
    int _file_exists = access(filepath, F_OK) + 1;
    return _file_exists;
}
