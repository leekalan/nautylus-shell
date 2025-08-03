#include <stdio.h>
#include <sys/select.h>
#include <unistd.h>

#include "input.h"

int main(void) {
    InputBuffer buf = input_buffer_new();
    enable_raw_mode();

    write(STDOUT_FILENO, "> ", 2);
    while (1) {
        if (update_input_buffer(&buf) == INPUT_BUFFER_READY) {
            printf("Input received: %s\n", buf.data);
            reset_input_buffer(&buf);
            write(STDOUT_FILENO, "> ", 2);
        }
    }

    disable_raw_mode();
    input_buffer_free(buf);
    return 0;
}
