#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>
#include <limits.h>

#include "exec.h"
#include "input.h"
#include "parse.h"

#define HOSTNAME_MAX 256

void write_prompt(int fd) {
    char cwd[PATH_MAX];
    char hostname[HOSTNAME_MAX + 1];
    char *user = getenv("USER");
    char *home = getenv("HOME");

    getcwd(cwd, sizeof(cwd));
    gethostname(hostname, sizeof(hostname));

    // Replace $HOME with ~ if cwd starts with home
    char *display_path = cwd;
    if (home && strncmp(cwd, home, strlen(home)) == 0) {
        // Allocate a buffer for "~ + suffix"
        size_t suffix_len = strlen(cwd + strlen(home));
        size_t total_len = 1 + suffix_len + 1;
        char *tilde_path = malloc(total_len);
        if (tilde_path) {
            tilde_path[0] = '~';
            strcpy(tilde_path + 1, cwd + strlen(home));
            display_path = tilde_path;
        }
    }

    dprintf(fd, "%s@%s:%s$ ", user ? user : "user", hostname, display_path);

    if (display_path != cwd) {
        free(display_path);
    }
}

int main(void) {
    InputBuffer buf = input_buffer_new();
    enable_raw_mode();

    write_prompt(STDOUT_FILENO);
    while (1) {
        if (update_input_buffer(&buf) == INPUT_BUFFER_READY) {
            reset_input_buffer(&buf);

            TokenList tokens = token_list_parse(buf.data);
            process_tokens(tokens.count, tokens.tokens);
            token_list_free(tokens);

            write_prompt(STDOUT_FILENO);
        }
    }

    disable_raw_mode();
    input_buffer_free(buf);
    return 0;
}
