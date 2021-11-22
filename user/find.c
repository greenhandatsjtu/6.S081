#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char *base(char *path) {
	char *res = strchr(path, '/');
	if (!res)res = path;
	return res;
}

void
find(char *path, char *target) {
	char buf[512], *p;
	int fd;
	struct dirent de;
	struct stat st;

	if ((fd = open(path, 0)) < 0) {
		fprintf(2, "find: cannot open %s\n", path);
		return;
	}

	if (fstat(fd, &st) < 0) {
		fprintf(2, "find: cannot stat %s\n", path);
		close(fd);
		return;
	}

	if (strcmp(target, base(path)) == 0)printf("%s\n", path);

	if (st.type == T_DIR) {
		strcpy(buf, path);
		p = buf + strlen(buf);
		*p++ = '/';
		while (read(fd, &de, sizeof(de)) == sizeof(de)) {
			if (de.inum == 0)
				continue;
			memmove(p, de.name, DIRSIZ);
			p[DIRSIZ] = 0;
			if (stat(buf, &st) < 0) {
				printf("find: cannot stat %s\n", buf);
				continue;
			}
			// don't recurse into "." and ".."
			if (st.type == T_DIR && strcmp(de.name, ".") && strcmp(de.name, "..")) {
				find(buf, target);
			} else if (strcmp(target, de.name) == 0)printf("%s\n", buf);
		}
	}
	close(fd);
}

int main(int argc, char *argv[]) {
	if (argc < 3) {
		fprintf(2, "Invalid arguments.\n");
		exit(1);
	}
	find(argv[1], argv[2]);
	exit(0);
}
