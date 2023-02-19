/*
Sistemas Operativos
Alejandra Ospina (00212243)
Tarea 1 Ejercicio 2
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main () 
{
    //Variables
    int statusH1, statusH2, statusH3, statusN1, statusN2; //Guardar status de cada proceso
    int pid1, pid2, pid3, pid4, pid5; //Para crear los procesos
    pid_t pidHijo1, pidHijo2, pidHijo3, pidNieto1, pidNieto2; //Para almacenar los pids de cada proceso

    //Creacion proceso hijo 1
    pid1 = fork();
    //pid1 > 0 proceso padre
    if(pid1>0)
    {
        /***Proceso padre***/
        //Creacion de proceso hijo 2
        pid2 = fork();
        //pid2 > 0 proceso padre 
        if(pid2>0)
        {
            /***Proceso padre***/
            //Creacion proceso hijo 3
            pid3 = fork();
            //pid3 > 0 proceso padre 
            if(pid3>0)
            {
                /***Proceso padre***/
                //Esperar hijo 2
                pidHijo2 = waitpid(pid2, &statusH2, 0);
                if(WIFEXITED(statusH2))
                    printf("Proceso hijo 2 termino con pid %d y status %d \n", pidHijo2, WEXITSTATUS(statusH2));

                //Esperar hijo 3
                pidHijo3 = waitpid(pid3, &statusH3, 0);
                if(WIFEXITED(statusH3))
                    printf("Proceso hijo 3 termino con pid %d y status %d \n", pidHijo3, WEXITSTATUS(statusH3));

                //Esperar hijo 1
                pidHijo1 = waitpid(pid1, &statusH1, 0);
                if(WIFEXITED(statusH1))
                    printf("Proceso hijo 1 termino con pid %d y status %d \n", pidHijo1, WEXITSTATUS(statusH1));

                printf("Proceso padre termino con pid %d\n", getpid());
            }
            else
            {
                /***Proceso hijo 3***/
            }

        }
        else
        {
            /***Proceso hijo 2***/
            //Creacion nieto 1
            pid4 = fork();
            //pid4 > 0 proceso hijo 2
            if(pid4>0)
            {
                /***Proceso hijo 2***/
                //Creacion nieto2
                pid5 = fork();
                //pid5 > 0 proceso hijo 2
                if(pid5 > 0)
                {
                    /***Proceso hijo 2***/
                    //Esperar hijo 2
                    pidNieto2 = waitpid(pid5, &statusN2, 0);
                    if(WIFEXITED(statusN2))
                        printf("Proceso nieto 2 termino con pid %d y status %d \n", pidNieto2, WEXITSTATUS(statusN2));

                    //Esperar hijo 1
                    pidNieto1 = waitpid(pid4, &statusN1, 0);
                    if(WIFEXITED(statusN1))
                        printf("Proceso nieto 1 termino con pid %d y status %d \n", pidNieto1, WEXITSTATUS(statusN1));
                }
                else
                {
                    /***Proceso nieto 2***/
                }

            }
            else
            {
                /***Proceso nieto 1***/
            }
        }
    }
    else 
    {
        /***Proceso hijo 1***/
    }

    return 0;
}