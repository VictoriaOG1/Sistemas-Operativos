#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Estructura de nuestro proceso
struct Process
{
    int id;
    int arrivalTime;
    int burstTime;
    int state; //Estado 0 si no se inicializa, 1 si esta corriendo, -1 si esta bloqueado y 2 si termino
    int responseTime;
    int waitingTime;
    int exitTime;
    int turnaroundTime;
    int remainingTime;
    int numberInterruptions;
    int remainingItrTime;
    int durationInterupts[8];
    int whenInterrupts[8];
};

//Funcion creacion de procesos 
void createProcess(struct Process pro[], int n, int condition)
{
    int n1 = 0;

    if (condition == 1) //50% CPU bound y 50% I/O bound (literal a)
    {
        n1 = n*0.5;
    }
    else if (condition == 2) //90% CPU bound y 10% I/O bound (literal b)
    {
        n1 = n*0.9;
    }
    else if (condition == 3)//10% CPU bound y 90% I/O bound (literal c)
    {
        n1 = n*0.1;
    }
    else{
        printf("Warning no se asigno la razon de bounds");
    }

    for(int i=0; i<n; i++) //Llena la informacion de cada proceso (uno a uno)
    {
        //Asignar el id
        pro[i].id = i+1;
        //Asignar un tiempo aleatorio del arrivalTime entre 1 y 10
        pro[i].arrivalTime = rand() % 10 + 1;
        //Para cada proceso de asigna un burst time aleatorio entre 10 y 50
        pro[i].burstTime = rand() % 15 + 10;
        //Asignar -1 cuando no a inicializado el proceso
        pro[i].responseTime = -1;
        //Asignar remainingTime igual al burstTime
        pro[i].remainingTime = pro[i].burstTime;
        pro[i].exitTime=0;
        pro[i].waitingTime=0;


        if (n1 > 0 && i <= n1) //Procesos CPU bound (pocas interrupiones de mucho tiempo)
        {
            int n_itrp = rand() % 4 + 1; //Número de interrupciones aleatorio entre 1 y 4

            pro[i].numberInterruptions = n_itrp; //Asignar numero interrupciones

            //Se asigna un valor aleatorio de tiempo a cada interrupcion
            for(int j=0; j<n_itrp; j++){
                int duration = rand() % 6 + 3; //Entre 3 y 6
                //agregarlas al arreglo de interrupciones
                pro[i].durationInterupts[j] = duration;
            }
            
            //determinar el multiplo para seleccionar los en donde se va a interruptir
            int multiple = pro[i].burstTime / n_itrp;
            
            //se asigna los segundos en donde se va a interruptir
            for(int j=0; j<n_itrp; j++){
                pro[i].whenInterrupts[j] = (j+1) * multiple;
            }
        }
        else if(i > n1) //Procesos I/O bound (muchas interrupciones de poco tiempo)
        {
            int n_itrp = rand() % 8 + 4; //Número de interrupciones aleatorio entre 4 y 8

            pro[i].numberInterruptions = n_itrp; //Asignar numero interrupciones

            //Se asigna un valor aleatorio de tiempo a cada interrupcion
            for(int j=0; j<n_itrp; j++){
                int duration = rand() % 3 + 1; //Entre 1 y 3
                //agregarlas al arreglo de interrupciones
                pro[i].durationInterupts[j] = duration;
            }
            
            //determinar el multiplo para seleccionar los en donde se va a interruptir
            int multiple = pro[i].burstTime / n_itrp;
            
            //se asigna los segundos en donde se va a interruptir
            for(int j=0; j<n_itrp; j++){
                pro[i].whenInterrupts[j] = (j+1) * multiple;
            }
        }
        else{
            printf("Error, asignar la razon de bounds");
        }
    }
}

// Intercambia las direcciones de memoria dentro del array
void swap(struct Process *a, struct Process *b)
{
    struct Process temp = *a;
    *a = *b;
    *b = temp;
}

