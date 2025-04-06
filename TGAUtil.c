#include "TGAutil.h"

int TGAUtil_GetHASH(const char *String)
{
	int hash = 0;

	while (*String != '\0') {
		hash = 31 * hash + *String++;
	}

	return hash;
}