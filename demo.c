#include<stdio.h>
#include "acl.h"
#include<errno.h>
#include<string.h>
#include<stdlib.h>

#define DEBUG_FILE_NAME "debug_files/sample.txt"

void print_acl_details(struct acl_data* acl)
{
    printf("owner: %s\n", acl -> owner);
    printf("group: %s\n", acl -> group);
    printf("owner_perm: %d\n", acl -> user_perm);
    printf("group_perm: %d\n", acl -> group_perm);
    printf("other_perm: %d\n", acl -> oth_perm);
    struct named_entity** named_users = acl -> named_users;
    struct named_entity** named_groups = acl -> named_groups;
    printf("named_users:\n");
    for (int i = 0; i < acl -> num_named_users; i++)
    {
        printf("username: %s permission: %d\n", (*(named_users + i)) -> name, (*(named_users + i)) -> permissions);
    }
    printf("\n");
    printf("named_groups:\n");
    for (int i = 0; i < acl -> num_named_users; i++)
    {
        printf("groupname: %s permission: %d\n", (*(named_groups + i)) -> name, (*(named_groups + i)) -> permissions);
    }
    printf("mask: %d\n", acl -> mask);
}

int main()
{
    struct acl_data* a1 = (struct acl_data*) malloc(sizeof(struct acl_data));
    a1 -> owner = "reeshabh";
    a1 -> group = "reeshabh";
    a1 -> user_perm = 111;
    a1 -> group_perm = 222;
    a1 -> oth_perm = 333;
    a1 -> mask = 444;

    struct named_entity* u1 = (struct named_entity*) malloc(sizeof(struct named_entity));
    u1 -> name = "face";
    u1 -> permissions = 555;

    struct named_entity* u2 = (struct named_entity*) malloc(sizeof(struct named_entity));
    u2 -> name = "eye";
    u2 -> permissions = 666;

    struct named_entity* g1 = (struct named_entity*) malloc(sizeof(struct named_entity));
    g1 -> name = "krypton";
    g1 -> permissions = 777;

    struct named_entity* g2 = (struct named_entity*) malloc(sizeof(struct named_entity));
    g2 -> name = "earth";
    g2 -> permissions = 888;

    struct named_entity** named_users = (struct named_entity**) calloc(2, sizeof(struct named_entity*));
    struct named_entity** named_groups = (struct named_entity**) calloc(2, sizeof(struct named_entity*));
    

    *named_users = u1;
    *(named_users + 1) = u2;

    *named_groups = g1;
    *(named_groups + 1) = g2;

    a1 -> named_users = named_users;
    a1 -> named_groups = named_groups;
    a1 -> num_named_users = 2;
    a1 -> num_named_groups = 2;

    // TODO handle file not exists (causing segfault)
    // TODO handle pointers in setacl, getacl (make it uniform)
    // TODO free dynamically allocated memory
    // TODO error/exception handling (like username doesn't exist)
    setacl(a1, DEBUG_FILE_NAME);
    struct acl_data* a2 = getacl(DEBUG_FILE_NAME);

    
    // testing validate
    char* username1 = "face";
    int permissions1 = 222;
    printf("validating %s for %d permissions: %d\n\n", username1, permissions1, validate(username1, DEBUG_FILE_NAME, permissions1));
    
    char* username2 = "eye";
    int permissions2 = 333;
    printf("validating %s for %d permissions: %d\n\n", username2, permissions2, validate(username2, DEBUG_FILE_NAME, permissions2));
    
    char* username3 = "reeshabh";
    int permissions3 = 111;
    printf("validating %s for %d permissions: %d\n\n", username3, permissions3, validate(username3, DEBUG_FILE_NAME, permissions3));

    print_acl_details(a2);
    printf("\n");
    // testing set_permission
    int new_permissions = 567;
    printf("Setting permissions of %s to %d\n", username3, new_permissions);
    set_permission(DEBUG_FILE_NAME, USER_TYPE, username3, new_permissions);

    struct acl_data* new_acl = getacl(DEBUG_FILE_NAME);

    print_acl_details(new_acl);

    

    return 0;
}