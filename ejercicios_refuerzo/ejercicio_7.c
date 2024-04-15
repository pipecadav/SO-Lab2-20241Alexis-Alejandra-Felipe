#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <comando>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Almacenar el tiempo de inicio
    struct timeval start, end;
    gettimeofday(&start, NULL);

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        fprintf(stderr, "Failed to create child process: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Este es el proceso hijo
        // Ejecutar el comando especificado
        execvp(argv[1], &argv[1]);

        // Si execvp retorna, significa que hubo un error
//        perror("exec");
        fprintf(stderr, "Failed to execute command: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    } else {
        // Este es el proceso padre
        // Esperar por la culminación del proceso hijo
        int status;
        waitpid(pid, &status, 0);

        // Almacenar el tiempo de finalización
        gettimeofday(&end, NULL);

        // Calcular el tiempo transcurrido en segundos y microsegundos
        double elapsed_time = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;

        // Mostrar el tiempo transcurrido
        printf("Elapsed time: %f seconds\n", elapsed_time);
    }

    return 0;
}
