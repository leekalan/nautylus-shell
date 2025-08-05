#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../util/util.h"

#include "parse.h"

int check_tokens(char *input, size_t expected_length, char **expected) {
    TokenList tokens = token_list_parse(input);

    if (tokens.count != expected_length) {
        fprintf(stderr, "Expected %ld tokens, got %ld\n", expected_length, tokens.count);
        return 1;
    }

    for (size_t i = 0; i < tokens.count; i++) {
        if (strcmp(tokens.tokens[i], expected[i]) != 0) {
            fprintf(stderr, "Expected %s, got %s\n", expected[i], tokens.tokens[i]);
            return 1;
        }
    }

    token_list_free(tokens);
    return 0;
}

int main(int argc, char **argv) {
    start_test_env(argc, argv);

    run_test(check_tokens(
        "cd folder",
        2,
        (char *[]){ "cd", "folder" }
    ), "simple command");

    run_test(check_tokens(
        "cd folder; ls",
        4,
        (char *[]){ "cd", "folder", ";", "ls" }
    ), "command with semicolon");

    run_test(check_tokens(
        ";cd folder;hello;;world;",
        9,
        (char *[]){ ";", "cd", "folder", ";", "hello", ";", ";", "world", ";" }
    ), "sequential semicolons");

    run_test(check_tokens(
        " \tcd folder\n ",
        2,
        (char *[]){ "cd", "folder" }
    ), "excess whitespace");

    return end_test_env();
}
