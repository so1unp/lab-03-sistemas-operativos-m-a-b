#define _POSIX_C_SOURCE 200809L // O 200112L, o 199309L. Habilita funciones POSIX
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h> // Para strsignal y memset
#include <unistd.h>
#include <errno.h> // Para perror

void manejador_senial(int signo)
{
    printf("Señal recibida: %s\n", strsignal(signo));
}

int main()
{
    struct sigaction sa;

    // 1. Inicializar la estructura COMPLETAMENTE
    memset(&sa, 0, sizeof(sa));       // Buena práctica: poner todo a cero primero
    sa.sa_handler = manejador_senial; // Asignar tu función manejadora
    sigemptyset(&sa.sa_mask);         // No bloquear ninguna señal adicional mientras se ejecuta el manejador
    sa.sa_flags = 0;                  // Sin flags especiales (podrías usar SA_RESTART si querés que algunas llamadas al sistema se reinicien)

    // 2. Usar sigaction() para registrar el manejador para cada señal
    for (int signo = 1; signo < _NSIG; signo++) // NSIG es más estándar que _NSIG
    {
        // Usamos sigaction() para establecer el manejador 'sa' para la señal 'signo'
        // El tercer argumento (NULL) es para no guardar la configuración anterior.
        if (sigaction(signo, &sa, NULL) == -1)
        {
            printf("No pude capturar: %s\n", strsignal(signo)); // Para idicar las señales que no pude capturar
        }
    }

    // 3. Esperar señales indefinidamente
    while (1)
    {
        pause(); // Pone el proceso a dormir hasta que llegue una señal
    }

    exit(EXIT_SUCCESS);
}