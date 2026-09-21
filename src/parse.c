#include "shell.h"

int tokenize(char *line, char **argv) {
	int argc = 0; // argument counts
	char *p = line; 

	while (*p && argc < MAX_ARGS - 1) {
		while (*p == ' ' || *p == '\t') p++;
		if (!*p) break;
		argv[argc++] = p; // increase count
		while (*p && *p != ' ' && *p != '\t') p++;
		if (*p) *p++ = '\0';
	}
	argv[argc] = NULL;
	return argc;

}
