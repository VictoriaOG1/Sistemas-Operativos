#include <stdio.h>

// Estructura de nuestro proceso
struct Process
{
    int id;
    int arrivalTime;
    int burstTime;
    int state; //0 si no inicia, 1 si ya inició y -1 si está bloqueado
    int responseTime;
    int waitingTime;
    int exitTime;
    int remainingTime;
    int turnaroundTime;
    int numberInterruptions;
    int timeInterrupt;
    int interruptLength[];
};

//Funcion creacion de procesos 
void createProcess(struct Process pro[], int n, int type)
{
    int n1;

    if (type == 1) //90% CPU bound y 10% I/O bound 
    {
        n1 = n*0.9;
    }
    else if (type == 2) //50% CPU bound y 50% I/O bound 
    {
        n1 = n*0.5;
    }
    else //10% CPU bound y 90% I/O bound 
    {
        n1 = n*0.1;
    }

    n1-=1;

    for(int i=0; i<n; i++) //Llena la informacion del proceso uno a uno
    {
        //Asignar el id
        pro[i].id = i+1;
        //Asignar un tiempo aleatorio del arrivalTime
        pro[i].arrivalTime = rand() % 10 +1;
        //Para cada proceso de asigna un burst time aleatorio entre 10 y 50
        pro[i].burstTime = rand() % 10 + 10;
        //Asignar -1 al response time
        pro[i].responseTime = -1;
        //Asignar el burstTime al reaminingTime
        pro[i].remainingTime = pro[i].burstTime;
        //Asignar un estado no inciado
        pro[i].state=0;
    
        // Inicializa el tiempo restante del proceso como su tiempo de ejecucion.
        if (i <= n1) //Procesos CPU bound (mas interrupiones de poco tiempo)
        {
            int n_itrp = rand() % 4; //Número de interrupciones aleatorio entre 4
            int itrp[n_itrp]; //Arreglo de interrupciones

            //Se asigna el numero de interrupciones en numberInterruptions
            pro[i].numberInterruptions = n_itrp;

            //Se asigna un valor aleatorio de tiempo a cada interrupcion
            for(int j=0; j<n_itrp; j++)
            {
                itrp[j] = rand() % 8 + 3; //Entre 3 y 8
            }
            pro[i].interruptLength[n_itrp] = itrp; 
        }
        else //Procesos I/O bound (menos interrupciones de mas tiempo)
        {
            int n_itrp = rand() % 8 + 4; //Número de interrupciones aleatorio entre 8 y 4
            int itrp[n_itrp]; //Arreglo de interrupciones

            //Se asigna el numero de interrupciones en numberInterruptions
            pro[i].numberInterruptions = n_itrp;

            //Se asigna un valor aleatorio de tiempo a cada interrupcion
            for(int j=0; j<n_itrp; j++)
            {
                itrp[j] = rand() % 3; //Entre 0 y 3
            }
            pro[i].interruptLength[n_itrp] = itrp; 
        }

        //Se asigna el momento que ocurre la interrupción
        pro[i].timeInterrupt = pro[i].burstTime/pro[i].numberInterruptions;
    }
}

// Intercambia las direcciones de memoria dentro del array
void swap(struct Process* a, struct Process* b) {
    struct Process temp = *a;
    *a = *b;
    *b = temp;
}


