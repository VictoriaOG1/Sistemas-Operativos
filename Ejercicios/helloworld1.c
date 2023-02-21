#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


int main(int argc, char**argv)
{
    if (argc!=2)
    {
        printf("Error. Usage: %s <Name> \n", argv[0]);
        return -1;
    }
    else
    {
        printf("Hello World: %s \n", argv[1]);
        // printf("Hello %s \n", argv[2]);
        return 0;
    }
}

