#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

int main(int argc, char **argv)
{
    FILE * readFile;
    FILE * writeFile;
    char nameReadFile[201];
    char nameWriteFile[201];
    char buffer[32];
    size_t num;

    if(argc>3 || argc<3)
    {
        printf("Número de argumentos no válido\n");
        return -1;
    }
    else
    {
        strcpy(nameReadFile, argv[1]);
        strcpy(nameWriteFile, argv[2]);

        if ( (readFile = fopen(nameReadFile, "r")) == NULL )
        {
            printf("No se pudo abrir archivo %s para leer\n", nameReadFile);
            return 3;
        }

        if ( (writeFile = fopen(nameWriteFile, "w")) == NULL )
        {
            printf("No se pudo abrir archivo %s para escritura\n", nameWriteFile);
            fclose(writeFile);
            return 4;
        }
        
        while((num = fread(buffer, sizeof(char), 32, readFile) ) > 0 )
        {
            fwrite(buffer, sizeof(char), num, writeFile);

            if (ferror(readFile) || ferror(writeFile))
            {
                printf("Error en la copia");
                fclose(readFile);
                fclose(writeFile);
                return 6;
            }
        }

        fclose(readFile);
        fclose(writeFile);
        printf("Archivo '%s' se copio correctamente al archivo '%s'\n", nameReadFile,
        nameWriteFile);
        return 0;
    }


}