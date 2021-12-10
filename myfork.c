#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>

const int ERROR = -1;


void My_fork_line(int num_of_process)
{
  int status = 0;

  for (int i = 0; i < num_of_process; ++i)
  {
      pid_t pid = fork();

      if (pid == 0)
      {
        printf("Child pid: pid = %d, ppid = %d\n", getpid(), getppid());
        return 0;
      }
    }

  for (int i = 0; i < num_of_process; ++i)
    wait(&status);
}

void My_fork_column(int num_of_process)
{
  int status = 0;

  for (int i = 0; i < num_of_process; ++i)
  {
    pid_t pid = fork();

    if (pid != 0)
      wait(&status);

    else if (pid == 0)
    {
      printf("Child pid: pid =  %d, ppid = %d\n", getpid(), getppid());
      return 0;
    }
  }
}

int main(int argc, char** argv)
{
  if (argc < 2 || argc > 2)
  {
    perror("Not available num of arguments!\n");
    return ERROR;
  }
  else
  {
    pid_t parent_pid = getpid();
    printf("Parent pid = %d\n", parent_pid);

    int num_of_process = strtol(argv[1], NULL, 10);

    printf("My fork in line:\n");
    My_fork_line(num_of_process);

    printf("\n\n");

    printf("My fork in column\n");
    My_fork_column(num_of_process);

    return 0;
  }
}
