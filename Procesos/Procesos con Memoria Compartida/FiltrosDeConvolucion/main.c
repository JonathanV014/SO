#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <ctype.h>

#include <sys/shm.h>
#include <sys/stat.h>

#include <signal.h>

size_t espacioMatriz(int filas, int columnas, size_t tamTipoDato){
    size_t tam;
    tam  = filas * sizeof(void *);
    tam += (columnas * filas * tamTipoDato);
    return tam;
}


//FUNCION PARA APUNTAR A LOS PUNTEROS SU POSICION CORRESPONDIENTE
void configurarPunteros(void **m, int filas, int columnas, size_t tamTipoDato){
    int i;
    size_t saltosFila = columnas * tamTipoDato;
    m[0] = m + filas;
    for (i = 1; i < filas; i++){
        m[i] = (m[i-1] + saltosFila);
    }
}

void showtree() {
    char cmd[30] = {""};
    sprintf(cmd, "pstree -cAlp %d", getpid());
    system(cmd);
}

void sighandler(int x){}

// double calcularDesenfoque(int **matriz_shm_img, int filas_img, int columnas_img, int i, int j, double **matriz_shm_desenfoque){

//     //printf("fi:%d co:%d i:%d j:%d\n", filas_img, columnas_img, i, j);
//     double resultado = 0.0;
//     int inicioFila = i - 1;
//     int finFila = i + 1;
//     int inicioColumna = j - 1;
//     int finColumna = j + 1;

//     int col = 0;
//     int fil = 0;
//     for (int pi = inicioFila; pi <= finFila; pi++){
//         for (int pj = inicioColumna; pj <= finColumna; pj++){
//             if ((pi >= 0 && pi < filas_img) && (pj >= 0 && pj < columnas_img)){
//                 resultado += matriz_shm_desenfoque[fil][col] * matriz_shm_img[pi][pj];   
//             }else{
//                 resultado += 0;
//             }
//             col++;
//         }
//         fil++;
//         col = 0;
//     }
    
//     return resultado;
// }

// double calcularBorde(int **matriz_shm_img, int filas_img, int columnas_img, int i, int j, int **matriz_shm_borde){

//     //printf("fi:%d co:%d i:%d j:%d\n", filas_img, columnas_img, i, j);
//     double resultado = 0.0;
//     int inicioFila = i - 1;
//     int finFila = i + 1;
//     int inicioColumna = j - 1;
//     int finColumna = j + 1;

//     int col = 0;
//     int fil = 0;
//     for (int pi = inicioFila; pi <= finFila; pi++){
//         for (int pj = inicioColumna; pj <= finColumna; pj++){
//             if ((pi >= 0 && pi < filas_img) && (pj >= 0 && pj < columnas_img)){
//                 resultado += matriz_shm_borde[fil][col] * matriz_shm_img[pi][pj];   
//             }else{
//                 resultado += 0;
//             }
//             col++;
//         }
//         fil++;
//         col = 0;
//     }
//     return resultado;
// }

// double calcularRealce(int **matriz_shm_img, int filas_img, int columnas_img, int i, int j, int **matriz_shm_realce){

//     //printf("fi:%d co:%d i:%d j:%d\n", filas_img, columnas_img, i, j);
//     double resultado = 0.0;
//     int inicioFila = i - 1;
//     int finFila = i + 1;
//     int inicioColumna = j - 1;
//     int finColumna = j + 1;

//     int col = 0;
//     int fil = 0;
//     for (int pi = inicioFila; pi <= finFila; pi++){
//         for (int pj = inicioColumna; pj <= finColumna; pj++){
//             if ((pi >= 0 && pi < filas_img) && (pj >= 0 && pj < columnas_img)){
//                 resultado += matriz_shm_realce[fil][col] * matriz_shm_img[pi][pj];   
//             }else{
//                 resultado += 0;
//             }
//             col++;
//         }
//         fil++;
//         col = 0;
//     }
    
//     return resultado;
// }

int **matriz_shm_bordes = NULL;
double **matriz_shm_desenfoque = NULL;
int **matriz_shm_realce = NULL;

