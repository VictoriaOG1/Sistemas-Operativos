#include <pthread.h>
#include <stdio.h>

#define CONTADOR_PRODUCTOR 4                                       // Numero de hilos productores
#define CONTADOR_CONSUMIDOR 4                                      // Numero de hilos consumiedores
#define CONTADOR_THREAD (CONTADOR_PRODUCTOR + CONTADOR_CONSUMIDOR) // Numero total de hilos
#define CONTADOR_TRABAJO 1000                                      // Cantidad de trabajo

#define TAMANO_COLA 64 // Tamano de la cola

// Mutex para la cabeza de la cola
static pthread_mutex_t lock_cabeza = PTHREAD_MUTEX_INITIALIZER;
// Posicion donde esta la cabeza de la cola
static volatile int cabeza_cola = 1;

// Mutex para el final de la cola
static pthread_mutex_t lock_final = PTHREAD_MUTEX_INITIALIZER;
// Posicion donde esta el final de la cola
static volatile int final_cola = 0;

static float cola[TAMANO_COLA]; // Se crea la cola como array

// Declaracion de las funciones productor y consumidor
static void *producer(void *data);
static void *consumer(void *data);

int main()
{
    pthread_t hilos[CONTADOR_THREAD]; // Array hilos
    int array_id[CONTADOR_THREAD];    // Array para guardar los id de los hilos
    int id_hilo;                      // Para los loops

    // Creacion de hilos productores y se almacenan sus ids
    for (id_hilo = 0; id_hilo < CONTADOR_PRODUCTOR; id_hilo++)
    {
        array_id[id_hilo] = id_hilo;
        pthread_create(&hilos[id_hilo], NULL, producer, &array_id[id_hilo]);
    }

    // Creacion de hilos consumidores y se almacenan sus ids
    for (; id_hilo < CONTADOR_THREAD; id_hilo++)
    {
        array_id[id_hilo] = id_hilo;
        pthread_create(&hilos[id_hilo], NULL, consumer, &array_id[id_hilo]);
    }

    // Espera a que terminen todos los hilos
    for (int i = 0; i < CONTADOR_THREAD; i++)
    {
        pthread_join(hilos[i], NULL);
    }

    return 0;
}

// Función para cambiar el puntero de la cola circular
static inline int setQueue(volatile int *index)
{
    int oldHead, newHead; // variables para setear el valor anterior y nuevo del indice dentro de la queue
    do
    {
        oldHead = *index;                      // Asignación desde el puntero actual de la posición del hilo que estaba en ejecución
        newHead = (oldHead + 1) % TAMANO_COLA; // Asignación de la nueva cabeza de la queue
        // Verifica que el valor actual de la cabeza y la anterior no sean iguales para continuar con la queue
    } while (!__sync_bool_compare_and_swap(index, oldHead, newHead));
    return oldHead; // return la cabeza que se va a usar
}

static void *producer(void *data)
{
    // Setea el id del hilo
    int idThread = *(int *)data;
    printf("[%d] producing\n", idThread);

    // produce un numero fijo de datos y los coloca en la cola
    for (int i = 0; i < CONTADOR_TRABAJO; i++)
    {
        float value = i;

        // Espera a que haya espacio para colocar un dato
        while (1)
        {
            pthread_mutex_lock(&lock_cabeza); /* solicita el acceso exclusivo al buffer*/

            if ((cabeza_cola + 1) % TAMANO_COLA != final_cola) // Verifica si hay espacio en el buffer
                break;

            pthread_mutex_unlock(&lock_cabeza); /* Libera el acceso al buffer*/
            sleep(0);
        }

        cola[cabeza_cola] = value;          /* pone el item en el buffer */
        setQueue(&cabeza_cola);             // setea la cabeza de la queue
        pthread_mutex_unlock(&lock_cabeza); /* Libera el acceso al buffer del ultimo thread*/
    }

    printf("[%d] finished producing\n", idThread);

    return NULL;
}

static void *consumer(void *data)
{
    int idThread = *(int *)data;
    printf("[%d] consuming\n", idThread);

    // Realizar trabajos segun el numero CONTADOR_TRABAJO
    for (int i = 0; i < CONTADOR_TRABAJO; i++)
    {
        while (1) // Esperar a que se agregue un elemento nuevo a la cola
        {
            pthread_mutex_lock(&lock_final); // El hilo hace lock

            if (final_cola != cabeza_cola) // Si hay un elemento en la cola sale del loop y el lock se mantiene
                break;

            pthread_mutex_unlock(&lock_final); // Si no hay elementos libera el lock
            sleep(0);                          // Duerme un poco para volver a intentar
        }

        int index = setQueue(&final_cola); // Para obtener el índice del siguiente elemento en la cola
        float data = cola[index];          // Se guarda el elemento
        pthread_mutex_unlock(&lock_final); // Se libera el lock
    }

    printf("[%d] finished consuming \n", idThread); // Se imprime que ya termino de consumir
    return NULL;
}