#ifndef PARSE_H
#define PARSE_H

#include <stddef.h>

#define OPERATOR_COUNT 1

extern const char *operators[OPERATOR_COUNT];

typedef struct {
    char **tokens;
    size_t count;
} TokenList;
void token_list_free(TokenList list);

int is_operator(char c);
/// Returns `NULL` if an invalid operator
const char *match_operator(char *input);

TokenList token_list_parse(char *input);

#endif // PARSE_H
