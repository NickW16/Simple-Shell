#ifndef SHELL_H
#define SHELL_H

#include <stddef.h>

#define MAX_LINE 1024
#define MAX_ARGS 32

int tokenize(char *line, char **argv);

int run_command(char **argv);

#endif
