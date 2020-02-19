#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pwd.h>
#include "acl.h"
#include<string.h>
#include "security.h"
#include<sys/stat.h>

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

    if (!file_exists(filepath))
    {
        perror("The file does not exist.");
        exit(1);
    }

    char mode_char = body[0];

    int colon_index_1 = 1;
    int colon_index_2 = colon_index_1 + 1 + strfind(body + colon_index_1 + 1, ':');

    char* name = substring(body, colon_index_1 + 1, colon_index_2);

    char* permission_string = substring(body, colon_index_2 + 1, strlen(body));

    if (strlen(permission_string) > 3)
    {
        perror("Improper permissions.");
        exit(1);
    }

    int read_permission = strfind(permission_string, 'r') != -1;
    int write_permission = strfind(permission_string, 'w') != -1;
    int execute_permission = strfind(permission_string, 'x') != -1;

    int permissions = 100 * read_permission + 10 * write_permission + execute_permission;

    if (!is_valid_permission(permissions))
    {
        perror("Improper permissions");
        exit(1);
    }

    char* owner_name = getacl(filepath) -> owner;
    int file_owner_uid = getpwnam(owner_name) -> pw_uid;

    int allowed = getuid() == 0 || getuid() == file_owner_uid || geteuid() == file_owner_uid;

    if (!allowed)
    {
        perror("Permission denied: Only root and owner of the file can modify acl-permissions.");
        exit(1);
    }

    printf("UID: %d EUID: %d\n", getuid(), geteuid());

    switch (mode_char)
    {
    case 'u':
        if (strlen(name) == 0)
        {
            set_permission(filepath, OTHER_TYPE, name, permissions);
        }
        else
        {
            if (!username_exists(name))
            {
                perror("The username does not exist.");
                exit(1);
            }
            set_permission(filepath, USER_TYPE, name, permissions);
        }
        
        break;
    case 'g':
        if (!groupname_exists(name))
        {
            perror("The group-name does not exist.");
            exit(1);
        }
        set_permission(filepath, GROUP_TYPE, name, permissions);
        break;
    case 'm':
        set_permission(filepath, MASK_TYPE, name, permissions);
        break;
    
    default:
        break;
    }

    seteuid(getuid());
    printf("UID: %d EUID: %d", getuid(), geteuid());

    return 0;
}