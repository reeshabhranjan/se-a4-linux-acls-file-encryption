#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include "acl.h"
#include <string.h>
#include "security.h"

void print_acl_details(struct acl_data* acl)
{
    printf("owner: %15s\n", acl -> owner);
    printf("group: %15s\n", acl -> group);
    printf("owner_perm: %3d\n", acl -> user_perm);
    printf("group_perm: %3d\n", acl -> group_perm);
    printf("other_perm: %3d\n", acl -> oth_perm);
    struct named_entity** named_users = acl -> named_users;
    struct named_entity** named_groups = acl -> named_groups;
    printf("named_users: (total: %3d)\n", acl -> num_named_users);

    for (int i = 0; i < acl -> num_named_users; i++)
    {
        printf("username: %15s permission: %3d\n", (*(named_users + i)) -> name, (*(named_users + i)) -> permissions);
    }
    printf("\n");

    printf("named_groups: (total: %3d)\n", acl -> num_named_groups);
    for (int i = 0; i < acl -> num_named_groups; i++)
    {
        printf("groupname: %15s permission: %3d\n", (*(named_groups + i)) -> name, (*(named_groups + i)) -> permissions);
    }
    printf("\n");

    printf("mask: %3d\n", acl -> mask);
}

int main(int argc, char* argv[])
{
    if (argc <= 1)
    {
        perror("Enter a file name.\n");
        exit(1);
    }

    if (argc > 2)
    {
        perror("Please enter only a file name with no spaces (or escaped).");
        exit(1);
    }

    printf("UID: %d EUID: %d\n", getuid(), geteuid());

    char* filename = argv[1];

    if (!file_exists(filename))
    {
        perror("The file does not exist.");
        exit(1);
    }    

    struct acl_data* acl = getacl(filename);

    print_acl_details(acl);

    seteuid(getuid());
    printf("UID: %d EUID: %d\n", getuid(), geteuid());
    
    return 0;
}