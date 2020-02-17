#include "acl.h"
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/xattr.h>
#include<errno.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<pwd.h>
#include<grp.h>

// delimiters
#define MEMBER_DELIMITER '|'
#define STRUCT_DELIMITER ';'

// keys
char* ACL_EXISTS_KEY = "se_acl_exists";
char* OWNER_KEY = "se_acl_owner";
char* GROUP_KEY = "se_acl_group";
char* OWNER_PERM_KEY = "se_acl_owner_perm";
char* GROUP_PERM_KEY = "se_acl_group_perm";
char* OTHER_PERM_KEY = "se_acl_other_perm";
char* NAMED_USER_PERMS_KEY = "se_acl_named_user";
char* NAMED_GROUPS_PERMS_KEY = "se_acl_named_group";
char* NUM_NAMED_USERS_KEY = "se_acl_num_named_users";
char* NUM_NAMED_GROUPS_KEY = "se_acl_num_named_groups";
char* MASK_KEY = "se_acl_mask";

// types
const int OWNER_TYPE = 0;
const int GROUP_TYPE = 1;
const int OTHER_TYPE = 2;
const int NAMED_USER_TYPE = 3;
const int NAMED_GROUP_TYPE = 4;
const int MASK_TYPE = 5;

int num_digits(int x)
{
    int n = 0;
    while (x > 0)
    {
        x /= 10;
        n++;
    }
    return n;
}

int strfind(char* hay, char needle)
{
    int pos = -1;
    int n = strlen(hay);

    for (int i = 0; i < n; i++)
    {
        if (hay[i] == needle)
        {
            pos = i;
            break;
        }
    }

    return pos;
}

char* substring(char* string, int start, int end)
{
    int n = end - start + 1;
    char* _substring = (char*) malloc(n + 1); // + 1 for null termination

    for (int i = start; i < end; i++)
    {
        _substring[i - start] = string[i];
    }

    _substring[n] = '\0';

    return _substring;
}

char* int_to_string(int x)
{
    int buf_size = num_digits(x) + 1;
    char* buf = (char*) malloc(sizeof(char) * buf_size);
    sprintf(buf, "%d", x);
    return buf;
}

int file_exists(char* filepath)
{
    int _file_exists = access(filepath, F_OK) + 1;
    return _file_exists;
}

char* setup_key(char* key)
{
    char* user_key = (char*) malloc(strlen(key) + 5);
    strcpy(user_key, "user.");
    strcat(user_key, key);
    return user_key;
}

int acl_exists(char* filepath)
{
    char* pre_check_result[2];
    char* user_key = setup_key(ACL_EXISTS_KEY);
    int pre_check = getxattr(filepath, user_key, pre_check_result, sizeof(pre_check_result));
    return pre_check + 1;
}


void write_pair_to_file(char* filename, char* key, char* value)
{
    // char user_key[strlen(key) + 5];
    // strcpy(user_key, "user.");
    // strcat(user_key, key);
    char* user_key = setup_key(key);
    int dummy = 0;
    if (setxattr(filename, user_key, value, strlen(value) + 1, XATTR_CREATE) == -1)
    {
        setxattr(filename, user_key, value, strlen(value) + 1, XATTR_REPLACE);
    }
}

char* read_value_from_file(char* filename, char* key)
{
    // char user_key[strlen(key) + 5];
    // strcpy(user_key, "user.");
    // strcat(user_key, key);
    char* user_key = setup_key(key);
    char buffer[1000];
    int status = getxattr(filename, user_key, buffer, 0);

    if (status == -1)
    {
        switch (errno)
        {
        case E2BIG:
            perror("Allowed size is too small for the attribute.\n");
            exit(1);
        
        case ENODATA:
            perror("The key does not exist.\n");
            exit(1);

        case ENOTSUP:
            perror("Extended attributes are not supported by your system.\n");
            exit(1);
        
        case ERANGE:
            perror("Buffer is too small for the attribute.\n");
            exit(1);
        
        default:
            break;
        }
    }

    int buf_size = status;
    char* value = (char*) malloc(sizeof(char) * buf_size);

    int dummy = sizeof(value);

    status = getxattr(filename, user_key, value, buf_size);
    
    if (status == -1)
    {
        switch (errno)
        {
        case E2BIG:
            perror("Allowed size is too small for the attribute.\n");
            exit(1);
        
        case ENODATA:
            perror("The key does not exist.\n");
            exit(1);

        case ENOTSUP:
            perror("Extended attributes are not supported by your system.\n");
            exit(1);
        
        case ERANGE:
            perror("Buffer is too small for the attribute.\n");
            exit(1);
        
        default:
            break;
        }
    }

    return value;
}

