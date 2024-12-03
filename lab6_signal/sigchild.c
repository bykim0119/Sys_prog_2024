#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/wait.h>

#define NUM 8

int ccount = 0;

void child_handler1(int sig){
    int status;

    pid_t pid = wait(&status); //
    ccount--;

    fprintf(stdout, "Received signal %d from process %d\n", sig, pid);
}

void child_handler2(int sig){ //큐잉이 안됨.
    fprintf(stdout, "child_handler2 . . .\n");
    int status;
    pid_t pid;
    while((pid = waitpid(-1, &status, WNOHANG)) > 0){ //-1 -> pid를 지정하지 않음. 즉, 죽은 프로세스가 없으면 waitpid, while이 종료
        ccount--;
        fprintf(stdout, "Received signal %d from process %d\n", sig, pid);
    }
}

int main(){
    pid_t pid[NUM];
    int i, status;

    ccount = NUM;

    signal(SIGINT, child_handler1);

    for(i = 0; i < NUM;i++){
        if((pid[i] = fork()) == 0){
            sleep(2);
            exit(0);
        }
    }
    while(ccount > 0) pause();
    return 0;
}