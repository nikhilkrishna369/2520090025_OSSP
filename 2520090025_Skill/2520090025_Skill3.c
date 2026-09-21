#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 16

/* Dynamic input buffer */
typedef struct {
    char *data;
    size_t size;
    size_t capacity;
} Buffer;

/* History linked-list node */
typedef struct Node {
    char *command;
    struct Node *next;
} Node;

/* Command history */
typedef struct {
    Node *head;
    Node *tail;
    Node *current;
} History;

/* ---------- Buffer Functions ---------- */

void initBuffer(Buffer *buffer)
{
    buffer->data = malloc(INITIAL_CAPACITY);

    if (buffer->data == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    buffer->size = 0;
    buffer->capacity = INITIAL_CAPACITY;
    buffer->data[0] = '\0';
}

void resizeBuffer(Buffer *buffer)
{
    size_t newCapacity = buffer->capacity * 2;

    char *temp = realloc(buffer->data, newCapacity);

    if (temp == NULL) {
        perror("realloc");
        free(buffer->data);
        exit(EXIT_FAILURE);
    }

    buffer->data = temp;
    buffer->capacity = newCapacity;
}

void appendChar(Buffer *buffer, char ch)
{
    /* Prevent buffer overflow */
    if (buffer->size + 1 >= buffer->capacity) {
        resizeBuffer(buffer);
    }

    buffer->data[buffer->size++] = ch;
    buffer->data[buffer->size] = '\0';
}

void clearBuffer(Buffer *buffer)
{
    buffer->size = 0;
    buffer->data[0] = '\0';
}

void freeBuffer(Buffer *buffer)
{
    free(buffer->data);
    buffer->data = NULL;
    buffer->size = 0;
    buffer->capacity = 0;
}

/* ---------- History Functions ---------- */

void initHistory(History *history)
{
    history->head = NULL;
    history->tail = NULL;
    history->current = NULL;
}

void addCommand(History *history, const char *command)
{
    Node *newNode = malloc(sizeof(Node));

    if (newNode == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    newNode->command = malloc(strlen(command) + 1);

    if (newNode->command == NULL) {
        perror("malloc");
        free(newNode);
        exit(EXIT_FAILURE);
    }

    strcpy(newNode->command, command);
    newNode->next = NULL;

    if (history->head == NULL) {
        history->head = newNode;
        history->tail = newNode;
    } else {
        history->tail->next = newNode;
        history->tail = newNode;
    }

    history->current = NULL;
}

void showHistory(History *history)
{
    Node *temp = history->head;
    int count = 1;

    printf("\nCommand History:\n");

    while (temp != NULL) {
        printf("%d. %s\n", count++, temp->command);
        temp = temp->next;
    }
}

const char *previousCommand(History *history)
{
    Node *temp;

    if (history->head == NULL)
        return NULL;

    /* Start from the last command */
    if (history->current == NULL) {
        history->current = history->tail;
        return history->current->command;
    }

    /* Find node before current */
    temp = history->head;

    while (temp != NULL && temp->next != history->current)
        temp = temp->next;

    if (temp != NULL)
        history->current = temp;

    return history->current->command;
}

const char *nextCommand(History *history)
{
    if (history->current == NULL)
        return NULL;

    if (history->current->next != NULL) {
        history->current = history->current->next;
        return history->current->command;
    }

    history->current = NULL;
    return "";
}

void freeHistory(History *history)
{
    Node *temp = history->head;

    while (temp != NULL) {
        Node *next = temp->next;

        free(temp->command);
        free(temp);

        temp = next;
    }

    history->head = NULL;
    history->tail = NULL;
    history->current = NULL;
}

/* ---------- Main Function ---------- */

int main(void)
{
    Buffer input;
    History history;

    initBuffer(&input);
    initHistory(&history);

    printf("Simple Command History Program\n");
    printf("Commands:\n");
    printf("  history - display command history\n");
    printf("  up      - previous command\n");
    printf("  down    - next command\n");
    printf("  exit    - terminate program\n");

    while (1) {
        char command[256];

        printf("\nshell> ");

        if (fgets(command, sizeof(command), stdin) == NULL)
            break;

        /* Remove newline */
        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "exit") == 0)
            break;

        if (strcmp(command, "history") == 0) {
            showHistory(&history);
            continue;
        }

        if (strcmp(command, "up") == 0) {
            const char *previous = previousCommand(&history);

            if (previous != NULL) {
                clearBuffer(&input);

                for (size_t i = 0; i < strlen(previous); i++)
                    appendChar(&input, previous[i]);

                printf("Recalled: %s\n", input.data);
            } else {
                printf("No previous command.\n");
            }

            continue;
        }

        if (strcmp(command, "down") == 0) {
            const char *next = nextCommand(&history);

            if (next != NULL) {
                clearBuffer(&input);

                for (size_t i = 0; i < strlen(next); i++)
                    appendChar(&input, next[i]);

                printf("Recalled: %s\n", input.data);
            }

            continue;
        }

        /* Update input buffer */
        clearBuffer(&input);

        for (size_t i = 0; i < strlen(command); i++)
            appendChar(&input, command[i]);

        addCommand(&history, input.data);

        printf("Stored: %s\n", input.data);
    }

    /* Release dynamically allocated memory */
    freeHistory(&history);
    freeBuffer(&input);

    printf("\nMemory released successfully.\n");

    return 0;
}