// Divide una particion del array sobre un pivote y mueve los mayores a este a un lado y menores al otro
int partition(struct Process arr[], int low, int high, int type)
{
    int i = (low - 1);

    if (type==1)
    {
        int pivot = arr[high].arrivalTime;
        
        for (int j = low; j <= high - 1; j++)
        {
            if (arr[j].arrivalTime < pivot)
            {
                i++;
                swap(&arr[i], &arr[j]);
            }
        }
    }
    else
    {
        int pivot = arr[high].burstTime;

        for (int j = low; j <= high - 1; j++)
        {
            if (arr[j].burstTime < pivot)
            {
                i++;
                swap(&arr[i], &arr[j]);
            }
        }
    }

    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}
// Utiliza recursividad para ordenar el arreglo de procesos (para asegurarnos que esten en order de llegada)
void quicksort(struct Process arr[], int low, int high, int type)
{
    if (low < high)
    {
        int pi = partition(arr, low, high, type);

        quicksort(arr, low, pi - 1, type);
        quicksort(arr, pi + 1, high, type);
    }
}

//Funcion para simular un scheduler con algoritmo Round Robin 
void roundRobin(struct Process pro[], int n, int quantum)
{
    int time = 0;
    int flag, totalWaitTime = 0, totalResponseTime = 0, totalTurnaroundTime = 0;

    //Ejercutar el round robin hasta que todos los procesos hayan terminado
    while(1)
    {
        flag = 0; //Se mantiene 0 si todos los procesos terminan

        //Transversar por cada proceso
        for(int i=0; i<n; i++)
        {
            // Si el proceso ya se completó o si está bloqueado salta a la siguiente iteración.
            if (pro[i].state == 2 || pro[i].state == -1)
            {
                continue;
            }

            // Marca que al menos un proceso está siendo procesado en esta iteración.
            flag = 1;

            // Si el tiempo de llegada del proceso es menor o igual al tiempo actual,
            // se procesa el proceso durante el quantum de tiempo y se actualiza el tiempo restante.
            if (pro[i].arrivalTime <= time)
            {
                // Marca que el proceso ya ha comenzado.
                pro[i].state = 1;

                // Si es la primera vez que se ejecuta el proceso,
                // registra su tiempo de respuesta como el tiempo actual menos su tiempo de llegada.
                if (pro[i].responseTime == -1)
                {
                    pro[i].responseTime = time - pro[i].arrivalTime;
                }

                int flag2=0;
                for(int j=1; j<=quantum; j++) //Cada segundo del quantum
                {
                    time++; //Se aumenta un segundo al tiempo total
                    pro[i].remainingTime--; //Se disminuye el remaining time en un segundo 

                    if(pro[i].remainingTime==0) //Cuando termina el proceso
                    {
                        // Registra el tiempo de finalización del proceso.
                        pro[i].exitTime = time;

                        // Calcula el tiempo de espera del proceso.
                        pro[i].waitingTime = pro[i].exitTime - pro[i].arrivalTime - pro[i].burstTime;

                        //Calcula el tiempo de turnaround 
                        pro[i].turnaroundTime = pro[i].exitTime - pro[i].arrivalTime;
                        
                        // Calcula los tiempos de espera y de respuesta promedio de todos los procesos.
                        totalWaitTime += pro[i].waitingTime;
                        totalResponseTime += pro[i].responseTime;
                        totalTurnaroundTime += pro[i].turnaroundTime;
                        
                        //Cambiar de estado
                        pro[i].state=2;

                        break;
                    }

                    /*Procesos bloqueados se reducen su tiempo de espera*/
                    for(int m=0; m<n; m++)
                    {
                        if(pro[m].state==-1)
                        {
                            pro[m].remainingItrTime--;

                            if(pro[m].remainingItrTime==0)
                            {
                                pro[m].state=1;
                            }
                        }
                    }
                    

                    /*Chequea si es que ocurre una interrupcion*/
                    for (int k=0; k<pro[i].numberInterruptions; k++) //Cada interrupción 
                    {
                        if(pro[i].whenInterrupts[k] == pro[i].burstTime - pro[i].remainingTime)
                        {
                            flag2=1;
                            pro[i].state=-1;
                            pro[i].remainingItrTime = pro[i].durationInterupts[k];
                            break;
                        }
                    }
                    if(flag2==1)
                    {
                        break;
                    }
                }
            }
            // Si el tiempo de llegada del proceso es mayor que el tiempo actual
            else
            {
                //Si los procesos anteriores ya iniciaron y el que se encuentra ahora todavía no llega la cola de ready
                if(i>0 && pro[i-1].state==1 && pro[i].state==0)
                {
                    i=-1;
                }
                //Se avanza el tiempo hasta el tiempo de llegada del proceso.
                else
                {
                    time++;
                    i--;
                } 
            }
        }
        // Si ningún proceso se procesó en esta iteración,
        // significa que todos los procesos se han completado.
        if(flag==0)
        {
            
            time++;
            int flag3=0;
            
            for(int p=0; p<n; p++)
            {
                if(pro[p].state==-1)
                {
                    flag3=1;
                    pro[p].remainingItrTime--;

                    if(pro[p].remainingItrTime<=0)
                    {
                        pro[p].state=1;
                    }
                }
            }
            if(flag3==0)
            {
                break;
            }
        }
    }

    // Calcula el tiempo de respuesta promedio.
    float avgResponseTime = (float)totalResponseTime / (float)n;

    // Calcula el tiempo de espera promedio.
    float avgWaitTime = (float)totalWaitTime / (float)n;

    //Calcula el turnaround time promedio
    float avgTurnaroundTime = (float)totalTurnaroundTime /(float)n;

    //Calcula el throughput o rendimiento
    float throughputTime = (float)n/(float)time;

    // Imprime los resultados.
    printf("\nProceso\t Tiempo de Llegada\t Tiempo de Ejecución\t Tiempo de Respuesta\t Tiempo Final\t Tiempo de Espera\t \n");
    for (int i = 0; i < n; i++)
    {
        printf("%d\t\t %d\t\t\t %d\t\t\t %d\t\t %d\t\t %d\n", pro[i].id, pro[i].arrivalTime, pro[i].burstTime, pro[i].responseTime, pro[i].exitTime , pro[i].waitingTime);
    }
    printf("\nTiempo de Respuesta Promedio = %.2f", avgResponseTime);
    printf("\nTiempo de Espera Promedio = %.2f", avgWaitTime);
    printf("\nTiempo de Turnaround Promedio = %.2f", avgTurnaroundTime);
    printf("\nThroughtput = %.2f\n", throughputTime);
}


