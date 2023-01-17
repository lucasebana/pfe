#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

#include "provenance.h"

int main(int argc,char**argv){

pid_t my_pid;
if(provenance_set_tracked(true)){
	    printf("Failed Tracking, error %d\n", errno);
            printf("%s\n\n",strerror(errno));
      }
      if(provenance_set_propagate(true)){
	    printf("Failed propagate, error %d\n", errno);
            printf("%s\n\n",strerror(errno));
      }
      if (0 == (my_pid = fork())) {
        char *new_argv[3];
      	new_argv[0] = "pkill";
      	new_argv[1] = "buggy";
      	new_argv[2] = NULL;
        
            if (-1 == execve("/usr/bin/pkill", new_argv , NULL)) {
                    perror("child process execve failed [%m]");
                    return -1;
            }
    }
      //system("pkill buggy");

return 0;
}
