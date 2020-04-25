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
    char stuff[20] = "123\0\0123123";
    write_to_file_with_len("stuff.txt", stuff, 12, 1);
    char* restuff = read_from_file("stuff.txt");
    for (int i = 0; i < 12; i++)
    {
        printf("%c", restuff[i]);
    }
    
    return 0;
}