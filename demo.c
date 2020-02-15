#include<stdio.h>
#include "acl.h"
#include<errno.h>
#include<string.h>

int main()
{
    // struct acl_data a1;
    // a1.owner = "reeshabh";
    // a1.group = "reeshabh";
    // a1.user_perm = 111;
    // a1.group_perm = 111;
    // a1.oth_perm = 111;
    // a1.mask = 777;
    // setacl(&a1, "debug/sample.txt");
    struct acl_data* a2 = getacl("debug/sample.txt");

    printf("%s\n", a2 -> owner);
    printf("%s\n", a2 -> group);
    printf("%d\n", a2 -> user_perm);
    printf("%d\n", a2 -> group_perm);
    printf("%d\n", a2 -> oth_perm);
    printf("%d\n", a2 -> mask);

    return 0;
}