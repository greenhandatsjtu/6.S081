#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
	int fd1[2];
	int fd2[2];
	// create pipes
	if (pipe(fd1) != 0) {
		fprintf(2, "Create pipe error.\n");
		exit(1);
	}
	if (pipe(fd2) != 0) {
		fprintf(2, "Create pipe error.\n");
		exit(1);
	}

	int pid;
	if ((pid = fork()) < 0) {
		fprintf(2, "Fork error.\n");
		exit(1);
	}
	// child
	if (pid == 0) {
		// close pipe write end
		close(fd1[1]);
		close(fd2[0]);
		char c;
		read(fd1[0], &c, 1);
		printf("%d: received ping\n", getpid());
		write(fd2[1], "1", 1);
		close(fd1[0]);
		close(fd2[1]);
	}
		// parent
	else {
		// close pipe read end
		close(fd1[0]);
		close(fd2[1]);
		write(fd1[1], "1", 1);
		char c;
		read(fd2[0], &c, 1);
		printf("%d: received pong\n", getpid());
		close(fd1[1]);
		close(fd2[0]);
	}
	exit(0);
}
