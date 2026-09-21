#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT 1024

/* Token types */
typedef enum {
    TOKEN_WORD,
    TOKEN_PIPE,
    TOKEN_INPUT,
    TOKEN_OUTPUT,
    TOKEN_APPEND,
    TOKEN_END
} TokenType;

/* Token structure */
typedef struct Token {
    TokenType type;
    char *value;
} Token;

/* Token stream */
typedef struct {
    Token *tokens;
    int count;
    int capacity;
} TokenStream;

/* Parse tree node */
typedef struct ParseNode {
    char *value;
    TokenType type;
    struct ParseNode *left;
    struct ParseNode *right;
} ParseNode;


/* ---------- Token Functions ---------- */

void initTokenStream(TokenStream *stream)
{
    stream->count = 0;
    stream->capacity = 10;

    stream->tokens =
        malloc(stream->capacity * sizeof(Token));

    if (stream->tokens == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
}

void resizeTokenStream(TokenStream *stream)
{
    stream->capacity *= 2;

    Token *temp = realloc(
        stream->tokens,
        stream->capacity * sizeof(Token)
    );

    if (temp == NULL) {
        perror("realloc");
        free(stream->tokens);
        exit(EXIT_FAILURE);
    }

    stream->tokens = temp;
}

void addToken(TokenStream *stream,
              TokenType type,
              const char *value)
{
    if (stream->count >= stream->capacity)
        resizeTokenStream(stream);

    stream->tokens[stream->count].type = type;

    stream->tokens[stream->count].value =
        malloc(strlen(value) + 1);

    if (stream->tokens[stream->count].value == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    strcpy(stream->tokens[stream->count].value, value);

    stream->count++;
}

const char *tokenTypeName(TokenType type)
{
    switch (type) {
        case TOKEN_WORD:
            return "WORD";

        case TOKEN_PIPE:
            return "PIPE";

        case TOKEN_INPUT:
            return "INPUT";

        case TOKEN_OUTPUT:
            return "OUTPUT";

        case TOKEN_APPEND:
            return "APPEND";

        case TOKEN_END:
            return "END";

        default:
            return "UNKNOWN";
    }
}


/* ---------- Tokenizer ---------- */

void tokenize(const char *input, TokenStream *stream)
{
    int i = 0;

    while (input[i] != '\0') {

        /* Ignore whitespace */
        if (isspace((unsigned char)input[i])) {
            i++;
            continue;
        }

        /* Pipe */
        if (input[i] == '|') {
            addToken(stream, TOKEN_PIPE, "|");
            i++;
            continue;
        }

        /* Input redirection */
        if (input[i] == '<') {
            addToken(stream, TOKEN_INPUT, "<");
            i++;
            continue;
        }

        /* Output redirection */
        if (input[i] == '>') {

            if (input[i + 1] == '>') {
                addToken(stream, TOKEN_APPEND, ">>");
                i += 2;
            } else {
                addToken(stream, TOKEN_OUTPUT, ">");
                i++;
            }

            continue;
        }

        /* Normal word */
        {
            char buffer[MAX_INPUT];
            int j = 0;

            while (input[i] != '\0' &&
                   !isspace((unsigned char)input[i]) &&
                   input[i] != '|' &&
                   input[i] != '<' &&
                   input[i] != '>') {

                if (j < MAX_INPUT - 1)
                    buffer[j++] = input[i];

                i++;
            }

            buffer[j] = '\0';

            if (j > 0)
                addToken(stream, TOKEN_WORD, buffer);
        }
    }

    addToken(stream, TOKEN_END, "END");
}


/* ---------- Display Tokens ---------- */

void printTokens(TokenStream *stream)
{
    printf("\nToken Stream:\n");

    for (int i = 0; i < stream->count; i++) {
        printf("[%d] %-7s : %s\n",
               i,
               tokenTypeName(stream->tokens[i].type),
               stream->tokens[i].value);
    }
}


/* ---------- Parser Functions ---------- */

ParseNode *createNode(TokenType type, const char *value)
{
    ParseNode *node = malloc(sizeof(ParseNode));

    if (node == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    node->type = type;

    node->value = malloc(strlen(value) + 1);

    if (node->value == NULL) {
        perror("malloc");
        free(node);
        exit(EXIT_FAILURE);
    }

    strcpy(node->value, value);

    node->left = NULL;
    node->right = NULL;

    return node;
}


/* Validate syntax */
int validateSyntax(TokenStream *stream)
{
    int expectingCommand = 1;

    for (int i = 0; i < stream->count - 1; i++) {

        TokenType type = stream->tokens[i].type;

        if (type == TOKEN_WORD) {
            expectingCommand = 0;
        }

        else if (type == TOKEN_PIPE) {

            if (expectingCommand) {
                printf("Syntax Error: Invalid pipe\n");
                return 0;
            }

            expectingCommand = 1;
        }

        else if (type == TOKEN_INPUT ||
                 type == TOKEN_OUTPUT ||
                 type == TOKEN_APPEND) {

            if (expectingCommand) {
                printf("Syntax Error: Redirection without command\n");
                return 0;
            }

            /* A filename must follow redirection */
            if (i + 1 >= stream->count - 1 ||
                stream->tokens[i + 1].type != TOKEN_WORD) {

                printf("Syntax Error: Missing filename after %s\n",
                       stream->tokens[i].value);

                return 0;
            }
        }
    }

    if (expectingCommand) {
        printf("Syntax Error: Command expected\n");
        return 0;
    }

    return 1;
}


/* ---------- Parse Tree ---------- */

ParseNode *buildParseTree(TokenStream *stream)
{
    ParseNode *root = NULL;
    ParseNode *currentCommand = NULL;

    for (int i = 0; i < stream->count - 1; i++) {

        Token *token = &stream->tokens[i];

        if (token->type == TOKEN_WORD) {

            ParseNode *node =
                createNode(TOKEN_WORD, token->value);

            if (root == NULL) {
                root = node;
                currentCommand = node;
            } else {
                currentCommand->right = node;
                currentCommand = node;
            }
        }

        else if (token->type == TOKEN_PIPE) {

            ParseNode *pipeNode =
                createNode(TOKEN_PIPE, "|");

            pipeNode->left = root;

            root = pipeNode;
            currentCommand = NULL;
        }

        else if (token->type == TOKEN_INPUT ||
                 token->type == TOKEN_OUTPUT ||
                 token->type == TOKEN_APPEND) {

            ParseNode *redirect =
                createNode(token->type, token->value);

            redirect->left = root;

            if (i + 1 < stream->count &&
                stream->tokens[i + 1].type == TOKEN_WORD) {

                redirect->right =
                    createNode(
                        TOKEN_WORD,
                        stream->tokens[i + 1].value
                    );

                i++;
            }

            root = redirect;
        }
    }

    return root;
}


/* ---------- Display Parse Tree ---------- */

void printTree(ParseNode *root, int level)
{
    if (root == NULL)
        return;

    for (int i = 0; i < level; i++)
        printf("  ");

    printf("|-- %s\n", root->value);

    printTree(root->left, level + 1);
    printTree(root->right, level + 1);
}


/* ---------- Free Parse Tree ---------- */

void freeTree(ParseNode *root)
{
    if (root == NULL)
        return;

    freeTree(root->left);
    freeTree(root->right);

    free(root->value);
    free(root);
}


/* ---------- Free Tokens ---------- */

void freeTokenStream(TokenStream *stream)
{
    for (int i = 0; i < stream->count; i++)
        free(stream->tokens[i].value);

    free(stream->tokens);

    stream->tokens = NULL;
    stream->count = 0;
}


/* ---------- Main ---------- */

int main(void)
{
    char input[MAX_INPUT];

    printf("Simple Command Tokenizer and Parser\n");
    printf("Enter 'exit' to terminate.\n");

    while (1) {

        TokenStream stream;
        initTokenStream(&stream);

        printf("\nshell> ");

        if (fgets(input, sizeof(input), stdin) == NULL)
            break;

        input[strcspn(input, "\n")] = '\0';

        /* Handle exit */
        if (strcmp(input, "exit") == 0) {
            freeTokenStream(&stream);
            break;
        }

        /* Handle empty command */
        int empty = 1;

        for (int i = 0; input[i] != '\0'; i++) {
            if (!isspace((unsigned char)input[i])) {
                empty = 0;
                break;
            }
        }

        if (empty) {
            printf("Empty command. Nothing to parse.\n");
            freeTokenStream(&stream);
            continue;
        }

        /* Tokenization */
        tokenize(input, &stream);

        /* Debug token output */
        printTokens(&stream);

        /* Syntax validation */
        if (!validateSyntax(&stream)) {
            freeTokenStream(&stream);
            continue;
        }

        printf("\nSyntax: Valid\n");

        /* Build parse tree */
        ParseNode *tree = buildParseTree(&stream);

        printf("\nParse Tree:\n");
        printTree(tree, 0);

        /* Free memory */
        freeTree(tree);
        freeTokenStream(&stream);
    }

    return 0;
}
