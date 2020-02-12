#include<stdio.h>
#include "acl.h"
#include<errno.h>
#include<string.h>

int main()
{
    struct acl_data a1;
    a1.owner = "reeshabh";
    a1.group = "reeshabh";
    a1.user_perm = 111;
    a1.group_perm = 111;
    a1.oth_perm = 111;
    a1.mask = 777;
    // setacl(&a1, "debug/sample.txt");
    getacl("debug/sample.txt");
    return 0;
}