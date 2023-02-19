#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

int main(int argc, char **argv)
{
    int num;
    int readFile;
    int writeFile;
    char nameReadFile[200];
    char nameWriteFile[200];
    char buffer[32];
    char command[10] = "ls";
    char commandInput[10];

    if(argc>4 || argc<4)
    {
        printf("Número de argumentos no válido\n");
        return -1;
    }
    else
    {
        strcpy(commandInput, argv[1]);
        strcpy(nameReadFile, argv[2]);
        strcpy(nameWriteFile, argv[3]);

        if(strcmp(commandInput, command)!=0)
        {
            printf("Comando incorrecto\n");
            return -1;
        }

        if ((readFile = open(nameReadFile, O_RDONLY)) == -1)
        {
            printf("No se pudo abrir archivo %s para leer\n", nameReadFile);
            return 3;
        }

        if ((writeFile = open(nameWriteFile, O_WRONLY|O_CREAT|O_TRUNC, 0700)) == -1)
        {
            printf("No se pudo abrir archivo %s para escritura\n", nameWriteFile);
            fclose(writeFile);
            return 4;
        }
        
        while ((num = read(readFile, &buffer, sizeof(char))) > 0)
        {
            write(writeFile, &buffer, num);
        }

        close(readFile);
        close(writeFile);
        printf("Archivo '%s' se copio correctamente al archivo '%s'\n", nameReadFile, nameWriteFile);
        return 0;
    }
}