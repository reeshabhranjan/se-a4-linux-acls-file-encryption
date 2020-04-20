#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "encrypt.h"

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
    printf("%d\n", fverify("checksum2.txt"));
    return 0;
}