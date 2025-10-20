#ifndef ULID_H
#define ULID_H

#include <stdint.h>

struct ulid {
	uint8_t ul_time[6];
	uint8_t ul_rand[10];
} __attribute__((__packed__));

int generate_ulid(struct ulid *ul);
void unparse_ulid(char *out, const struct ulid *ul);

#endif	/* !ULID_H */
