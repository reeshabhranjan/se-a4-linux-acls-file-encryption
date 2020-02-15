#ifndef ACL_H
#define ACL_H

// structures

struct pair
{
    char *first;
    char *second;
};

struct named_entity
{
    char* name;
    int permissions;
};

struct acl_data
{
    char* owner;
    char* group;
    int user_perm;
    int group_perm;
    int oth_perm;
    struct named_entity** named_users;
    struct named_entity** named_groups;
    int num_named_users;
    int num_named_groups;
    int mask;
};

// function declarations

void setacl(struct acl_data* data, char* filepath);
struct acl_data* getacl(char* filepath);
int file_exists();
int add_permission(char* filepath, char* permission, int type);
char* named_entity_list_to_string(struct named_entity** named_entities, int num_named_entities);
#endif