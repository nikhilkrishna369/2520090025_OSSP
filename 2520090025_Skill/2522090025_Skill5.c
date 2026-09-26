#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT 1024
#define INITIAL_TOKENS 10

typedef enum {
    NORMAL,
    SINGLE_QUOTE,
    DOUBLE_QUOTE
} QuoteMode;

typedef struct {
    char **tokens;
    int count;
    int capacity;
} TokenList;


/* Initialize token list */
void initTokenList(TokenList *list)
{
    list->count = 0;
    list->capacity = INITIAL_TOKENS;

    list->tokens =
        malloc(list->capacity * sizeof(char *));

    if (list->tokens == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
}


/* Resize token list */
void resizeTokenList(TokenList *list)
{
    list->capacity *= 2;

    char **temp = realloc(
        list->tokens,
        list->capacity * sizeof(char *)
    );

    if (temp == NULL) {
        perror("realloc");
        free(list->tokens);
        exit(EXIT_FAILURE);
    }

    list->tokens = temp;
}


/* Add token */
void addToken(TokenList *list, const char *token)
{
    if (list->count >= list->capacity)
        resizeTokenList(list);

    list->tokens[list->count] =
        malloc(strlen(token) + 1);

    if (list->tokens[list->count] == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    strcpy(list->tokens[list->count], token);
    list->count++;
}


/* Get environment variable */
void expandVariable(const char *name,
                    char *output,
                    size_t outputSize)
{
    const char *value = getenv(name);

    if (value == NULL)
        value = "";

    strncat(output, value,
            outputSize - strlen(output) - 1);
}


/* Tokenize input */
int tokenize(const char *input, TokenList *list)
{
    int i = 0;

    while (input[i] != '\0') {

        /* Skip whitespace */
        if (isspace((unsigned char)input[i])) {
            i++;
            continue;
        }

        char token[MAX_INPUT];
        int j = 0;
        QuoteMode mode = NORMAL;

        while (input[i] != '\0') {

            /* Normal mode */
            if (mode == NORMAL) {

                if (isspace((unsigned char)input[i]))
                    break;

                if (input[i] == '\'') {
                    mode = SINGLE_QUOTE;
                    i++;
                    continue;
                }

                if (input[i] == '"') {
                    mode = DOUBLE_QUOTE;
                    i++;
                    continue;
                }

                if (j < MAX_INPUT - 1)
                    token[j++] = input[i];

                i++;
            }

            /* Single quote mode */
            else if (mode == SINGLE_QUOTE) {

                if (input[i] == '\'') {
                    mode = NORMAL;
                    i++;
                    continue;
                }

                /*
                 * Everything inside single quotes
                 * is treated literally.
                 */
                if (j < MAX_INPUT - 1)
                    token[j++] = input[i];

                i++;
            }

            /* Double quote mode */
            else if (mode == DOUBLE_QUOTE) {

                if (input[i] == '"') {
                    mode = NORMAL;
                    i++;
                    continue;
                }

                /* Variable expansion */
                if (input[i] == '$') {

                    char variable[128];
                    int k = 0;

                    i++;

                    while (isalnum((unsigned char)input[i]) ||
                           input[i] == '_') {

                        if (k < (int)sizeof(variable) - 1)
                            variable[k++] = input[i];

                        i++;
                    }

                    variable[k] = '\0';

                    expandVariable(
                        variable,
                        token,
                        sizeof(token)
                    );

                    j = strlen(token);
                    continue;
                }

                if (j < MAX_INPUT - 1)
                    token[j++] = input[i];

                i++;
            }
        }

        /* Unmatched quote */
        if (mode != NORMAL) {
            printf("Syntax Error: Unmatched quote.\n");
            return 0;
        }

        token[j] = '\0';

        if (j > 0)
            addToken(list, token);
    }

    return 1;
}


/* Display tokens */
void printTokens(TokenList *list)
{
    printf("\nParsed Tokens:\n");

    for (int i = 0; i < list->count; i++) {
        printf("Token %d: [%s]\n",
               i + 1,
               list->tokens[i]);
    }
}


/* Free token list */
void freeTokenList(TokenList *list)
{
    for (int i = 0; i < list->count; i++)
        free(list->tokens[i]);

    free(list->tokens);

    list->tokens = NULL;
    list->count = 0;
}


/* Main function */
int main(void)
{
    char input[MAX_INPUT];

    printf("Single and Double Quote Parser\n");
    printf("Enter 'exit' to terminate.\n");

    while (1) {

        TokenList list;
        initTokenList(&list);

        printf("\nshell> ");

        if (fgets(input, sizeof(input), stdin) == NULL)
            break;

        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit") == 0) {
            freeTokenList(&list);
            break;
        }

        if (strlen(input) == 0) {
            printf("Empty command.\n");
            freeTokenList(&list);
            continue;
        }

        if (tokenize(input, &list)) {
            printTokens(&list);
        }

        freeTokenList(&list);
    }

    return 0;
}
