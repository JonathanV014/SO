#include<sys/types.h>
#include <sys/wait.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

/* 
Procesos con la siguiente estructura:
    |--p2  /|p5
P1--|--p3---|p6
    |--p4
*/

pid_t hijosPadrePrincipal[3];

int main(int argc, char const *argv[]){
    int i = 0, status;
    for (i = 0; i < 3; i++){
        hijosPadrePrincipal[i] = fork();
        if (hijosPadrePrincipal[i] == 0){
            if (i == 1){
                pid_t h2 = fork();
                if (h2 == 0){
                    
                    pid_t hH2[2];
                    int j = 0;
                    for (j = 0; j < 2; j++){
                        hH2[j] = fork();
                        if (hH2[j] == 0){
                            break;
                        } 
                    }
                    if (j == 2) {
                        for (int i = 0; i < 2; i++){
                            waitpid(hH2[i], &status, 0);
                            printf("\t|\t\t|--Soy el hijo (%d y mi padre es %d)\n", hH2[i], getpid());
                        }
                        printf("\t|\t|--Soy el padre (%d y mi padre es %d)\n", getpid(), getppid());
                    }
                    
                }else{
                    waitpid(h2, &status, 0);
                    printf("\t|--Soy el padre (%d y mi padre es %d)\n", getpid(), getppid());
                }
            }
            break;
        }else{
            if (i!=1)
            {
                printf("\t|--Soy el hijo (%d y mi padre es %d)\n", hijosPadrePrincipal[i], getpid());
            }
            waitpid(hijosPadrePrincipal[i], &status, 0);
        }
    }
    
    if (i==3){
        printf("Soy el padre principal(%d)\n", getpid());
    }

    return 0;
}
