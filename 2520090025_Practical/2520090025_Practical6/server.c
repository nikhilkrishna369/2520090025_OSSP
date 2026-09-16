#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#define SERVER_FIFO "/tmp/server_fifo"
#define BUFFER_SIZE 256

typedef struct {
    pid_t pid;
    char message[BUFFER_SIZE];
} Request;

typedef struct {
    char message[BUFFER_SIZE];
} Response;

int main() {
    int server_fd;
    Request request;
    Response response;

    /* Create the server FIFO */
    if (mkfifo(SERVER_FIFO, 0666) == -1 && errno != EEXIST) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    printf("Server started.\n");
    printf("Waiting for client messages...\n");

    /* Open server FIFO for reading */
    server_fd = open(SERVER_FIFO, O_RDONLY);

    if (server_fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    while (1) {
        ssize_t bytes_read = read(server_fd, &request, sizeof(request));

        if (bytes_read == -1) {
            perror("read");
            break;
        }

        if (bytes_read == 0) {
            /*
             * No writers are currently connected.
             * Reopen the FIFO so the server can accept
             * future clients.
             */
            close(server_fd);
            server_fd = open(SERVER_FIFO, O_RDONLY);
            continue;
        }

        printf("\nMessage from client %d: %s",
               request.pid, request.message);

        /* Process the message */
        snprintf(response.message,
                 BUFFER_SIZE,
                 "Server received: %s",
                 request.message);

        /* Client-specific response FIFO */
        char client_fifo[100];

        snprintf(client_fifo,
                 sizeof(client_fifo),
                 "/tmp/client_%d_fifo",
                 request.pid);

        int response_fd = open(client_fifo, O_WRONLY);

        if (response_fd == -1) {
            perror("open client FIFO");
            continue;
        }

        write(response_fd, &response, sizeof(response));

        close(response_fd);

        printf("Response sent to client %d.\n", request.pid);
    }

    close(server_fd);
    unlink(SERVER_FIFO);

    return 0;
}
