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
    int i = 0;
    pid_t padre = getpid();


    int nHijos;
    printf("Enter the number of child processes: "); scanf("%d", &nHijos);getchar();
    int nPipes = nHijos + 1;

    pid_t *hijosD = NULL;
    hijosD = (pid_t *) malloc(nHijos*sizeof(pid_t));
    if (hijosD == NULL){
        printf("No memory allocated\n");
        exit(1);
    }

    int **pipesD = NULL;
    pipesD = (int **)malloc(nPipes * sizeof(int *));
    if (pipesD == NULL){
        printf("No memory allocated\n");
        exit(1);
    }else{   
        for (int k = 0; k < nPipes; k++){
            pipesD[k] = (int *) malloc(2 * sizeof(int));
            if (pipesD[k] == NULL){
                printf("No memory allocated\n");
                exit(1);
            }  
        }
    }

    
    for (int j = 0; j < nPipes; j++){
        pipe(pipesD[j]);
    }


    for (i = 0; i < nHijos; i++){
        hijosD[i] = fork();
        if (hijosD[i] == 0){
            break;
        }
    }


    int status;
    char ms[TAMA], llega[TAMA], resultado[TAMA];
    
    if (padre == getpid()){
        sleep(1);
        showtree();
        close(pipesD[i-i][1]);
        close(pipesD[i][0]);
        
        printf("Enter the message you wish to send: ");
        
        fgets(ms, TAMA, stdin);
        size_t length = strlen(ms);
        if (ms[length - 1] == '\n') {
            ms[length - 1] = '\0';
        }

        printf("SEND: %s | SHIPPING PIPE: %d | ROOT PROCESS PID(START): %d | DESTINATION PID: %d\n", ms, i, getpid(), hijosD[i-1]);
        printf("--------------------------------------------------------------------------------------------------\n");
        write(pipesD[i][1], &ms, sizeof(char[TAMA]));
        close(pipesD[i][1]);

        read(pipesD[i-i][0], &llega, sizeof(char[TAMA]));
        close(pipesD[i-i][0]);
        printf("--------------------------------------------------------------------------------------------------\n");
        printf("RECEIVED DATA: %s | PIPE: %d | ROOT PROCESS PID: %d\n", llega, i-i, getpid());
    }else{
        close(pipesD[i+1][1]);
        close(pipesD[i][0]);

        read(pipesD[i+1][0], &resultado, sizeof(char[TAMA]));
        close(pipesD[i+1][0]);

        if (i>0){
            printf("RECEIVED DATA: %s | SHIPPING PIPE: %d | START PID: %d | DESTINATION PID: %d\n", resultado, i, getpid(), hijosD[i-1]);
        }else{
            printf("RECEIVED DATA: %s | SHIPPING PIPE: %d | START PID: %d | DESTINATION PID: %d\n", resultado, i, getpid(), getppid());
        }
        

        write(pipesD[i][1], &resultado, sizeof(char[TAMA]));
        close(pipesD[i][1]);
    }

    free(hijosD);
    hijosD = NULL;
    for (int i = 0; i < nPipes; i++){
        free(pipesD[i]);
        pipesD[i] = NULL;
    }
    free(pipesD);
    pipesD = NULL;
    
    return 0;
}
