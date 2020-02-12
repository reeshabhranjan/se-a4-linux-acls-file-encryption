#ifndef ACL_H
#define ACL_H

struct acl_data
{
    char* owner;
    char* group;
    int user_perm;
    int group_perm;
    int oth_perm;
    struct named_user* named_users;
    struct named_group* named_groups;
    int mask;
};

struct named_user
{
    char* username;
    int permissions;
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
void getacl(char* filepath);
int file_exists();

#endif