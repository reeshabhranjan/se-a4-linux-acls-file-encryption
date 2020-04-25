#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "encrypt.h"
#include "utils.h"

int main()
{
/*     char* key;
    char* iv;
    generate_key_iv(&key, &iv);
    char* plaintext = "Hello world!";
    char* ciphertext = encrypt_string(plaintext, key, iv);
    printf("Plaintext:  %s\n", plaintext);
    printf("Ciphertext: %s\n", ciphertext);
    char* plaintext2 = decrypt_string(ciphertext, key, iv);
    printf("Plaintext2: %s\n", plaintext2); */
    // fsign("Hello", "checksum2.txt");
    // printf("%d\n", fverify("abc2.txt"));
    // char stuff[11] = "123\0\0123123";
    // write_to_file_with_len("stuff.txt", stuff, 11, 1);
    // int num_bytes;
    // char* restuff = read_from_file_with_num_bytes("stuff.txt", &num_bytes);
    // for (int i = 0; i < 11; i++)
    // {
    //     printf("%c", restuff[i]);
    // }
    // printf("\n%d\n", num_bytes);

    char* plaintext = "Helloooo";
    int ciphertext_len;
    char* ciphertext = encrypt_string_trapdoor(plaintext, &ciphertext_len);
    printf("Hello\n");
    printf("Ciphertext len: %d\n", ciphertext_len);
    int hmac_len;
    char* hmac = create_hmac_trapdoor(ciphertext, ciphertext_len, &hmac_len);

    printf_custom("hmac: ", hmac, hmac_len);

    char* plaintext_derived = decrypt_string_trapdoor(ciphertext, ciphertext_len);
    printf("derived plaintext: %s\n", plaintext_derived);
    
    
    return 0;
}