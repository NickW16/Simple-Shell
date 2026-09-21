#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "shell.h"

int main(void) {
	char line[MAX_LINE]; // buffer for typing
	char *argv[MAX_ARGS + 1]; // array of word pointers

	for (;;) {
		fputs("shellzinha> ", stdout); // print prompt
		fflush(stdout);

		if (!fgets(line, sizeof line, stdin)) {
			putchar('\n');		// user pressed Ctrl-D
			break;
		}
	

		line[strcspn(line, "\n")] = '\0'; // finds \n and replaces with \0
													 
		int argc = tokenize(line, argv);
		if (argc == 0) continue;

		// built-in commands:
		if(strcmp(argv[0], "exit") == 0) break; // if user types "exit", quit
		if(strcmp(argv[0], "pwd") == 0) {
			char buf[1024];

			if(getcwd(buf, sizeof buf) == NULL) {
				perror("pwd");
				continue;
			}
			printf("%s\n", buf);
		}

		if(strcmp(argv[0], "cd") == 0) {
			if(argc == 1) {
				char *env_value = getenv("HOME");
				if (env_value == NULL) { // bug fix incase home is not set
					fprintf(stderr, "cd: HOME not set\n");
					continue;
				}
				if (chdir(env_value) == 0) {
					printf("Changed directory to %s\n", env_value);	
					continue;
				} else {
					perror("cd failed");
					continue;
				}
			} else {
				if (chdir(argv[1]) == 0) {
					continue;
				} else { 
					perror("cd failed");
					continue;
				}
			}
		}
		
		// for debug:
		//	for(int i = 0; i < argc; i++) {
		//		printf(" arg[%d] = \"%s\"\n", i, argv[i]);
		//	}
															 
		run_command(argv);
	}
	return 0;
}
