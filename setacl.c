#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pwd.h>
#include "acl.h"
#include<string.h>

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        printf("Incorrect arguments.\n");
        printf("./setacl -m <u/g/m>:<username/groupname/leave_empty_for_mask_or_other>:<permission> <filepath>\n");
        exit(1);
    }

    char* flag = argv[1];
    char* body = argv[2];
    char* filepath = argv[3];

    char mode_char = body[0];

    int colon_index_1 = 1;
    int colon_index_2 = colon_index_1 + 1 + strfind(body + colon_index_1 + 1, ':');

    char* name = substring(body, colon_index_1 + 1, colon_index_2);
    char* permission_string = substring(body, colon_index_2 + 1, strlen(body));

    int read_permission = strfind(permission_string, 'r') != -1;
    int write_permission = strfind(permission_string, 'w') != -1;
    int execute_permission = strfind(permission_string, 'x') != -1;

    int permissions = 100 * read_permission + 10 * write_permission + execute_permission;

    switch (mode_char)
    {
    case 'u':
        if (strlen(name) == 0)
        {
            set_permission(filepath, OTHER_TYPE, name, permissions);
        }
        else
        {
            set_permission(filepath, USER_TYPE, name, permissions);
        }
        
        break;
    case 'g':
        set_permission(filepath, GROUP_TYPE, name, permissions);
        break;
    case 'm':
        set_permission(filepath, MASK_TYPE, name, permissions);
        break;
    
    default:
        break;
    }

    return 0;
}