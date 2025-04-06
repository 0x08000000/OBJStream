#ifndef TGAUTIL_H
#define TGAUTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct STRINGWORD
{
	char *String;
	int   WordCount;
};

int TGAUtil_GetHASH(const char *String);
#endif