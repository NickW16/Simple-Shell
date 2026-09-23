#include "shell.h"
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int run_command(char **argv) {
	// legacy
	// char *redirect_file = NULL;
	int out_open_flags = O_WRONLY | O_CREAT | O_TRUNC; // kernel flags for file handling
	int in_open_flags = O_RDONLY; // kernel flags for file handling
	
	// this is for enabling < and > to be used at the same time
	char *in_file = NULL;
	char *out_file = NULL;

	// scan for redirect
	for (int i = 0; argv[i] != NULL; i++) {
		if (strcmp(argv[i], ">") == 0) {
			if (argv[i + 1] == NULL) { // error in redirect usage
				perror("found redirect with no filename");
				return -1;
			}
			//redirect_file = argv[i + 1];
			out_file = argv[i + 1];
			argv[i] = NULL;
		} else if (strcmp(argv[i], "<") == 0) {
			if (argv[i + 1] == NULL) {
				perror("found redirect with no filename");
				return -1;
			}
			//redirect_file = argv[i + 1];
			in_file = argv[i + 1];
			argv[i] = NULL;
		}
	}

	// fork for command exec
	pid_t pid = fork();
	if (pid < 0) {
		perror("fork");
		return -1;
	}

	if (pid == 0) {
		if (in_file != NULL) {
			int fd = open(in_file, in_open_flags, 0644);
			if (fd == -1) {
				perror("open");
				_exit(1);
			} else {
				dup2(fd, STDIN_FILENO);
				close(fd);
			}
		}
		if (out_file != NULL) {
			int fd = open(out_file, out_open_flags, 0644);
			if (fd == -1) {
				perror("open");
				_exit(1);
			} else {
				dup2(fd, STDOUT_FILENO);
				close(fd);
			}
		}
		execvp(argv[0], argv);
		perror("execvp");
		_exit(127);
	}

	int status;
	if (waitpid(pid, &status, 0) < 0) {
		perror("waitpid");
		return -1;
	}

	if (WIFEXITED(status)) return WEXITSTATUS(status);
	if (WIFSIGNALED(status)) return 128 + WTERMSIG(status);
	return -1;
}
