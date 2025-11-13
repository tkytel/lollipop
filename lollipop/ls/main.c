#define _GNU_SOURCE

#include <sys/types.h>
#include <sys/stat.h>

#include <arpa/inet.h>

#include <linux/if_tun.h>

#include <err.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "path.h"
#include "ulid.h"

static void usage(FILE *fp);
static int detail_packet(int dir, const char *name);

int
main(int argc, char *argv[])
{
	struct dirent *d;
	DIR *dirp;
	int c, dir, i, lflag, match;

	lflag = 0;
	while ((c = getopt(argc, argv, "lh")) != -1)
		switch (c) {
		case 'l':
			lflag++;
			break;
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
		if (d->d_name[0] != '.')
			if (lflag)
				(void)detail_packet(dirfd(dirp), d->d_name);
			else
				(void)puts(d->d_name);
	if (errno != 0)
		err(1, "%s", WAITINGDIR);

	(void)closedir(dirp);

	return 0;
}

static int
detail_packet(int dir, const char *name)
{
	struct stat st;
	struct tm tm;
	struct ulid ul;
	struct timeval tv;
	struct tun_pi pi;
	int error, fd;
	char fmt[64], str[64];

	fd = openat(dir, name, O_RDONLY);
	if (fd == -1)
		return -1;

	if (read(fd, &pi, sizeof(pi)) == -1) {
		error = errno;
		(void)close(fd);
		errno = error;
		return -1;
	}

	if (fstat(fd, &st) == -1) {
		error = errno;
		(void)close(fd);
		errno = error;
		return -1;
	}

	(void)close(fd);

	if (parse_ulid(&ul, strchr(name, '.')+1) == -1)
		tv.tv_sec = tv.tv_usec = 0;
	else
		timestamp_ulid(&tv, &ul);
	(void)localtime_r(&tv.tv_sec, &tm);
	(void)strftime(fmt, sizeof(fmt), "%FT%T.%%03d%z", &tm);
	(void)snprintf(str, sizeof(str), fmt, tv.tv_usec / 1000);

	(void)printf("%04X\t%zu\t%s\t%s\n",
			ntohs(pi.proto), (size_t)st.st_size, str, name);

	return 0;
}

static void
usage(FILE *fp)
{
	(void)fprintf(fp, "usage: lollipop ls [-lh]\n");
}
