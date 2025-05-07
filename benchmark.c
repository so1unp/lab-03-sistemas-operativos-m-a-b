#define _POSIX_C_SOURCE 200809L
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void test_fork(int count, int wait_flag);
void test_thread(int count, int wait_flag);

void *start();

int main(int argc, char *argv[])
{
    int modo, wait, count;

    // Chequea los  parametros
    if (argc < 3)
    {
        fprintf(stderr, "Uso: %s [-p | -h] [-w] cantidad\n", argv[0]);
        fprintf(stderr, "\t-p           Crear procesos.\n");
        fprintf(stderr, "\t-t           Crear hilos.\n");
        fprintf(stderr, "\t-w           Esperar que proceso o hilo finalice.\n");
        fprintf(stderr, "\tcantidad     Número de procesos o hilos a crear.\n");
        exit(EXIT_FAILURE);
    }

    modo = argv[1][1]; // debe ser p o t

    if (argc == 4)
    {
        wait = argv[2][1];
        if (wait != 'w')
        {
            fprintf(stderr, "Error: opción invalida %s.\n", argv[2]);
            exit(EXIT_FAILURE);
        }
        count = atoi(argv[3]);
    }
    else
    {
        wait = 0;
        count = atoi(argv[2]);
    }

    if (count <= 0)
    {
        fprintf(stderr, "Error: el contador debe ser mayor que cero.\n");
        exit(EXIT_FAILURE);
    }

    if (modo != 'p' && modo != 't')
    {
        fprintf(stderr, "Error: opción invalida %s.\nUsar -p (procesos) o -t (hilos)\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    if (modo == 'p')
    {
        printf("Probando fork()...\n");
        test_fork(count, wait);
    }
    else if (modo == 't')
    {
        printf("Probando pthread_create()...\n");
        test_thread(count, wait);
    }

    exit(EXIT_SUCCESS);
}

//==========================================================================
// Código para la prueba con fork()
//==========================================================================
void test_fork(int count, int wait_flag)
{
    pid_t pid;

    for (int i = 0; i < count; i++)
    {
        pid = fork();
        if (pid == 0) // Proceso hijo
        {
            exit(EXIT_SUCCESS); // Termina el hijo inmediatamente
        }
        else
        {
            // Proceso padre
            if (wait_flag)
            {
                // Esperar que termine el hijo antes de crear el siguiente
                waitpid(pid, NULL, 0);
            }
        }
    }

    // Si no esperamos antes (sin -w), esperamos por todos al final
    if (!wait_flag)
    {
        for (int i = 0; i < count; i++)
        {
            wait(NULL);
        }
    }
}

//==========================================================================
// Código para la prueba con pthread_create()
//==========================================================================

void test_thread(int count, int wait_flag)
{
    pthread_t *threads;
    threads = (pthread_t *)malloc((size_t)count * sizeof(pthread_t));
    pthread_attr_t attr;
    pthread_attr_init(&attr); // Inicializar los atributos

    for (int i = 0; i < count; i++)
    {
        pthread_create(&threads[i], &attr, start, NULL); // Crear el hilo

        // Si wait_flag está activo, esperar por el hilo antes de crear el siguiente
        if (wait_flag)
        {
            pthread_join(threads[i], NULL); // Esperar a que el hilo termine
        }
    }

    // Si wait_flag no está activo, esperar por todos los hilos al final
    if (!wait_flag)
    {
        for (int j = 0; j < count; j++)
        {
            pthread_join(threads[j], NULL);
        }
    }

    // Liberar recursos
    pthread_attr_destroy(&attr);
    free(threads);
}

void *start()
{
    // No hace nada, simplemente retorna
    return NULL;
}