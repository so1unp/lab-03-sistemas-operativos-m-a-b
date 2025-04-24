#define _POSIX_C_SOURCE 200809L // O 200112L, o 199309L. Habilita funciones POSIX
#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h> // times()
#include <signal.h>
#include <string.h> // Para strsignal y memset
#include <unistd.h>
#include <errno.h>        // Para perror
#include <sys/resource.h> // Para getpriority y PRIO_PROCESS

int busywork(void)
{
    struct tms buf;
    for (;;)
    {
        times(&buf);
    }
}

// Manejador de señales para los hijos
void sigchld_handler()
{
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);

    long total_time = usage.ru_utime.tv_sec + usage.ru_stime.tv_sec; // suma de tiempos de usuario y sistema

    printf("Child %d (nice %2d):\t%3li\n", getpid(), getpriority(PRIO_PROCESS, (id_t)getpid()), total_time);
}

int main(int argc, char *argv[])
{

    if (argc < 4)
    {
        fprintf(stderr, "Uso: %s <num_procesos> <segundos> <prioridad>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int num_procesos = atoi(argv[1]);
    pid_t pid[num_procesos];

    // Cantidad de procesos a crear
    for (int i = 0; i < num_procesos; i++)
    {
        // Ejecutable de hijos
        if ((pid[i] = fork()) == 0) // Guardamos el PID directamente en el array
        {
            // Cambiar la prioridad del proceso hijo
            setpriority(PRIO_PROCESS, (id_t)getpid(), atoi(argv[3])); // Cambia la prioridad del proceso hijo

            // Manejar señal kill de los hijos
            struct sigaction sa;

            memset(&sa, 0, sizeof(sa));      // Buena práctica: poner todo a cero primero
            sa.sa_handler = sigchld_handler; // Asignar tu función manejadora
            sigemptyset(&sa.sa_mask);        // No bloquear ninguna señal adicional mientras se ejecuta el manejador
            sa.sa_flags = 0;                 // Sin flags especiales (podrías usar SA_RESTART si querés que algunas llamadas al sistema se reinicien)

            sigaction(SIGTERM, &sa, NULL);

            busywork(); // Hago trabajo pesado
        }
    }

    sleep((unsigned int)atoi(argv[2])); // Espero cantidad des segundos indicada

    // Envio señal a los hijos
    for (int i = 0; i < num_procesos; i++)
    {
        kill(pid[i], SIGTERM); // Mando señal kill a todos los procesos del grupo
    }

    exit(EXIT_SUCCESS);
}