#define _GNU_SOURCE

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>
#include <sys/param.h>

#include <linux/if.h>
#include <linux/if_tun.h>

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ulid.h"

#define SUNPATHLEN	sizeof(((struct sockaddr_un *)0)->sun_path)
#define BACKLOG	5

#define IFNAME		"post%d"
#define SOCKPATH	"/var/run/lollipop.%s.socket"
#define SPOOLPATH	"/var/spool/lollipop/waiting/"
#define TUNTAPPATH	"/dev/net/tun"

static int nPacket;

static int sock_alloc(char *sockpath);
static int tun_alloc(char *dev);
static void usage(void);

int
main(int argc, char *argv[])
{
	struct ulid ul;
	fd_set rfds;
	ssize_t bytes;
	int c, fd, nfds, rxfd, sockfd, spoolfd, tunfd;
	char filepath[PATH_MAX], sockpath[PATH_MAX], spoolpath[PATH_MAX];
	char buf[2048], identifier[27], ifname[IFNAMSIZ];

	(void)strncpy(ifname, IFNAME, sizeof(ifname));
	(void)strncpy(spoolpath, SPOOLPATH, sizeof(spoolpath));
	while ((c = getopt(argc, argv, "i:")) != -1)
		switch (c) {
		case 'i':
			(void)strncpy(ifname, optarg, sizeof(ifname));
			ifname[sizeof(ifname)-1] = '\0';
			if (strcmp(ifname, optarg) != 0
					|| ifname[0] == '\0')
				errx(1, "invalid device name: \"%s\"", optarg);
			break;
		case '?':
			usage();
		}
	argc -= optind;
	argv += optind;

	spoolfd = open(spoolpath, O_RDONLY | O_DIRECTORY | O_PATH);
	if (spoolfd == -1)
		err(1, "%s", spoolpath);

	tunfd = tun_alloc(ifname);
	if (tunfd == -1)
		err(1, "tun_alloc");

	(void)snprintf(sockpath, sizeof(sockpath), SOCKPATH, ifname);
	if (unlink(sockpath) == -1
			&& errno != ENOENT)
		err(1, "%s", sockpath);
	sockfd = sock_alloc(sockpath);
	if (sockfd == -1)
		err(1, "sock_alloc");

	for (;; nPacket++) {
		FD_ZERO(&rfds);
		FD_SET(sockfd, &rfds);
		FD_SET(tunfd, &rfds);
		nfds = MAX(sockfd, tunfd) + 1;
		if (select(nfds, &rfds, NULL, NULL, NULL) == -1)
			err(1, "select");

		rxfd = FD_ISSET(sockfd, &rfds) ? sockfd : tunfd;
		bytes = read(rxfd, buf, sizeof(buf));
		if (bytes == -1)
			err(1, "read");

		if (generate_ulid(&ul) == -1)
			err(1, "generate_ulid");
		unparse_ulid(identifier, &ul);
		(void)snprintf(filepath, sizeof(filepath),
				"%s.%s", ifname, identifier);
		(void)fprintf(stderr, "%s -> %s",
				FD_ISSET(sockfd, &rfds) ? sockpath : ifname,
				FD_ISSET(sockfd, &rfds) ? ifname : filepath);
		for (ssize_t i = 0; i < bytes; i++) {
			if ((i & 0x0F) == 0)
				(void)fprintf(stderr, "\n%05zX\t", (size_t)i);
			(void)fprintf(stderr, "%02x ", buf[i] & 0xFF);
		}
		(void)fprintf(stderr, "\n%05zX\n", (size_t)bytes);

		if (FD_ISSET(sockfd, &rfds)) {
			if (write(tunfd, buf, bytes) == -1)
				err(1, "write");
		} else {
			fd = openat(spoolfd, filepath,
					O_WRONLY | O_CREAT | O_EXCL, 0644);
			if (fd == -1)
				err(1, "%s/%s", spoolpath, filepath);
			if (write(fd, buf, bytes) == -1)
				err(1, "write");
			(void)close(fd);
		}
	}
	/* NOTREACHED */

	(void)close(spoolfd);
	(void)close(sockfd);
	(void)close(tunfd);
	return -1;
}

static int
sock_alloc(char *sockpath)
{
	struct sockaddr_un *sun;
	socklen_t addrlen;
	int error, fd;

	addrlen = offsetof(struct sockaddr_un, sun_path) + strlen(sockpath)+1;
	sun = malloc((size_t)addrlen);
	if (sun == NULL)
		return -1;

	fd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (fd == -1) {
		error = errno;
		free(sun);
		errno = error;
		return -1;
	}

	(void)memset(sun, 0, addrlen);
	sun->sun_family = AF_UNIX;
	(void)strcpy(sun->sun_path, sockpath);
	if (bind(fd, (struct sockaddr *)sun, addrlen) == -1) {
		error = errno;
		free(sun);
		(void)close(fd);
		errno = error;
		return -1;
	}
	free(sun);

	return fd;
}

static int
tun_alloc(char *dev)
{
	struct ifreq ifr;
	int error, fd;

	fd = open(TUNTAPPATH, O_RDWR);
	if (fd == -1)
		return -1;

	(void)memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
	if (dev != NULL && *dev != '\0')
		(void)strncpy(ifr.ifr_name, dev, IFNAMSIZ);

	if (ioctl(fd, TUNSETIFF, (void *)&ifr) == -1) {
		error = errno;
		(void)close(fd);
		errno = error;
		return -1;
	}
	(void)strcpy(dev, ifr.ifr_name);

	return fd;
}

static void
usage(void)
{
	(void)fprintf(stderr, "usage: lollipopd [-i devname]\n");
	exit(1);
}
