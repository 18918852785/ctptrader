#include "string_convert.h"

#include <assert.h>
#include <string.h> //strtok
#include <stdlib.h>
#include <stdio.h>


#ifdef _WIN32
#else

char* _i64toa(long long i, char* buf, int ord)
{
	sprintf(buf, "%lld", i);
	return buf;
}
char* _itoa(int a, char* b, int c) {
	sprintf(b, "%d", a);
	return b;
}

int _stricmp(const char *s1, const char *s2)
{
	return strcasecmp(s1, s2);
}

#endif