int array_length(struct named_entity** array)
{
    // int length = (int) (sizeof(array) / sizeof(array));

    int length = 0;

    while (array != NULL)
    {
        array++;
        length++;
    }

    return length;
}

char* named_entity_to_string(struct named_entity* named_permission)
{
    int req_len = strlen(named_permission -> name) + strlen(int_to_string(named_permission ->permissions)) + 1 + 1; // +1 for "|" and +1 for null termination 
    char* perm_str = (char*) malloc(req_len);
    
    if (perm_str == NULL)
    {
        perror("Internal failure: Cannot allocate memory for named user/group permissions.\n");
        exit(1);
    }

    snprintf(perm_str, req_len, "%s%c%d", named_permission -> name, MEMBER_DELIMITER, named_permission ->permissions);

    return perm_str;
}

char* named_entity_list_to_string(struct named_entity** named_entities, int num_named_entities)
{
    int combined_string_len = 0;

    // calculate the length of the entire string
    for (int i = 0; i < num_named_entities; i++)
    {
        char* converted_string = named_entity_to_string(*(named_entities + i));
        combined_string_len += strlen(converted_string) + 1; // + 1 for comma-separation
    }

    char* combined_string = (char*) malloc(combined_string_len + 1); // + 1 for null-termination

    int string_position = 0;
    for (int i = 0; i < num_named_entities; i++)
    {
        char* converted_string = named_entity_to_string(*(named_entities + i));

        for (int j = 0; j < (int) strlen(converted_string); j++)
        {
            combined_string[string_position] = converted_string[j];
            string_position++;
        }
        combined_string[string_position] = STRUCT_DELIMITER;
        string_position++;
    }

    combined_string[combined_string_len] = '\0';
    
    return combined_string;
}

struct named_entity* string_to_named_entity(char* string)
{
    int n = strlen(string);
    int pos_delimiter = strfind(string, MEMBER_DELIMITER);
    char* name = substring(string, 0, pos_delimiter);
    char* perm_string = substring(string, pos_delimiter + 1, n);
    int permission = atoi(perm_string);
    
    struct named_entity* named_permission = (struct named_entity*) malloc(sizeof(struct named_entity));
    named_permission -> name = name;
    named_permission -> permissions = permission;

    return named_permission;
}

struct named_entity** string_to_named_entity_list(char* string, int num_entities)
{
    int n = strlen(string);

    int starting_index = 0;

    struct named_entity** named_entity_list = (struct named_entity**) calloc(num_entities, sizeof(struct named_entity));

    for (int i = 0; i < num_entities; i++)
    {
        int ending_index = starting_index + strfind(string + starting_index, STRUCT_DELIMITER);
        char* entity_string = substring(string, starting_index, ending_index);
        struct named_entity* named_permissions = string_to_named_entity(entity_string);
        *(named_entity_list + i) = named_permissions;
        starting_index = ending_index + 1;
    }

    return named_entity_list;
}

void setacl(struct acl_data* data, char* filepath)
{
    if (!acl_exists(filepath))
    {
        switch (errno)
        {
        case E2BIG:
            perror("Pre_check_result buffer size is too small.\n");
            exit(1);
            break;
        
        case ENODATA:
            perror("This file does not contain acl structure, creating one now");
            write_pair_to_file(filepath, ACL_EXISTS_KEY, "Y");
            break;
        
        case ENOTSUP:
            perror("Extended attributes is not supported by your PC.\n");
            exit(1);

        default:
            break;
        }
    }

    write_pair_to_file(filepath, OWNER_KEY, data -> owner);
    write_pair_to_file(filepath, GROUP_KEY, data -> group);
    write_pair_to_file(filepath, OWNER_PERM_KEY, int_to_string(data -> user_perm));
    write_pair_to_file(filepath, GROUP_PERM_KEY, int_to_string(data -> group_perm));
    write_pair_to_file(filepath, OTHER_PERM_KEY, int_to_string(data -> oth_perm));
    write_pair_to_file(filepath, MASK_KEY, int_to_string(data -> mask));
    write_pair_to_file(filepath, NAMED_USER_PERMS_KEY, named_entity_list_to_string(data ->named_users, data -> num_named_users));
    write_pair_to_file(filepath, NAMED_GROUPS_PERMS_KEY, named_entity_list_to_string(data -> named_groups, data ->num_named_groups));
    write_pair_to_file(filepath, NUM_NAMED_USERS_KEY, int_to_string(data -> num_named_users));
    write_pair_to_file(filepath, NUM_NAMED_GROUPS_KEY, int_to_string(data -> num_named_groups));
}