//Funcion para simular un scheduler con algoritmo First Come First Served
void FCFS(struct Process pro[], int n)
{
    int time = 0;
    int flag, totalWaitTime = 0, totalResponseTime = 0, totalTurnaroundTime = 0;

    //Ejercutar el FCFS hasta que todos los procesos hayan terminado
    while(1)
    {
        flag = 0; //Se mantiene 0 si todos los procesos terminan

        //Transversar por cada proceso
        for(int i=0; i<n; i++)
        {
            // Si el proceso ya se completó, salta a la siguiente iteración.
            if (pro[i].state == 2 || pro[i].state == -1)
            {
                continue;
            }

            // Marca que al menos un proceso está siendo procesado en esta iteración.
            flag = 1;

            // Si el tiempo de llegada del proceso es menor o igual al tiempo actual,
            // se procesa el proceso durante el quantum de tiempo y se actualiza el tiempo restante.
            if (pro[i].arrivalTime <= time)
            {
                // Marca que el proceso ya ha comenzado.
                pro[i].state = 1;

                // Si es la primera vez que se ejecuta el proceso,
                // registra su tiempo de respuesta como el tiempo actual menos su tiempo de llegada.
                if (pro[i].responseTime == -1)
                {
                    pro[i].responseTime = time - pro[i].arrivalTime;
                }
              
                int flag2=0;
                
                for(int j=1; j<=pro[i].burstTime; j++) 
                {
                    time++; //Se aumenta un segundo al tiempo total
                    pro[i].remainingTime--; //Se disminuye el remaining time en un segundo 

                    if(pro[i].remainingTime==0)
                    {
                        // Registra el tiempo de finalización del proceso.
                        pro[i].exitTime = time;

                        // Calcula el tiempo de espera del proceso.
                        pro[i].waitingTime = pro[i].exitTime - pro[i].arrivalTime - pro[i].burstTime;

                        //Calcula el tiempo de turnaround 
                        pro[i].turnaroundTime = pro[i].exitTime - pro[i].arrivalTime;
                        
                        // Calcula los tiempos de espera y de respuesta promedio de todos los procesos.
                        totalWaitTime += pro[i].waitingTime;
                        totalResponseTime += pro[i].responseTime;
                        totalTurnaroundTime += pro[i].turnaroundTime;
                        
                        //Cambiar de estado
                        pro[i].state=2;

                        break;
                    }

                    /*Procesos bloqueados se reducen su tiempo de espera*/
                    for(int m=0; m<n; m++)
                    {
                        if(pro[m].state==-1)
                        {
                            pro[m].remainingItrTime--;

                            if(pro[m].remainingItrTime==0)
                            {
                                pro[m].state=1;
                            }
                        }
                    }
                    

                    /*Chequea si es que ocurre una interrupcion*/
                    for (int k=0; k<pro[i].numberInterruptions; k++) //Cada interrupción 
                    {
                        if(pro[i].whenInterrupts[k] == pro[i].burstTime - pro[i].remainingTime)
                        {
                            flag2=1;
                            pro[i].state=-1;
                            pro[i].remainingItrTime = pro[i].durationInterupts[k];
                            break;
                        }
                    }
                    if(flag2==1)
                    {
                        break;
                    }
                    
                }
            }
            // Si el tiempo de llegada del proceso es mayor que el tiempo actual
            else
            {
                //Si los procesos anteriores ya iniciaron y el que se encuentra ahora todavía no llega la cola de ready
                if(i>0 && pro[i-1].state==1 && pro[i].state==0)
                {
                    quicksort(pro,0,i-1,2);
                    i=-1;
                }
                else
                {
                    time++;
                    i--;
                }
            }
        }
        // Si ningún proceso se procesó en esta iteración,
        // significa que todos los procesos se han completado.
        if(flag==0)
        {
            time++;
            int flag3=0;
            
            for(int p=0; p<n; p++)
            {
                if(pro[p].state==-1)
                {
                    flag3=1;
                    pro[p].remainingItrTime--;

                    if(pro[p].remainingItrTime<=0)
                    {
                        pro[p].state=1;
                    }
                }
            }
            if(flag3==0)
            {
                break;
            }
        }
    }

    // Calcula el tiempo de respuesta promedio.
    float avgResponseTime = (float)totalResponseTime / (float)n;

    // Calcula el tiempo de espera promedio.
    float avgWaitTime = (float)totalWaitTime / (float)n;

    //Calcula el turnaround time promedio
    float avgTurnaroundTime = (float)totalTurnaroundTime /(float)n;

    //Calcula el throughput o rendimiento
    float throughputTime = (float)n/(float)time;

    // Imprime los resultados.
    printf("\nProceso\t Tiempo de Llegada\t Tiempo de Ejecución\t Tiempo de Respuesta\t Tiempo Final\t Tiempo de Espera\t \n");
    for (int i = 0; i < n; i++)
    {
        printf("%d\t\t %d\t\t\t %d\t\t\t %d\t\t\t %d\t\t %d\n", pro[i].id, pro[i].arrivalTime, pro[i].burstTime, pro[i].responseTime,pro[i].exitTime , pro[i].waitingTime);
    }
    printf("\nTiempo de Respuesta Promedio = %.2f", avgResponseTime);
    printf("\nTiempo de Espera Promedio = %.2f", avgWaitTime);
    printf("\nTiempo de Turnaround Promedio = %.2f", avgTurnaroundTime);
    printf("\nThroughtput = %.2f\n", throughputTime);
}

