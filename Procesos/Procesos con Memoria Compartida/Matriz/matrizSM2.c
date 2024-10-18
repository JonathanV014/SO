#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/shm.h>
#include <sys/stat.h>

//FUNCION PARA CALCULAR EL TAMANO EN BYTES E LA MATRIZ
unsigned int espacioMatriz(int filas, int columnas, size_t tamTipoDato){
    size_t tam;
    tam  = filas * sizeof(void *);
    tam += (columnas * filas * tamTipoDato);
    return tam;
}

//FUNCION PARA APUNTAR A LOS PUNTEROS SU POSICION CORRESPONDIENTE
void configurarPunteros(void **m, int filas, int columnas, size_t tamTipoDato){
    int i;
    size_t saltosFila = columnas * tamTipoDato;
    m[0] = m + filas;
    for (i = 1; i < filas; i++){
        m[i] = (m[i-1] + saltosFila);
    }
}

int main(int argc, char const *argv[])
{
    int filas = 8, columnas = 5;
    int espacioMat = espacioMatriz(filas, columnas, sizeof(int));

    int **matriz = NULL;
    int idMatriz = shmget(IPC_PRIVATE, espacioMat, IPC_CREAT | S_IRUSR | S_IWUSR);
    matriz = shmat(idMatriz, 0, 0);
    configurarPunteros((void *) matriz, filas, columnas, sizeof(int));

    for (int i = 0; i < filas; i++){
        for (int j = 0; j < columnas; j++){
            matriz[i][j] = 1;
        }
    }

    for (int i = 0; i < filas; i++){
        for (int j = 0; j < columnas; j++){
            printf("[%d] ", matriz[i][j]);
        }
        printf("\n");
    }

    shmdt(matriz);
    shmctl(idMatriz, IPC_RMID, 0);

    return 0;
}
