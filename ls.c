#include<stdio.h>
#include<dirent.h>
#include<sys/stat.h>
#include<string.h>
#include<stdlib.h>
#include "acl.h"
#include<unistd.h>
#include<pwd.h>
#include "security.h"

int main(int argc, char* argv[])
{
    if (argc <= 1)
    {
        perror("Give a directory as input!\n");
        exit(1);
    }

    if (argc > 2)
    {
        perror("Too many arguments!\n");
        exit(1);
    }

    char* base_directory_name = argv[1];

    if (!file_exists(base_directory_name))
    {
        perror("The directory does not exist.");
        exit(1);
    }

    if (!is_directory(base_directory_name))
    {
        perror("The input path does not point to a directory.");
        exit(1);
    }

    int caller_uid = getuid();
    struct passwd* pwd = getpwuid(caller_uid);
    char* caller_username = pwd -> pw_name;

    if (!validate(caller_username, base_directory_name, 100))
    {
        perror("You do not have appropriate permissions!");
        exit(1);
    }

    int n = strlen(base_directory_name);
    if (base_directory_name[n - 2] != '/')
    {
        base_directory_name = (char*) malloc(n + 1 + 1); // + 1 for adding "/" and + 1 for null-termination
        strcpy(base_directory_name, argv[1]);
        strcat(base_directory_name, "/");
    }
    DIR* base_directory = opendir(base_directory_name);
    struct dirent* sub_entry;

    sub_entry = readdir(base_directory);

    char* file_full_name = (char*) malloc(strlen(base_directory_name) + strlen(sub_entry -> d_name) + 1); // + 1 for null-termination
    strcpy(file_full_name, base_directory_name);
    strcat(file_full_name, sub_entry -> d_name);

    while (sub_entry != NULL)
    {
        char* entry_name = sub_entry -> d_name;
        struct stat st;
        stat(entry_name, &st);
        struct acl_data* acl = getacl(file_full_name);

        FILE* file = fopen(file_full_name, "r");
        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        fclose(file);

        // TODO fix file-size
        printf("file: %-15s owner: %-10s (%-3d) group: %-10s (%-3d) others: (%-3d) size: (%-10lu)\n", sub_entry -> d_name, acl -> owner, acl -> user_perm, acl -> group, acl -> group_perm, acl -> oth_perm, st.st_size);

        sub_entry = readdir(base_directory);

        free(file_full_name);

        file_full_name = NULL;

        if (sub_entry != NULL)
        {
            file_full_name = (char*) malloc(strlen(base_directory_name) + strlen(sub_entry -> d_name) + 1);
            strcpy(file_full_name, base_directory_name);
            strcat(file_full_name, sub_entry -> d_name);
        }
    }

    return 0;
}