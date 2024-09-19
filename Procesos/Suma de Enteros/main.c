#include"suma.h"

int *arreglo = NULL;

int main(int argc, char *argv[]){
    int tamaño;
    FILE *archivo = leerArchivo(argv[1], "r");

    fscanf(archivo, "%d", &tamaño);
    arreglo = espacioArreglo(tamaño);
    copiarArchivoAArreglo(archivo, arreglo, tamaño);
    mostrarArreglo(tamaño, arreglo);
    FILE *archivoSalida = leerArchivo(argv[2], "w");


    pid_t hijos[2];
    int i = 0, status;

    for (i = 0; i < 2; i++){
        hijos[i] = fork();
        if (hijos[i]==0){
            break;
        }
    }

    if (i!=2){
        if (i==0){
            int suma = 0;
            for (int k = 0; k < tamaño/2; k++){
                suma += arreglo[k];
            }
            fprintf(archivoSalida, "%d\n", suma);
        }else{
            int suma = 0;
            for (int l = tamaño/2; l < tamaño; l++){
                suma += arreglo[l];
            }
            fprintf(archivoSalida, "%d\n", suma);
        }
    }else{
        int recolect;
        for (int j = 0; j < 2; j++){
            waitpid(hijos[j], &status, 0);
        }
        fscanf(archivoSalida, "%d", &recolect);
        
        printf("Resultado total hijos %d\n", recolect);  
    }
    
    
    fclose(archivo);
    fclose(archivoSalida);
    liberarEspacioArreglo(tamaño, &arreglo);
    return 0;
}
