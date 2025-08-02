#ifndef INPUT_H
#define INPUT_H

#include <stddef.h>
#include <termios.h>

typedef struct {
    char *data;
    size_t len;
} InputBuffer;

#define INPUT_BUFFER_SIZE 4096
#define INPUT_BUFFER_READY 1

void enable_raw_mode(void);
void disable_raw_mode(void);
InputBuffer input_buffer_new(void);
void input_buffer_free(InputBuffer buf);
void reset_input_buffer(InputBuffer *buf);
int update_input_buffer(InputBuffer *buf);

#endif // INPUT_H
