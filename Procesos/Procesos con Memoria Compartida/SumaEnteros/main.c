#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/shm.h>
#include <sys/stat.h>

int main(int argc, char *argv[]){
    

    FILE *data = NULL;
    data = fopen(argv[1], "r");
    if (data == NULL) { //MANEJO DE ERROR SI NO SE ENCUENTRA
        printf("No se pudo abrir archivo\n");
        exit(1);
    }
    int tamData;
    fscanf(data,"%d", &tamData);

    int nHijos;
    printf("\nDigite el numero de procesos hijos (NProcesos > 0 and NProcesos <= %d): ",tamData);
    scanf("%d", &nHijos);

    if (nHijos < 0 || nHijos > tamData){
        printf("Numero de procesos no valido");
        fclose(data);
        exit(1);
    }
    
    //ESPACIO DE MEMORIA COMPARTIDA PARA QUE CADA HIJO ESCRIBA SU RESULTADO DE LA SUMA
    long int *vecCajasHijos;
    int shmTamCajasHijos = (nHijos) * sizeof(long int); 
    int idCajasHijos = shmget(IPC_PRIVATE, shmTamCajasHijos, IPC_CREAT | S_IRUSR | S_IWUSR); //CLAVE PA CONECTARSE
    vecCajasHijos = (long int *) shmat(idCajasHijos, 0, 0);

    //ESPACIO DE MEMORIA COMPARTIDA PARA GUARDAR LOS DATOS DEL ARCHIVO 
    int shmTamData = tamData * sizeof(int);
    int idData = shmget(IPC_PRIVATE, shmTamData, IPC_CREAT | S_IRUSR | S_IWUSR);
    int *vecData = (int *)shmat(idData, 0,0);

    for (int j = 0; j < tamData; j++){
        fscanf(data, "%d", &vecData[j]);
    }

    //ESPACIO DE MEMORIA COMPARTIDA PARA GUARDAR LOS PID DE LOS PROCESOS HIJOS CREADOS
    //LA HICE COMPARTIDA PORQUE DESPUES SE PUEDE HACER LOCURAS CON TUBERIAS(AUN NO JSJS)
    pid_t shmTamHijos = nHijos * sizeof(pid_t);
    int idHijos = shmget(IPC_PRIVATE, shmTamHijos, IPC_CREAT | S_IRUSR | S_IWUSR);
    pid_t *vecHijos = (pid_t *) shmat(idHijos, 0, 0);
    

    //CERRAR LETURA DE ARCHIVO
    fclose(data);
    

    //CALCULAR PORCION DE ESPACIO EN EL VECTOR DE DATOS PARA CADA HIJO
    int particion = (int)(tamData / nHijos);
    int restante = tamData - (particion * nHijos);


    printf("----------------------------------------------------------\n");
    //CREAR PROCESOS HIJOS
    int i, status;
    for (i = 0; i < nHijos; i++){
        vecHijos[i] = fork();
        if (vecHijos[i] == 0){
            break;
        }
    }

    if (i == nHijos){ //PADRE
        long int suma = 0;

        for (int k = 0; k < nHijos; k++){ //ESPERO A QUE TODOS LOS HIJOS HAYAN TERMINADO DE GUARDAR SUS RESULTADOS
            waitpid(vecHijos[k], &status, 0);
        }

        for (int j = 0; j < nHijos; j++){
            suma += vecCajasHijos[j];
        }
        printf("----------------------------------------------------------\n");
        printf("Soy el padre con pid[%d]\nEl valor de la suma de los resultados de mis hijos -> %ld\n", getpid(), suma);
        
        shmdt(vecCajasHijos);
        shmctl(idCajasHijos, IPC_RMID, 0);
        shmdt(vecData);
        shmctl(idData, IPC_RMID, 0);
        shmdt(vecHijos);
        shmctl(idHijos, IPC_RMID, 0);

    }else{//HIJOS

        int inicio = i * particion;
        int fin =  (i+1) * particion; //CALCULAR DESDE DONDE HASTA DONDE VA A TOMAR DATOS DEL VECTOR PARA SUMARLOS
        long int suma = 0;
        if (i == (nHijos-1)){
            for (int k = inicio; k < fin + restante; k++){
                suma += vecData[k];
            }
            
        }else{
            for (int k = inicio; k < fin + restante; k++){
                suma += vecData[k];
            }
        }
        printf("Calculo hijo pid[%d] -> %ld\n", getpid(), suma);
        vecCajasHijos[i] = suma;
        shmdt(vecCajasHijos);
        shmdt(vecData);
        shmdt(vecHijos);
    }


    return EXIT_SUCCESS;
}