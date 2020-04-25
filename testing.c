#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "encrypt.h"
#include "utils.h"
#include "acl.h"

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

    // printf("Hello\n");
    // printf("Ciphertext len: %d\n", ciphertext_len);

    // printf_custom("hmac: ", hmac, hmac_len);
    // printf("derived plaintext: %s\n", plaintext_derived);
    

    char* plaintext = "Helloooo";
    int ciphertext_len;
    char* ciphertext = encrypt_string_trapdoor(plaintext, &ciphertext_len);
    int hmac_len;
    char* hmac = create_hmac_trapdoor(ciphertext, ciphertext_len, &hmac_len);
    char* plaintext_derived = decrypt_string_trapdoor(ciphertext, ciphertext_len);

    printf("encryption done\n");

    char* file_name = "part-2/rsa.txt";
    char* hmac_file = "part-2/rsa.txt.sign";

    if (!file_exists(file_name))
    {
        // TODO add ACL support in create file
        // TODO fix mistakes in assignment 2
        create_file(file_name, getuid(), getgid(), 0644);
    }

    if (!file_exists(hmac_file))
    {
        create_file(hmac_file, getuid(), getgid(), 0644);
    }

    write_to_file_with_len(file_name, ciphertext, ciphertext_len, 1);
    write_to_file_with_len(hmac_file, hmac, hmac_len, 1);

    int result = verify_hmac_trapdoor(file_name);

    printf("Result: %d\n", result);

    // TODO make setup.sh instead of make setup


    return 0;
}