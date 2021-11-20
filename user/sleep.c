#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
	if (argc == 1) {
		fprintf(2, "Please provide argument.\n");
		exit(1);
	}
	int second = atoi(argv[1]);
	sleep(second);
	exit(0);
}
