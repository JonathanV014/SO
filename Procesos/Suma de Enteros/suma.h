#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

FILE *leerArchivo(char *nombreArchivo, char *modo);
int *espacioArreglo(int n);
void copiarArchivoAArreglo(FILE *nombreArchivo, int *arreglo, int n);
void mostrarArreglo(int n, int *arreglo);
pid_t *espacioArregloPro(int n);
