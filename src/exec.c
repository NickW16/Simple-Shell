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
	int out_append_flags = O_WRONLY | O_CREAT | O_APPEND; // for >>
	
	// this is for enabling < and > to be used at the same time
	char *in_file = NULL;
	char *out_file = NULL;
	char *err_file = NULL;
	int out_append = 0; // for >>

	// scan for redirect
	for (int i = 0; argv[i] != NULL; i++) {
		if (strcmp(argv[i], "2>") == 0) {
			if (argv[i + 1] == NULL) {
				perror("found redirect with no filename");
				return -1;
			}
			err_file = argv[i + 1];
			argv[i] = NULL;
		} else if (strcmp(argv[i], ">>") == 0) {
			if (argv[i + 1] == NULL) {  // error
				perror("found append with no filename");
				return -1;
			}
			out_append = 1;
			out_file = argv[i + 1];
			argv[i] = NULL;
		} else if (strcmp(argv[i], ">") == 0) {
			if (argv[i + 1] == NULL) { // error in redirect usage
				perror("found redirect with no filename");
				return -1;
			}
			//redirect_file = argv[i + 1];
			out_append = 0;
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
		if (err_file != NULL) { // 2>
			int fd = open(err_file, out_open_flags, 0644);
			if (fd == -1) {
				perror("open");
				_exit(1);
			} else {
				dup2(fd, STDERR_FILENO);
				close(fd);
			}
		}
		if (in_file != NULL) { // <
			int fd = open(in_file, in_open_flags, 0644);
			if (fd == -1) {
				perror("open");
				_exit(1);
			} else {
				dup2(fd, STDIN_FILENO);
				close(fd);
			}
		}
		if (out_file != NULL) { // >> and >
			int fd = open(out_file, out_append ? out_append_flags : out_open_flags, 0644);
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
