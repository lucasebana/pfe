#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>


//#include "provenance.h"


void usage(){
    printf("usage: pause_track_pid program_path\n");
    exit(-1);
}

int main(int argc, char* argv[]){

    if(argc != 2){ //les args ?
        usage();
    }


    const char * my_argv[64] = {argv[1],NULL};


    printf("Lancement de %s\n",argv[1]);

    pid_t   my_pid;
    int status;
    my_pid = fork();
    if (0 == (my_pid)) {
        //printf("pid = %jd\n", (intmax_t) my_pid);
        //kill(getpid(),SIGSTOP);
        //man 7 signal
        //SIGSTOP
        //SIGCONT
        //getpid ici et kill(monpidfils)
        //puis resume dans le parent
            if (-1 == execve(argv[1], (char **)argv , NULL)) {
                    perror("child process execve failed [%m]");
                    return -1;
            }
    }
    else{
        //printf("pid = %jd\n", (intmax_t) my_pid);
        kill(my_pid,SIGSTOP);
        puts("traitement PID");
        kill(my_pid,SIGCONT);
    }
    
    while (0 == waitpid(my_pid , &status , WNOHANG)) {
            //printf(" PID FILS %d",my_pid);
            sleep(1);
    }
    

    printf("%s WEXITSTATUS %d WIFEXITED %d [status %d]\n",
            argv[0], WEXITSTATUS(status), WIFEXITED(status), status);


    return 0;
}