#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	//Varibles
	char command[10] = "ls";
	char commandInput[10];

	struct dirent *dent;
	DIR *directory;
	char *buffer[50];

	if(argc == 2) //Si se ingresan dos parametros 
    {

		//Copiar
		strcpy(commandInput, argv[1]);

		//Si es el primer parametro no es igual a cp no se hace la copia
        if(strcmp(commandInput, command)!=0) 
        {
            printf("Comando incorrecto\n");
            return 1;
        }

		//Se obtiene el path del directorio en el cual se est´a trabajando
		getcwd(buffer, sizeof(buffer));

		//Se abre el directorio
		directory = opendir(buffer);

		if(directory == NULL)
		{
			printf("No se pudo abrir el directorio %s\n", buffer);
			return -1;
		}

		//Leer archivos del directorio
		while((dent = readdir(directory)) != NULL)
		{
			printf("[%s]\n",dent -> d_name);
		}

		closedir(directory);
		return 0;

    }
	else if(argc == 3) //Si se ingresan 3 parametros
	{

		//Copiar
		strcpy(commandInput, argv[1]);
		strcpy(buffer, argv[2]);

		//Si es el primer parametro no es igual a cp no se hace la copia
        if(strcmp(commandInput, command)!=0) 
        {
            printf("Comando incorrecto\n");
            return 1;
        }

		//Se abre el directorio
		directory = opendir(buffer);

		if(directory == NULL)
		{
			printf("No se pudo abrir el directorio %s\n", buffer);
			return -1;
		}

		//Leer archivos del directorio
		while((dent = readdir(directory) != NULL))
		{
			printf("[%s]\n",dent -> d_name);
		}

		closedir(directory);

		return 0;

	}
    else
    {
		printf("Numero de argumentos no válido\n");
        return -1;
	}
}