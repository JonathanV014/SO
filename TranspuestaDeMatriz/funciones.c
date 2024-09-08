#include "Ematriz.h"


void escribirArchivo(FILE *archivo, int **matriz, int filas, int columnas){
    fprintf(archivo, "%d %d\n", filas, columnas); 

    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            fprintf(archivo, "%d ", matriz[i][j]);
        }
        fprintf(archivo, "\n");
    }
}

FILE *salidaArchivo(char *nombre, char *modo){
    FILE *outfile = fopen(nombre, modo); 
    if (outfile == NULL) {
        printf("No se pudo abrir el archivo para escribir\n");
        exit(1);
    }

    return outfile;
}

void copiarMatrizAMatriz(int **matrizA, int ***matrizAT, int filas, int columnas){
    for (int i = 0; i < filas; i++){
        for (int j = 0; j < columnas; j++){
            (*matrizAT)[i][j] = matrizA[j][i];
        }
    }
}

void liberarEspacioMatriz(int filas, int columnas, int ***matriz){
    for (int i = 0; i < filas; i++) { //LIBERAR EL ESPACIO DE CADA COLUMNA
        free((*matriz)[i]); 
        (*matriz)[i] = NULL; 
    }

    free(*matriz); 
    *matriz = NULL; 
}

void mostrarMatriz(int filas, int columnas, int **matriz){
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            printf("%d ", matriz[i][j]);
        }
        printf("\n");
    }
}

FILE *leerArchivo(char *nombre, char *modo){
    FILE *file = NULL;
    file = fopen(nombre, modo); //ABRIR ARCHIVO

    if (file == NULL) { //MANEJO DE ERROR SI NO SE ENCUENTRA
        printf("No se pudo abrir archivo\n");
        exit(1);
    }
    return file;
}


int **espacioMatriz(int filas, int columnas){
    int **matriz = NULL;
    
    matriz = (int **)malloc(filas * sizeof(int *)); //RESERVAR ESPACIO PARA LAS FILAS
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

void copiarArchivoAMatriz(FILE *archivo, int**matriz, int filas, int columnas){
    for (int i = 0; i < filas; i++) { //EXTRAER DATOS DEL ARCHIVO Y UBICARLOS DENTRO DE LA MATRIZ
        for (int j = 0; j < columnas; j++) {
            fscanf(archivo, "%d", &matriz[i][j]);
        }
    }
}