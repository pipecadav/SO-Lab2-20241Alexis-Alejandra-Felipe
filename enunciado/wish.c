#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_SISE 100 // max size of command

char *mypath[] = {"bli", "bla", "/bin", "", NULL}; // path to search for commands

int main (int argc, char *argv[]) {
    char str[MAX_SIZE];
    char *command_string;
    char *s;
    int fd;

    do {
        printf("wish> ");
        fgets(str, MAX_SIZE, stdin);
        s = strchr(str, '\n');
        if (s != NULL) {
            *s = '\0';
        }

        command_string = strtok(str, " ");
        if (command_string == NULL) {
            continue;
        }

        if (strcmp(command_string, "exit") == 0) {
            break;
        }

        if (strcmp(command_string, "cd") == 0) {
            command_string = strtok(NULL, " ");
            if (command_string == NULL) {
                fprintf(stderr, "cd: expected argument to \"cd\"\n");
            } else {
                if (chdir(command_string) != 0) {
                    perror("cd");
                }
            }
            continue;
        }

        if (strcmp(command_string, "path") == 0) {
            command_string = strtok(NULL, " ");
            if (command_string == NULL) {
                fprintf(stderr, "path: expected argument to \"path\"\n");
            } else {
                mypath[0] = command_string;
            }
            continue;
        }

        if (fork() == 0) {
            for (int i = 0; mypath[i] != NULL; i++) {
                char *path = malloc(strlen(mypath[i]) + strlen(command_string) + 2);
                sprintf(path, "%s/%s", mypath[i], command_string);
                if (access(path, X_OK) == 0) {
                    if (fd != -1) {
                        dup2(fd, 1);
                        close(fd);
                    }
                    execv(path, argv);
                    perror(path);
                    exit(1);
                }
            }
            fprintf(stderr, "%s: command not found\n", command_string);
            exit(1);
        } else {
            wait(NULL);
        }
    } while (1);

    return 0;

}
