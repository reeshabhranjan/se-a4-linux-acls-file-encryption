#include "acl.h"
#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/xattr.h>
#include<errno.h>
#include<stdlib.h>
#include<string.h>

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

int acl_exists(char* filepath)
{
    char* pre_check_result[2];
    int pre_check = getxattr(filepath, "user.se_acl_exists", pre_check_result, sizeof(pre_check_result));
    return pre_check + 1;
}


void write_pair_to_file(char* filename, char* key, char* value)
{
    char user_key[strlen(key) + 5];
    strcpy(user_key, "user.");
    strcat(user_key, key);
    int dummy = 0;
    if (setxattr(filename, user_key, value, sizeof(user_key), XATTR_CREATE) == -1)
    {
        setxattr(filename, user_key, value, sizeof(user_key), XATTR_REPLACE);
    }
}

char* read_value_from_file(char* filename, char* key)
{
    char user_key[strlen(key) + 5];
    strcpy(user_key, "user.");
    strcat(user_key, key);
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

    snprintf(perm_str, req_len, "%s|%d", named_permission -> name, named_permission ->permissions);

    return perm_str;
}

char* named_entity_list_to_string(struct named_entity** named_entities, int num_named_entities)
{
    int combined_string_len = 0;

    // calculate the length of the entire string
    for (int i = 0; i < num_named_entities; i++)
    {
        char* converted_string = named_entity_to_string(*named_entities + i);
        combined_string_len += strlen(converted_string) + 1; // + 1 for comma-separation
    }

    char* combined_string = (char*) malloc(combined_string_len + 1); // + 1 for null-termination

    int string_position = 0;
    for (int i = 0; i < num_named_entities; i++)
    {
        char* converted_string = named_entity_to_string(*named_entities + i);

        for (int j = 0; j < (int) strlen(converted_string); j++)
        {
            combined_string[string_position] = converted_string[j];
            string_position++;
            // printf("%d ", i + j);
            // printf("%c ", converted_string[j]);
        }
        combined_string[string_position] = ';';
        string_position++;
    }

    combined_string[combined_string_len] = '\0';
    
    return combined_string;
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
            write_pair_to_file(filepath, "se_acl_exists", "Y");
            break;
        
        case ENOTSUP:
            perror("Extended attributes is not supported by your PC.\n");
            exit(1);

        default:
            break;
        }
    }

    write_pair_to_file(filepath, "se_acl_owner", data -> owner);
    write_pair_to_file(filepath, "se_acl_group", data -> group);
    write_pair_to_file(filepath, "se_acl_user_perm", int_to_string(data -> user_perm));
    write_pair_to_file(filepath, "se_acl_group_perm", int_to_string(data -> group_perm));
    write_pair_to_file(filepath, "se_acl_other_perm", int_to_string(data -> oth_perm));
    write_pair_to_file(filepath, "se_acl_mask", int_to_string(data -> mask));
    //TODO write named user and named groups to the file
}

struct acl_data* getacl(char* filepath)
{
    struct acl_data* acl = (struct acl_data*) malloc(sizeof(struct acl_data));
    
    char* owner = read_value_from_file(filepath, "se_acl_owner");
    char* group = read_value_from_file(filepath, "se_acl_group");
    int se_acl_user_perm = atoi(read_value_from_file(filepath, "se_acl_user_perm"));
    int se_acl_group_perm = atoi(read_value_from_file(filepath, "se_acl_group_perm"));
    int se_acl_other_perm = atoi(read_value_from_file(filepath, "se_acl_other_perm"));
    int se_acl_mask = atoi(read_value_from_file(filepath, "se_acl_mask"));

    acl -> owner = owner;
    acl -> group = group;
    acl -> user_perm = se_acl_user_perm;
    acl -> group_perm = se_acl_group_perm;
    acl -> oth_perm = se_acl_user_perm;
    acl -> mask = se_acl_mask;

    return acl;
}

