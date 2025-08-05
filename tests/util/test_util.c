#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ERROR_TEXT(x) "\033[31m" x "\033[0m"
#define WARNING_TEXT(x) "\033[33m" x "\033[0m"
#define SUCCESS_TEXT(x) "\033[32m" x "\033[0m"

#define TEXT_B(x) "\033[1m" x "\033[0m"
#define ERROR_TEXT_B(x) "\033[1;31m" x "\033[0m"
#define WARNING_TEXT_B(x) "\033[1;33m" x "\033[0m"
#define SUCCESS_TEXT_B(x) "\033[1;32m" x "\033[0m"

#define PURPLE_B(x) "\033[1;35m" x "\033[0m"

int env_flag = 0;

int depth;

int log_pipefd[2];
int err_pipefd[2];

int stdoutfd;

FILE *err;

int test_count;
int err_count;

void start_test_env(int argc, char **argv) {
    if (argc > 1) {
        fprintf(stderr, "ERROR: \"%s\" should not take arguments!\n", argv[0]);
        return;
    }

    if (env_flag != 0) {
        fprintf(stderr, "ERROR: test environment already started\n");
        return;
    }

    fprintf(stdout, PURPLE_B("Running tests for \"%s\":") "\n", argv[0]);

    env_flag = 1;

    depth = 0;
    test_count = 0;
    err_count = 0;

    pipe(log_pipefd);
    pipe(err_pipefd);

    int flags;
    flags = fcntl(log_pipefd[0], F_GETFL, 0);
    fcntl(log_pipefd[0], F_SETFL, flags | O_NONBLOCK);
    flags = fcntl(err_pipefd[0], F_GETFL, 0);
    fcntl(err_pipefd[0], F_SETFL, flags | O_NONBLOCK);

    stdoutfd = dup(STDOUT_FILENO);
    dup2(log_pipefd[1], STDOUT_FILENO);

    err = fdopen(err_pipefd[1], "w");
}

int end_test_env(void) {
    if (env_flag == 0) {
        fprintf(stderr, "ERROR: test environment not started\n");
    }
    env_flag = 0;

    dup2(stdoutfd, STDOUT_FILENO);
    fclose(err);

    close(log_pipefd[0]);
    close(log_pipefd[1]);
    close(err_pipefd[0]);
    close(err_pipefd[1]);

    if (err_count != 0) {
        fprintf(stdout, ERROR_TEXT_B("Tests failed "));
        if (test_count - err_count > test_count / 5) {
            fprintf(stdout,
                    WARNING_TEXT_B("(%d/%d)"
                                   "\n"),
                    test_count - err_count, test_count);
        } else {
            fprintf(stdout,
                    ERROR_TEXT_B("(%d/%d)"
                                 "\n"),
                    test_count - err_count, test_count);
        }
        fflush(stdout);
        return 1;
    } else {
        fprintf(stdout, SUCCESS_TEXT_B("Tests passed (%d/%d)") "\n", test_count,
                test_count);
        fflush(stdout);
        return 0;
    }
}

void print_spaces(int n, FILE *stream) {
    for (int i = 0; i < n; i++) {
        fputc(' ', stream);
    }
}

void run_test(int ret_code, char *name) {
    if (env_flag == 0) {
        fprintf(stderr, "ERROR: test environment not started\n");
        return;
    }

    test_count += 1;
    if (ret_code != 0) {
        err_count += 1;
    }

    depth += 1;

    fflush(stdout);
    dup2(stdoutfd, STDOUT_FILENO);

    print_spaces(depth, stdout);
    printf(TEXT_B("Running test \"%s\":") "\n", name);
    fflush(stdout);

    print_spaces(depth, stdout);
    if (ret_code != 0) {
        fprintf(stdout, ERROR_TEXT_B(" Test failed:") "\n");
    } else {
        fprintf(stdout, SUCCESS_TEXT_B(" Test passed!") "\n");
    }

    fflush(err);
    FILE *read_err = fdopen(err_pipefd[0], "r");
    if (read_err == NULL) {
        fprintf(stderr, "ERROR: failed to open stderr pipe\n");
        exit(1);
    }

    char *lineptr = NULL;
    size_t lineptr_size = 0;
    while (getline(&lineptr, &lineptr_size, read_err) != EOF) {
        print_spaces(depth, stdout);
        fprintf(stdout, "  " ERROR_TEXT("%s"), lineptr);
        free(lineptr);
        lineptr = NULL;
        lineptr_size = 0;
    }

    FILE *read_log = fdopen(log_pipefd[0], "r");
    if (read_log == NULL) {
        fprintf(stderr, "ERROR: failed to open stdout pipe\n");
        exit(1);
    }

    if (ret_code != 0) {
        // Only print debug if the test failed
        print_spaces(depth, stdout);
        fprintf(stdout, " " WARNING_TEXT_B("Output:") "\n");

        lineptr = NULL;
        lineptr_size = 0;
        while (getline(&lineptr, &lineptr_size, read_log) != EOF) {
            print_spaces(depth, stdout);
            fprintf(stdout, "  %s", lineptr);
            free(lineptr);
            lineptr = NULL;
            lineptr_size = 0;
        }
    } else {
        static char discard_buf[4096];
        size_t n;
        while ((n = fread(discard_buf, sizeof(discard_buf), 1, read_log)) > 0)
            ;
    }

    fflush(stdout);
    dup2(log_pipefd[1], STDOUT_FILENO);

    depth -= 1;
}
