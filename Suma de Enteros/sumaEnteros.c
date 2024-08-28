#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include <math.h>

struct masRepetido{
    long int numero;
    int frecuencia;
};

FILE *abrirArchivo( char *);
long int suma(int, int *);
double var(int, int*, double);
double median(int, int *);
void copiar(int ,int *, int *);
void quickSort(int, int, int *);
void mostrarArray(int, int *);
struct masRepetido maxRep(int, int *, int*);

int main(int argc, char *argv[]){

    //LEER ARCHIVO
    double readingTime, sumTime, varTime, desvEsTime, medianaTime, numFrec;
    struct timespec start, end;

    int size;
    int *array = NULL;

    //---------------------------------------------------------LECTURA DE ARCHIVO
    clock_gettime(CLOCK_REALTIME, &start); //INICIO TIEMPO

    /*Tiempo gastado en Apertura, lectura del primer valor, 
    reserva de memoria y carga de los N enteros*/

    FILE *file = abrirArchivo(argv[1]);

    fscanf(file, "%d", &size);
    array =(int *)malloc(size * sizeof(int));

    for (int i = 0; i < size; i++){
        fscanf(file, "%d", &array[i]);
    }
    
    clock_gettime(CLOCK_REALTIME, &end); //FIN TIEMPO
    readingTime = (end.tv_sec - start.tv_sec) + ((end.tv_nsec - start.tv_nsec)/(1000000000.0));
    printf("Archivo [%s] abierto\n", argv[1]);
    printf("Amount_of_data_read: %d\n", size);
    printf("reading_time: %lf seg\n", readingTime);

    //---------------------------------------------------------SUMA DE TODOS LOS NUMEROS
    clock_gettime(CLOCK_REALTIME, &start); //INICIO TIEPO

    /*Tiempo usado en la suma*/
    long int sum = suma(size, array);

    clock_gettime(CLOCK_REALTIME, &end); //FIN TIEMPO
    sumTime = (end.tv_sec - start.tv_sec) + ((end.tv_nsec - start.tv_nsec)/(1000000000.0));
    printf("sum_time: %lf seg\n", sumTime);
    printf("sum: %ld\n", sum);


    //---------------------------------------------------------VARIANZA
    clock_gettime(CLOCK_REALTIME, &start); //INICIO TIEPO

    /*Tiempo usado calculando la varianza */
    double media = suma(size, array) / size;
    double varianza = var(size, array, media);

    clock_gettime(CLOCK_REALTIME, &end); //FIN TIEMPO
    varTime = (end.tv_sec - start.tv_sec) + ((end.tv_nsec - start.tv_nsec)/(1000000000.0));
    printf("variance_time: %lf seg\n", varTime);
    printf("variance: %.4lf\n", varianza);


    //---------------------------------------------------------DESVIACION ESTANDAR
    clock_gettime(CLOCK_REALTIME, &start); //INICIO TIEPO
    /*Tiempo usado calculando la desviacion estandar*/
    media = suma(size, array) / size;
    varianza = var(size, array, media);
    double desviacionEstandar = sqrt(varianza);
    clock_gettime(CLOCK_REALTIME, &end); //FIN TIEMPO
    desvEsTime = (end.tv_sec - start.tv_sec) + ((end.tv_nsec - start.tv_nsec)/(1000000000.0));
    printf("standard_deviation_time: %lf seg\n", desvEsTime);
    printf("standard_deviation: %.4lf\n", desviacionEstandar);


    //---------------------------------------------------------MEDIANA
    clock_gettime(CLOCK_REALTIME, &start); //INICIO TIEPO
    /*Tiempo usado calculando la mediana */
    int *copyArray = NULL;
    copyArray = (int *)malloc(size * sizeof(int));
    copiar(size, array, copyArray);
    quickSort(0, size-1, copyArray);
    double mediana = median(size, copyArray);

    clock_gettime(CLOCK_REALTIME, &end); //FIN TIEMPO
    medianaTime = (end.tv_sec - start.tv_sec) + ((end.tv_nsec - start.tv_nsec)/(1000000000.0));
    printf("median_time: %.lf seg\n", medianaTime);
    printf("median: %.2lf\n", mediana);

    //---------------------------------------------------------NUMERO CON MAS FRECUENCIA
    clock_gettime(CLOCK_REALTIME, &start); //INICIO TIEPO
    /*Tiempo usado buscando numero con mas frecuencia*/
    quickSort(0, size-1, copyArray);
    int *frecuencias = NULL;
    struct masRepetido mRep = maxRep(size, copyArray, frecuencias);
    clock_gettime(CLOCK_REALTIME, &end); //FIN TIEMPO
    numFrec = (end.tv_sec - start.tv_sec) + ((end.tv_nsec - start.tv_nsec)/(1000000000.0)); 
    printf("frequency estimation: %.lf seg\n", numFrec);
    printf("The most common value is (%ld, %d)\n", mRep.numero, mRep.frecuencia);

    fclose(file);
    free(array);
    free(copyArray);
    free(frecuencias);


    return EXIT_SUCCESS;
}

struct masRepetido maxRep(int size, int *x, int *frecuencias){

    struct masRepetido maxRep; 
    int maximo = x[size-1]; 
    frecuencias = (int *)malloc(maximo * sizeof(int));

    for (int i = 0; i < size; i++){
        frecuencias[x[i]-1]++;
    }

    int numMayor = 0;
    int frec = 0;
    for (int i = 0; i < maximo; i++){
        if (frecuencias[i] > frec){
            frec = frecuencias[i];
            numMayor = i + 1;
        }
    }

    maxRep.numero = numMayor;
    maxRep.frecuencia = frec;
    return maxRep;
}

void mostrarArray(int size, int *x){
    int limit = 12, itera = 0;
    for (int i = 0; i < size; i++){
        printf("[%ld], ", x[i]);
        itera++;
        if (limit == itera){
            itera = 0;
            printf("\n");
        }
    } 
    printf("\n");
}

void quickSort(int inicio, int fin, int *x){
    int i = inicio;
    int j = fin;
    int pivote = x[(i+j)/2];

    while (i <= j){
        while (x[i] < pivote)i++;
        while (x[j] > pivote)j--;
        if (i<=j){
            int temp = x[i];
            x[i] = x[j];
            x[j] = temp;

            i++;
            j--;   
        }
    }
    if (i < fin){
        quickSort(i, fin, x);
    }
    if (j > inicio){
        quickSort(inicio, j, x);
    }
}

void copiar(int size, int *x, int *copy){
    for (int i = 0; i < size; i++){
        copy[i] = x[i];
    }
}

double median(int size, int *x){
    double mediana = 0;
    if (size % 2 == 0){
        mediana = (x[size/2] + x[(size/2)-1]) / 2;
    }else{
        mediana = x[(size-1)/2];
    } 
    return mediana;
}

double var(int size, int *x, double media){
    double varianza = 0;
    for (int i = 0; i < size; i++){
        varianza += pow((x[i]-media), 2);
    }

    return varianza/size;
}

long int suma(int size, int *x){
    long int sum = 0;
    for (int i = 0; i < size; i++){
        sum += x[i];
    }
    return sum;
}

FILE *abrirArchivo(char *x){
    FILE *file;
    file = fopen(x, "r");
    if (file == NULL){
        printf("Error al abrir el archivo\n");
        exit(1);
    }
    return file;
}