//Funcion para simular un scheduler con algoritmo Short Job First
void SJF(struct Process pro[], int n)
{
    int time = 0;
    int flag, totalWaitTime = 0, totalResponseTime = 0, totalTurnaroundTime = 0;

    //Ejercutar el SJF hasta que todos los procesos hayan terminado
    while(1)
    {
        flag = 0; //Se mantiene 0 si todos los procesos terminan

        //Transversar por cada proceso
        for(int i=0; i<n; i++)
        {
            int l;
            for(l=i; l<n; l++)
            {
                if(pro[l].arrivalTime > time)
                {
                    break;
                }
            }
            if(l>i)
            {
                quicksort(pro,i,l-1,2);
            }

            // Si el proceso ya se completó, salta a la siguiente iteración.
            if (pro[i].state == 2 || pro[i].state==-1)
            {
                continue;
            }

            // Marca que al menos un proceso está siendo procesado en esta iteración.
            flag = 1;

            // Si el tiempo de llegada del proceso es menor o igual al tiempo actual,
            // se procesa el proceso durante el quantum de tiempo y se actualiza el tiempo restante.
            if (pro[i].arrivalTime <= time)
            {
            
                // Marca que el proceso ya ha comenzado.
                pro[i].state = 1;

                // Si es la primera vez que se ejecuta el proceso,
                // registra su tiempo de respuesta como el tiempo actual menos su tiempo de llegada.
                if (pro[i].responseTime == -1)
                {
                    pro[i].responseTime = time - pro[i].arrivalTime;
                }

                
                int flag2=0;
                for(int j=1; j<=pro[i].burstTime; j++) //Cada segundo del tiempo de ejecucion
                {
                    time++; //Se aumenta un segundo al tiempo total
                    pro[i].remainingTime--; //Se disminuye el remaining time en un segundo 

                    if(pro[i].remainingTime==0) //Si termina el proceso
                    {
                        // Registra el tiempo de finalización del proceso.
                        pro[i].exitTime = time;

                        // Calcula el tiempo de espera del proceso.
                        pro[i].waitingTime = pro[i].exitTime - pro[i].arrivalTime - pro[i].burstTime;

                        //Calcula el tiempo de turnaround 
                        pro[i].turnaroundTime = pro[i].exitTime - pro[i].arrivalTime;
                        
                        // Calcula los tiempos de espera y de respuesta promedio de todos los procesos.
                        totalWaitTime += pro[i].waitingTime;
                        totalResponseTime += pro[i].responseTime;
                        totalTurnaroundTime += pro[i].turnaroundTime;
                        
                        //Cambiar de estado
                        pro[i].state=2;

                        break;
                    }

                    /*Procesos bloqueados se reducen su tiempo de espera*/
                    for(int m=0; m<n; m++)
                    {
                        if(pro[m].state==-1)
                        {
                            pro[m].remainingItrTime--;

                            if(pro[m].remainingItrTime==0)
                            {
                                pro[m].state=1;
                            }
                        }
                    }

                    /*Chequea si es que ocurre una interrupcion*/
                    for (int k=0; k<pro[i].numberInterruptions; k++) //Cada interrupción 
                    {
                        if(pro[i].whenInterrupts[k] == pro[i].burstTime - pro[i].remainingTime)
                        {
                            flag2=1;
                            pro[i].state=-1;
                            pro[i].remainingItrTime = pro[i].durationInterupts[k];
                            break;
                        }
                    }
                    if(flag2==1)
                    {
                        break;
                    }
                }
            }
            // Si el tiempo de llegada del proceso es mayor que el tiempo actual
            else
            {
                //Si los procesos anteriores ya iniciaron y el que se encuentra ahora todavía no llega la cola de ready
                if(i>0 && pro[i-1].state==1 && pro[i].state==0)
                {
                    i=-1;
                }
                else
                {
                    time++;
                    i--;
                }
            }
        }
        // Si ningún proceso se procesó en esta iteración,
        // significa que todos los procesos se han completado.
        if(flag==0)
        {
            time++;
            int flag3=0;
            
            for(int p=0; p<n; p++)
            {
                if(pro[p].state==-1)
                {
                    flag3=1;
                    pro[p].remainingItrTime--;

                    if(pro[p].remainingItrTime<=0)
                    {
                        pro[p].state=1;
                    }
                }
            }
            if(flag3==0)
            {
                break;
            }
        }
    }

    // Calcula el tiempo de respuesta promedio.
    float avgResponseTime = (float)totalResponseTime / (float)n;

    // Calcula el tiempo de espera promedio.
    float avgWaitTime = (float)totalWaitTime / (float)n;

    //Calcula el turnaround time promedio
    float avgTurnaroundTime = (float)totalTurnaroundTime /(float)n;

    //Calcula el throughput o rendimiento
    float throughputTime = (float)n/(float)time;

    // Imprime los resultados.
    printf("\nProceso\t Tiempo de Llegada\t Tiempo de Ejecución\t Tiempo de Respuesta\t Tiempo Final\t Tiempo de Espera\t \n");
    for (int i = 0; i < n; i++)
    {
        printf("%d\t\t %d\t\t\t %d\t\t\t %d\t\t\t %d\t\t %d\n", pro[i].id, pro[i].arrivalTime, pro[i].burstTime, pro[i].responseTime,pro[i].exitTime , pro[i].waitingTime);
    }
    printf("\nTiempo de Respuesta Promedio = %.2f", avgResponseTime);
    printf("\nTiempo de Espera Promedio = %.2f", avgWaitTime);
    printf("\nTiempo de Turnaround Promedio = %.2f", avgTurnaroundTime);
    printf("\nThroughtput = %.2f\n", throughputTime);
}



