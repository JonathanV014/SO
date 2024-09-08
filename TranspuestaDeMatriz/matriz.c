#include"Ematriz.h"


int main(int argc, char *argv[]){
    int filas, columnas; //LEER NUMERO DE FILAS Y NUMERO DE COLUMNAS

    int **matriz = NULL; //CREAR DOBLE PUNTERO PARA RESERVAR ESPACIO EN MEMORIA

    FILE *file = leerArchivo(argv[1], "r"); //LEER ARCHIVO

    fscanf(file, "%d%d", &filas, &columnas); //LEER CANTIDAD DE FILAS Y COLUMNAS DEL ARCHIVO
    printf("Cantidad de filas: %d\n", filas); 
    printf("Cantidad de Columnas: %d\n", columnas);


    matriz = espacioMatriz(filas, columnas); //RESERVAR EL ESPACIO PARA LA MATRIZ

    copiarArchivoAMatriz(file, matriz, filas, columnas); //PASAR DATOS DE ARCHIVO A LA MATRIZ

    fclose(file); //CERRAR EL ARCHIVO PORQUE SE TERMINO DE USAR


    printf("Matriz leída normal:\n"); //VERIFICAR SI SE GUARDO CORRECTAMENTE LA MATRIZ DEL ARCHIVO
    mostrarMatriz(filas, columnas, matriz);


    //TRANSPUESTA
    int **matrizTranspuesta = espacioMatriz(columnas, filas);
    copiarMatrizAMatriz(matriz, &matrizTranspuesta, columnas, filas);
    printf("Matriz leída transpuesta:\n"); //VERIFICAR SI SE GUARDO CORRECTAMENTE LA MATRIZ DEL ARCHIVO
    printf("Cantidad de filas: %d\n", columnas); 
    printf("Cantidad de Columnas: %d\n", filas);
    mostrarMatriz(columnas, filas, matrizTranspuesta);
    
    FILE *salidaAr = salidaArchivo(argv[2], "w");
    escribirArchivo(salidaAr, matrizTranspuesta, columnas, filas);
    fclose(salidaAr);

   
    liberarEspacioMatriz(filas, columnas, &matriz);
    liberarEspacioMatriz(columnas, filas, &matrizTranspuesta);
    
    if (matriz == NULL && matrizTranspuesta == NULL){
        printf("Memoria liberada\n");
    }

    return 0;
}