#include "acl.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/xattr.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>

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
const int USER_TYPE = 6;

// code constants
#define MAX_GROUP_COUNT 65536
#define DEBUG_MODE 1

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
    char* user_key = (char*) malloc(strlen(key) + 5 + 1); // + 5 for "user." and + 1 for null-termination
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
            printf("This file does not contain acl structure, creating one now.\n");
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
    struct passwd* pwd = (struct passwd*) malloc(sizeof(struct passwd));
    struct group* grp = (struct group*) malloc(sizeof(struct group));
    *pwd = *(getpwuid(owner_uid));
    *grp = *(getgrgid(group_gid));
    char* owner_name = (char*) malloc(strlen(pwd -> pw_name) + 1);
    strcpy(owner_name, pwd -> pw_name);
    char* group_name = (char*) malloc(strlen(grp -> gr_name) + 1);
    strcpy(group_name, grp -> gr_name);
    acl -> owner = owner_name;
    acl -> group = group_name;

    int owner_read_perm = (st.st_mode & S_IRUSR) != 0;
    int owner_write_perm = (st.st_mode & S_IWUSR) != 0;
    int owner_exec_perm = (st.st_mode & S_IXUSR) != 0;
    
    int group_read_perm = (st.st_mode & S_IRGRP) != 0;
    int group_write_perm = (st.st_mode & S_IWGRP) != 0;
    int group_exec_perm = (st.st_mode & S_IXGRP) != 0;

    int other_read_perm = (st.st_mode & S_IROTH) != 0;
    int other_write_perm = (st.st_mode & S_IWOTH) != 0;
    int other_exec_perm = (st.st_mode & S_IXOTH) != 0;

    int owner_perm = 100 * owner_read_perm + 10 * owner_write_perm + owner_exec_perm;
    int group_perm = 100 * group_read_perm + 10 * group_write_perm + group_exec_perm;
    int other_perm = 100 * other_read_perm + 10 * other_write_perm + other_exec_perm;

    acl -> user_perm = owner_perm;
    acl -> group_perm = group_perm;
    acl -> oth_perm = other_perm;
    acl -> mask = 111;
    acl -> num_named_groups = 0;
    acl -> num_named_users = 0;

    setacl(acl, filepath);

    return acl;
}

struct acl_data* getacl(char* filepath)
{

