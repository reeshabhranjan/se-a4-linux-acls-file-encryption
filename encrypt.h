#ifndef ENCRYPT_H
#define ENCRYPT_H

// interface function
void generate_key_iv(char** key, char** iv);
char* encrypt_string(char* plaintext, char* key, char* iv);
char* decrypt_string(char* ciphertext, char* key, char* iv);
void fsign(char* buffer);
int fverify(char* filepath);
#endif