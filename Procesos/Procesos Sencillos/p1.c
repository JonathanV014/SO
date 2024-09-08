#include<sys/types.h>
#include<unistd.h>
#include<stdio.h>

int main(int argc, char const *argv[])
{
    
    pid_t pid;
    pid = fork();
    if (pid == 0){
        printf("Soy el hijo (%d, y mi padre es %d)\n", getpid(), getppid());
    }else{
        printf("Soy el padre (%d, y mi padre es %d)\n", getpid(), getppid());
    }
    
    return 0;
}
