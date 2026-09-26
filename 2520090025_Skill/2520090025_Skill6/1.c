#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT 1024
#define MAX_TOKENS 100

void parse_input(char *input)
{
    char *tokens[MAX_TOKENS];
    char token[MAX_INPUT];

    int token_count = 0;
    int token_pos = 0;
    int escaped = 0;

    for (int i = 0; input[i] != '\0'; i++)
    {
        char ch = input[i];

        if (escaped)
        {
            token[token_pos++] = ch;
            escaped = 0;
        }
        else if (ch == '\\')
        {
            escaped = 1;
        }
        else if (isspace((unsigned char)ch))
        {
            if (token_pos > 0)
            {
                token[token_pos] = '\0';

                tokens[token_count] = malloc(strlen(token) + 1);
                strcpy(tokens[token_count], token);

                token_count++;
                token_pos = 0;
            }
        }
        else
        {
            token[token_pos++] = ch;
        }
    }

    if (escaped)
    {
        printf("Warning: trailing escape character.\n");
    }

    if (token_pos > 0)
    {
        token[token_pos] = '\0';

        tokens[token_count] = malloc(strlen(token) + 1);
        strcpy(tokens[token_count], token);

        token_count++;
    }

    printf("\nParsed Tokens:\n");

    for (int i = 0; i < token_count; i++)
    {
        printf("Token %d: [%s]\n", i + 1, tokens[i]);
        free(tokens[i]);
    }

    printf("Total tokens: %d\n", token_count);
}

int main()
{
    char input[MAX_INPUT];

    printf("Escape Sequence Parser\n");
    printf("Type 'exit' to terminate.\n");

    while (1)
    {
        printf("\nparser> ");

        if (fgets(input, sizeof(input), stdin) == NULL)
            break;

        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit") == 0)
            break;

        if (strlen(input) == 0)
        {
            printf("Empty command.\n");
            continue;
        }

        parse_input(input);
    }

    printf("Parser terminated.\n");

    return 0;
}
