#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "encrypt.h"

int main()
{
    char* key;
    char* iv;
    generate_key_iv(&key, &iv);
    return 0;
}