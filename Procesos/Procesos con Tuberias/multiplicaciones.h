#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#define SIZE 512



void generarMatriz(int ***matriz, int filas, int columnas);
int validarMultiplicacion(int ca, int fb);
int **reservarEspacioMatriz(int filas, int columnas);
void mostrarMatriz(int **matriz, int filas, int columnas);
int totalElementosA(int i, int cf, int cc);

struct Result{
    int x,y,value;
    pid_t pid;
};
