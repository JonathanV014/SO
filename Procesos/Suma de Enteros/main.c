#include"suma.h"
int *arreglo = NULL;
pid_t *hijos = NULL;

int main(int argc, char *argv[]){
    int tamaño, nProcess;

    FILE *archivo = leerArchivo(argv[1], "r");
    fscanf(archivo, "%d", &tamaño);
    arreglo = espacioArreglo(tamaño);
    copiarArchivoAArreglo(archivo, arreglo, tamaño);
    printf("Size of array: %d\n", tamaño);

    printf("Enter number of process (Nprocesses > 0 and Nprocesses <= %d): ",tamaño);
    scanf("%d", &nProcess);
    printf("-----------------------------------------------\n");
    if (nProcess < 0 || nProcess > tamaño){
        printf("invalid process number\n");
        exit(1);
    }
    hijos = espacioArregloPro(nProcess);
    int tam = (int)tamaño/nProcess;
    int rest = tamaño - (nProcess*tam);
    printf("Tam %d, Restante %d\n", tam, rest);

    //mostrarArreglo(tamaño, arreglo);
    FILE *archivoSalida = leerArchivo(argv[2], "w");
    
    int i = 0, status;

    for (i = 0; i < nProcess; i++){
        hijos[i] = fork();
        if (hijos[i]==0){
            break;
        }
    }

    if (i != nProcess){
        printf("Start sum CHILD with pid[%d]\n", getpid());
        long int suma = 0;
        int inicio = (i*(tam));
        int fin = ((i+1)*tam);
        if (i == nProcess-1){
            for (int k = inicio; k < fin+rest; k++){
                suma += arreglo[k];
            }
        }else{
            for (int k = inicio; k < fin; k++){
                suma += arreglo[k];
            }
        }
        
        fprintf(archivoSalida, "%ld\n", suma);

    }else{
        FILE *archivoS = leerArchivo(argv[2], "r");
        long int sum = 0, recolect;
        for (int j = 0; j < nProcess; j++){
            waitpid(hijos[j], &status, 0);
            fscanf(archivoS, "%ld", &recolect);
            printf("Result CHILD with pid[%d]: %ld\n", hijos[j], recolect);
            sum += recolect;
        }
        printf("-----------------------------------------------\n");
        printf("Result FATHER pid[%d] of the sum of the results of children: %ld\n", getpid(),sum);  
        fclose(archivoS);
    }
    

    fclose(archivo);
    fclose(archivoSalida);
    free(arreglo);
    arreglo = NULL;
    free(hijos);
    hijos = NULL;

    return 0;
}
