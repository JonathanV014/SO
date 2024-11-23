#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <ctype.h>

#include <pthread.h>
//Instrucciones de compilación: gcc programa_sincronizado.c -lpthread

int nFilas = 0;
int nColumnas = 0;
int nHilos = 0;
int tiempos = 0;
int parte = 0;
int restante;

int **matriz_d_datos = NULL;
int **matriz_d_resultado = NULL;
pthread_t *vector_d_hilos = NULL;

typedef struct data{
    int i;
}data;

pthread_barrier_t mybarrier1;
pthread_barrier_t mybarrier2;

int calcularRelajacionJacobi(int i, int j){
    return((matriz_d_datos[i-1][j] + matriz_d_datos[i+1][j] + matriz_d_datos[i][j-1] + matriz_d_datos[i][j+1])/4);
}

void *funcionHilo(void *x){
    //printf("Parte %d Restante %d\n", parte, restante);
    data *datos = (data *)x;
    int inicio = datos->i * parte;
    int fin = ((datos->i) + 1) * parte;
    if (datos->i == (nHilos-1)){
        fin += restante;
    }

    for (int k = 0; k < tiempos; k++){
        for (int i = (inicio + 1); i < (fin + 1); i++){
            for (int j = 0; j < nColumnas; j++){
                if (j != 0 && j != (nColumnas-1)){
                    matriz_d_resultado[i][j] = calcularRelajacionJacobi(i, j);
                }
            }
        }
        pthread_barrier_wait(&mybarrier1);
        pthread_barrier_wait(&mybarrier2);
    }
    
    
    

    pthread_exit(EXIT_SUCCESS);
}

int main(int argc, char const *argv[]){
    if (argc < 2){
        printf("Mande sus argumentos completos\n");
        exit(EXIT_FAILURE);
    }
 
    FILE *archivo_data = NULL;
    archivo_data = fopen(argv[1], "r");
    if(archivo_data == NULL){
        printf("No se pudo abrir archivo %s\n", argv[2]);
        exit(EXIT_FAILURE);
    }
    
    fscanf(archivo_data, "%d %d", &nFilas, &nColumnas);
    printf("#FILAS: %d | #COLUMNAS: %d\n", nFilas, nColumnas);

    if (nFilas <= 2 && nColumnas <= 2){
        printf("Matriz no valida para calcilo\n");
        exit(EXIT_FAILURE);
    }
    
    int filas_m_d_datos = nFilas;
    int columnas_m_d_datos = nColumnas;
    matriz_d_datos = (int **) malloc(filas_m_d_datos * sizeof(int *));
    if(matriz_d_datos == NULL){
        printf("No se pudo reservar espacio para matriz_d_datos\n");
        exit(EXIT_FAILURE);
    }
    for(int i = 0; i < filas_m_d_datos; i++){
        matriz_d_datos[i] = (int *) malloc(columnas_m_d_datos * sizeof(int));
    }

    int filas_m_d_resultado = nFilas;
    int columnas_m_d_resultado = nColumnas;
    matriz_d_resultado = (int **) malloc(filas_m_d_resultado * sizeof(int *));
    if(matriz_d_resultado == NULL){
        printf("No se pudo reservar espacio para matriz_d_resultado\n");
        exit(EXIT_FAILURE);
    }
    for(int i = 0; i < filas_m_d_resultado; i++){
        matriz_d_resultado[i] = (int *) malloc(columnas_m_d_resultado * sizeof(int));
    }
    
    printf("MATRIZ DE DATOS INICIALES: \n");
    for (int pi = 0; pi < filas_m_d_datos; pi++){
        for (int pj = 0; pj < columnas_m_d_datos; pj++){
            fscanf(archivo_data, "%d", &matriz_d_datos[pi][pj]);
            matriz_d_resultado[pi][pj] = matriz_d_datos[pi][pj];
            printf("[%d]\t", matriz_d_datos[pi][pj]);
        }
        printf("\n");
    }
    
    printf("Digite la cantidad de hilos para el calculo: "); scanf("%d", &nHilos);
    if (nHilos <=0 || nHilos > (nFilas-2)){
        printf("Numero de hilos no valido\n");
        exit(EXIT_FAILURE);
    }

    printf("Digite el tiempo: "); scanf("%d", &tiempos);
    if (tiempos <= 0){
        printf("Numero de tiempo no valido, debe ser mayor a 0\n");
        exit(EXIT_FAILURE);
    }

    parte = (nFilas-2) / nHilos;
    restante = (nFilas-2) - (parte * nHilos);


    int tam_vector_d_hilos = nHilos;
    vector_d_hilos = (pthread_t *) malloc(tam_vector_d_hilos * sizeof(pthread_t));
    if(vector_d_hilos == NULL){
        printf("No se pudo reservar espacio para vector_d_hilos\n");
        exit(EXIT_FAILURE);
    }

    pthread_barrier_init(&mybarrier1, NULL, nHilos+1);
    pthread_barrier_init(&mybarrier2, NULL, nHilos+1);

    for (int i = 0; i < nHilos; i++){
        data *datos = NULL;
        datos = (data *) malloc(sizeof(data));
        if (datos == NULL){
            printf("No se pudo asignar memoria\n");
            exit(EXIT_FAILURE);
        }
        datos->i = i;
        pthread_create(&vector_d_hilos[i], NULL, funcionHilo, (void *)datos);   
    }


    for (int k = 0; k < tiempos; k++){
        printf("Resultado tiempo %d\n", k+1);
        pthread_barrier_wait(&mybarrier1);

        for (int pi = 0; pi < nFilas; pi++){
            for (int pj = 0; pj < nColumnas; pj++){
                printf("[%d]\t", matriz_d_resultado[pi][pj]);
                matriz_d_datos[pi][pj] = matriz_d_resultado[pi][pj];
            }
            printf("\n");
        }
        pthread_barrier_wait(&mybarrier2);
    }
    
    

    for (int i = 0; i < nHilos; i++){
        pthread_join(vector_d_hilos[i], NULL);
    }
    
   

    pthread_barrier_destroy(&mybarrier1);
    pthread_barrier_destroy(&mybarrier2);
    //LIBERAR MEMORIA DE vector_d_hilos
    free(vector_d_hilos);
    vector_d_hilos = NULL;
    
    //LIBERAR ESPACIO DE LA matriz_d_resultado
    for(int i = 0; i < filas_m_d_resultado; i++){
        free(matriz_d_resultado[i]);
        matriz_d_resultado[i] = NULL;
    }
    free(matriz_d_resultado);
    matriz_d_resultado = NULL;
    
    //LIBERAR ESPACIO DE LA matriz_d_datos
    for(int i = 0; i < filas_m_d_datos; i++){
        free(matriz_d_datos[i]);
        matriz_d_datos[i] = NULL;
    }
    free(matriz_d_datos);
    matriz_d_datos = NULL;    
    
    //CERRAR ARCHIVO archivo_data
    fclose(archivo_data);
    archivo_data = NULL;
    
    return EXIT_SUCCESS;
}
