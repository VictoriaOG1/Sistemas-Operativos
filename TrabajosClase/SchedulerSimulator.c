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
    int serviceTime;
    int remainingTime;
    int interruptLength[];
};


void roundRobin(struct Process pro[], int n, int quantum)
{
    int time = 0;
    int i, j;
    int remainingTime[n];
    int flag, totalWaitTime = 0, totalResponseTime = 0;

    // Inicializa el tiempo restante del proceso como su tiempo de ejecución.
    for (i = 0; i < n; i++)
    {
        pro[i].remainingTime = pro[i].burstTime;
    }

    // Ejecuta el Round Robin hasta que todos los procesos hayan terminado.
    while (1)
    {
        flag = 0;

        // Procesa todos los procesos uno por uno.
        for (i = 0; i < n; i++)
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

                // Si el tiempo restante es menor o igual al quantum,
                // el proceso se completará en esta iteración, así que actualiza el tiempo restante.
                if (pro[i].remainingTime <= quantum)
                {
                    time += pro[i].remainingTime;
                    pro[i].remainingTime = 0;

                    // Registra el tiempo de finalización del proceso.
                    pro[i].exitTime = time;

                    // Calcula el tiempo de espera del proceso.
                    pro[i].waitingTime = pro[i].exitTime - pro[i].arrivalTime - pro[i].burstTime;

                    // Calcula los tiempos de espera y de respuesta promedio de todos los procesos.
                    totalWaitTime += pro[i].waitingTime;
                    totalResponseTime += pro[i].responseTime;
                }

                // Si el tiempo restante es mayor que el quantum,
                // el proceso todavía no se ha completado, así que se ejecuta durante el quantum.
                else
                {
                    time += quantum;
                    pro[i].remainingTime -= quantum;
                }
            }

            // Si el tiempo de llegada del proceso es mayor que el tiempo actual,
            // se avanza el tiempo hasta el tiempo de llegada del proceso.
            else
            {
                time++;
                i--;
            }
        }

        // Si ningún proceso se procesó en esta iteración,
        // significa que todos los procesos se han completado.
        if (flag == 0)
        {
            break;
        }
    }

    // Calcula el tiempo de respuesta promedio.
    float avgResponseTime = (float)totalResponseTime / (float)n;

    // Calcula el tiempo de espera promedio.
    float avgWaitTime = (float)totalWaitTime / (float)n;

    // Imprime los resultados.
    printf("\nProceso\t Tiempo de Llegada\t Tiempo de Ejecución\t Tiempo de Respuesta\t Tiempo de Espera\n");
    for (i = 0; i < n; i++)
    {
        printf("%d\t\t %d\t\t\t %d\t\t\t %d\t\t\t %d\n", pro[i].id, pro[i].arrivalTime, pro[i].burstTime, pro[i].responseTime, pro[i].waitingTime);
    }
    printf("\nTiempo de Respuesta Promedio = %.2f", avgResponseTime);
    printf("\nTiempo de Espera Promedio = %.2f", avgWaitTime);
}

int main()
{
    int i, n, quantum;
    printf("Ingrese el número de procesos: ");
    scanf("%d", &n);

    struct Process pro[n];

    for (i = 0; i < n; i++)
    {
        printf("Ingrese el tiempo de llegada para el Process %d: ", i + 1);
        scanf("%d", &pro[i].arrivalTime);

        printf("Ingrese el tiempo de CPU para el Process %d: ", i + 1);
        scanf("%d", &pro[i].burstTime);

        pro[i].id = i + 1;
        pro[i].state = 0; // procesos no iniciados
    }

    printf("Ingrese el quantum para el algoritmo de Round Robin: ");
    scanf("%d", &quantum);

    roundRobin(pro, n, quantum);

    return 0;
}