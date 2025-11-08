#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>

#include <err.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "path.h"

static void usage(FILE *fp);

int
main(int argc, char *argv[])
{
	struct dirent *d;
	DIR *dirp;
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

	dirp = opendir(WAITINGDIR);
	if (dirp == NULL)
		err(1, "%s", WAITINGDIR);

	while (errno = 0, (d = readdir(dirp)) != NULL)
		(void)puts(d->d_name);
	if (errno != 0)
		err(1, "%s", WAITINGDIR);

	(void)closedir(dirp);

	return 0;
}

static void
usage(FILE *fp)
{
	(void)fprintf(fp, "usage: lollipop ls [-h]\n");
}
