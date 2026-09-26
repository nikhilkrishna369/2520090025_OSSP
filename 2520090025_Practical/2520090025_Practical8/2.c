#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define SIZE (10 * 1024 * 1024)

void show_memory(const char *process)
{
    char command[100];

    printf("\nMemory information for %s (PID = %d):\n",
           process, getpid());

    snprintf(command, sizeof(command),
             "grep -E 'VmSize|VmRSS' /proc/%d/status",
             getpid());

    system(command);
}

int main()
{
    char *memory;
    pid_t pid;

    /* Allocate memory */
    memory = (char *)malloc(SIZE);

    if (memory == NULL)
    {
        perror("malloc");
        return 1;
    }

    /* Initialize memory */
    for (size_t i = 0; i < SIZE; i++)
        memory[i] = 'A';

    printf("Parent process started.\n");
    printf("Parent PID: %d\n", getpid());

    show_memory("Parent before fork");

    /* Create child process */
    pid = fork();

    if (pid < 0)
    {
        perror("fork");
        free(memory);
        return 1;
    }

    if (pid == 0)
    {
        /* Child process */
        printf("\nChild process created.\n");
        printf("Child PID: %d\n", getpid());

        show_memory("Child before modification");

        printf("\nChild is modifying the allocated memory...\n");

        /* Modify shared memory */
        for (size_t i = 0; i < SIZE; i++)
            memory[i] = 'B';

        show_memory("Child after modification");

        printf("\nChild memory modification completed.\n");

        free(memory);
        exit(0);
    }
    else
    {
        /* Parent process */
        wait(NULL);

        show_memory("Parent after child exits");

        free(memory);

        printf("\nParent process completed.\n");
    }

    return 0;
}
