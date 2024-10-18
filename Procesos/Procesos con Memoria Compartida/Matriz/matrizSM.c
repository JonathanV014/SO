#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/shm.h>
#include <sys/stat.h>

int main(int argc, char *argv[]){
    int filas; printf("Filas: "); scanf("%d", &filas);
    int columnas; printf("Columnas: "); scanf("%d", &columnas);
    
    //VECTOR EN MEMORIA COMPARTIDA PARA GUARDAR LOS ID DE LOS ESPACIOS PARA CADA COLUMNA
    int *vecIdColumnas = NULL;
    int idVecIdColumnas = shmget(IPC_PRIVATE, filas * sizeof(int), IPC_CREAT | S_IRUSR | S_IWUSR);
    vecIdColumnas = shmat(idVecIdColumnas, 0, 0);

    //SE RESERVA ESPACIO PARA LA MATRIZ, EN CONCRETO ESPACIO PARA CADA FILA
    int **matriz;
    int shmTamMatrizF = (filas) * sizeof(int *); 
    int shmTamMatrizC = (columnas) * sizeof(int);
    int idMatriz = shmget(IPC_PRIVATE, shmTamMatrizF, IPC_CREAT | S_IRUSR | S_IWUSR); //CLAVE PA CONECTARSE
    matriz = (int **) shmat(idMatriz, 0, 0);

    //RESEVAR ESPACIO PARA CADA COLUMNA GUARDANDO EL ID DEL ESPACIO DE MEMORIA DE CADA COLUMNA
    for (int i = 0; i < filas; i++){
        vecIdColumnas[i] = shmget(IPC_PRIVATE, shmTamMatrizC, IPC_CREAT | S_IRUSR | S_IWUSR);
        matriz[i] = (int *) shmat(vecIdColumnas[i], 0, 0);
    }

    if (fork()){
        //ESPERAR A QUE EL HIJO LLENE LA MATRIZ
        wait(NULL);

        //MOSTRAR EN PANTALA LOS DATOS DE LA MATRIZ
        for (int i = 0; i < filas; i++){
            for (int j = 0; j < columnas; j++){
                printf("[%d] ", matriz[i][j]);
            }
            printf("\n");
        }

        //SEPARAR AL PADRE DEL ESPACIO DE MEMORIA PARA CADA COLUMNA DENTRO DE LA MATRIZ Y ELIMINANDO LA MEMORIA
        for (int i = 0; i < filas; i++){
            shmdt(matriz[i]);
            shmctl(vecIdColumnas[i], IPC_RMID, 0);
        }

        //SEPARAR AL PADRE DEL ESPACIO DE MEMORIA PARA LA MATRIZ Y ELIMINAR LA MEMORIA PARA LA MATRIZ
        shmdt(matriz);
        shmctl(idMatriz, IPC_RMID, 0);

        //SEPARAR AL PADRE DEL ESPACIO DE MEMPORIA PARA EL VECTOR DE ID DE MEMORIA DE LA COLUMNA Y ELIMINAR LA MEMORIA
        shmdt(vecIdColumnas);
        shmctl(idVecIdColumnas, IPC_RMID, 0);
    }else{

        //LLENAR DE DATOS LA MATRIZ CON EL HIJO
        for (int i = 0; i < filas; i++){
            for (int j = 0; j < columnas; j++){
                matriz[i][j] = 1;
            }
        }

        //SEPARAR AL HIJO DE LOS ESPACIOS DE MEMORIA PARA CADA COLUMNA
        for (int i = 0; i < filas; i++){
            shmdt(matriz[i]);
        }

        //SEPARAR AL HIJO DEL ESPACIO DE MEMORIA DE LA MATRIZ Y DEL VECTOR DE ID DE MEMORIA PARA CADA COLUMNA
        shmdt(matriz);
        shmdt(vecIdColumnas);
    }

    
    




    return EXIT_SUCCESS;
}