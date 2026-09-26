#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define MAX_PATH 4096

int main()
{
    char *path;
    char input[100];
    char path_copy[MAX_PATH];
    char full_path[MAX_PATH];

    printf("Enter command name: ");
    scanf("%99s", input);

    /* Retrieve PATH */
    path = getenv("PATH");

    if (path == NULL)
    {
        printf("PATH variable not found.\n");
        return 1;
    }

    /* Copy PATH because strtok modifies the string */
    strncpy(path_copy, path, MAX_PATH - 1);
    path_copy[MAX_PATH - 1] = '\0';

    /* Search each directory */
    char *dir = strtok(path_copy, ":");

    while (dir != NULL)
    {
        snprintf(full_path, sizeof(full_path),
                 "%s/%s", dir, input);

        /* Check whether executable exists */
        if (access(full_path, X_OK) == 0)
        {
            struct stat file_info;

            if (stat(full_path, &file_info) == 0)
            {
                printf("\nCommand found.\n");
                printf("Command: %s\n", input);
                printf("Executable path: %s\n", full_path);

                if (S_ISREG(file_info.st_mode))
                {
                    printf("Type: Regular file\n");
                }

                printf("Execute permission: Yes\n");

                return 0;
            }
        }

        dir = strtok(NULL, ":");
    }

    printf("\nCommand not found: %s\n", input);
    printf("No executable with execute permission was found in PATH.\n");

    return 1;
}
