#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <linux/if.h>
#include <linux/if_tun.h>

#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define TUNFILE	"/dev/net/tun"

static int tun_alloc(char *dev);

int
main(void)
{
	ssize_t bytes;
	int fd;
	char buf[2048], ifname[IFNAMSIZ];

	(void)strncpy(ifname, "post0", sizeof(ifname));
	fd = tun_alloc(ifname);
	if (fd == -1)
		err(1, "tun_alloc");

	while (1) {
		bytes = read(fd, buf, sizeof(buf));
		if (bytes == -1)
			err(1, "read");

		for (ssize_t i = 0; i < bytes; i++) {
			if ((i & 0x0F) == 0)
				(void)fprintf(stderr, "\n%05zX\t", (size_t)i);
			(void)fprintf(stderr, "%02x ", buf[i] & 0xFF);
		}
		(void)fprintf(stderr, "\n%05zX\n", (size_t)bytes);

		if (write(fd, buf, bytes) == -1)
			err(1, "write");
	}

	(void)close(fd);
	return 0;
}

static int
tun_alloc(char *dev)
{
	struct ifreq ifr;
	int fd;

	fd = open(TUNFILE, O_RDWR);
	if (fd == -1)
		return -1;

	(void)memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
	if (dev != NULL && *dev != '\0')
		(void)strncpy(ifr.ifr_name, dev, IFNAMSIZ);

	if (ioctl(fd, TUNSETIFF, (void *)&ifr) == -1) {
		(void)close(fd);
		return -1;
	}
	(void)strcpy(dev, ifr.ifr_name);

	return fd;
}