    if (!acl_exists(filepath))
    {
        return create_basic_acl(filepath);
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
    acl -> oth_perm = se_acl_other_perm;
    acl -> mask = se_acl_mask;
    acl -> num_named_users = num_named_users;
    acl -> num_named_groups = num_named_groups;
    acl -> named_users = named_users;
    acl -> named_groups = named_groups;

    return acl;
}

// interface functions

void update_named_entity_permission(int perm_type, char* filename, struct acl_data* acl, char* entity_name, int permission)
{
    struct named_entity** named_permissions;
    int num_named;
    if (perm_type == USER_TYPE)
    {
        named_permissions = acl -> named_users;
        num_named = acl -> num_named_users;
    }
    else if (perm_type == GROUP_TYPE)
    {
        named_permissions = acl -> named_groups;
        num_named = acl -> num_named_groups;
    }

    struct named_entity** modified_named_permissions;

    int exists = 0;

    for (int i = 0; i < num_named; i++)
    {
        if (strcmp((*(named_permissions + i)) -> name, entity_name) == 0)
        {
            (*(named_permissions + i)) -> permissions = permission;
            modified_named_permissions = named_permissions;
            exists = 1;
            break;
        }
    }
    
    if (!exists)
    {
        // create a new entry
        struct named_entity** new_named_permissions = (struct named_entity**) calloc(num_named + 1, sizeof(struct named_entity*));

        for (int i = 0; i < num_named; i++)
        {
            *(new_named_permissions + i) = *(named_permissions + i);
        }

        struct named_entity* new_named_entity = (struct named_entity*) malloc(sizeof(struct named_entity));
        new_named_entity -> name = entity_name;
        new_named_entity -> permissions = permission;

        *(new_named_permissions + num_named) = new_named_entity;
        modified_named_permissions = new_named_permissions;

        if (perm_type == USER_TYPE)
        {
            acl -> num_named_users = num_named + 1;
        }

        else if (perm_type == GROUP_TYPE)
        {
            acl -> num_named_groups = num_named + 1;
        }
        
        
    }

    if (perm_type == USER_TYPE)
    {
        acl -> named_users = modified_named_permissions;
    }

    else if (perm_type == GROUP_TYPE)
    {
        acl -> named_groups = modified_named_permissions;
    }

    setacl(acl, filename);
    
}

void set_permission(char* filename, int perm_type, char* entity_name, int permission)
{
    struct acl_data* acl = getacl(filename);

    // when username is specified
    if (perm_type == USER_TYPE)
    {
        if (strcmp(entity_name, acl -> owner) == 0)
        {
            if (DEBUG_MODE)
            {
                printf("User is of category: OWNER\n");
            }
            acl -> user_perm = permission;
            setacl(acl, filename);
        }
        else
        {
            // TODO define this function
            if (DEBUG_MODE)
            {
                printf("User is of category: NAMED_USER\n");
            }
            update_named_entity_permission(perm_type, filename, acl, entity_name, permission);
        }
        
    }

    // when username is not specified
    else if (perm_type == OTHER_TYPE)
    {
        if (DEBUG_MODE)
        {
            printf("User is of category: OTHER\n");
        }
        acl -> oth_perm = permission;
        setacl(acl, filename);
    }

    else if (perm_type == GROUP_TYPE)
    {
        
        if (strcmp(entity_name, acl -> group) == 0)
        {
            if (DEBUG_MODE)
            {
                printf("User is of category: GROUP_MEMBER\n");
            }
            acl -> group_perm = permission;
            setacl(acl, filename);
        }

        else
        {
            // TODO define this function
            if (DEBUG_MODE)
            {
                printf("User is of category: NAMED_GROUP_MEMBER\n");
            }
            update_named_entity_permission(perm_type, filename, acl, entity_name, permission);
        }
    
    }

    else if (perm_type == MASK_TYPE)
    {
        acl -> mask = permission;
        setacl(acl, filename);
    }
    
    else
    {
        perror("Incorrect perm_type provided.");
        exit(1);
    }
    
}

int check_permissions(int file_permission, int requested_permission)
{
    int file_permission_read = file_permission / 100;
    file_permission -= file_permission_read * 100;
    int file_permission_write = file_permission / 10;
    file_permission -= file_permission_write * 10;
    int file_permission_execute = file_permission;
    
    int requested_permission_read = requested_permission / 100;
    requested_permission -= requested_permission_read * 100;
    int requested_permission_write = requested_permission / 10;
    requested_permission -= requested_permission_write * 10;
    int requested_permission_execute = requested_permission;

    int read_violation = requested_permission_read > file_permission_read;
    int write_violation = requested_permission_write > file_permission_write;
    int execute_violation = requested_permission_execute > file_permission_execute;

    if (read_violation || write_violation || execute_violation)
    {
        return 0;
    }

    return 1;
    
}

int* get_groups(char* username, int* group_count_address)
{
    int user_gid = getpwnam(username) -> pw_gid;
    int group_count = MAX_GROUP_COUNT;
    gid_t groups[group_count];
    getgrouplist(username, user_gid, groups, &group_count);
    gid_t* reduced_groups = (gid_t*) calloc(group_count, sizeof(gid_t));

    for (int i = 0; i < group_count; i++)
    {
        reduced_groups[i] = groups[i];
    }

    *group_count_address = group_count;
    
    return reduced_groups;
}

int user_is_in_file_group(char* username, char* filename)
{
    char* file_group_name = getacl(filename) -> group;
    struct group* grp = (struct group*) malloc(sizeof(struct group));
    *grp = *(getgrnam(file_group_name));
    int file_group_gid = grp -> gr_gid;

    int group_count;
    gid_t* groups = get_groups(username, &group_count);

    int file_group_member = 0;

    for (int i = 0; i < group_count; i++)
    {
        if (file_group_gid == groups[i])
        {
            file_group_member = 1;
            break;
        }
    }
    
    return file_group_member;
}

struct named_entity* get_named_entity(struct named_entity** named_entities, int num_named, char* name)
{
    struct named_entity* _named_entity = NULL;

