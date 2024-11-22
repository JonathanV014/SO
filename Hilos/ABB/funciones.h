#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <ctype.h>

#include <pthread.h>
//Instrucciones de compilación: gcc programa_sincronizado.c -lpthread

#include <sys/shm.h>
#include <sys/stat.h>

