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
int valor = 0;
pthread_mutex_t mt = PTHREAD_MUTEX_INITIALIZER;
int max_iter = 1000;

void* funcion_hilo(void* arg) {
    printf("Hilo [%lu] hará [%d] iteraciones\n", pthread_self(), max_iter);

    for (int i = 0; i < max_iter; ++i) {
        pthread_mutex_lock(&mt); 
        valor = valor + 1;
        pthread_mutex_unlock(&mt); 
    }

    pthread_exit(0); 
}

int main(int argc, char const *argv[]){
    int nHijos = 2;
    pthread_t hId[nHijos];
    for (int i = 0; i < nHijos; i++){
        pthread_create(&hId[i], NULL, funcion_hilo, NULL);
    }

    
    for (int i = 0; i < nHijos; i++){
        pthread_join(hId[i], NULL);
    }
    
    printf("Padre principal [%lu]\n", pthread_self());
    printf("%d\n", valor);


    pthread_mutex_destroy(&mt);
    return 0;
}
