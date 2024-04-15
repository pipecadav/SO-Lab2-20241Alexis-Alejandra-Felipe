#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command>\n", argv[0]);
        exit(1);
    }

    struct timeval start, end;
    //Capture Start Time
    gettimeofday(&start, NULL);

    //
    pid_t pid = fork();
    if (pid == -1) {
        fprintf(stderr, "Failed to create child process: %s\n", strerror(errno));
        exit(1);

    } else if (pid == 0) {
        execvp(argv[1], &argv[1]);
        fprintf(stderr, "Failed to run command in child process: %s\n", strerror(errno));
        exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0);
        gettimeofday(&end, NULL); // Capture end time

        // Calculate and print the elapsed time in seconds
        double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
        printf("Elapsed Time: %.6f seconds\n", elapsed);
    }

    exit(0);
}