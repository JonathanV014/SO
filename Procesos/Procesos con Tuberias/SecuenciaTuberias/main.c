#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

void showtree() {
    char cmd[40] = {""};
    sprintf(cmd, "pstree -cAlp %d", getpid()); // Formatea el comando con el PID del proceso actual.
    system(cmd); // Ejecuta el comando en el shell.
}
char* imprePos(int quien){
    char *text;
    if (quien == 1){
        text = "a-";
    }
    if (quien == 2){
        text = "d-";
    }
    if (quien == 3){
        text = "g-";
    }
    if (quien == 4){
        text = "i-";
    }
    if (quien == 5){
        text = "c-";
    }
    if (quien == 6){
        text = "f-";
    }
    if (quien == 7){
        text = "b-";
    }
    if (quien == 8){
        text = "e-";
    }
    if (quien == 9){
        text = "h-";
    }
    return text;
}

int main(int argc, char const *argv[]){
    int nTuberias = 14;
    pid_t pidHijosNivel1[3];
    pid_t pidHijos0[2];
    pid_t pidHijo1;
    pid_t pidHijos2[2];


    int tuberias[nTuberias][2];
    for (int i = 0; i < nTuberias; i++){
        pipe(tuberias[i]);
    }

    int idx;
    for (idx = 0; idx < 3; idx++){
        pidHijosNivel1[idx] = fork();
        if (pidHijosNivel1[idx] == 0){
            if(idx == 0){
                pidHijos0[0] = fork();
                if (pidHijos0[0] == 0){
                    pidHijos0[1] = fork();
                }

            }else if(idx == 1){
                pidHijo1 = fork();
            }else{
                pidHijos2[0] = fork();
                if (pidHijos2[0] == 0){
                    pidHijos2[1] = fork();
                }
            }
            break;
        }
    }
    int quien = -1;
    char mensaje[30] = "";
    if (idx == 3){
        quien = 1;
        for (int i = 0; i < nTuberias; i++){
            if (i != 0 && i != 13){
                close(tuberias[i][0]);
                close(tuberias[i][1]);
            }
        }
        

        strcat(mensaje,  imprePos(quien));

        close(tuberias[0][0]);
        close(tuberias[13][1]);

        write(tuberias[0][1], &mensaje, sizeof(char[30]));
        close(tuberias[0][1]);

        
        
        read(tuberias[13][0], &mensaje, sizeof(char[30]));
        strcat(mensaje, "a");
        printf("%s\n", mensaje);
        close(tuberias[13][0]);
    }else{
        int lectura1 = -1, escritura1 = -1, lectura2 = -1, escritura2 = -1;

        if (idx == 0){
            if (pidHijos0[0] == 0){
                if (pidHijos0[1] == 0){ //HOJA
                    lectura1 = 10;
                    escritura1 = 11;
                    quien = 9;
                }else{
                    lectura1 = 9;
                    escritura1 = 10;
                    lectura2 = 11;
                    escritura2 = 12;
                    quien = 8;
                }
            }else{
                lectura1 = 8;
                escritura1 = 9;
                lectura2 = 12;
                escritura2 = 13;
                quien = 7;

            } 
        }else if(idx == 1){ //HOJA
            if (pidHijo1 == 0){
                lectura1 = 6;
                escritura1 = 7;
                quien = 6;
            }else{
                lectura1 = 5;
                escritura1 = 6;
                lectura2 = 7;
                escritura2 = 8;
                quien = 5;
            }
        }else{
            if (pidHijos2[0] == 0){
                if (pidHijos2[1] == 0){ //HOJA
                    lectura1 = 2;
                    escritura1 = 3;
                    quien = 4;

                }else{
                    lectura1 = 1;
                    escritura1 = 2;
                    lectura2 = 3;
                    escritura2 = 4;
                    quien = 3;
                }
            }else{
                lectura1 = 0;
                escritura1 = 1;
                lectura2 = 4;
                escritura2 = 5;
                quien = 2;
            }
        }
       

        for (int i = 0; i < nTuberias; i++){
            if (lectura2 == -1 && escritura2 == -1){
                if (i != lectura1 && i != escritura1){
                    close(tuberias[i][0]);
                    close(tuberias[i][1]);
                }
            }else{
                if (i != lectura1 && i != escritura1 && i != lectura2 && i != escritura2){
                    close(tuberias[i][0]);
                    close(tuberias[i][1]);
                }
            }
        }
        close(tuberias[lectura1][1]);
        close(tuberias[escritura1][0]);
        if (lectura2 != -1 && escritura2 != -1){
            close(tuberias[lectura2][1]);
            close(tuberias[escritura2][0]);
        }
        

        read(tuberias[lectura1][0], &mensaje, sizeof(char[30]));
        close(tuberias[lectura1][0]);

        strcat(mensaje, imprePos(quien));
        

        write(tuberias[escritura1][1], &mensaje, sizeof(char[30])); //ESCRITURA 1
        close(tuberias[escritura1][1]);

        if (lectura2 != -1 && escritura2 != -1){

            //LECTURA 2
            read(tuberias[lectura2][0], &mensaje, sizeof(char[30]));
            close(tuberias[lectura2][0]);
            strcat(mensaje, imprePos(quien));


            write(tuberias[escritura2][1], &mensaje, sizeof(char[30])); //ESCRITURA 2
            close(tuberias[escritura2][1]);
        }
        
    }
    return EXIT_SUCCESS;
}
