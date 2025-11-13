#include <sys/random.h>
#include <sys/time.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "ulid.h"

static const char *ctab = "0123456789ABCDEFGHJKMNPQRSTVWXYZ";

int
generate_ulid(struct ulid *ul)
{
	struct timeval tv;
	uint_least64_t msec;
	size_t i;

	if (gettimeofday(&tv, NULL) == -1)
		return -1;

	msec = (uint_least64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
	for (i = sizeof(ul->ul_time); i-- > 0; ) {
		ul->ul_time[i] = msec & 0xFF;
		msec >>= 8;
	}

	if (getrandom(ul->ul_rand, sizeof(ul->ul_rand), GRND_NONBLOCK) == -1)
		return -1;

	return 0;
}

int
parse_ulid(struct ulid *ul, const char *in)
{
	size_t i, j, offset;
	uint_least32_t tmp;
	char buf[16], *p;

	if (strlen(in) != 26)
		return -1;

	offset = -2;
	tmp = 0;
	for (i = 0, j = 0; i < 26; i++) {
		p = strchr(ctab, in[i]);
		if (p == NULL)
			return -1;

		tmp = tmp << 5 | p - ctab;
		offset += 5;
		while (offset >= 8) {
			offset -= 8;
			buf[j++] = tmp >> offset;
		}
	}
	(void)memcpy(ul, buf, sizeof(buf));

	return 0;
}

void
timestamp_ulid(struct timeval *tv, const struct ulid *ul)
{
	uint_least64_t msec;
	size_t i;

	msec = 0;
	for (i = 0; i < sizeof(ul->ul_time); i++)
		msec = msec << 8 | ul->ul_time[i];
	tv->tv_usec = msec % 1000 * 1000;
	tv->tv_sec = msec / 1000;
}

void
unparse_ulid(char *out, const struct ulid *ul)
{
	size_t i, j, offset;
	uint_least16_t tmp;
	uint8_t buf[16];

	(void)memcpy(buf, ul, sizeof(buf));
	offset = 0;
	tmp = 0;
	for (i = 26, j = 16; i-- > 0; ) {
		if (offset < 8) {
			tmp |= buf[--j] << offset;
			offset += 8;
		}
		out[i] = ctab[tmp & 0x1F];
		offset -= 5;
		tmp >>= 5;
	}
	out[26] = '\0';
}
