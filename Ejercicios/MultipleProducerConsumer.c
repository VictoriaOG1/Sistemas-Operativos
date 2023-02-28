#include <pthread.h>
#include <stdio.h>

#define CONTADOR_PRODUCTOR 4  //Numero de hilos productores
#define CONTADOR_CONSUMIDOR 4 //Numero de hilos consumiedores 
#define CONTADOR_THREAD (CONTADOR_PRODUCTOR + CONTADOR_CONSUMIDOR) //Numero total de hilos
#define CONTADOR_TRABAJO 1000 //Cantidad de trabajo

#define TAMANO_COLA 64 //Tamano de la cola 

//Mutex para la cabeza de la cola
static pthread_mutex_t lock_cabeza = PTHREAD_MUTEX_INITIALIZER; 
//Posicion donde esta la cabeza de la cola
static volatile int cabeza_cola = 1;

//Mutex para el final de la cola
static pthread_mutex_t lock_final = PTHREAD_MUTEX_INITIALIZER;
//Posicion donde esta el final de la cola 
static volatile int final_cola = 0;

static float cola[TAMANO_COLA]; //Se crea la cola como array

//Declaracion de las funciones productor y consumidor 
static void *producer(void *data);
static void *consumer(void *data);

int main()
{
    pthread_t hilos[CONTADOR_THREAD]; //Array hilos 
    int array_id[CONTADOR_THREAD]; //Array para guardar los id de los hilos
    int id_hilo; //Para los loops

    //Creacion de hilos productores y se almacenan sus ids
    for(id_hilo = 0; id_hilo < CONTADOR_PRODUCTOR; id_hilo++)
    {
        array_id[id_hilo] = id_hilo;
        pthread_create(&hilos[id_hilo], NULL, producer, &array_id[id_hilo]);
    }

    //Creacion de hilos consumidores y se almacenan sus ids
    for(; id_hilo < CONTADOR_THREAD; id_hilo++)
    {
        array_id[id_hilo] = id_hilo;
        pthread_create(&hilos[id_hilo], NULL, consumer, &array_id[id_hilo]);
    }

    for(int i = 0; i < CONTADOR_THREAD; i++)
    {
        pthread_join(hilos[i], NULL);
    }

    return 0;
}

static inline int advance(volatile int *idx)
{
    int old, new;
    do
    {
        old = *idx;
        new = (old + 1) % TAMANO_COLA;
    } while(!__sync_bool_compare_and_swap(idx, old, new));
    return old;
}

static void *producer(void *data)
{
    int thread_id = *(int *)data;
    printf("[%d] producing\n", thread_id);

    for(int i = 0; i < CONTADOR_TRABAJO; i++)
    {
        float value = i;

        while(1)
        {
            pthread_mutex_lock(&lock_cabeza);

            if((cabeza_cola + 1) % TAMANO_COLA != final_cola)
                break;

            pthread_mutex_unlock(&lock_cabeza);
            sleep(0);
        }

        cola[cabeza_cola] = value;
        advance(&cabeza_cola);
        pthread_mutex_unlock(&lock_cabeza);
    }

    printf("[%d] finished producing\n", thread_id);

    return NULL;
}

static void *consumer(void *data)
{
    int thread_id = *(int *)data;
    printf("[%d] consuming\n", thread_id);

    // instead of poison pill let's just consume exactly what is produced.
    for(int i = 0; i < CONTADOR_TRABAJO; i++)
    {
        while(1)
        {
            pthread_mutex_lock(&lock_final);

            if(final_cola != cabeza_cola)
                break;

            pthread_mutex_unlock(&lock_final);
            sleep(0);
        }

        int idx = advance(&final_cola);
        float data = cola[idx];
        pthread_mutex_unlock(&lock_final);
    }

    printf("[%d] finished consuming \n", thread_id);
    return NULL;
}