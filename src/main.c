#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shell.h"

int main(void) {
	char line[MAX_LINE]; // buffer for typing
	char *argv[MAX_ARGS + 1]; // array of word pointers

	for (;;) {
		fputs("shellzinha>", stdout); // print prompt
		fflush(stdout);

		if (!fgets(line, sizeof line, stdin)) {
			putchar('\n');		// user pressed Ctrl-D
			break;
		}
	

		line[strcspn(line, "\n")] = '\0'; // finds \n and replaces with \0
													 
		int argc = tokenize(line, argv);
		if (argc == 0) continue;

		if(strcmp(argv[0], "exit") == 0) break; // if user types "exit", quit

		for(int i = 0; i < argc; i++) {
			printf(" arg[%d] = \"%s\"\n", i, argv[i]);
		}
															 
		run_command(argv);
	}
	return 0;
}
