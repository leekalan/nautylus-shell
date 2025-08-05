#include <stdio.h>
#include <sys/select.h>
#include <unistd.h>

#include "input.h"
#include "parse.h"

void write_prompt(int fd) { write(fd, "> ", 2); }

int main(void) {
    InputBuffer buf = input_buffer_new();
    enable_raw_mode();

    write_prompt(STDOUT_FILENO);
    while (1) {
        if (update_input_buffer(&buf) == INPUT_BUFFER_READY) {
            reset_input_buffer(&buf);

            TokenList tokens = token_list_parse(buf.data);

            // Executor is not implemented yet
            for (size_t i = 0; i < tokens.count; i++) {
                printf("Token[%ld]: %s\n", i, tokens.tokens[i]);
            }

            token_list_free(tokens);

            write_prompt(STDOUT_FILENO);
        }
    }

    disable_raw_mode();
    input_buffer_free(buf);
    return 0;
}
