#include <string.h>

#include "match.h"

int
ends_with(const char *s, const char *t)
{
	size_t ls, lt;

	ls = strlen(s);
	lt = strlen(t);

	return lt <= ls && memcmp(s+ls-lt, t, lt) == 0;
}
