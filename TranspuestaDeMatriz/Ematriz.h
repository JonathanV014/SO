#include<stdio.h>
#include<stdlib.h>

FILE *leerArchivo(char *, char*);
int **espacioMatriz(int, int);
void copiarArchivoAMatriz(FILE *, int**, int, int);
void mostrarMatriz(int, int, int**);
void liberarEspacioMatriz(int, int, int ***);
void calcularTranspuesta(int **, int ***, int, int);
FILE *salidaArchivo(char *, char*);
void escribirArchivo(FILE *, int **, int, int);