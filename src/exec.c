#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "exec.h"

int change_dir(const char* path) {
    if (chdir(path) != 0) {
        perror("cd");
        return 1;
    }
    return 0;
}

int run_command_blocking(char *filename, int argc, char *argv[]) {
    char **command_argv = malloc((argc + 1) * sizeof(char *));
    for (int i = 0; i < argc; i++) {
        command_argv[i] = argv[i];
    }
    command_argv[argc] = NULL;

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        execvp(filename, command_argv);
        // execvp should never return
        perror("execvp");
        _exit(127);
    } else {
        // Parent
        waitpid(pid, NULL, 0);
        return 0;
    }
}

void process_tokens(int argc, char *argv[]) {
    int curr_arg = 0;

    while (curr_arg < argc) {
        int end = curr_arg;
        while (end < argc && strcmp(argv[end], ";") != 0) {
            end += 1;
        }

        if (end == curr_arg) {
            curr_arg += 1;
            continue;
        }

        if (strcmp(argv[curr_arg], "cd") == 0) {
            if (end - curr_arg > 2) {
                fprintf(stderr, "cd: too many arguments\n");
            } else if (end - curr_arg == 2) {
                change_dir(argv[curr_arg + 1]);
            } else {
                fprintf(stderr, "cd: missing argument\n");
            }
            curr_arg = end + 1;
            continue;
        }

        char *full_path = NULL;

        if (strchr(argv[curr_arg], '/')) {
            full_path = realpath(argv[curr_arg], NULL);
        } else {
            char *path = strdup(getenv("PATH"));
            char *p = strtok(path, ":");
            while (p) {
                char try_path[512];
                snprintf(try_path, sizeof(try_path), "%s/%s", p, argv[curr_arg]);
                if (access(try_path, X_OK) == 0) {
                    full_path = realpath(try_path, NULL);
                    break;
                }
                p = strtok(NULL, ":");
            }
        }

        if (full_path) {
            run_command_blocking(full_path, end - curr_arg, &argv[curr_arg]);
        } else {
            fprintf(stderr, "Unkown command: %s\n", argv[curr_arg]);
        }

        curr_arg = end + 1;
    }
}
