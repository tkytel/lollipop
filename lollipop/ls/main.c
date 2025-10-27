#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>

#include <err.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define WAITINGDIR	"/var/spool/lollipop/waiting/"

static void usage(FILE *fp);

int
main(int argc, char *argv[])
{
	struct dirent **list;
	int c, dir, i, match;

	while ((c = getopt(argc, argv, "h")) != -1)
		switch (c) {
		case 'h':
			usage(stdout);
			exit(0);
		case '?':
		default:
			usage(stderr);
			exit(1);
		}
	argc -= optind;
	argv += optind;

	dir = open(WAITINGDIR, O_RDONLY | O_DIRECTORY | O_PATH);
	if (dir == -1)
		err(1, "%s", WAITINGDIR);

	match = scandirat(dir, ".", &list, NULL, alphasort);
	if (match == -1)
		err(1, "scandir");

	for (i = 0; i < match; i++) {
		(void)puts(list[i]->d_name);
		free(list[i]);
	}
	free(list);

	return 0;
}

static void
usage(FILE *fp)
{
	(void)fprintf(fp, "usage: lollipop ls [-h]\n");
}
