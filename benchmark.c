#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void test_fork(int count, int wait);
void test_thread(int count, int wait);

int main(int argc, char *argv[]) 
{
    int modo, wait, count;
    
    // Chequea los  parametros
    if (argc < 3) {
        fprintf(stderr, "Uso: %s [-p | -h] [-w] cantidad\n", argv[0]);
        fprintf(stderr, "\t-p           Crear procesos.\n");
        fprintf(stderr, "\t-t           Crear hilos.\n");
        fprintf(stderr, "\t-w           Esperar que proceso o hilo finalice.\n");
        fprintf(stderr, "\tcantidad     Número de procesos o hilos a crear.\n");
        exit(EXIT_FAILURE);
    }
    
    modo = argv[1][1];  // debe ser p o t

    if (argc == 4) { 
        wait = argv[2][1];
        if (wait != 'w') {
            fprintf(stderr, "Error: opción invalida %s.\n", argv[2]);
            exit(EXIT_FAILURE);
        }
        count = atoi(argv[3]);
    } else {
        wait = 0;
        count = atoi(argv[2]);
    }

    
    if (count <= 0) {
        fprintf(stderr, "Error: el contador debe ser mayor que cero.\n");
        exit(EXIT_FAILURE);
    }

    if (modo != 'p' && modo != 't') {
        fprintf(stderr, "Error: opción invalida %s.\nUsar -p (procesos) o -t (hilos)\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    
    if (modo == 'p') {
        printf("Probando fork()...\n");
        test_fork(count, wait);
    } else if (modo == 't') {
        printf("Probando pthread_create()...\n");
        test_thread(count, wait);
    }

    exit(EXIT_SUCCESS);
}

//==========================================================================
// Código para la prueba con fork()
//==========================================================================
void test_fork(int count, int wait1)
{
    pid_t pids[count];
    for (int i = 0; i < count; i++)
    {
        if (wait1 != 0)
        {
            pids[i] = fork();
        }
        wait(0);
    }
    



}

//==========================================================================
// Código para la prueba con pthread_create()
//==========================================================================

void test_thread(int count, int wait) 
{
    pthread_t *threads;
    threads = (pthread_t*) malloc(count * sizeof(pthread_t));
    pthread_attr_t attr;


    for(int i = 0; i < count; i++) {
        int thread = pthread_create(&threads[i], &attr, NULL, (void *) i);
        if(thread != 0) {
             fprintf(stderr, "Error: No se pudo crear el hilo.\n");
             exit(EXIT_FAILURE);
        }
    }

    
    for(int j = 0; j < count; j++) {
        pthread_join(threads[j], NULL);
    }

    pthread_exit(NULL);

}
