#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/shm.h>
#include <sys/stat.h>

int main(int argc, char *argv[]){
    int filas = 6;
    int columnas = 6;
    int vecIdColumnas[filas];
    
    //ESPACIO DE MEMORIA COMPARTIDA PARA QUE CADA HIJO ESCRIBA SU RESULTADO DE LA SUMA
    int **matriz;
    int shmTamMatrizF = (filas) * sizeof(int *); 
    int shmTamMatrizC = (columnas) * sizeof(int);
    int idMatriz = shmget(IPC_PRIVATE, shmTamMatrizF, IPC_CREAT | S_IRUSR | S_IWUSR); //CLAVE PA CONECTARSE
    matriz = (int **) shmat(idMatriz, 0, 0);

    for (int i = 0; i < filas; i++){
        vecIdColumnas[i] = shmget(IPC_PRIVATE, shmTamMatrizC, IPC_CREAT | S_IRUSR | S_IWUSR);
        matriz[i] = (int *) shmat(vecIdColumnas[i], 0, 0);
    }
        

    

    if (fork()){
        wait(NULL);

        for (int i = 0; i < filas; i++){
            for (int j = 0; j < columnas; j++){
                printf("[%d] ", matriz[i][j]);
            }
            printf("\n");
        }

        for (int i = 0; i < filas; i++){
            shmdt(matriz[i]);
            shmctl(vecIdColumnas[i], IPC_RMID, 0);
        }
        shmdt(matriz);
        shmctl(idMatriz, IPC_RMID, 0);

        
    }else{
        for (int i = 0; i < filas; i++){
            for (int j = 0; j < columnas; j++){
                matriz[i][j] = 1;
            }
        }

        for (int i = 0; i < filas; i++){
            shmdt(matriz[i]);
        }
        shmdt(matriz);
    }

    
    




    return EXIT_SUCCESS;
}