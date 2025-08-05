#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

extern FILE* err;

void start_test_env(int argc, char **argv);
int end_test_env(void);

void run_test(int ret_code, char* name);

#endif
