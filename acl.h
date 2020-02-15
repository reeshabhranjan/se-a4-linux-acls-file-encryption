#ifndef ACL_H
#define ACL_H

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
    struct named_entity* named_users;
    struct named_entity* named_groups;
    int mask;
};


struct named_group
{
    char* groupname;
    int permissions;
};

struct pair
{
    char *first;
    char *second;
};

void setacl(struct acl_data* data, char* filepath);
struct acl_data* getacl(char* filepath);
int file_exists();
int add_permission(char* filepath, char* permission, int type);

#endif