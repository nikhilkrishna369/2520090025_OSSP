#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    pid_t pid;
    int status;

    printf("Parent process started.\n");
    printf("Parent PID: %d\n", getpid());

    pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        /* Child process */
        printf("\nChild process started.\n");
        printf("Child PID: %d\n", getpid());
        printf("Child is performing some work...\n");

        sleep(3);

        printf("Child process completed.\n");

        exit(5);
    }
    else
    {
        /* Parent process */
        printf("\nParent is waiting for child PID: %d\n", pid);

        if (waitpid(pid, &status, 0) == -1)
        {
            perror("waitpid");
            return 1;
        }

        printf("\nParent received child status.\n");

        if (WIFEXITED(status))
        {
            printf("Child exited normally.\n");
            printf("Child exit status: %d\n",
                   WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status))
        {
            printf("Child terminated by signal: %d\n",
                   WTERMSIG(status));
        }

        printf("Parent process completed.\n");
    }

    return 0;
}
