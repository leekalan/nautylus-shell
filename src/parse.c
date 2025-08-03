#include <stdlib.h>
#include <string.h>

#include "parse.h"

const char *operators[OPERATOR_COUNT] = {
    ";", // Semicolon
};

void token_list_free(TokenList list) {
    for (size_t i = 0; i < list.count; i++) {
        free(list.tokens[i]);
    }
    free(list.tokens);
}

int is_operator(char c) {
    return strchr(";()*", c) != NULL;
}

const char *match_operator(char *input) {
    for (size_t i = 0; i < OPERATOR_COUNT; i++) {
        // Checking if the input starts with operator[i]
        if (strncmp(input, operators[i], strlen(operators[i])) == 0) {
            return operators[i];
        }
    }

    return NULL;
}

TokenList token_list_parse(char *input) {
    TokenList list;
    list.count = 0;

    // TODO:
    list.tokens = &input;
    return list;
}
