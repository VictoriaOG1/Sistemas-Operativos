#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>


void getPermission(unsigned int m)
{
	putchar('\t');
	putchar( m & S_IRUSR ? 'r' : '-' );
    putchar( m & S_IWUSR ? 'w' : '-' );
    putchar( m & S_IXUSR ? 'x' : '-' );
    putchar( m & S_IRGRP ? 'r' : '-' );
    putchar( m & S_IWGRP ? 'w' : '-' );
    putchar( m & S_IXGRP ? 'x' : '-' );
    putchar( m & S_IROTH ? 'r' : '-' );
    putchar( m & S_IWOTH ? 'w' : '-' );
    putchar( m & S_IXOTH ? 'x' : '-' );
    putchar('\n');
}

int main(int argc, char **argv)
{
	//Varibles
	char command[10] = "ls";
	char commandInput[10];
	struct dirent *dent;
	DIR *directory;
	char *path[50];
	struct stat statistics;
	char header[200];

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
		getcwd(path, sizeof(path));

		//Se abre el directorio
		directory = opendir(path);

		if(directory == NULL)
		{
			printf("No se pudo abrir el directorio %s\n", path);
			return -1;
		}

		printf("Nombre\t\t\t\tTamaño(bytes)\tModo\n");
		//Leer archivos del directorio
		while((dent = readdir(directory)) != NULL)
		{
			stat(dent->d_name, &statistics);

			printf("%-30s\t%d\t\t%d \n", dent->d_name, statistics.st_size, statistics.st_mode);
		}

		closedir(directory);
		return 0;

    }
	else if(argc == 3) //Si se ingresan 3 parametros
	{

		//Copiar
		strcpy(commandInput, argv[1]);
		strcpy(path, argv[2]);

		//Si es el primer parametro no es igual a cp no se hace la copia
        if(strcmp(commandInput, command)!=0) 
        {
            printf("Comando incorrecto\n");
            return 1;
        }

		//Se abre el directorio
		directory = opendir(path);

		if(directory == NULL)
		{
			printf("No se pudo abrir el directorio %s\n", path);
			return -1;
		}

		printf("\nName\t\t\t\tSize(bytes)\tMode\tPermissions\n");
		//Leer archivos del directorio
		while((dent = readdir(directory)) != NULL)
		{
			stat(dent->d_name, &statistics);
			printf("%-30s\t%-12d\t%-6X", dent->d_name, statistics.st_size, statistics.st_mode);
			getPermission(statistics.st_mode);
		}
		printf("\n");
		closedir(directory);
		return 0;
	}
    else
    {
		printf("Numero de argumentos no válido\n");
        return -1;
	}
}