#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#define TAMA 100



void showtree() {
    char cmd[20] = {""};
    sprintf(cmd, "pstree -cAlp %d", getpid()); // Formatea el comando con el PID del proceso actual.
    system(cmd); // Ejecuta el comando en el shell.
}

int main(int argc, char const *argv[]){
    int nHijos = 2;
    int nTuberias = 8;

    pid_t hijos[nHijos], hijosN2[nHijos], padre = getpid();
    int tuberias[nTuberias][2];

    for (int i = 0; i < nTuberias; i++){
        int statusP = pipe(tuberias[i]);
        if (statusP == -1){
            printf("No se pudo crear la tuberia\n");
            exit(1);
        }
    }

    int i = 0;
    for (int i = 0; i < nHijos; i++){
        hijos[i] = fork();
        if (hijos[i] == 0){
            hijosN2[i] = fork();      
            break;
        }
    }

    if (getpid() == padre){

        close(tuberias[7][0]);
        close(tuberias[0][1]);

        int n = 7;
        write(tuberias[7][1], &n, sizeof(int));

        close(tuberias[7][1]);

        int leido;
        read(tuberias[0][0], &leido, sizeof(int));
        printf("Leido %d\n", leido);

    }else{
        
    }


    return 0;
}
