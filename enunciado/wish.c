
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_LINE 1024
#define MAX_ARGS 128
#define WHITESPACE " \t\n"

char error_message[30] = "An error has occurred\n";

void print_error() {
    write(STDERR_FILENO, error_message, strlen(error_message));
}

void process_command(char *cmd, char **path);
void run_command(char **args, char **path, int background);
int is_builtin_command(char **args);
void execute_builtin_command(char **args, char **path);
void set_path(char **args, char **path);
void change_directory(char **args);

int main(int argc, char *argv[]) {
    char *cmd = NULL;
    size_t len = 0;
    FILE *input_stream = stdin;

    if (argc == 2) {
        input_stream = fopen(argv[1], "r");
        if (!input_stream) {
            print_error();
            exit(1);
        }
    } else if (argc > 2) {
        print_error();
        exit(1);
    }

    char *path[MAX_ARGS] = {"/bin", NULL};

    while (1) {
        if (argc == 1) {
            printf("wish> ");
        }
        if (getline(&cmd, &len, input_stream) == -1) {
            if (argc == 2) fclose(input_stream);
            exit(0);
        }

        process_command(cmd, path);
        free(cmd);
        cmd = NULL;
    }
}

void process_command(char *cmd, char **path) {
    char *args[MAX_ARGS];
    int argc = 0;
    int background = 0;
    char *token = strtok(cmd, WHITESPACE);

    while (token != NULL) {
        if (strcmp(token, "&") == 0) {
            background = 1;
        } else if (strcmp(token, ">") == 0) {
            token = strtok(NULL, WHITESPACE);
            if (!token || strtok(NULL, WHITESPACE)) {
                print_error();
                return;
            }
            int fd = open(token, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1) {
                print_error();
                return;
            }
            dup2(fd, STDOUT_FILENO);
            dup2(fd, STDERR_FILENO);
            close(fd);
        } else {
            args[argc++] = token;
        }
        token = strtok(NULL, WHITESPACE);
    }
    args[argc] = NULL;

    if (args[0] == NULL) return;  // Empty command

    if (!is_builtin_command(args)) {
        run_command(args, path, background);
    } else {
        execute_builtin_command(args, path);
    }
}

void run_command(char **args, char **path, int background) {
    int status;
    pid_t pid = fork();
    if (pid == 0) {
        char execpath[MAX_LINE];
        int i = 0;
        while (path[i] != NULL) {
            snprintf(execpath, sizeof(execpath), "%s/%s", path[i], args[0]);
            execv(execpath, args);
            i++;
        }
        print_error();
        exit(1);
    } else if (pid > 0) {
        if (!background) {
            waitpid(pid, &status, 0);
        }
    } else {
        print_error();
    }
}

int is_builtin_command(char **args) {
    if (strcmp(args[0], "exit") == 0 ||
        strcmp(args[0], "cd") == 0 ||
        strcmp(args[0], "path") == 0) {
        return 1;
    }
    return 0;
}

void execute_builtin_command(char **args, char **path) {
    if (strcmp(args[0], "exit") == 0) {
        exit(0);
    } else if (strcmp(args[0], "cd") == 0) {
        change_directory(args);
    } else if (strcmp(args[0], "path") == 0) {
        set_path(args, path);
    }
}

void change_directory(char **args) {
    if (args[1] == NULL || args[2] != NULL) {
        print_error();
    } else if (chdir(args[1]) != 0) {
        print_error();
    }
}

void set_path(char **args, char **path) {
    int i = 1;
    int j = 0;
    while (args[i] != NULL) {
        path[j++] = args[i++];
    }
    path[j] = NULL;
}