double calcular(int **matriz_shm_img, int filas_img, int columnas_img, int i, int j, int cual){

    //printf("fi:%d co:%d i:%d j:%d\n", filas_img, columnas_img, i, j);
    double resultado = 0;
    int inicioFila = i - 1;
    int finFila = i + 1;
    int inicioColumna = j - 1;
    int finColumna = j + 1;

    int col = 0;
    int fil = 0;
    for (int pi = inicioFila; pi <= finFila; pi++){
        for (int pj = inicioColumna; pj <= finColumna; pj++){
            if ((pi >= 0 && pi < filas_img) && (pj >= 0 && pj < columnas_img)){
                if (cual==0){
                    resultado += matriz_shm_bordes[fil][col] * matriz_shm_img[pi][pj];  
                }else if (cual == 1){
                    resultado += matriz_shm_desenfoque[fil][col] * matriz_shm_img[pi][pj];  
                }else{
                    resultado += matriz_shm_realce[fil][col] * matriz_shm_img[pi][pj];  
                }
            }else{
                resultado += 0;
            }
            col++;
        }
        fil++;
        col = 0;
    }
    
    return resultado;
}






int main(int argc, char const *argv[]){
    FILE *archivo_matriz = NULL;
    archivo_matriz = fopen(argv[1], "r");
    if(archivo_matriz == NULL){
        printf("No se pudo abrir archivo %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    int filas, columnas, tipoKernel;
    fscanf(archivo_matriz, "%d %d %d", &filas, &columnas, &tipoKernel);

    printf("Filas: %d | Columnas: %d | Tipo Kernel:%d\n", filas, columnas, tipoKernel);


    //--------------------------------------------------------------------------------------------------------MATRIZ_IMAGEN                                                                                                                                            
    int **matriz_shm_img = NULL;
    int filas_img = filas;
    int columnas_img = columnas;
    int id_m_img = shmget(IPC_PRIVATE, espacioMatriz(filas_img, columnas_img, sizeof(int)), IPC_CREAT | S_IRUSR | S_IWUSR);
    matriz_shm_img = shmat(id_m_img, 0, 0);
    if(matriz_shm_img == NULL){
        printf("No se pudo reservar espacio para matriz_shm_img\n");
        exit(EXIT_FAILURE);
    }
    configurarPunteros((void *) matriz_shm_img, filas_img, columnas_img, sizeof(int));
    
    printf("IMAGEN: \n");
    for (int i = 0; i < filas_img; i++){
        for (int j = 0; j < columnas_img; j++){
            fscanf(archivo_matriz, "%1d", &matriz_shm_img[i][j]);
            printf("[%d] ", matriz_shm_img[i][j]);
        }
        printf("\n");
    }
    
    int filasK, columnasK;
    filasK = columnasK = 3;

    //----------------------------------------------------------------------MATRIZ_BORDES

    int filas_bordes = filasK;
    int columnas_bordes = columnasK;
    int id_m_bordes = shmget(IPC_PRIVATE, espacioMatriz(filas_bordes, columnas_bordes, sizeof(int)), IPC_CREAT | S_IRUSR | S_IWUSR);
    matriz_shm_bordes = shmat(id_m_bordes, 0, 0);
    if(matriz_shm_bordes == NULL){
        printf("No se pudo reservar espacio para matriz_shm_bordes\n");
        exit(EXIT_FAILURE);
    }
    configurarPunteros((void *) matriz_shm_bordes, filas_bordes, columnas_bordes, sizeof(int));
    printf("MATRIZ DE BORDES\n");
    for (int pi = 0; pi < filas_bordes; pi++){
        for (int pj = 0; pj < columnas_bordes; pj++){
            fscanf(archivo_matriz, "%2d", &matriz_shm_bordes[pi][pj]);
            printf("[%d] ", matriz_shm_bordes[pi][pj]);
        }
        printf("\n");
    }


    //------------------------------------------------------------------ MATRIZ DESENFOQUE

    int filas_desenfoque = filasK;
    int columnas_desenfoque = columnasK;
    int id_m_desenfoque = shmget(IPC_PRIVATE, espacioMatriz(filas_desenfoque, columnas_desenfoque, sizeof(double)), IPC_CREAT | S_IRUSR | S_IWUSR);
    matriz_shm_desenfoque = shmat(id_m_desenfoque, 0, 0);
    if(matriz_shm_desenfoque == NULL){
        printf("No se pudo reservar espacio para matriz_shm_desenfoque\n");
        exit(EXIT_FAILURE);
    }
    configurarPunteros((void *) matriz_shm_desenfoque, filas_desenfoque, columnas_desenfoque, sizeof(double));
    printf("MATRIZ DE DESENFOQUE\n");
    for (int pi = 0; pi < filas_desenfoque; pi++){
        for (int pj = 0; pj < columnas_desenfoque; pj++){
            fscanf(archivo_matriz, "%lf", &matriz_shm_desenfoque[pi][pj]);
            printf("[%lf] ", matriz_shm_desenfoque[pi][pj]);
        }
        printf("\n");
    }

    //-----------------------------------------MATRIZ DE REALCE
    int filas_realce = filasK;
    int columnas_realce = columnasK;
    int id_m_realce = shmget(IPC_PRIVATE, espacioMatriz(filas_realce, columnas_realce, sizeof(int)), IPC_CREAT | S_IRUSR | S_IWUSR);
    matriz_shm_realce = shmat(id_m_realce, 0, 0);
    if(matriz_shm_realce == NULL){
        printf("No se pudo reservar espacio para matriz_shm_realce\n");
        exit(EXIT_FAILURE);
    }
    configurarPunteros((void *) matriz_shm_realce, filas_realce, columnas_realce, sizeof(int));
    printf("MATRIZ DE REALCE\n");
    for (int pi = 0; pi < filas_realce; pi++){
        for (int pj = 0; pj < columnas_realce; pj++){
            fscanf(archivo_matriz, "%d", &matriz_shm_realce[pi][pj]);
            printf("[%d] ", matriz_shm_realce[pi][pj]);
        }
        printf("\n");
    }

    double **matriz_shm_imgS = NULL;
    int filas_imgS = filas_img;
    int columnas_imgS = columnas_img;
    int id_m_imgS = shmget(IPC_PRIVATE, espacioMatriz(filas_imgS, columnas_imgS, sizeof(double)), IPC_CREAT | S_IRUSR | S_IWUSR);
    matriz_shm_imgS = shmat(id_m_imgS, 0, 0);
    if(matriz_shm_imgS == NULL){
        printf("No se pudo reservar espacio para matriz_shm_imgS\n");
        exit(EXIT_FAILURE);
    }
    configurarPunteros((void *) matriz_shm_imgS, filas_imgS, columnas_imgS, sizeof(double));
    printf("MATRIZ IMAGEN DE SALIDA\n");
    for (int pi = 0; pi < filas_imgS; pi++){
        for (int pj = 0; pj < columnas_imgS; pj++){
            matriz_shm_imgS[pi][pj] = 0;
            printf("[%lf] ", matriz_shm_imgS[pi][pj]);
        }
        printf("\n");
    }
    

    

    void (*oldhandler)(int);
    oldhandler = signal(SIGUSR1, sighandler);
    if (oldhandler == SIG_ERR) {
        perror("signal:");
        exit(EXIT_FAILURE);
    }

    //-----------------------------------------------------------------CREACION DE LOS HIJOS
    int nProcesos = 3;
    pid_t *pidProcesos = NULL;
    pidProcesos = (pid_t *)malloc(nProcesos * sizeof(pid_t));
    if(pidProcesos == NULL){
        printf("No se pudo reservar memoria para pidProcesos\n");
        exit(EXIT_FAILURE);
    }

    int *vector_listo = NULL;
    int tam_vector_listo = 1;
    int id_vector_listo = shmget(IPC_PRIVATE, (tam_vector_listo * sizeof(int)), IPC_CREAT | S_IRUSR | S_IWUSR);
    vector_listo = (int *) shmat(id_vector_listo, 0, 0);
    if(vector_listo == NULL){
        printf("No se pudo reservar espacio para vector_listo\n");
        exit(EXIT_FAILURE);
    }
    vector_listo[0] = 0;
    


    int idx;
    for (idx = 0; idx < nProcesos; idx++){
        pidProcesos[idx] = fork();
        if (pidProcesos[idx] == 0){
            break;
        }
    }

    if(idx == nProcesos){
        
        while (vector_listo[0] == 0){}
        
        printf("RESULTADO\n");
        for (int i = 0; i < filas_imgS; i++){
            for (int j = 0; j < columnas_imgS; j++){
                printf("[%.4lf] ", matriz_shm_imgS[i][j]);
            }
            printf("\n");
        }
        

        //DESCONECTARSE Y ELIMINAR matriz_shm_imgS
        shmdt(matriz_shm_imgS);
        shmctl(id_m_imgS, IPC_RMID, 0);
        
        //DESCONECTARSE Y ELIMINAR matriz_shm_realce
        shmdt(matriz_shm_realce);
        shmctl(id_m_realce, IPC_RMID, 0);

        //DESCONECTARSE Y ELIMINAR matriz_shm_desenfoque
        shmdt(matriz_shm_desenfoque);
        shmctl(id_m_desenfoque, IPC_RMID, 0);
        
        //DESCONECTARSE Y ELIMINAR matriz_shm_bordes
        shmdt(matriz_shm_bordes);
        shmctl(id_m_bordes, IPC_RMID, 0);    

        //DESCONECTARSE Y ELIMINAR matriz_shm_img
        shmdt(matriz_shm_img);
        shmctl(id_m_img, IPC_RMID, 0);

        //DESCONECTARSE Y ELIMINAR vector_listo
        shmdt(vector_listo);
        shmctl(id_vector_listo, IPC_RMID, 0);
        vector_listo = NULL;
        

    }else{
  
        // if (idx == 0){ //SOY EL HIJO DE BORDES
        //     //printf("Soy bordes\n");
        //     for (int pi = 0; pi < filas_imgS; pi++){
        //         for (int pj = 0; pj < columnas_imgS; pj++){
        //             matriz_shm_imgS[pi][pj] = (double) calcular(matriz_shm_img, filas_img, columnas_img, pi, pj, idx);
        //         }
        //     }

        // }else if (idx == 1){ //SOY EL HIJO DESENFOQUE
        //     //printf("Soy desenfoque\n");
        //     for (int pi = 0; pi < filas_imgS; pi++){
        //         for (int pj = 0; pj < columnas_imgS; pj++){
        //             matriz_shm_imgS[pi][pj] = (double) calcular(matriz_shm_img, filas_img, columnas_img, pi, pj, idx);
        //         }
        //     }
        //     kill(getppid(), SIGUSR1);
            
        // }else{//SOY EL HIJP DE REALCE
        //     //printf("Soy realce\n");
        //     for (int pi = 0; pi < filas_imgS; pi++){
        //         for (int pj = 0; pj < columnas_imgS; pj++){
        //             matriz_shm_imgS[pi][pj] = (double) calcular(matriz_shm_img, filas_img, columnas_img, pi, pj, idx);
        //         }
        //     }
        // }

        if (idx == (tipoKernel-1)){
            for (int pi = 0; pi < filas_imgS; pi++){
                for (int pj = 0; pj < columnas_imgS; pj++){
                    matriz_shm_imgS[pi][pj] = calcular(matriz_shm_img, filas_img, columnas_img, pi, pj, tipoKernel-1);
                }
            }

            vector_listo[0] = 1;
        }
        
        
        
        
        //DESCONECTARSE Y ELIMINAR matriz_shm_imgS
        shmdt(matriz_shm_imgS);
        
        //DESCONECTARSE Y ELIMINAR matriz_shm_realce
        shmdt(matriz_shm_realce);
    
        //DESCONECTARSE Y ELIMINAR matriz_shm_desenfoque
        shmdt(matriz_shm_desenfoque);
        
        //DESCONECTARSE Y ELIMINAR matriz_shm_bordes
        shmdt(matriz_shm_bordes);
        
        //DESCONECTARSE Y ELIMINAR matriz_shm_img
        shmdt(matriz_shm_img);
    }


    //ELIMINAR ESPACIO DE MEMORIA PARA pidProcesos
    free(pidProcesos);
    pidProcesos = NULL;
    
    
    //CERRAR ARCHIVO archivo_matriz
    fclose(archivo_matriz);
    archivo_matriz = NULL;

    //RESTAURAR EL MANEJADOR PARA SIGUSR1
    if (signal(SIGUSR1, oldhandler) == SIG_ERR) {
        perror("signal:");
        exit(EXIT_FAILURE);
    }
    
    return 0;
}
