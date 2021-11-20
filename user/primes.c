#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
	int fd[2];
	// create pipe
	if (pipe(fd) != 0) {
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
		child:
		// close pipe write end
		close(fd[1]);

		// dup pipe read end to stdin
		close(0);
		dup(fd[0]);
		close(fd[0]);

		int p;
		if (read(0, &p, sizeof(int)) == 0) {
			exit(0);
		}
		printf("prime %d\n", p);

		// create pipe
		if (pipe(fd) != 0) {
			fprintf(2, "Create pipe error.\n");
			exit(1);
		}

		if ((pid = fork()) < 0) {
			fprintf(2, "Fork error.\n");
			exit(1);
		}
		// child
		if (pid == 0) {
			goto child;
		} else {
			// dup pipe write end to stdout
			close(1);
			dup(fd[1]);
			close(fd[1]);

			int n;
			while (read(0, &n, sizeof(int)))
				// only send to child if p does not divide n
				if (n % p)write(1, &n, sizeof(int));
			close(1);
			close(fd[1]);

			wait(0);
			exit(0);
		}
		exit(0);
	}
		// parent
	else {
		// close pipe read end
		close(fd[0]);
		for (int i = 2; i <= 35; i++) {
			write(fd[1], &i, sizeof(int));
		}
		close(fd[1]);
		wait(0);
	}
	exit(0);
}
