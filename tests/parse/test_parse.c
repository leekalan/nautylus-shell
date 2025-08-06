#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test_util.h"

#include "parse.h"

int check_tokens(char *input, size_t expected_length, char **expected) {
    printf("Input: %s\n", input);

    TokenList tokens = token_list_parse(input);

    if (tokens.count != expected_length) {
        fprintf(err, "Expected %ld tokens, got %ld\n", expected_length,
                tokens.count);
        return 1;
    }

    for (size_t i = 0; i < expected_length; i++) {
        if (strcmp(tokens.tokens[i], expected[i]) != 0) {
            fprintf(err, "Expected %s, got %s (token %ld)\n", expected[i],
                    tokens.tokens[i], i);
            return 1;
        }
    }

    token_list_free(tokens);
    return 0;
}

int main(int argc, char **argv) {
    start_test_env(argc, argv);

    run_test(check_tokens("cd folder", 2, (char *[]){"cd", "folder"}),
             "simple command");

    run_test(
        check_tokens("cd folder; ls", 4, (char *[]){"cd", "folder", ";", "ls"}),
        "command with semicolon");

    run_test(check_tokens(";cd hello;;ls;", 7,
                          (char *[]){";", "cd", "hello", ";", ";", "ls", ";"}),
             "sequential semicolons");

    run_test(check_tokens(" \tcd folder\n ", 2, (char *[]){"cd", "folder"}),
             "excess whitespace");

    run_test(check_tokens("ls ab\\;\\;c", 2, (char *[]){"ls", "ab;;c"}),
             "escape semicolons");

    run_test(check_tokens("ls ab\\ \\ c", 2, (char *[]){"ls", "ab  c"}),
             "escape spaces");

    run_test(check_tokens("ls \"a b c \"", 2, (char *[]){"ls", "a b c "}),
             "double quotes");

    run_test(check_tokens("ls \"a\\\"\"", 2, (char *[]){"ls", "a\""}),
             "double quotes support escaping");

    run_test(check_tokens("ls \"abc", 2, (char *[]){"ls", "abc"}),
             "unfinished double quotes");

    run_test(check_tokens("ls \'a b c \'", 2, (char *[]){"ls", "a b c "}),
             "single quotes");

    run_test(check_tokens("ls \'a\\b\'", 2, (char *[]){"ls", "a\\b"}),
             "single quotes don't support escaping");

    run_test(check_tokens("ls \'abc", 2, (char *[]){"ls", "abc"}),
             "unfinished single quotes");

    return end_test_env();
}