    for (int i = 0; i < num_named; i++)
    {
        if (strcmp((*(named_entities + i)) -> name, name) == 0)
        {
            _named_entity = *(named_entities + i);
            break;
        }
    }
    
    return _named_entity;
}

struct named_entity* get_named_group(struct named_entity** named_groups, int num_groups, char* username)
{
    struct named_entity* named_group = NULL;
    int group_count;
    gid_t* groups = get_groups(username, &group_count);

    for (int i = 0; i < group_count; i++)
    {
        for (int j = 0; j < num_groups; j++)
        {
            int user_group_gid = groups[i];
            int file_group_gid = getgrnam((*(named_groups + j)) -> name) -> gr_gid;

            if (user_group_gid == file_group_gid)
            {
                named_group = *(named_groups + j);
                break;
            }
        }
    }
    
    return named_group;
}

int mask_permission(int permission, int mask)
{
    int read_permission = permission / 100;
    permission -= read_permission * 100;
    int write_permission = permission / 10;
    permission -= write_permission * 10;
    int execute_permission = permission;

    int mask_read = mask / 100;
    mask -= mask_read * 100;
    int mask_write = mask / 10;
    mask -= mask_write * 10;
    int mask_execute = mask;

    int masked_permission = 100 * (read_permission * mask_read) + 10 * (write_permission * mask_write) + (execute_permission * mask_execute);
    return masked_permission;
}

int validate(char* username, char* filename, int permissions)
{
    struct acl_data* acl = getacl(filename);

    // if owner
    if (strcmp(username, acl -> owner) == 0)
    {
        if (DEBUG_MODE)
        {
            printf("User lies in the category: OWNER\n");
        }
        return check_permissions(acl -> user_perm, permissions);
    }

    // if group
    if (user_is_in_file_group(username, filename))
    {
        if (DEBUG_MODE)
        {
            printf("User lies in the category: GROUP_MEMBER\n");
        }

        return check_permissions(mask_permission(acl -> group_perm, acl -> mask), permissions);
    }

    // if named_user
    struct named_entity* named_user = get_named_entity(acl -> named_users, acl -> num_named_users, username);

    if (named_user) // equivalent to is named_user is not NULL?
    {
        if (DEBUG_MODE)
        {
            printf("User lies in the category: NAMED_USER\n");
        }
        return check_permissions(mask_permission(named_user -> permissions, acl -> mask), permissions);
    }

    // if named_group
    struct named_entity* named_group = get_named_group(acl -> named_groups, acl -> num_named_groups, username);

    if (named_group) // equivalent to is named_user not NULL?
    {
        if (DEBUG_MODE)
        {
            printf("User lies in the category: NAMED_GROUP_MEMBER\n");
        }
        return check_permissions(mask_permission(named_group -> permissions, acl -> mask), permissions);
    }

    // if other
    if (DEBUG_MODE)
    {
        printf("User lies in the category: OTHER\n");
    }
    return check_permissions(mask_permission(acl -> oth_perm, acl -> mask), permissions);
}

void change_owner(char* filename, int perm_type, char* entity_name)
{
    struct acl_data* acl = getacl(filename);

    if (perm_type == USER_TYPE)
    {
        acl -> owner = entity_name;
    }
    else if (perm_type == GROUP_TYPE)
    {
        acl -> group = entity_name;
    }

    setacl(acl, filename);
}