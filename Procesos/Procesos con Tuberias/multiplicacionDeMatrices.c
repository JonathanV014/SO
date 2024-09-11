#include "multiplicaciones.h"

static int **A = NULL;
static int **B = NULL;
static int **C = NULL;

int main(int argc, char const *argv[])
{
    int af, ac, bf, bc, cf, cc;

    printf("Digite las dimensiones de la matriz A (Filas X Columnas): ");
    scanf("%d %d", &af , &ac);

    printf("Digite las dimensiones de la matriz B (Filas X Columnas): ");
    scanf("%d %d", &bf , &bc);

    if (validarMultiplicacion(ac, bf) == 0)
    {
        printf("No se pueden multiplicar las matrices\n");
        exit(1);
    }

    cf = af;
    cc = bc;

    A = reservarEspacioMatriz(af, ac);
    generarMatriz(&A, af, ac);
    printf("MATRIZ A\n");
    mostrarMatriz(A, af, ac);

    B = reservarEspacioMatriz(bf, bc);
    generarMatriz(&B, bf, bc);
    printf("MATRIZ B\n");
    mostrarMatriz(B, bf, bc);

    C =  reservarEspacioMatriz(cf, cc);

    pid_t hijos[3];
    int i, status, comunicacion[2];
    pipe(comunicacion);
    for (i = 0; i < 3; i++){
        hijos[i] = fork();
        if (hijos[i] == 0){
            break;
        }
    }

    if (i != 3){ 
        
        if (i == 0){ // i > j HIJO 1
            close(comunicacion[0]);

            for (int i = 0; i < af; i++){
                for (int j = 0; j < bc; j++){
                    if (i > j){
                        int val = 0;
                        for (int k = 0; k < ac; k++){
                            val = (val + (A[i][k]*B[k][j]));
                        }
                        struct Result resultado = {i, j, val, getpid()};
                        write(comunicacion[1], &resultado, sizeof(struct Result));
                    }
                }
            }
            
            close(comunicacion[1]);
        }
        if (i == 1){ // i == j
            close(comunicacion[0]);

            int iterador = 0;
            for (int i = 0; i < af; i++){
                for (int j = 0; j < bc; j++){
                    if (i == j){
                        int val = 0;
                        for (int k = 0; k < ac; k++){
                            val = (val + (A[i][k]*B[k][j]));
                        }
                        struct Result resultado = {i, j, val, getpid()};
                        write(comunicacion[1], &resultado, sizeof(struct Result));
                    }
                }
            }

            close(comunicacion[1]);
        }
        if (i == 2){ // i < j
            close(comunicacion[0]);

            for (int i = 0; i < af; i++){
                for (int j = 0; j < bc; j++){
                    if (i < j){
                        int val = 0;
                        for (int k = 0; k < ac; k++){
                            val = (val + (A[i][k]*B[k][j]));
                        }
                        struct Result resultado = {i, j, val, getpid()};
                        write(comunicacion[1], &resultado, sizeof(struct Result));
                    }
                }
            }

            close(comunicacion[1]);
        }
    }else{
        close(comunicacion[1]);
        printf("C =  A x B\nRESULTADOS TUBERIAS HIJOS\n");
        
        for (int i = 0; i < 3; i++){
            int total = totalElementosA(i, cf, cc);
            for (int j = 0; j < total; j++){
                waitpid(hijos[i], &status, 0); 
                struct Result resultado;
                read(comunicacion[0], &resultado, sizeof(struct Result));
                printf("Hijo (PID %d) = i: %d, j: %d, valor: %d\n", resultado.pid, resultado.x, resultado.y, resultado.value);
                C[resultado.x][resultado.y] = resultado.value;
            }
        }

        printf("\nSoy el padre con (PID %d), resultado de:\nMATRIZ C\n", getpid());
        mostrarMatriz(C, cf, cc);

        close(comunicacion[0]);
    }
    
    free(A);
    free(B);
    free(C);

    return 0;
}
