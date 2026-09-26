#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_INPUT 1024
#define MAX_ARGS 100

typedef void (*builtin_func)(char **);

void builtin_cd(char **args)
{
    if (args[1] == NULL)
    {
        char *home = getenv("HOME");

        if (home == NULL)
        {
            printf("HOME variable not set.\n");
            return;
        }

        if (chdir(home) != 0)
            perror("cd");
    }
    else
    {
        if (chdir(args[1]) != 0)
            perror("cd");
    }
}

void builtin_pwd(char **args)
{
    char cwd[1024];

    (void)args;

    if (getcwd(cwd, sizeof(cwd)) != NULL)
        printf("%s\n", cwd);
    else
        perror("pwd");
}

void builtin_echo(char **args)
{
    int i = 1;

    while (args[i] != NULL)
    {
        printf("%s", args[i]);

        if (args[i + 1] != NULL)
            printf(" ");

        i++;
    }

    printf("\n");
}

void builtin_help(char **args)
{
    (void)args;

    printf("Built-in commands:\n");
    printf("  cd <directory>  Change directory\n");
    printf("  pwd             Display current directory\n");
    printf("  echo <text>     Display text\n");
    printf("  help            Display this help message\n");
    printf("  exit            Exit the shell\n");
}

void builtin_exit(char **args)
{
    (void)args;
    printf("Shell terminated.\n");
    exit(0);
}

typedef struct
{
    const char *name;
    builtin_func function;
} Builtin;

Builtin builtins[] =
{
    {"cd", builtin_cd},
    {"pwd", builtin_pwd},
    {"echo", builtin_echo},
    {"help", builtin_help},
    {"exit", builtin_exit}
};

#define BUILTIN_COUNT \
    (sizeof(builtins) / sizeof(builtins[0]))

int execute_builtin(char **args)
{
    for (size_t i = 0; i < BUILTIN_COUNT; i++)
    {
        if (strcmp(args[0], builtins[i].name) == 0)
        {
            builtins[i].function(args);
            return 1;
        }
    }

    return 0;
}

int main()
{
    char input[MAX_INPUT];

    printf("Built-in Command Shell\n");
    printf("Type 'help' to see available commands.\n");

    while (1)
    {
        char *args[MAX_ARGS];
        int argc = 0;

        printf("\nshell> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL)
            break;

        input[strcspn(input, "\n")] = '\0';

        if (strlen(input) == 0)
            continue;

        /* Tokenize input */
        char *token = strtok(input, " ");

        while (token != NULL && argc < MAX_ARGS - 1)
        {
            args[argc++] = token;
            token = strtok(NULL, " ");
        }

        args[argc] = NULL;

        if (argc == 0)
            continue;

        /* Check and execute built-in command */
        if (!execute_builtin(args))
        {
            printf("Invalid command: %s\n", args[0]);
            printf("Type 'help' to see available built-in commands.\n");
        }
    }

    return 0;
}
