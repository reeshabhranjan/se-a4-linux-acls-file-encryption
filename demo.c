#include<stdio.h>
#include "acl.h"
#include<errno.h>
#include<string.h>

int main()
{
    // setacl(NULL, "debug/sample.txt");
    getacl("debug/sample.txt");
    printf("%s\n", strerror((int)errno));
    return 0;
}