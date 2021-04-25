#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/wait.h>

int *total_children;
int *children_pids;
int parent_hp;

void unregister_shared()
{
  shmdt((void *)children_pids);
  shmdt((void *)total_children);
}
void sig_handler_child_do_damage(int signum)
{
  printf("Child #%d Attacking!\n", getpid());
  kill(getppid(), SIGKILL);
  unregister_shared();
  exit(0);
}

void sig_handler_parent_take_damage(int signum)
{
  printf("I am under attack from my children!!!\n");
  parent_hp = parent_hp - 1;
  if (parent_hp == 0)
  {
    printf("I am dead!!!\n");
    unregister_shared();
    exit(0);
  }
}

int main()
{
  key_t key = 23456;

  // shmget returns an identifier in shmid
  int shmid = shmget(key, 4, 0666 | IPC_CREAT);

  // shmat to attach to shared memory
  total_children = (int *)shmat(shmid, (void *)0, 0);
  *total_children = 0;

  time_t t;
  srand((unsigned)time(&t));

  parent_hp = (rand() % 16) + 5;

  // we create our child array and do that stuff
  key_t pid_key = 12345;
  // this is problematic because it only accounts for the fact that every child will do 1+ damage.
  int pid_shmid = shmget(pid_key, parent_hp * sizeof(int), 0666 | IPC_CREAT);

  // assign our signals
  signal(SIGKILL, sig_handler_parent_take_damage);

  printf("Parent: My current HP is: %d\n", parent_hp);

  while (1)
  {
    int pid = fork();
    if (pid == 0)
    {
      //total_children = (int*) shmat(shmid,(void*)0,0);

      children_pids = (int *)shmat(pid_shmid, (void *)0, 0);

      // we assign all of our signals
      signal(SIGUSR1, sig_handler_child_do_damage);

      // this happens when the total_children is still one less so we don't have to do *total_children - 1 when we grab from the array bucket
      children_pids[*total_children] = getpid();

      // we add us to the total_children value
      *total_children = *total_children + 1;

      // if we can commit the genocide of our mother...
      if (*total_children >= parent_hp)
      {
        // tell everybody to kill
        for (int i = 0; i < *total_children; i++)
        {
          kill(children_pids[i], SIGUSR1);
        }
      }

      pause();
    }
    else
    {
      sleep(2);
      printf("Total Children: %d\n", *total_children);
    }
  }
}