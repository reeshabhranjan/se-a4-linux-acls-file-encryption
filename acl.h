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

// TODO convert to enum
extern const int OWNER_TYPE;
extern const int GROUP_TYPE;
extern const int OTHER_TYPE;
extern const int NAMED_USER_TYPE;
extern const int NAMED_GROUP_TYPE;
extern const int MASK_TYPE;
extern const int USER_TYPE;

// function declarations

// essential functions
void setacl(struct acl_data* data, char* filepath);
struct acl_data* getacl(char* filepath);
int file_exists();
int add_permission(char* filepath, char* permission, int type);
char* named_entity_list_to_string(struct named_entity** named_entities, int num_named_entities);
struct named_entity** string_to_named_entity_list(char* string, int num_entities);

// interface functions
void set_permission(char* filename, int perm_type, char* entity_name, int permission);
int validate(char* username, char* filename, int permissions);
int strfind(char* hay, char needle);
char* substring(char* string, int start, int end);
int file_exists(char* filename);
char* int_to_string(int x);

#endif