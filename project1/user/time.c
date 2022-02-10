#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    //Create and populate a new argument array w/ all except 'time'
	char *newargarray[(argc-1)];
    int i = 0;
    for ( i = 1; i < argc; i = i+1 ){
        newargarray[i-1] = argv[i];
    }
    
    int start = -1;
    int end = -1;
    int diff = -1;
    
    int pid = fork();
    //Proceed to execute
    if (pid == -1){//fork() failed
        printf("ERROR: fork() was unsuccessful.\n");
        exit(1);
    }
    else if ( pid > 0 ){//parent process
		start = uptime();
		pid = wait((int *) 0);
		end = uptime();
    }
    else {//child process
		exec(newargarray[0], newargarray);
		printf("ERROR: Execution failed, invalid command entered.\n");
		exit(0);
    }
    //Calculate real-time in ticks
    diff = end-start;
    printf("Real-time in ticks: %d\n", diff);
    exit(0);
}
