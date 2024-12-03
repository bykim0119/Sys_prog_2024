#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>

#define NUM 4

void sig_int_handler(int sig){
    fprintf(stdout, "Process %d received signal %d\n", getpid(), sig);
    exit(0);
}

int main(){
    pid_t pid[NUM];
    int i, child_status;
    signal(SIGKILL, sig_int_handler);

    for(i=0; i< NUM;i++){
        if((pid[i] = fork()) == 0)
        while(1);
    }
    for(i=0;i < NUM; i++){
        printf("Killing processes\n");
        kill(pid[i], SIGKILL);
    }
    for(i=0;i<NUM;i++){
        pid_t wpid = wait(&child_status);
        if(WIFEXITED(child_status))
            printf("child %d terminated with exit status %d\n", wpid, WEXITSTATUS(child_status));
        else
            printf("Child %d terminated abnormally\n", wpid);
    }
    return 0;
}