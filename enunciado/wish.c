#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_INPUT 512

int main(int argc, char *argv[]) {
    char *cmd;
    size_t len = 0;
    ssize_t nread;
    FILE *input_stream = stdin;

    // Determine the mode of the shell: interactive or batch
    if (argc == 1) {
        printf("wish> ");  // Only show prompt in interactive mode
    } else if (argc == 2) {
        input_stream = fopen(argv[1], "r");
        if (!input_stream) {
            fprintf(stderr, "An error has occurred\n");
            exit(1);
        }
    } else {
        fprintf(stderr, "An error has occurred\n");
        exit(1);
    }

    // Processing loop
    while ((nread = getline(&cmd, &len, input_stream)) != -1) {
        // Process command here

        if (argc == 1) {  // Only show prompt in interactive mode
            printf("wish> ");
        }
    }

    if (argc == 2) {
        fclose(input_stream);
    }

    return 0;
}

void execute_command(char *cmd) {
    char *args[MAX_INPUT];
    char *token;
    int i = 0;

    token = strtok(cmd, " \n\t");
    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " \n\t");
    }
    args[i] = NULL;  // Null terminate the list of arguments

    // Built-in commands
    if (strcmp(args[0], "exit") == 0) {
        exit(0);
    } else if (strcmp(args[0], "cd") == 0) {
        if (i != 2) {
            fprintf(stderr, "An error has occurred\n");
        } else if (chdir(args[1]) != 0) {
            fprintf(stderr, "An error has occurred\n");
        }
    } else if (strcmp(args[0], "path") == 0) {
        // Handle setting path here
    } else {
        // External commands
        pid_t pid = fork();
        if (pid == 0) {
            if (execvp(args[0], args) == -1) {
                fprintf(stderr, "An error has occurred\n");
                exit(1);
            }
        } else if (pid > 0) {
            wait(NULL);
        } else {
            fprintf(stderr, "An error has occurred\n");
        }
    }
}
