#include "multiplicaciones.h"

int totalElementosA(int i, int cf, int cc){
    int total = 0;
    switch (i)
    {
    case 0:
        for (int i = 0; i < cf; i++){
            for (int j = 0; j < cc; j++){
                if (i > j){
                    total++;
                }
            }
        }
        break;
    case 1:
        for (int i = 0; i < cf; i++){
            for (int j = 0; j < cc; j++){
                if (i == j){
                    total++;
                }
            }
        }
        break;
    case 2:
        for (int i = 0; i < cf; i++){
            for (int j = 0; j < cc; j++){
                if (i < j){
                    total++;
                }
            }
        }
        break;
    default:
        break;
    }
    return total;
}

int **reservarEspacioMatriz(int filas, int columnas){
    int **matriz = NULL;
    matriz = (int **)malloc(filas * sizeof(int *));
    if (matriz == NULL) { //MANEJAR ERROR DE RESERVA
        printf("ERROR al asignar memoria\n");
        exit(1);
    }

    for (int i = 0; i < filas; i++) { //RESERVAR EL ESPACIO PARA LAS COLUMNAS EN CADA FILA
        matriz[i] = (int *)malloc(columnas * sizeof(int));
        if (matriz[i] == NULL) {
            printf("ERROR al asignar memoria\n");
            exit(1);
        }
    }

    return matriz;
}

int validarMultiplicacion(int ca, int fb){
    int resultado = 0;
    if (ca == fb){
        resultado = 1;
    } 
    return resultado;
}


void generarMatriz(int ***matriz, int filas, int columnas){
    for (int i = 0; i <filas; i++){
        for (int j = 0; j < columnas; j++){
            (*matriz)[i][j] = 1;
        }
    }
}

void mostrarMatriz(int **matriz, int filas, int columnas){
    for (int i = 0; i <filas; i++){
        for (int j = 0; j < columnas; j++){
            printf("[%d] ",matriz[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}