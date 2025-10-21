#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>

#include <err.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define WAITINGDIR	"/var/spool/lollipop/waiting/"
#define REMOVEDDIR	"/var/spool/lollipop/removed/"

static void usage(FILE *fp);
static int search_packet(char *path, size_t pathlen, const char *identifier);

int
main(int argc, char *argv[])
{
	int c, error, rdir, wdir;
	char name[NAME_MAX];

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

	if (argc == 0) {
		usage(stderr);
		exit(1);
	}

	wdir = open(WAITINGDIR, O_RDONLY | O_DIRECTORY | O_PATH);
	if (wdir == -1)
		err(1, "%s", WAITINGDIR);

	rdir = open(REMOVEDDIR, O_RDONLY | O_DIRECTORY | O_PATH);
	if (rdir == -1)
		err(1, "%s", REMOVEDDIR);

	error = 0;
	for (; *argv != NULL; argv++)
		switch (search_packet(name, sizeof(name), *argv)) {
		case 0:
			if (renameat(wdir, name, rdir, name) == -1)
				err(1, "%s", name);
			break;
		case 1:
			warnx("%s: No such packet", *argv);
			error = 1;
			break;
		case -1:
			err(1, "%s", *argv);
			break;
		}

	(void)close(rdir);
	(void)close(wdir);

	return error;
}

static int
search_packet(char *path, size_t pathlen, const char *identifier)
{
	struct dirent *d;
	DIR *dirp;
	int error;

	dirp = opendir(WAITINGDIR);
	if (dirp == NULL)
		return -1;

	while (errno = 0, (d = readdir(dirp)) != NULL)
		if (strstr(d->d_name, identifier) != NULL) {
			strncpy(path, d->d_name, pathlen);
			path[pathlen-1] = '\0';
			(void)closedir(dirp);
			return 0;
		}
	error = errno;
	(void)closedir(dirp);
	errno = error;

	return errno != 0 ? -1 : 1;
}

static void
usage(FILE *fp)
{
	(void)fprintf(fp, "usage: lollipop rm [-h] identifier [...]\n");
}
