#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

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
    int server_fd, client_fd;
    Request request;
    Response response;

    pid_t pid = getpid();

    /* Create a unique FIFO for this client */
    char client_fifo[100];

    snprintf(client_fifo,
             sizeof(client_fifo),
             "/tmp/client_%d_fifo",
             pid);

    if (mkfifo(client_fifo, 0666) == -1) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    /* Get message from user */
    printf("Enter message: ");
    fgets(request.message, BUFFER_SIZE, stdin);

    request.pid = pid;

    /* Open server FIFO */
    server_fd = open(SERVER_FIFO, O_WRONLY);

    if (server_fd == -1) {
        perror("open server FIFO");
        unlink(client_fifo);
        exit(EXIT_FAILURE);
    }

    /* Send request */
    write(server_fd, &request, sizeof(request));

    close(server_fd);

    printf("Message sent to server.\n");

    /* Wait for server response */
    client_fd = open(client_fifo, O_RDONLY);

    if (client_fd == -1) {
        perror("open client FIFO");
        unlink(client_fifo);
        exit(EXIT_FAILURE);
    }

    read(client_fd, &response, sizeof(response));

    printf("Response from server: %s\n",
           response.message);

    close(client_fd);

    /* Remove client FIFO */
    unlink(client_fifo);

    return 0;
}
