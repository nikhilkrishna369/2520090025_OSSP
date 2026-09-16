#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

volatile sig_atomic_t sigint_received = 0;
volatile sig_atomic_t sigterm_received = 0;
volatile sig_atomic_t sigusr1_received = 0;

/* SIGINT handler */
void handle_sigint(int sig) {
    sigint_received = 1;
}

/* SIGTERM handler */
void handle_sigterm(int sig) {
    sigterm_received = 1;
}

/* SIGUSR1 handler */
void handle_sigusr1(int sig) {
    sigusr1_received = 1;
}

int main() {
    struct sigaction sa_int;
    struct sigaction sa_term;
    struct sigaction sa_usr1;

    /* Initialize signal action structures */
    sigemptyset(&sa_int.sa_mask);
    sigemptyset(&sa_term.sa_mask);
    sigemptyset(&sa_usr1.sa_mask);

    sa_int.sa_flags = 0;
    sa_term.sa_flags = 0;
    sa_usr1.sa_flags = 0;

    sa_int.sa_handler = handle_sigint;
    sa_term.sa_handler = handle_sigterm;
    sa_usr1.sa_handler = handle_sigusr1;

    /* Register SIGINT */
    if (sigaction(SIGINT, &sa_int, NULL) == -1) {
        perror("sigaction SIGINT");
        exit(EXIT_FAILURE);
    }

    /* Register SIGTERM */
    if (sigaction(SIGTERM, &sa_term, NULL) == -1) {
        perror("sigaction SIGTERM");
        exit(EXIT_FAILURE);
    }

    /* Register SIGUSR1 */
    if (sigaction(SIGUSR1, &sa_usr1, NULL) == -1) {
        perror("sigaction SIGUSR1");
        exit(EXIT_FAILURE);
    }

    printf("Signal handling program started.\n");
    printf("Process ID: %d\n", getpid());
    printf("Press Ctrl+C to generate SIGINT.\n");

    while (1) {
        sleep(1);

        if (sigint_received) {
            printf("\nSIGINT received! Handling interrupt...\n");
            sigint_received = 0;
        }

        if (sigusr1_received) {
            printf("SIGUSR1 received! User-defined event occurred.\n");
            sigusr1_received = 0;
        }

        if (sigterm_received) {
            printf("SIGTERM received! Terminating program...\n");
            break;
        }

        printf("Program is running...\n");
    }

    printf("Program terminated normally.\n");

    return 0;
}
