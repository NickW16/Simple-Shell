#include "shell.h"

int tokenize(char *line, char **argv) {
	int argc = 0;
	char *p = line;

	while (*p && argc < MAX_ARGS - 1) {
		while (*p == ' ' || *p == '\t') p++;
		if (!*p) break;
		argv[argc++] = p;
		while (*p && *p != ' ' && *p != '\t') p++;
		if (*p) *p++ = '\0';
	}
	argv[argc] = NULL;
	return argc;
}
