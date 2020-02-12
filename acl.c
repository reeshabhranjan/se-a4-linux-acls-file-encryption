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

void setacl(struct acl_data* data, char* filepath)
{
    char* pre_check_result[2];
    int pre_check = getxattr(filepath, "user.se_acl_exists", pre_check_result, sizeof(pre_check_result));
    if (pre_check == -1)
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
    return NULL;
}

