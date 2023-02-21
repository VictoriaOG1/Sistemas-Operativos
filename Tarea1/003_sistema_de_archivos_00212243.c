#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>

//Funcion para obtener los permisos de los archivos a partir del mode_t
void printPermission(unsigned int m)
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
	char command[10] = "ls"; //Comando
	char commandInput[10]; //Comando que se ingresa
	struct dirent *dent; //
	DIR *directory; //Guardar directorio
	char *path[50]; //Guardar path
	struct stat statistics; //Informacion de cada archivo

	
	if(argc == 3 || argc == 2) //Si se ingresan 3 parametros
	{
		//Obtener comando y resto de parametros
		strcpy(commandInput, argv[1]);

		if(argc == 2)
			getcwd(path, sizeof(path));
		if(argc == 3)
			strcpy(path, argv[2]);

		//Si es el primer parametro no es igual a ls 
        if(strcmp(commandInput, command)!=0) 
        {
            printf("Comando incorrecto\n");
            return 1;
        }

		//Se abre el directorio
		directory = opendir(path);

		//Si el directorio actual o el proporcionado no se puede abrir
		if(directory == NULL)
		{
			printf("No se pudo abrir el directorio %s\n", path);
			return -1;
		}

		//Leer archivos del directorio
		printf("\nName\t\t\t\t\tSize(bytes)\tMode\tPermissions\n");
		while((dent = readdir(directory)) != NULL)
		{
			stat(dent->d_name, &statistics);
			printf("%-32s\t%-12d\t%-6X", dent->d_name, statistics.st_size, statistics.st_mode);
			printPermission(statistics.st_mode);
		}
		printf("\n");

		//Cerrar directorio
		closedir(directory);
		return 0;
	}
    else
    {
		printf("Numero de argumentos no válido\n");
        return -1;
	}
}