#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include "input.h"

static struct termios orig_termios;

void enable_raw_mode(void) {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &orig_termios);
    raw = orig_termios;
    
    // disable canonical mode and echo
    raw.c_lflag &= ~(ICANON | ECHO);
    // min 1 char
    raw.c_cc[VMIN] = 1;
    // no timeout
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disable_raw_mode(void) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

InputBuffer input_buffer_new(void) {
    InputBuffer buf;
    buf.len = 0;
    buf.data = malloc(INPUT_BUFFER_SIZE);
    return buf;
}

void input_buffer_free(InputBuffer buf) {
    free(buf.data);
}

void reset_input_buffer(InputBuffer *buf) {
    buf->len = 0;
}

int update_input_buffer(InputBuffer *buf) {
    char c;
    ssize_t nread = read(STDIN_FILENO, &c, 1);

    if (nread != 1) {
        return -1;
    }

    if (c == '\x1b') {
        // Escape sequence: try to read the next two bytes
        char seq[2];
        if (read(STDIN_FILENO, &seq[0], 1) != 1) return -1;

        // Handle arrow keys
        if (seq[0] != '[') {
            return 0;
        }

        if (read(STDIN_FILENO, &seq[1], 1) != 1) return -1;

        switch (seq[1]) {
            case 'A': /* up */    return 0;
            case 'B': /* down */  return 0;
            case 'C': /* right */ return 0;
            case 'D': /* left */  return 0;
        }

        return 0; // Unrecognized escape sequence
    } else if (c == 127 || c == '\b') {
        // Backspace
        if (buf->len > 0) {
            buf->len--;
            write(STDOUT_FILENO, "\b \b", 3);
        }

        return 0;
    } else if (c == '\n' || c == '\r') {
        // Enter
        write(STDOUT_FILENO, "\n", 1);
        buf->data[buf->len] = '\0';
        return INPUT_BUFFER_READY;
    }

    if (buf->len == INPUT_BUFFER_SIZE - 1) {
        // Buffer is full
        return 0;
    }

    buf->data[buf->len++] = c;
    write(STDOUT_FILENO, &c, 1);
    return 0;
}
