#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#define TAMA 100


void showtree() {
    char cmd[20] = {""};
    sprintf(cmd, "pstree -cAlp %d", getpid()); // Formatea el comando con el PID del proceso actual.
    system(cmd); // Ejecuta el comando en el shell.
}

int main(void){
    
    int nHijos = 2;
    int nTuberias = 8;
    pid_t hijos[nHijos], hijosN2[nHijos], padre = getpid();
    int tuberias[nTuberias][2];

    for (int k = 0; k < nTuberias; k++){
        // int statusP = 
        pipe(tuberias[k]);
        // if (statusP == -1){
        //     printf("No se pudo crear la tuberia\n");
        //     exit(1);
        // }
    }

    int i = 0;
    for (i = 0; i < nHijos; i++){
        hijos[i] = fork();
        if (hijos[i] == 0){
            hijosN2[i] = fork(); 
            break;
        }
    }

    
    if (getpid() == padre){
        showtree();
        int n = 7;
        int leido;
        


        close(tuberias[7][0]);    
        write(tuberias[7][1], &n, sizeof(int));
        close(tuberias[7][1]);

        
        close(tuberias[4][1]);
        read(tuberias[4][0], &leido, sizeof(int));
        close(tuberias[4][0]);
        printf("PID[%d] -> %d\n", getpid(), leido);

        close(tuberias[3][0]);
        write(tuberias[3][1], &leido, sizeof(int));
        close(tuberias[3][1]);

        close(tuberias[0][1]);
        read(tuberias[0][0], &leido, sizeof(int));
        close(tuberias[0][0]);
        printf("PID[%d] -> %d\n", getpid(), leido);
        for (int i = 0; i < nHijos; i++){
            wait(NULL);
        }
        
    }else{
        
        int package;
        if (i == 1){
            if (hijosN2[i] != 0){
                close(tuberias[7][1]);
                read(tuberias[7][0], &package, sizeof(int));
                close(tuberias[7][0]);
                printf("PID[%d] -> %d\n", getpid(), package);

                close(tuberias[6][0]);
                write(tuberias[6][1], &package, sizeof(int));
                close(tuberias[6][1]);

                close(tuberias[5][1]);
                read(tuberias[5][0], &package, sizeof(int));
                close(tuberias[5][0]);
                printf("PID[%d] -> %d\n", getpid(), package);

                close(tuberias[4][0]);
                write(tuberias[4][1], &package, sizeof(int));
                close(tuberias[4][1]);

            }else{
                close(tuberias[6][1]);
                read(tuberias[6][0], &package, sizeof(int));
                close(tuberias[6][0]);
                printf("PID[%d] -> %d\n", getpid(), package);

                close(tuberias[5][0]);
                write(tuberias[5][1], &package, sizeof(int));
                close(tuberias[5][1]);
            }
        }else{
            if (hijosN2[i] != 0){
                close(tuberias[3][1]);
                read(tuberias[3][0], &package, sizeof(int));
                close(tuberias[3][0]);
                printf("PID[%d] -> %d\n", getpid(), package);

                close(tuberias[2][0]);
                write(tuberias[2][1], &package, sizeof(int));
                close(tuberias[2][1]);

                close(tuberias[1][1]);
                read(tuberias[1][0], &package, sizeof(int));
                close(tuberias[1][0]);
                printf("PID[%d] -> %d\n", getpid(), package);

                close(tuberias[0][0]);
                write(tuberias[0][1], &package, sizeof(int));
                close(tuberias[0][1]);

            }else{
                close(tuberias[2][1]);
                read(tuberias[2][0], &package, sizeof(int));
                close(tuberias[2][0]);
                printf("PID[%d] -> %d\n", getpid(), package);

                close(tuberias[1][0]);
                write(tuberias[1][1], &package, sizeof(int));
                close(tuberias[1][1]);
            }
        }
    }

    

    return 0;
}