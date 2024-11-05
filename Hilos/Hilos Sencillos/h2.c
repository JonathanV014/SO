#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <ctype.h>

#include <sys/shm.h>
#include <sys/stat.h>

#include <pthread.h>
//Instrucciones de compilación: gcc programa_sincronizado.c -lpthread

pthread_mutex_t mt = PTHREAD_MUTEX_INITIALIZER;

typedef struct mat{
    int filas;
    int columnas;
    int i;
    int idMem;
}mat;

int **matriz_mem = NULL;

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

void *funcionHilos(void *arg){
    mat *data = NULL;
    data = (mat *)malloc(sizeof(mat));
    if (data == NULL){
        printf("No se pudo crear espacio para guardar info en hijo [%ul]\n");
        pthread_exit(0);
    }
    
    data = (mat*)arg;
    int **matriz = shmat(data->idMem, 0, 0);
    //printf("Hijo con id[%lu] n[%d]\n", pthread_self(), data->i);
    
    for (int i = 0; i < data->filas; i++){
        for (int j = 0; j < data->columnas; j++){
            pthread_mutex_lock(&mt);
            matriz[i][j] += 1;//data->i; 
            pthread_mutex_unlock(&mt);
        }   
    }
    
    free(data);
    data = NULL;
    shmdt(matriz);
    matriz = NULL;

    pthread_exit(0);
}


int main(int argc, char const *argv[]){

    int **matriz_shm_trabajo = NULL;
    int filas_trabajo = 10;
    int columnas_trabajo = 10;
    int id_m_trabajo = shmget(IPC_PRIVATE, espacioMatriz(filas_trabajo, columnas_trabajo, sizeof(int)), IPC_CREAT | S_IRUSR | S_IWUSR);
    matriz_shm_trabajo = shmat(id_m_trabajo, 0, 0);
    if(matriz_shm_trabajo == NULL){
        printf("No se pudo reservar espacio para matriz_shm_trabajo\n");
        exit(EXIT_FAILURE);
    }
    configurarPunteros((void *) matriz_shm_trabajo, filas_trabajo, columnas_trabajo, sizeof(int));
    


    pthread_t *vector_d_hijos = NULL;
    int tam_vector_d_hijos = atoi(argv[1]);
    vector_d_hijos = (pthread_t *) malloc(tam_vector_d_hijos * sizeof(pthread_t));
    if(vector_d_hijos == NULL){
        printf("No se pudo reservar espacio para vector_d_hijos\n");
        exit(EXIT_FAILURE);
    }

    for (int pi = 0; pi < filas_trabajo; pi++){
        for (int pj = 0; pj < columnas_trabajo; pj++){
            matriz_shm_trabajo[pi][pj] = 0;
        }
        printf("\n");
    }
    


    for (int i = 0; i < tam_vector_d_hijos; i++){
        mat *data = NULL;
        data = (mat *) malloc(sizeof(mat));
        if (data == NULL){
            printf("No se pudo crear memoria para enviar info hijos\n");
            exit(EXIT_FAILURE);
        }
    
        data->filas = filas_trabajo;
        data->columnas = columnas_trabajo;
        data->i = i;
        data->idMem = id_m_trabajo;
        pthread_create(&vector_d_hijos[i], NULL, funcionHilos, (void *) data);
        
    }
    


    for (int i = 0; i < tam_vector_d_hijos; i++){
        pthread_join(vector_d_hijos[i], NULL);
    }
    
    for (int pi = 0; pi < filas_trabajo; pi++){
        for (int pj = 0; pj < columnas_trabajo; pj++){
            printf("[%d] ", matriz_shm_trabajo[pi][pj]);
        }
        printf("\n");
    }

    //LIBERAR MEMORIA DE vector_d_hijos
    free(vector_d_hijos);
    vector_d_hijos = NULL;

    //DESCONECTARSE Y ELIMINAR matriz_shm_trabajo
    shmdt(matriz_shm_trabajo);
    shmctl(id_m_trabajo, IPC_RMID, 0);

    pthread_mutex_destroy(&mt);
    return EXIT_SUCCESS;
}
