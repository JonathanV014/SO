#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <ctype.h>

#include <unistd.h>  

#include <sys/shm.h>
#include <sys/stat.h>

#include <pthread.h>
//Instrucciones de compilación: gcc programa_sincronizado.c -lpthread

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int NA = 0;
int NT = 0;
int NC = 0;
int NG = 0;

char *vector_data_adn = NULL;

typedef struct dataHilo{
    int i;
    int parte;
    int restante;
    int nBases;
    int nHilos;
}dataHilo;

int tamSecuencia = 6;
char secuencia[6] = {'T','T','G','T','A','C'};


int verificarCadena(int posi, int tamano){
    int existe = 1;
    int ini = posi+1;
    if ((posi+(tamSecuencia-1)) < tamano){
        for (int i = 1; i < tamSecuencia ; i++){
            if (secuencia[i] != vector_data_adn[ini]){
                existe = -1;
            }
            ini++;
        }
    }else{
        existe = -1;
    }

    return existe;
}


int tuberiaData[2];

int nEncuentros = 0;
void *funcionADN(void *x){
    dataHilo *data = (dataHilo *)x;
    
    if (data->i == ((data->nHilos)-1)){
        for (int i = 0; i < data->nBases; i++){
            char y = vector_data_adn[i];
            if (y == secuencia[0]){
                int verificado = verificarCadena(i, data->nBases);
                if (verificado == 1){
                    write(tuberiaData[1], &i, sizeof(int));
                    nEncuentros++; 
                }
            }
        }
    }else{
        int inicio = data->i * data->parte;
        int fin = ((data->i) + 1) * data->parte;

        if (data->i == ((data->nHilos)-2)){
            fin = fin + data->restante;
        }

        for (int i = inicio; i < fin; i++){
            char x = vector_data_adn[i];
            //SECCION CRITICA
            pthread_mutex_lock(&mutex);
            if (x == 'A'){
                NA++;
            }
            if (x == 'T'){
                NT++;
            }
            if (x == 'C'){
                NC++;
            }
            if (x == 'G'){
                NG++;
            }
            pthread_mutex_unlock(&mutex);
        }
    }

    free(x);
    pthread_exit(0);
}

int main(int argc, char const *argv[]){
    FILE *archivo_data = NULL;
    archivo_data = fopen(argv[1], "r");
    if(archivo_data == NULL){
        printf("No se pudo abrir archivo %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    int nBases;
    fscanf(archivo_data, "%d", &nBases);
    printf("%d\n", nBases);

    int tam_vector_data_adn = nBases;
    int id_vector_data_adn = shmget(IPC_PRIVATE, (tam_vector_data_adn * sizeof(char)), IPC_CREAT | S_IRUSR | S_IWUSR);
    vector_data_adn = (char *) shmat(id_vector_data_adn, 0, 0);
    if(vector_data_adn == NULL){
        printf("No se pudo reservar espacio para vector_data_adn\n");
        exit(EXIT_FAILURE);
    }
    
    int comienzo = 0;
    for (int i = 0; i < nBases; i++){
        fscanf(archivo_data, "%c", &vector_data_adn[i]);
        #if VERSION == 0
            printf("%c",vector_data_adn[i]);
        #endif
    }
    #if VERSION == 0
        printf("\n");
    #endif

    int nHilos = atoi(argv[2]);
    if (nHilos < 2){
        printf("Necesita crear mas de 1 hilo para este trabajo\n");
        exit(EXIT_FAILURE);
    }

    int partes = nBases / (nHilos-1);
    int restante =  nBases - ((nHilos-1)*partes);

    pthread_t *vector_hilos = NULL;
    int tam_vector_hilos = nHilos;
    int id_vector_hilos = shmget(IPC_PRIVATE, (tam_vector_hilos * sizeof(pthread_t)), IPC_CREAT | S_IRUSR | S_IWUSR);
    vector_hilos = (pthread_t *) shmat(id_vector_hilos, 0, 0);
    if(vector_hilos == NULL){
        printf("No se pudo reservar espacio para vector_hilos\n");
        exit(EXIT_FAILURE);
    }
    
    pipe(tuberiaData);
    for (int i = 0; i < nHilos; i++){
        dataHilo *data = NULL;
        data = (dataHilo *) malloc(sizeof(dataHilo));
        if (data == NULL){
            printf("No se pudo crear memoria para enviar info hilos\n");
            exit(EXIT_FAILURE);
        }

        data->i = i;
        data->parte = partes;
        data->restante = restante;
        data->nBases = nBases;
        data->nHilos = nHilos;

        pthread_create(&vector_hilos[i], NULL, funcionADN, (void *) data);
    }
    
    for (int i = 0; i < nHilos; i++){
        pthread_join(vector_hilos[i], NULL);
    }
    close(tuberiaData[1]);

    printf("A: %d\n", NA);
    printf("T: %d\n", NT);
    printf("C: %d\n", NC);
    printf("G: %d\n", NG);
    
    for (int i = 0; i < nEncuentros; i++){
        int recibir;
        read(tuberiaData[0], &recibir, sizeof(int));
        printf("Secuencia en la posicion: %d\n", recibir);
    }
    close(tuberiaData[0]);
   
    
    pthread_mutex_destroy(&mutex);

    //DESCONECTARSE Y ELIMINAR vector_hilos
    shmdt(vector_hilos);
    shmctl(id_vector_hilos, IPC_RMID, 0);
    vector_hilos = NULL;

    //DESCONECTARSE Y ELIMINAR vector_data_adn
    shmdt(vector_data_adn);
    shmctl(id_vector_data_adn, IPC_RMID, 0);
    vector_data_adn = NULL;

    //CERRAR ARCHIVO archivo_data
    fclose(archivo_data);
    archivo_data = NULL;

    return EXIT_SUCCESS;
}
