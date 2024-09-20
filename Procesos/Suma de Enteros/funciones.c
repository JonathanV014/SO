#include"suma.h"

FILE *leerArchivo(char *nombreArchivo, char *modo){
    FILE *file = NULL;
    file = fopen(nombreArchivo, modo); //ABRIR ARCHIVO

    if (file == NULL) { //MANEJO DE ERROR SI NO SE ENCUENTRA
        printf("No se pudo abrir archivo\n");
        exit(1);
    }
    return file;
}

int *espacioArreglo(int n){
    int *array = NULL;
    array =(int *)malloc(n * sizeof(int));
    if (array == NULL){
        printf("No se reservó memoria\n");
        exit(1);
    }
    return array;
}

pid_t *espacioArregloPro(int n){
    pid_t *array = NULL;
    array =(pid_t *)malloc(n * sizeof(pid_t));
    if (array == NULL){
        printf("No se reservó memoria\n");
        exit(1);
    }
    return array;
}

void copiarArchivoAArreglo(FILE *nombreArchivo, int *arreglo, int n){
    for (int i = 0; i < n; i++){
        fscanf(nombreArchivo, "%d", &arreglo[i]);
    }
}

void mostrarArreglo(int n, int *arreglo){
    int limit = 12, itera = 0;
    for (int i = 0; i < n; i++){
        printf("[%ld], ", arreglo[i]);
        itera++;
        if (limit == itera){
            itera = 0;
            printf("\n");
        }
    } 
    printf("\n");
}


