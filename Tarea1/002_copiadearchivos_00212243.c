#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>


int main(int argc, char **argv)
{
    //Variables
    int num; //Bytes
    int readFile; //Archivo de lectura
    int writeFile; //Archivo de escritura
    char nameReadFile[200]; //Nombre del archivo de lectura
    char nameWriteFile[200]; //Nombre del archivo de escritura
    char buffer[32]; //Buffer
    char command[10] = "cp"; //Comando cp para copiar de un archivo a otro
    char commandInput[10]; //Comando ingresado

    //Se deben ingresar 4 parametros (nombre archivo, comando, archivo de lectura, archivo de escritura)
    if(argc>4 || argc<4) //Si no se ingresan el numero de parametros
    {
        printf("Número de argumentos no válido\n");
        return -1;
    }
    else
    {
        //Copia los parametros ingresados a variables
        strcpy(commandInput, argv[1]);
        strcpy(nameReadFile, argv[2]);
        strcpy(nameWriteFile, argv[3]);

        //Si es el primer parametro no es igual a cp no se hace la copia
        if(strcmp(commandInput, command)!=0) 
        {
            printf("Comando incorrecto\n");
            return 1;
        }

        //Si no se puede abrir el archivo de lectura
        if ((readFile = open(nameReadFile, O_RDONLY)) == -1)
        {
            printf("No se pudo abrir archivo %s para leer\n", nameReadFile);
            return 2;
        }

        //Si no se puede abrir el archivo de escritura
        if ((writeFile = open(nameWriteFile, O_WRONLY|O_CREAT|O_TRUNC, 0700)) == -1)
        {
            printf("No se pudo abrir archivo %s para escritura\n", nameWriteFile);
            close(writeFile);
            return 3;
        }
        
        //Bucle para que ha medida que lee de un archivo lo escriba en el otro
        while ((num = read(readFile, &buffer, sizeof(char))) > 0)
        {
            write(writeFile, &buffer, num);
        }

        //Finaliza cerrando archivos y mostrando al usuario que se copio correctamente
        close(readFile);
        close(writeFile);
        printf("Archivo '%s' se copio correctamente al archivo '%s'\n", nameReadFile, nameWriteFile);
        return 0;
    }
}