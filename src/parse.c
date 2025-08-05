#include <ctype.h>
#include <stdio.h>
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

int is_operator(char c) { return strchr(";()*", c) != NULL; }

const char *match_operator(char *input) {
    for (size_t i = 0; i < OPERATOR_COUNT; i++) {
        // Checking if the input starts with operator[i]
        if (strncmp(input, operators[i], strlen(operators[i])) == 0) {
            return operators[i];
        }
    }

    return NULL;
}

char *clear_whitespace(const char *input) {
    if (*input == '\0') return NULL;

    while (isspace((unsigned char)*input)) {
        input += 1;
    }

    return (char *)input;
}

char *parse_token(char **input) {
    char *cleared = clear_whitespace(*input);

    if (!cleared || *cleared == '\0') {
        *input = NULL;
        return NULL;
    }

    if (is_operator(*cleared)) {
        const char *const_op = match_operator(cleared);
        char *op = malloc(strlen(const_op) + 1);
        strcpy(op, const_op);
        *input = cleared + strlen(op);
        return (char *)op;
    }

    char *token = malloc(strlen(cleared) + 1);
    size_t index = 0;
    while (*cleared != '\0' && !isspace(*cleared) && !is_operator(*cleared)) {
        if (*cleared == '\\') {
            cleared += 1;
            if (*cleared == '\0') {
                break;
            }
        }

        token[index] = *cleared;
        cleared += 1;
        index += 1;
    }
    token[index] = '\0';
    token = realloc(token, index + 1);

    *input = cleared;
    return token;
}

TokenList token_list_parse(char *input) {
    TokenList list;
    list.count = 0;

    char *token = parse_token(&input);
    if (token == NULL) {
        list.tokens = NULL;
        return list;
    }

    size_t data_cap = 1;
    list.count = 1;
    list.tokens = malloc(sizeof(char *));
    list.tokens[0] = token;
    token = parse_token(&input);
    while (token != NULL) {
        if (list.count == data_cap) {
            data_cap *= 2;
            list.tokens = realloc(list.tokens, data_cap * sizeof(char *));
        }
        list.tokens[list.count] = token;
        list.count += 1;
        token = parse_token(&input);
    }

    list.tokens = realloc(list.tokens, list.count * sizeof(char *));

    return list;
}
