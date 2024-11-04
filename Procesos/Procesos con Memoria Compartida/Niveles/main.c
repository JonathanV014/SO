#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <ctype.h>

#include <sys/shm.h>
#include <sys/stat.h>

void showtree()
{
    char cmd[40] = {""};
    sprintf(cmd, "pstree -cAlp %d", getpid());
    system(cmd);
}

unsigned int espacioMatriz(int filas, int columnas, size_t tamTipoDato)
{
    size_t tam;
    tam = filas * sizeof(void *);
    tam += (columnas * filas * tamTipoDato);
    return tam;
}

// FUNCION PARA APUNTAR A LOS PUNTEROS SU POSICION CORRESPONDIENTE
void configurarPunteros(void **m, int filas, int columnas, size_t tamTipoDato)
{
    int i;
    size_t saltosFila = columnas * tamTipoDato;
    m[0] = m + filas;
    for (i = 1; i < filas; i++)
    {
        m[i] = (m[i - 1] + saltosFila);
    }
}

int main(int argc, char const *argv[])
{
    FILE *archivo_matrices = NULL;
    archivo_matrices = fopen(argv[1], "r");
    if (archivo_matrices == NULL)
    {
        printf("No se pudo abrir archivo %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    int N;
    int M;

    fscanf(archivo_matrices, "%d", &N);
    printf("%d\n", N);
    fscanf(archivo_matrices, "%d", &M);
    printf("%d\n", M);

    size_t tamMatEnt = espacioMatriz(N, M, sizeof(int));

    printf("MATRIZ DE NIVELES\n");
    int **matrizNiveles = NULL;
    int idMatrizNiveles = shmget(IPC_PRIVATE, tamMatEnt, IPC_CREAT | S_IRUSR | S_IWUSR);
    matrizNiveles = shmat(idMatrizNiveles, 0, 0);
    configurarPunteros((void *)matrizNiveles, N, M, sizeof(int));
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            fscanf(archivo_matrices, "%d", &matrizNiveles[i][j]);
            printf("[%d] ", matrizNiveles[i][j]);
        }
        printf("\n");
    }

    printf("MATRIZ DE CALIDAD\n");
    int **matrizCalidad = NULL;
    int idMatrizCalidad = shmget(IPC_PRIVATE, tamMatEnt, IPC_CREAT | S_IRUSR | S_IWUSR);
    matrizCalidad = shmat(idMatrizCalidad, 0, 0);
    configurarPunteros((void *)matrizCalidad, N, M, sizeof(int));
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            fscanf(archivo_matrices, "%d", &matrizCalidad[i][j]);
            printf("[%d] ", matrizCalidad[i][j]);
        }
        printf("\n");
    }

    printf("MATRIZ DE PREDICCION\n");
    int **matrizPrediccion = NULL;
    int idMatrizPrediccion = shmget(IPC_PRIVATE, tamMatEnt, IPC_CREAT | S_IRUSR | S_IWUSR);
    matrizPrediccion = shmat(idMatrizPrediccion, 0, 0);
    configurarPunteros((void *)matrizPrediccion, N, M, sizeof(int));
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            fscanf(archivo_matrices, "%d", &matrizPrediccion[i][j]);
            printf("[%d] ", matrizPrediccion[i][j]);
        }
        printf("\n");
    }

    printf("MATRIZ DE RIESGO\n");
    size_t tamMatFlo = espacioMatriz(N, M, sizeof(double));

    double **matrizRiesgo = NULL;
    int idMatrizRiesgo = shmget(IPC_PRIVATE, tamMatFlo, IPC_CREAT | S_IRUSR | S_IWUSR);
    matrizRiesgo = shmat(idMatrizRiesgo, 0, 0);
    configurarPunteros((void *)matrizRiesgo, N, M, sizeof(double));
    for (int i = 0; i < N; i++){
        for (int j = 0; j < M; j++){
            matrizRiesgo[i][j] = 0;
            printf("[%lf] ", matrizRiesgo[i][j]);
        }
        printf("\n");
    }

    printf("MATRIZ DE SOSTENIBILIDAD\n");
    double **matrizSostenibilidad = NULL;
    int idMatrizSostenibilidad = shmget(IPC_PRIVATE, tamMatFlo, IPC_CREAT | S_IRUSR | S_IWUSR);
    matrizSostenibilidad = shmat(idMatrizSostenibilidad, 0, 0);
    configurarPunteros((void *)matrizSostenibilidad, N, M, sizeof(double));
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            matrizSostenibilidad[i][j] = 0;
            printf("[%lf] ", matrizSostenibilidad[i][j]);
        }
        printf("\n");
    }

    int nProcesos = 2;

    int *vector_padre = NULL;
    int tam_vector_padre = nProcesos;
    int id_vector_padre = shmget(IPC_PRIVATE, (tam_vector_padre * sizeof(int)), IPC_CREAT | S_IRUSR | S_IWUSR);
    vector_padre = (int *)shmat(id_vector_padre, 0, 0);
    if (vector_padre == NULL)
    {
        printf("No se pudo reservar espacio para vector_padre\n");
        exit(EXIT_FAILURE);
    }

    int *vector_esHijos = NULL;
    int tam_vector_esHijos = nProcesos;
    int id_vector_esHijos = shmget(IPC_PRIVATE, (tam_vector_esHijos * sizeof(int)), IPC_CREAT | S_IRUSR | S_IWUSR);
    vector_esHijos = (int *)shmat(id_vector_esHijos, 0, 0);
    if (vector_esHijos == NULL)
    {
        printf("No se pudo reservar espacio para vector_esHijos\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < nProcesos; i++)
    {
        vector_padre[i] = 0;
        vector_esHijos[i] = 0;
    }

    pid_t padre = getpid();
    pid_t *pidProcesos = NULL;
    pidProcesos = (pid_t *)malloc(nProcesos * sizeof(pid_t));
    if (pidProcesos == NULL)
    {
        printf("No se pudo reservar memoria para pidProcesos\n");
        exit(EXIT_FAILURE);
    }
    printf("ITERACIONESSSSSSSSS--------------------------------\n");
    int idx;
    for (idx = 0; idx < nProcesos; idx++)
    {
        pidProcesos[idx] = fork();
        if (pidProcesos[idx] == 0)
        {
            break;
        }
    }

    if (idx == nProcesos)
    { // PADRE

        int **matriz_d_alerta = NULL;
        int filas_m_d_alerta = N;
        int columnas_m_d_alerta = M;
        matriz_d_alerta = (int **)malloc(filas_m_d_alerta * sizeof(int *));
        if (matriz_d_alerta == NULL)
        {
            printf("No se pudo reservar espacio para matriz_d_alerta\n");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < filas_m_d_alerta; i++)
        {
            matriz_d_alerta[i] = (int *)malloc(columnas_m_d_alerta * sizeof(int));
        }

        for (int i = 0; i < atoi(argv[2]); i++){
            printf("-------------------------ITERACION %d\n", i+1);

            while (vector_esHijos[0] == 0)
            {
            }
            
            //printf("-------------------Hijo 1 termino\n");
            while (vector_esHijos[1] == 0)
            {
            }

            //printf("------------------Hijo 2 termino\n");

            printf("MATRIZ DE ALERTA\n");
            for (int pi = 0; pi < N; pi++)
            {
                for (int pj = 0; pj < M; pj++)
                {
                    // if (matrizRiesgo[pi][pj] > 1.5 && matrizSostenibilidad[pi][pj] < 0.3)
                    // {
                    //     matriz_d_alerta[pi][pj] = 2;
                    // }
                    // else if (matrizRiesgo[pi][pj] > 1.2 && matrizSostenibilidad[pi][pj] < 0.4)
                    // {
                    //     matriz_d_alerta[pi][pj] = 1;
                    // }
                    // else
                    // {
                    //     matriz_d_alerta[pi][pj] = 0;
                    // }
                    matriz_d_alerta[pi][pj] = matrizRiesgo[pi][pj] + matrizSostenibilidad[pi][pj];
                    printf("[%d] ", matriz_d_alerta[pi][pj]);
                }
                printf("\n");
            }

            for (int i = 0; i < nProcesos; i++){
                vector_esHijos[i] = 0;
                vector_padre[i] = 1;
            }
        }

        // LIBERAR ESPACIO DE LA matriz_d_alerta
        for (int i = 0; i < filas_m_d_alerta; i++)
        {
            free(matriz_d_alerta[i]);
            matriz_d_alerta[i] = NULL;
        }
        free(matriz_d_alerta);
        matriz_d_alerta = NULL;

        showtree();
    }
    else{ // HIJOS
        for (int i = 0; i < atoi(argv[2]); i++){
            if (idx == 0){ // SOY HIJO 1 - ANALISTA DE RIESGO
                printf("MATRIZ DE RIESGO\n");
                for (int pi = 0; pi < N; pi++)
                {
                    for (int pj = 0; pj < M; pj++)
                    {
                        // matrizRiesgo[pi][pj] = (double)((100 - matrizNiveles[pi][pj]) * (2 - matrizCalidad[pi][pj]) * (1 - matrizPrediccion[pi][pj])) / 100;
                        matrizRiesgo[pi][pj] += 1; 
                        printf("[%.4lf] ", matrizRiesgo[pi][pj]);
                    }
                    printf("\n");
                }
            }else{ // SOY HIJO 2 - ANALISTA DE SOSTENIBILIDAD
                printf("MATRIZ DE SOSTENIBILIDAD\n");
                for (int pi = 0; pi < N; pi++)
                {
                    for (int pj = 0; pj < M; pj++)
                    {
                        // matrizSostenibilidad[pi][pj] = (double)(2 * (matrizNiveles[pi][pj]) * (matrizCalidad[pi][pj] + 1) * (matrizPrediccion[pi][pj] + 2)) / 100;
                        // printf("[%.4lf] ", matrizSostenibilidad[pi][pj]);
                        matrizSostenibilidad[pi][pj] += 1;
                        printf("[%.4lf] ", matrizSostenibilidad[pi][pj]);
                        
                    }
                    printf("\n");
                }
            }
            vector_esHijos[idx] = 1;
            while (vector_padre[idx] == 0){}
            vector_padre[idx] = 0;
            
        }
    }

    // DESCONECTARSE Y ELIMINAR vector_esHijos
    shmdt(vector_esHijos);
    shmctl(id_vector_esHijos, IPC_RMID, 0);
    vector_esHijos = NULL;

    // DESCONECTARSE Y ELIMINAR vector_padre
    shmdt(vector_padre);
    shmctl(id_vector_padre, IPC_RMID, 0);
    vector_padre = NULL;

    // ELIMINAR ESPACIO DE MEMORIA PARA pidProcesos
    free(pidProcesos);
    pidProcesos = NULL;

    // CERRAR ARCHIVO archivo_matrices
    fclose(archivo_matrices);
    archivo_matrices = NULL;

    return 0;
}
