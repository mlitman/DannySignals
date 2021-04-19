#include<stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include<signal.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/wait.h>


int childPIDS[1000];
int numChildren = 0;

void sig_handler_child(int signum)
{
     printf("Child Received %d\nNow Dying\n", numChildren);
}

void sig_handler_parent(int signum)
{
    printf("Parent Received %d\n", numChildren);
    for(int i = 0; i < numChildren; i++)
    {
        printf("Child PID: %d\n", childPIDS[i]);
        kill(childPIDS[i], SIGUSR1);
    }
    exit(0);
}

int main()
{
    signal(SIGINT,sig_handler_parent);

    while(1)
    {
        printf("Parent spawning a child\n");
        int pid = fork();
        if(pid == 0)
        {
            //child
            printf("Child is born, now waiting....\n");
            signal(SIGUSR1,sig_handler_child);
            pause();
        }
        else
        {
            childPIDS[numChildren] = pid;
            numChildren++;

            printf("parent sleeping for 1 second\n");
            sleep(1); 
        }
    }
    

    
}