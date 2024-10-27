#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/shm.h>
#include <sys/stat.h>

typedef struct data{
    pid_t hijo;
    int i;
}data;


int main(int argc, char const *argv[]){
    int tamVector;
    FILE *archivoVector = NULL;
    archivoVector = fopen(argv[1], "r");
    if (archivoVector == NULL){
        printf("No se pudo abrir el archivo\n");
        exit(EXIT_FAILURE);
    }

    fscanf(archivoVector, "%d", &tamVector);

    //VALIDAR NUMERO DE PROCESOS HIJOS
    int nHijos;
    printf("Digite el numero de procesos hijos (NProcesos > 0 y NProcesos <= %d): ", tamVector);scanf("%d", &nHijos);
    if (nHijos < 0 || nHijos > tamVector ){
        printf("Numero de hijos no valido");
        exit(EXIT_FAILURE);
    }


    //Asigar memoria compartida del vector y guardar contenido del archivo
    int *vector = NULL;
    size_t espacioVector = tamVector * sizeof(int);
    int idVector = shmget(IPC_PRIVATE, espacioVector, IPC_CREAT | S_IRUSR | S_IWUSR);
    vector = (int *) shmat(idVector, 0, 0);
    if (vector == NULL){
        printf("No se pudo reservar espacio de memoria compartida para vector\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < tamVector; i++){
        fscanf(archivoVector, "%d", &vector[i]);
    }
    fclose(archivoVector);
    archivoVector = NULL;

    

    int valorBuscado;
    printf("Digite el numero buscado: ");scanf("%d", &valorBuscado);

    //Calculo de particiones del vector para cada proceso hijo
    int parte = (int) (tamVector / nHijos);
    int restante = tamVector - (parte * nHijos);
    printf("Parte %d Restante %d\n", parte, restante);

    //Vector para guardar los idDeLasMemorias creadas por los hijos
    int *idsMemoriasHijos = NULL;
    int idIdsMemoriasHijos = shmget(IPC_PRIVATE, nHijos * sizeof(int), IPC_CREAT | S_IRUSR | S_IWUSR);
    idsMemoriasHijos = (int *) shmat(idIdsMemoriasHijos, 0, 0);
    //Vector para guardar el numero de incidencias de los hijos
    int *numIncidencias = NULL;
    int idNumIncidencias = shmget(IPC_PRIVATE, nHijos * sizeof(int), IPC_CREAT | S_IRUSR | S_IWUSR);
    numIncidencias = (int *) shmat(idNumIncidencias, 0, 0);

    //Vector para guardar los PID de los hijos
    pid_t *pidHijos = NULL;
    pidHijos = (pid_t *) malloc(nHijos * sizeof(pid_t));
    if (pidHijos == NULL){
        printf("No se pudo crear espacio de memoria PIDS hijos\n");
        exit(EXIT_FAILURE);
    }
    

    //Creacion de los hijos
    int idx;
    for (idx = 0; idx < nHijos; idx++){
        pidHijos[idx] = fork();
        if (pidHijos[idx] == 0){
            break;
        }
    }
    if (idx == nHijos){
        int status, incidenciasTotales;
        for (int i = 0; i < nHijos; i++){
            waitpid(pidHijos[i], &status, 0);
            int tamIncidencias = numIncidencias[i];
            int idMemoriaIncidencias = idsMemoriasHijos[i];
            data *vecIncidencias = (data *) shmat(idMemoriaIncidencias, 0, 0);
            for (int j = 0; j < tamIncidencias; j++){
                //printf("Incidencia Encontrada por hijo PID %d en la posicion %d\n", vecIncidencias[j].hijo, vecIncidencias[j].i);
                incidenciasTotales++;
            }
            shmdt(vecIncidencias);
            shmctl(idMemoriaIncidencias, IPC_RMID, 0);
        }
        printf("Inicidencias totales del numero %d: %d\n", valorBuscado, incidenciasTotales);
        shmdt(vector);
        shmdt(idsMemoriasHijos);
        shmdt(numIncidencias);
        shmctl(idVector, IPC_RMID, 0);
        shmctl(idIdsMemoriasHijos, IPC_RMID, 0);
        shmctl(idNumIncidencias, IPC_RMID, 0);

    }else{
        int inicio = idx * parte;
        int fin = (idx + 1) * parte;
        if (idx == (nHijos - 1)){
            fin = fin + restante;
        }

        int numeroIncidencias = 0;
        for (int i = inicio; i < fin; i++){
            if (vector[i] ==  valorBuscado){
                numeroIncidencias++;
            }
        }

        numIncidencias[idx] = numeroIncidencias;
        data *incidenciasE = NULL;
        int idIncidenciasE = shmget(IPC_PRIVATE, numeroIncidencias * sizeof(data), IPC_CREAT | S_IRUSR | S_IWUSR);
        incidenciasE = (data *) shmat(idIncidenciasE, 0, 0);

        int counter = 0;
        for (int i = inicio; i < fin; i++){
            if (vector[i] == valorBuscado){
                data valor;
                valor.hijo = getpid();
                valor.i = i;
                incidenciasE[counter] = valor;
                counter++;
            }
        }
        idsMemoriasHijos[idx] = idIncidenciasE;

        shmdt(incidenciasE);
        shmdt(vector);
        shmdt(idsMemoriasHijos);
        shmdt(numIncidencias);
    }
    
    free(pidHijos);
    pidHijos = NULL;

    return EXIT_SUCCESS;
}