struct acl_data* create_basic_acl(char* filepath)
{
    struct acl_data* acl = (struct acl_data*) malloc(sizeof(struct acl_data));
    struct stat st;
    stat(filepath, &st);
    int owner_uid = st.st_uid;
    int group_gid = st.st_gid;
    struct passwd* pwd;
    struct group* grp;
    pwd = getpwuid(owner_uid);
    grp = getgrgid(group_gid);
    char* owner_name = pwd ->pw_name;
    char* group_name = grp -> gr_name;
    acl -> owner = owner_name;
    acl -> group = group_name;

    int owner_read_perm = st.st_mode & S_IRUSR;
    int owner_write_perm = st.st_mode & S_IWUSR;
    int owner_exec_perm = st.st_mode & S_IXUSR;
    
    int group_read_perm = st.st_mode & S_IRGRP;
    int group_write_perm = st.st_mode & S_IWGRP;
    int group_exec_perm = st.st_mode & S_IXGRP;

    int other_read_perm = st.st_mode & S_IROTH;
    int other_write_perm = st.st_mode & S_IWOTH;
    int other_exec_perm = st.st_mode & S_IXOTH;

    int owner_perm = 100 * owner_read_perm + 10 * owner_write_perm + owner_exec_perm;
    int group_perm = 100 * group_read_perm + 10 * group_write_perm + group_exec_perm;
    int other_perm = 100 * other_read_perm + 10 * other_write_perm + other_exec_perm;

    acl -> user_perm = owner_perm;
    acl -> group_perm = group_perm;
    acl -> oth_perm = other_perm;
    acl -> mask = 777;
    acl -> num_named_groups = 0;
    acl -> num_named_users = 0;

    setacl(acl, filepath);

    return acl;
}

struct acl_data* getacl(char* filepath)
{

    if (!acl_exists(filepath))
    {
        create_basic_acl(filepath);
    }

    struct acl_data* acl = (struct acl_data*) malloc(sizeof(struct acl_data));
    
    char* owner = read_value_from_file(filepath, OWNER_KEY);
    char* group = read_value_from_file(filepath, GROUP_KEY);
    int se_acl_user_perm = atoi(read_value_from_file(filepath, OWNER_PERM_KEY));
    int se_acl_group_perm = atoi(read_value_from_file(filepath, GROUP_PERM_KEY));
    int se_acl_other_perm = atoi(read_value_from_file(filepath, OTHER_PERM_KEY));
    int se_acl_mask = atoi(read_value_from_file(filepath, MASK_KEY));
    int num_named_users = atoi(read_value_from_file(filepath, NUM_NAMED_USERS_KEY));
    int num_named_groups = atoi(read_value_from_file(filepath, NUM_NAMED_GROUPS_KEY));
    struct named_entity** named_users = string_to_named_entity_list(read_value_from_file(filepath, NAMED_USER_PERMS_KEY), num_named_users);
    struct named_entity** named_groups = string_to_named_entity_list(read_value_from_file(filepath, NAMED_GROUPS_PERMS_KEY), num_named_groups);

    acl -> owner = owner;
    acl -> group = group;
    acl -> user_perm = se_acl_user_perm;
    acl -> group_perm = se_acl_group_perm;
    acl -> oth_perm = se_acl_user_perm;
    acl -> mask = se_acl_mask;
    acl -> num_named_users = num_named_users;
    acl -> num_named_groups = num_named_groups;
    acl -> named_users = named_users;
    acl -> named_groups = named_groups;

    return acl;
}

// interface functions

void set_permission(char* filename, int perm_type, char* entity_name, int permission)
{
    struct acl_data* acl = getacl(filename);
}