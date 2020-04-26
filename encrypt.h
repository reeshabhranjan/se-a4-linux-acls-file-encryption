#ifndef ENCRYPT_H
#define ENCRYPT_H

// interface function
void generate_key_iv(char** key, char** iv);
char* encrypt_string(char* plaintext, char* key, char* iv, int* ciphertext_len_returned);
char* decrypt_string(char* ciphertext, char* key, char* iv, int ciphertext_len);
char* fsign(char* buffer, int buffer_len, int* checksum_len_return);
int fverify(char* filepath);
char* gen_rand(int length);
char* encrypt_string_trapdoor(char* buffer, int* ciphertext_len_return);
int verify_hmac_trapdoor(char* filepath);
char* create_hmac_trapdoor(char* buffer, int buffer_len, int* signature_len);
char* decrypt_string_trapdoor(char* ciphertext, int ciphertext_len);
char* get_username();

#endif