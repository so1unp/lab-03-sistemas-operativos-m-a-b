#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void manejador_senial(int signo)
{
    printf("Señal recibida: %s\n", strsignal(signo));
}

int main()
{
    // modificar el manejador de las señales
    for (int signo = 0; signo < _NSIG; signo++)
    {
        sigaction(signo, manejador_senial, NULL);
    }

    // esperar señales
    while (1)
    {
        pause();
    }

    exit(EXIT_SUCCESS);
}
