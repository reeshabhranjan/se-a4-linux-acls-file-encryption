#ifndef SECURITY_H
#define SECURITY_H

// interface function
int username_exists(char* username);
int groupname_exists(char* groupname);
int is_file(char* filename);
int is_directory(char* directory_name);

#endif