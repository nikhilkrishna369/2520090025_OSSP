#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_INPUT 1024
#define MAX_ARGS 100

int main()
{
    char input[MAX_INPUT];

    while (1)
    {
        char *args[MAX_ARGS];
        int argc = 0;

        printf("\nshell> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL)
            break;

        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit") == 0)
            break;

        if (strlen(input) == 0)
            continue;

        /* Split input into arguments */
        char *token = strtok(input, " ");

        while (token != NULL && argc < MAX_ARGS - 1)
        {
            args[argc++] = token;
            token = strtok(NULL, " ");
        }

        args[argc] = NULL;

        if (argc == 0)
            continue;

        /* Create child process */
        pid_t pid = fork();

        if (pid < 0)
        {
            perror("fork");
            continue;
        }

        if (pid == 0)
        {
            /* Child process */
            printf("Child PID: %d\n", getpid());
            printf("Executing: %s\n", args[0]);

            execvp(args[0], args);

            /* Executed only if execvp fails */
            perror("execvp");
            exit(EXIT_FAILURE);
        }
        else
        {
            /* Parent process */
            int status;

            printf("Parent waiting for child PID: %d\n", pid);

            if (waitpid(pid, &status, 0) == -1)
            {
                perror("waitpid");
                continue;
            }

            if (WIFEXITED(status))
            {
                printf("Child exited with status: %d\n",
                       WEXITSTATUS(status));
            }
            else if (WIFSIGNALED(status))
            {
                printf("Child terminated by signal: %d\n",
                       WTERMSIG(status));
            }
        }
    }

    printf("\nShell terminated.\n");

    return 0;
}