int main()
{
    //Variables
    int i, n, quantum;

    // Ingreso de datos: numero de procesos, el burst time. el arrival time de cada proceso y el quantum
    printf("Ingrese el número de proceso: ");
    scanf("%d", &n);

    //Creación de arreglos de procesos
    struct Process pro[n];
    struct Process pro1[n];
    struct Process pro2[n];

    //Instrucción que inicializa el generador de números aleatorios
    srand(time(NULL)); 

    //Crear procesos
    createProcess(pro, n, 1);

    //Copia de procesos 
    for(int x=0; x<n; x++)
    {
        pro1[x] = pro[x];
        pro2[x] = pro[x];
    }

    
    //Se ingresa el quantum para el algoritmo de round robin
    printf("Ingrese el quantum para el algoritmo de round robin: ");
    scanf("%d", &quantum);

    // Ordenamiento del arreglo segun su tiempo de llegada
    quicksort(pro, 0, n - 1, 1);
    quicksort(pro1, 0, n - 1, 1);
    quicksort(pro2, 0, n - 1, 1);

    //Scheduling con Round Robin
    printf("\nRound Robin Scheduler: \n");
    roundRobin(pro, n, quantum);

    //Scheduling con FIFO 
    printf("\nFirst Come First Served Scheduler: \n");
    FCFS(pro1,n);

    //Scheduling con SJF
    printf("\nShortest Job First Scheduler: \n");
    SJF(pro2, n);
    
    return 0;
}