#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/shm.h>
#include <sys/stat.h>

double calcularNine(int **matrizIMG, int filas, int columnas, int l, int j, int factor){
    int inicioFila = l - factor;
    int inicioColumna = j - factor;
    int finFila = l + factor;
    int finColumna = j + factor;

    double resultado = 0;
    for (int i = inicioFila; i <= finFila; i++){
        for (int k = inicioColumna; k <= finColumna; k++){
            if ((i >-1 && i < filas) && (k > -1 && k < columnas)){
                resultado +=  matrizIMG[i][k];             
            }else{
                resultado += 0;
            }
        }
    }
    return (resultado/((factor+factor+1)*(factor+factor+1)));
}

int main(int argc, char const *argv[]){
    FILE *img = NULL;
    img = fopen(argv[1], "r");
    if (img == NULL){
        printf("No se pudo abrir la imagen %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    int filas, columnas;
    fscanf(img, "%d \n %d", &filas, &columnas);
    printf("Filas %d, Columnas %d\n", filas, columnas);


    int nHijos;
    printf("Digite el numero de procesos hijos (NProcesos > 0 y NProcesos <= %d): ", filas);scanf("%d", &nHijos);
    if (nHijos < 0 || nHijos > filas ){
        printf("Numero de hijos no valido");
        exit(EXIT_FAILURE);
    }
    
    size_t tamFilas = filas * sizeof(int *);
    size_t tamColumnas = columnas * sizeof(int);
    int **matrizIMG = NULL;
    int idMatrizIMG = shmget(IPC_PRIVATE, tamFilas, IPC_CREAT | S_IRUSR | S_IWUSR);
    matrizIMG = (int **) shmat(idMatrizIMG, 0, 0);
    if (matrizIMG == NULL){
        printf("No se pudo crear y asignar la memoria compartida para la matrizIMG\n");
        exit(EXIT_FAILURE);
    }

    int *idsColumnasMatriz = NULL;
    int idColumnas = shmget(IPC_PRIVATE, filas * sizeof(int), IPC_CREAT | S_IRUSR | S_IWUSR);
    idsColumnasMatriz = shmat(idColumnas, 0, 0);
    if (idsColumnasMatriz == NULL){
        printf("No se pudo crear y asignar la memoria compartida para la matrizIMG\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < filas; i++){
        idsColumnasMatriz[i] = shmget(IPC_PRIVATE, tamColumnas, IPC_CREAT | S_IRUSR | S_IWUSR);
        matrizIMG[i] = (int *) shmat(idsColumnasMatriz[i], 0, 0);
    }

    for (int i = 0; i < filas; i++){
        for (int j = 0; j < columnas; j++){
            fscanf(img, "%d", &matrizIMG[i][j]);
        }
    }

    fclose(img);
    img = NULL;
    printf("\nMatriz original: \n");
    for (int i = 0; i < filas; i++){
        for (int j = 0; j < columnas; j++){
            printf("[%d] ", matrizIMG[i][j]);
        }
        printf("\n");
    }
    
    int tamFXHijo = (int)(filas / nHijos);
    int restante = filas - (tamFXHijo * nHijos);

    pid_t *pidHijos = NULL;
    int idPidHijos = shmget(IPC_PRIVATE, nHijos * sizeof(pid_t), IPC_CREAT | S_IRUSR | S_IWUSR);
    pidHijos = shmat(idPidHijos, 0, 0);
    if (pidHijos == NULL){
        printf("No se pudo crear la memoria compartida para PID hijos\nn");
        exit(EXIT_FAILURE);
    }

    double **matrizBlur = NULL;
    size_t tamFilasBlur = filas * sizeof(double *);
    size_t tamColumnasBlur = columnas * sizeof(double);
    int idMatrizBlur = shmget(IPC_PRIVATE, tamFilasBlur, IPC_CREAT | S_IRUSR | S_IWUSR);
    matrizBlur = (double **) shmat(idMatrizBlur, 0, 0);
    if (matrizBlur == NULL){
        printf("No se pudo asignar memoria a la matriz blur de salida\n");
        exit(EXIT_FAILURE);
    }
    int *idsColumnasMatrizBlur = NULL;
    int idColumnasMB = shmget(IPC_PRIVATE, filas * sizeof(int), IPC_CREAT | S_IRUSR | S_IWUSR);
    idsColumnasMatrizBlur = shmat(idColumnasMB, 0, 0);
    if (idsColumnasMatrizBlur == NULL){
        printf("No se pudo asignar memoria a la matriz blur de salida\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < filas; i++){
        idsColumnasMatrizBlur[i] = shmget(IPC_PRIVATE, tamColumnasBlur, IPC_CREAT | S_IRUSR | S_IWUSR);
        matrizBlur[i] = (double *) shmat(idsColumnasMatrizBlur[i], 0, 0);
    }

    int i;
    for (i = 0; i < nHijos; i++){
        pidHijos[i] = fork();
        if (pidHijos[i] == 0){
            break;
        }
    }


    if (i == nHijos){
        int status;
        for (int i = 0; i < nHijos; i++){
            waitpid(pidHijos[i], &status, 0);
        }
        printf("\nMatriz blur: \n");
        FILE *salida = NULL;
        salida = fopen("salida.txt", "w");
        fprintf(salida, "%d\n", filas);
        fprintf(salida, "%d\n", columnas);
        for (int i = 0; i < filas; i++){
            for (int j = 0; j < columnas; j++){
                printf("[%.1lf] ", matrizBlur[i][j]);
                fprintf(salida, "%.1lf ", matrizBlur[i][j]);
            }
            printf("\n");
            fprintf(salida, "\n");
        }
        fclose(salida);
        salida = NULL;

        //ELIMINAR MEMORIA MATRIZ IMAGEN ORIGINAL
        for (int i = 0; i < filas; i++){
            shmdt(matrizIMG[i]);
            shmdt(matrizBlur[i]);
            shmctl(idsColumnasMatriz[i], IPC_RMID, 0);
            shmctl(idsColumnasMatrizBlur[i], IPC_RMID, 0);
        }

        shmdt(matrizIMG);
        shmdt(matrizBlur);
        shmctl(idMatrizIMG, IPC_RMID, 0);
        shmctl(idMatrizBlur, IPC_RMID, 0);
        matrizIMG = NULL;
        matrizBlur = NULL;
        shmdt(idsColumnasMatriz);
        shmdt(idsColumnasMatrizBlur);
        shmctl(idColumnas, IPC_RMID, 0);
        shmctl(idColumnasMB, IPC_RMID, 0);
        idsColumnasMatriz = NULL;
        idsColumnasMatrizBlur = NULL;

        //ELIMINAR MEMORIA VECTOR DE PIDS HIJOS
        shmdt(pidHijos);
        shmctl(idPidHijos, IPC_RMID, 0);
        pidHijos = NULL;
        
    }else{
        int inicio = i * tamFXHijo;
        int fin = (i+1) * tamFXHijo;
        if (i == nHijos-1){
            fin = fin + restante;
        }

        for (int l = inicio; l < fin; l++){
            for (int j = 0; j < columnas; j++){
                double calculo = calcularNine(matrizIMG, filas, columnas, l, j, atoi(argv[2]));
                matrizBlur[l][j] = calculo;
            }
        }


        for (int i = 0; i < filas; i++){
            shmdt(matrizIMG[i]);
            shmdt(matrizBlur[i]);
        }
        shmdt(matrizIMG);
        shmdt(matrizBlur);
        matrizIMG = NULL;
        matrizBlur = NULL;
        shmdt(idsColumnasMatriz);
        shmdt(idsColumnasMatrizBlur);
        idsColumnasMatriz = NULL;
        idsColumnasMatrizBlur = NULL;

        shmdt(pidHijos);
        pidHijos = NULL;

    }

    return EXIT_SUCCESS;
}

