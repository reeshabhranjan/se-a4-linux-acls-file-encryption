#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<unistd.h>
#include<pwd.h>
#include "acl.h"
#include<string.h>
#include<grp.h>
#include "security.h"

int main(int argc, char* argv[])
{
    if (argc <= 1)
    {
        printf("Enter a directory name.\n");
        exit(1);
    }

    if (argc > 2)
    {
        perror("Enter only a directory name with no spaces (or escaped).\n");
        exit(1);
    }

    int n = strlen(argv[1]);

    char* dir_name = argv[1];
    
    if (argv[1][n - 1] != '/')
    {
        dir_name = (char*) malloc(n + 2);
        strcpy(dir_name, argv[1]);
        strcat(dir_name, "/");
    }

    int slash_position = strfind(dir_name, '/');

    char* parent_directory = NULL;
    n = strlen(dir_name); // BEWARE: re-assigning n
    if (slash_position != n - 1)
    {
        // this means there are more slashes in the path
        // need to extract the parent folder where the new directory is being created
        int prev_position = slash_position;
        int next_position = prev_position + 1 + strfind(dir_name + prev_position + 1, '/');

        while (next_position < n - 1)
        {
            prev_position = next_position;
            next_position = prev_position + 1 + strfind(dir_name + prev_position + 1, '/');
        }
        parent_directory = substring(dir_name, 0, prev_position + 1);   
    }
    else
    {
        parent_directory = ".";
    }

    if (!file_exists(parent_directory))
    {
        perror("Invalid path.");
        exit(1);
    }

    if (!is_directory(parent_directory))
    {
        perror("Path does not lead to a valid directory.");
        exit(1);
    }

    int caller_uid = getuid();
    struct passwd* pwd = (struct passwd*) malloc(sizeof(struct passwd));
    *pwd = *(getpwuid(caller_uid));
    char* caller_name = (char*) malloc(strlen(pwd -> pw_name) + 1);
    strcpy(caller_name, pwd -> pw_name);

    if (!validate(caller_name, parent_directory, 10))
    {
        perror("You do not have sufficient permissions!");
        exit(1);
    }

    printf("UID: %d EUID: %d\n", getuid(), geteuid());

    // TODO set permissions inherited from the parent directory

    struct acl_data* parent_acl = getacl(parent_directory);

    int default_permissions = 0777; // TODO sync it with acl, remove sticky bit
    int status = mkdir(dir_name, default_permissions);
    if (status == -1)
    {
        perror("Cannot create directory.");
        exit(1);
    }

    setacl(parent_acl, dir_name);

    seteuid(getuid());
    printf("UID: %d EUID: %d\n", getuid(), geteuid());

    return 0;
}