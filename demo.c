#include<stdio.h>
#include "acl.h"
#include<errno.h>
#include<string.h>
#include<stdlib.h>

int main()
{
    struct acl_data* a1 = (struct acl_data*) malloc(sizeof(struct acl_data));
    a1 -> owner = "reeshabhkumarranjan";
    a1 -> group = "reeshabh's group";
    a1 -> user_perm = 111;
    a1 -> group_perm = 222;
    a1 -> oth_perm = 333;
    a1 -> mask = 444;

    struct named_entity* u1 = (struct named_entity*) malloc(sizeof(struct named_entity));
    u1 -> name = "rohan";
    u1 -> permissions = 555;

    struct named_entity* u2 = (struct named_entity*) malloc(sizeof(struct named_entity));
    u2 -> name = "rajesh";
    u2 -> permissions = 666;

    struct named_entity* g1 = (struct named_entity*) malloc(sizeof(struct named_entity));
    g1 -> name = "rohan's group";
    g1 -> permissions = 777;

    struct named_entity* g2 = (struct named_entity*) malloc(sizeof(struct named_entity));
    g2 -> name = "rajesh's group";
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
    setacl(a1, "debug_files/sample.txt");
    printf("hello\n");
    struct acl_data* a2 = getacl("debug_files/sample.txt");
    return 0;
}