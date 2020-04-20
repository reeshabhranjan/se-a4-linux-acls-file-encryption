#ifndef UTILS_H
#define UTILS_H

char* concatenate_strings(char* s1, char* s2);
void create_file(char* filepath, int owner_id, int group_id, int permissions);
void write_to_file(char* filepath, char* buffer, int overwrite);
char* read_from_file(char* filepath);
#endif