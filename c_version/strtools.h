#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define STRTOOLS_ERR_INVALID -2
#define STRTOOLS_ERR_NOT_FOUND -1
#define STRTOOLS_OK 0

#ifndef STDTOOLS_H
#define STDTOOLS_H

int trim(char** out, size_t len, const char *str);
int grep_tr_64(char** out, char* in, size_t len, char* str);

#endif