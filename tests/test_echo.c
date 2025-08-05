#include <stdio.h>

#include "./util/util.h"

int echo(void);

int echo(void) {
    printf("echo...\n");
    return 0;
}

int main(int argc, char **argv) {
    start_test_env(argc, argv);
    run_test(echo(), "echo");
    return end_test_env();
}
