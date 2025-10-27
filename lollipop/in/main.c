#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <linux/if.h>

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define INPUTSOCKET	"/var/run/lollipop.%s.socket"
#define DEFAULTIFNAME	"post0"

static int open_input(const char *ifname);
static void usage(FILE *fp);

int
main(int argc, char *argv[])
{
	size_t head, length;
	ssize_t bytes;
	int c, fd;
	char *buf, tmp[1024];
	char ifname[IFNAMSIZ];

	(void)strncpy(ifname, DEFAULTIFNAME, sizeof(ifname));
	while ((c = getopt(argc, argv, "hi:")) != -1)
		switch (c) {
		case 'h':
			usage(stdout);
			exit(0);
		case 'i':
			(void)strncpy(ifname, optarg, sizeof(ifname));
			ifname[sizeof(ifname)-1] = '\0';
			if (strcmp(ifname, optarg) != 0 || ifname[0] == '\0')
				errx(1, "invalid interface name: %s", optarg);
			break;
		case '?':
		deafult:
			usage(stderr);
			exit(1);
		}
	argc -= optind;
	argv += optind;

	fd = open_input(ifname);
	if (fd == -1)
		err(1, "open_input");

	buf = NULL;
	head = length = 0;
	while ((bytes = read(STDIN_FILENO, tmp, sizeof(tmp))) > 0) {
		buf = realloc(buf, length += bytes);	/* XXX */
		if (buf == NULL)
			err(1, "realloc");
		(void)memcpy(buf+head, tmp, bytes);
		head += bytes;
	}
	if (bytes == -1)
		err(1, "stdin");

	if (send(fd, buf, length, 0) == -1)
		err(1, "send");

	free(buf);

	(void)close(fd);

	return 0;
}

static int
open_input(const char *ifname)
{
	struct sockaddr_un sun;
	int error, fd;

	fd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (fd == -1)
		return -1;

	sun.sun_family = AF_UNIX;
	if (snprintf(sun.sun_path, sizeof(sun.sun_path),
				INPUTSOCKET, ifname) > sizeof(sun.sun_path)) {
		(void)close(fd);
		errno = ENAMETOOLONG;
		return -1;
	}
	if (connect(fd, (struct sockaddr *)&sun, sizeof(sun)) == -1) {
		error = errno;
		(void)close(fd);
		errno = error;
		return -1;
	}

	return fd;
}

static void
usage(FILE *fp)
{
	(void)fprintf(fp, "usage: lollipop in [-h] [-i interface]\n");
}
