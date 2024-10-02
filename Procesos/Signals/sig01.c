#include <stdio.h>    // Para funciones de entrada/salida como printf
#include <stdlib.h>   // Para EXIT_SUCCESS y EXIT_FAILURE
#include <unistd.h>   // Para fork(), getpid(), usleep() y pause()
#include <sys/wait.h> // Para wait()
#include <signal.h>   // Para signal(), SIGUSR1 y kill()

// Declaración del manejador de señales
void sighandler(int sig);

int main() {
    pid_t *pidhijos = NULL;
    int nHijos, i=0, status;

    void (*oldhandler)(int);
    oldhandler = signal(SIGUSR1, sighandler);
    if (oldhandler == SIG_ERR) {
        perror("signal:");
        exit(EXIT_FAILURE);
    }


    printf("Digite el numero procesos hijos: ");
    scanf("%d", &nHijos);

    pidhijos = (pid_t *) malloc(nHijos * sizeof(pid_t));
    if (pidhijos == NULL){
        printf("No se dio memoria\n");
        exit(1);
    }
    
    for (i = 0; i < nHijos; i++){
        pidhijos[i] = fork();
        if (pidhijos[i] == 0){
            
            break;
        }
    }
    
    if(i == nHijos){
        printf("Soy PADRE PID(%d) envio a hijo PID(%d) Señal %d\n", getpid(), pidhijos[i-1], SIGUSR1);
        usleep(100);
        kill(pidhijos[i-1], SIGUSR1);
        pause();
        printf("Señal %d recibida - soy PADRE PID(%d)\n", SIGUSR1, pidhijos[0], getpid());

    }else{
        pause();
        if (i>0){
            printf("H%d | Señal %d recibida - Soy hijo PID(%d) envio a hermano PID(%d) Señal %d\n", i+1, SIGUSR1, getpid(), pidhijos[i-1], SIGUSR1);
            kill(pidhijos[i-1], SIGUSR1);
        }else{
            printf("H%d | Señal %d recibida - Soy hijo PID(%d) envio a PADRE PID(%d) Señal %d\n", i+1, SIGUSR1, getpid(), getppid(), SIGUSR1);
            kill(getppid(), SIGUSR1);
        }
    }
    
        
    if (signal(SIGUSR1, oldhandler) == SIG_ERR) {
        perror("signal:");
        exit(EXIT_FAILURE);
    }


    return EXIT_SUCCESS; // Indica que el programa terminó exitosamente
}

void sighandler(int sig) {}