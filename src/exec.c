#include "shell.h"
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int run_command(char **argv) {
	char *redirect_file = NULL;
	int target_fd = -1;

	for (int i = 0; argv[i] != NULL; i++) {
		if (strcmp(argv[i], ">") == 0) {
			if (argv[i + 1] == NULL) { // error in redirect usage
				perror("found redirect with no filename");
				return -1;
			}
			redirect_file = argv[i + 1];
			target_fd = STDOUT_FILENO; // dup
			argv[i] = NULL;
			break;
		}
	}

	pid_t pid = fork();
	if (pid < 0) {
		perror("fork");
		return -1;
	}

	if (pid == 0) {
		if (redirect_file != NULL) {
			int fd = open(redirect_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fd == -1) {
				perror("open");
				_exit(1);
			} else {
				dup2(fd, target_fd);
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
