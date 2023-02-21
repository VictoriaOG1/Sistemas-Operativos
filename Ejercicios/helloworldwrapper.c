#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void main()
{
   char name[20];
   printf("Ingrese su nombre: ");
   scanf("%s", name);
   char* args[] = {"./helloworld1", name, NULL};
   execve("./helloworld1", args, NULL);
}

