#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#ifndef STDTOOLS_H

#define STRTOOLS_ERR_INVALID -2
#define STRTOOLS_ERR_NOT_FOUND -1
#define STRTOOLS_OK 0

#define STDTOOLS_H

int trim(char** out, size_t len, const char *str);
int grep_tr_64(char** out, char* in, size_t len, char* str);
int grep2_tr_64(char** out1, char** out2, char* in1, char* in2, size_t len, char* str);
char** str_split(char* a_str, const char a_delim);
#endif