// Divide una particion del array sobre un pivote y mueve los mayores a este a un lado y menores al otro
int partition(struct Process arr[], int low, int high) {
    int pivot = arr[high].arrivalTime;
    int i = (low - 1);
 
    for (int j = low; j <= high - 1; j++) {
        if (arr[j].arrivalTime < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}
// Utiliza recursividad para ordenar el arreglo de procesos (para asegurarnos que esten en order de llegada)
void quicksort(struct Process arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
 
        quicksort(arr, low, pi - 1);
        quicksort(arr, pi + 1, high);
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
            // Si el proceso ya se completó, salta a la siguiente iteración.
            if (pro[i].remainingTime == 0)
            {
                continue;
            }

            // Marca que al menos un proceso está siendo procesado en esta iteración.
            flag = 1;

            // Si el tiempo de llegada del proceso es menor o igual al tiempo actual,
            // se procesa el proceso durante el quantum de tiempo y se actualiza el tiempo restante.
            if (pro[i].arrivalTime <= time && pro[i].state != -1)
            {
                // Marca que el proceso ya ha comenzado.
                pro[i].state = 1;

                // Si es la primera vez que se ejecuta el proceso,
                // registra su tiempo de respuesta como el tiempo actual menos su tiempo de llegada.
                if (pro[i].responseTime == -1)
                {
                    pro[i].responseTime = time - pro[i].arrivalTime;
                }

                // Si el tiempo restante es menor o igual al quantum,
                // el proceso se completará en esta iteración, así que actualiza el tiempo restante.
                if (pro[i].remainingTime <= quantum)
                {
                    printf("Proceso %d: Tiempo %d - %d\n", pro[i].id, time, time + pro[i].remainingTime);
                    time += pro[i].remainingTime;
                    pro[i].remainingTime = 0;

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
                }

                // Si el tiempo restante es mayor que el quantum,
                // el proceso todavía no se ha completado, así que se ejecuta durante el quantum.
                else
                {
                    printf("Proceso %d: Tiempo %d - %d\n", pro[i].id, time, time + quantum); 
                    time += quantum;
                    pro[i].remainingTime -= quantum;
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
                else if(pro[i].state==-1) //Si ocurrió una interrupción
                {
                    for(int j=0; j<pro[i].numberInterruptions; j++)
                    {
                        if(pro[i].interruptLength[j]!=0)
                        {
                            time+=pro[i].interruptLength[j];
                            pro[i].interruptLength[j]=0;
                            pro[i].state = 1;
                            break;
                        }
                    }
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
            break;
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
    printf("\nProceso\t Tiempo de Llegada\t Tiempo de Ejecución\t Tiempo de Respuesta\t Tiempo Final\t Tiempo de servicio\t Tiempo de Espera\t \n");
    for (int i = 0; i < n; i++)
    {
        printf("%d\t\t %d\t\t\t %d\t\t\t %d\t\t\t %d\t\t %d\n", pro[i].id, pro[i].arrivalTime, pro[i].burstTime, pro[i].responseTime,pro[i].exitTime , pro[i].waitingTime);
    }
    printf("\nTiempo de Respuesta Promedio = %.2f", avgResponseTime);
    printf("\nTiempo de Espera Promedio = %.2f", avgWaitTime);
    printf("\nTiempo de Turnaround Promedio = %.2f", avgTurnaroundTime);
    printf("\nThroughtput = %.2f", throughputTime);
}

int main()
{
    int n, quantum;

    //Ingreso de datos: numero de procesos, el burst time. el arrival time de cada proceso y el quantum
    printf("Ingrese el número de proceso: ");
    scanf("%d", &n);

    struct Process pro[n];

    /*
    for (i = 0; i < n; i++)
    {
        printf("Ingrese el tiempo de llegada para el proceso %d: ", i + 1);
        scanf("%d", &pro[i].arrivalTime);

        printf("Ingrese el tiempo de CPU para el proceso %d: ", i + 1);
        scanf("%d", &pro[i].burstTime);

        pro[i].id = i + 1;
        pro[i].state = 0; //Procesos no inicializados 
        pro[i].responseTime = -1;
    }
    */

    printf("Ingrese el quantum para el algoritmo de round robin: ");
    scanf("%d", &quantum);

    //Crear procesos
    createProcess(pro, n, 1);

    //Ordenamiento del arreglo segun su tiempo de llegada
    quicksort(pro,0, n-1);


    printf("\nRound Robin Scheduler: \n");
    //Se realiza el scheduling
    roundRobin(pro, n, quantum);

    return 0;
}