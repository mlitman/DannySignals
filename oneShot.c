#include<stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include<signal.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/wait.h>

int* total_children;
int parent_hp;

void sig_handler_child_do_damage(int signum)
{
  //need to wait until there an enough of them to all attack
  //the parent at once.
  //all the children pause and there needs to be a centralized
  //collection of child PIDs so that the child that is 
  //born last can send kill signals to all of the rest
  //of the children and raise the signal for themselves.
}

void sig_handler_parent_take_damage(int signum)
{
  printf("I am under attack from my children!!!\n");
  parent_hp = parent_hp - 1;
  if(parent_hp == 0)
  {
    printf("I am dead!!!\n");
    exit(0);
  }
}

int main()
{
  key_t key = ftok("shmfile",65); 
  
  // shmget returns an identifier in shmid 
  int shmid = shmget(key,4,0666|IPC_CREAT); 

  // shmat to attach to shared memory 
  total_children = (int*) shmat(shmid,(void*)0,0);
  *total_children = 0;

  time_t t;
  srand((unsigned) time(&t));
  
  parent_hp = (rand() % 16) + 5; 

  printf("Parent: My current HP is: %d\n", parent_hp);

  while(1)
  {
    int pid = fork();
    if(pid == 0)
    {
      //we are the child
      printf("Child %d\n", shmid);
      //total_children = (int*) shmat(shmid,(void*)0,0);
      *total_children = *total_children + 1;
      pause();
    }
    else
    {
      sleep(2);
      printf("Total Children: %d\n", *total_children);
    }
  }
}