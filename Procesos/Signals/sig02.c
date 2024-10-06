#include <stdio.h>    // Para funciones de entrada/salida como printf
#include <stdlib.h>   // Para EXIT_SUCCESS y EXIT_FAILURE
#include <unistd.h>   // Para fork(), getpid(), usleep() y pause()
#include <sys/wait.h> // Para wait()
#include <signal.h>   // Para signal(), SIGUSR1 y kill()

int nVeces = 0;
void sighandler(int sig);

void showTree(){
    char b[500];
    sprintf(b, "pstree -lp %d", getpid());
    system(b);
}

int main() {
    pid_t pidHijos[3];
    int i=0, status;
    int n;

    void (*oldhandler)(int);
    oldhandler = signal(SIGUSR1, sighandler);
    if (oldhandler == SIG_ERR) {
        perror("signal:");
        exit(EXIT_FAILURE);
    }

    printf("Digite el numero de ciclos: ");
    scanf("%d", &n);
    
    for (i = 0; i < 3; i++){
        pidHijos[i] = fork();
        if (pidHijos[i] == 0){
            break;
        }
    }

    while (nVeces < n){
            
        if(i == 3){
            printf("----------------------------------------------------------- ciclo%d\n", nVeces+1);
            printf("Padre [%d]\n", getpid());
            
            usleep(100);
            kill(pidHijos[i-1], SIGUSR1);

            pause();
        }else{
            pause();
            pid_t h21 = -1;
            if (i > 0){
                printf("Hijo%d [%d]\n", i+1, getpid());
                if (i == 1){
         
                    h21 = fork();
                    if (h21 == 0){
                        pause();
                        printf("Hijo21 [%d]\n", getpid());
                        kill(getppid(), SIGUSR1);
                    }else{
                        usleep(100);
                        kill(h21, SIGUSR1);
                        pause();
                        printf("Hijo%d [%d]\n", i+1, getpid());
                        h21 = -1;
                    }
                }
                
                if (h21 == -1){
                    kill(pidHijos[i-1], SIGUSR1);
                }
                

            }else{
                printf("Hijo%d [%d]\n", i+1, getpid());
                kill(getppid(), SIGUSR1);
            }
        }
    
        nVeces++;
    }
    
        
    if (signal(SIGUSR1, oldhandler) == SIG_ERR) {
        perror("signal:");
        exit(EXIT_FAILURE);
    }
    
    return EXIT_SUCCESS; // Indica que el programa terminó exitosamente
}

void sighandler(int sig) {}