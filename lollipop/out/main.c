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
#define SENDINGDIR	"/var/spool/lollipop/sending/"
#define SENTDIR		"/var/spool/lollipop/sent/"

static void usage(FILE *fp);
static int search_packet(char *path, size_t pathlen, const char *identifier);

int
main(int argc, char *argv[])
{
	ssize_t bytes;
	int c, dryrun, fd, sdir, wdir, xdir;
	char buf[1024], name[NAME_MAX];

	dryrun = 0;
	while ((c = getopt(argc, argv, "hn")) != -1)
		switch (c) {
		case 'h':
			usage(stdout);
			exit(0);
		case 'n':
			dryrun = 1;
			break;
		case '?':
		default:
			usage(stderr);
			exit(1);
		}
	argc -= optind;
	argv += optind;

	if (argc != 1) {
		usage(stderr);
		exit(1);
	}

	wdir = open(WAITINGDIR, O_RDONLY | O_DIRECTORY | O_PATH);
	if (wdir == -1)
		err(1, "%s", WAITINGDIR);

	sdir = open(SENDINGDIR, O_RDONLY | O_DIRECTORY | O_PATH);
	if (sdir == -1)
		err(1, "%s", SENDINGDIR);

	xdir = open(SENTDIR, O_RDONLY | O_DIRECTORY | O_PATH);
	if (xdir == -1)
		err(1, "%s", SENTDIR);

	switch (search_packet(name, sizeof(name), *argv)) {
	case 1:
		errx(1, "%s: No such packet", *argv);
		break;
	case -1:
		err(1, "%s", *argv);
		break;
	}

	if (!dryrun)
		if (renameat(wdir, name, sdir, name) == -1)
			err(1, "%s", name);

	fd = openat(dryrun ? wdir : sdir, name, O_RDONLY);
	if (fd == -1)
		err(1, "%s", name);

	while ((bytes = read(fd, buf, sizeof(buf))) > 0)
		if (write(STDOUT_FILENO, buf, bytes) == -1)
			err(1, "stdout");
	if (bytes == -1)
		err(1, "%s", name);

	(void)close(fd);

	if (!dryrun)
		if (renameat(sdir, name, xdir, name) == -1)
			err(1, "%s", name);

	(void)close(xdir);
	(void)close(sdir);
	(void)close(wdir);

	return 0;
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
	(void)fprintf(fp, "usage: lollipop rm [-hn] identifier\n");
}
