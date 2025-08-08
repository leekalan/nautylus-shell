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
    if (argc < 1) {
        return;
    }

    // TODO: Handle semicolons

    if (strcmp(argv[0], "cd") == 0) {
        if (argc == 2) {
            change_dir(argv[1]);
        } else {
            fprintf(stderr, "cd: missing argument\n");
        }
        return;
    }

    char *full_path = NULL;

    if (strchr(argv[0], '/')) {
        full_path = realpath(argv[0], NULL);
    } else {
        char *path = strdup(getenv("PATH"));
        char *p = strtok(path, ":");
        while (p) {
            char try_path[512];
            snprintf(try_path, sizeof(try_path), "%s/%s", p, argv[0]);
            if (access(try_path, X_OK) == 0) {
                full_path = realpath(try_path, NULL);
                break;
            }
            p = strtok(NULL, ":");
        }
    }

    if (full_path) {
        run_command_blocking(full_path, argc, argv);
    } else {
        fprintf(stderr, "Unkown command: %s\n", argv[0]);
    }
}
