#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

#define BUFSIZE 1024

int main(int argc, char *argv[]) {
	if (argc == 1) {
		fprintf(2, "Please provide argument.\n");
		exit(1);
	}
	char *args[MAXARG];
	for (int i = 0; i < MAXARG; i++) {
		args[i] = 0;
	}
	char line[BUFSIZE];
	for (int i = 1; i < argc; i++) {
		args[i - 1] = argv[i];
	}
	int n = argc - 1;
	while (gets(line, BUFSIZE)) {
		n = argc - 1;
		if (strlen(line) == 0)break;
		char *p = line;
		int k = 0;
		while (p && *p != '\n') {
			k = 0;
			while (*(p + k) != ' ' && *(p + k) != '\n') {
				k++;
			}
			if (args[n]) free(args[n]);
			args[n] = malloc(k + 1);
			args[n][k] = 0;
			memcpy(args[n], p, k);
			p += k;
			while (*p == ' ')p++;
			n++;
		}

		if (args[n]) {
			free(args[n]);
			args[n] = 0;
		}

		int pid = fork();
		if (pid < 0) {
			fprintf(2, "fork error\n");
		} else if (pid == 0) {
			// child
			exec(args[0], args);
		} else {
			// parent
			wait(&pid);
		}
	}
	exit(0);
}
