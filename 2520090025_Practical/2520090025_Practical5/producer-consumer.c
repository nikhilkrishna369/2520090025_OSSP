#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define NUM_ITEMS 100000

int main() {
    int fd[2];
    pid_t pid;

    if (pipe(fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        /* Parent: Producer */
        close(fd[0]);

        int data;
        struct timespec start, end;

        clock_gettime(CLOCK_MONOTONIC, &start);

        for (int i = 0; i < NUM_ITEMS; i++) {
            data = i;

            if (write(fd[1], &data, sizeof(data)) != sizeof(data)) {
                perror("write");
                exit(EXIT_FAILURE);
            }
        }

        close(fd[1]);

        wait(NULL);

        clock_gettime(CLOCK_MONOTONIC, &end);

        double elapsed =
            (end.tv_sec - start.tv_sec) +
            (end.tv_nsec - start.tv_nsec) / 1e9;

        double throughput =
            (NUM_ITEMS * sizeof(int)) / elapsed;

        printf("\nProducer finished.\n");
        printf("Items transferred : %d\n", NUM_ITEMS);
        printf("Data transferred  : %ld bytes\n",
               (long)NUM_ITEMS * sizeof(int));
        printf("Time taken        : %.6f seconds\n", elapsed);
        printf("Throughput        : %.2f MB/s\n",
               throughput / (1024 * 1024));
    }
    else {
        /* Child: Consumer */
        close(fd[1]);

        int data;
        long sum = 0;

        while (read(fd[0], &data, sizeof(data)) > 0) {
            sum += data;
        }

        close(fd[0]);

        printf("Consumer received %d items.\n", NUM_ITEMS);
        printf("Sum of received data = %ld\n", sum);
    }

    return 0;
}
