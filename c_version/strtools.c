#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "strtools.h"

int trim(char **out, size_t len, const char *str) {
	if (len == 0) {
		return STRTOOLS_ERR_INVALID;
	}

	const char *end;
	size_t out_size;
	// Trim leading space
	while (isspace((unsigned char) *str)) {
		str++;
	}

	if (*str == 0) {// All spaces?
		*out = realloc(*out, 1);
		(*out)[0] = '\0';
		return STRTOOLS_ERR_INVALID;
	}

	// Trim trailing space
	end = str + strlen(str) - 1;
	while (end > str && isspace((unsigned char)*end)) {
		end--;
	}
	end++;

	// Set output size to minimum of trimmed string length and buffer size minus 1
	out_size = (end - str) < len-1 ? (end - str) : len-1;

	
	*out = realloc(*out , out_size+1);
	if (*out == NULL) {
		printf("realloc failed");
	}

	// Copy trimmed string and add null terminator
	memcpy(*out, str, out_size);
	(*out)[out_size] = 0;

	return STRTOOLS_OK;
}

int grep_tr_64(char** out, char* in, size_t len, char* str) {
	char* curLine = str;
	while (curLine) {
		char* nextLine = strchr(curLine, '\n');
		if (nextLine) {
			*nextLine = '\0';  // temporarily terminate the current line
		}
		
		if (strlen(curLine) != 0) {
			char* trimmed = NULL;
			trim(&trimmed, strlen(curLine)+1, curLine);
			
			if (strlen(trimmed) == 0) {
				free(trimmed);
			} else {
				//printf("%s\n", trimmed);
				if (strstr(trimmed, in) != NULL) {
					*out = realloc(*out , len+1);
					if (*out == NULL) {
						printf("realloc failed");
					}
					memcpy(*out, trimmed, len);
					(*out)[strlen(*out)] = '\0';
					// Return prematurely, trimmed freed
					free(trimmed);
					return STRTOOLS_OK;
				}
			}
		}
		
		if (nextLine) {
			*nextLine = '\n';  // then restore newline-char, just to be tidy 
		}   
		curLine = nextLine ? (nextLine+1) : NULL;
	}
	return STRTOOLS_ERR_NOT_FOUND;
}