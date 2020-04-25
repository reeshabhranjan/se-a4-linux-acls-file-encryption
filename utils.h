#ifndef UTILS_H
#define UTILS_H

char* concatenate_strings(char* s1, char* s2);
void create_file(char* filepath, int owner_id, int group_id, int permissions);
void write_to_file(char* filepath, char* buffer, int overwrite);
char* read_from_file(char* filepath);
void write_to_file_with_len(char* filepath, char* buffer, int len, int overwrite);
char* read_from_file_with_num_bytes(char* filepath, int* num_bytes_read);
void printf_custom(char* label, char* s, int len);
#endif