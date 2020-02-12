#ifndef ACL_H
#define ACL_H

struct acl_data
{
    char ** acl;
    int acl_count;
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