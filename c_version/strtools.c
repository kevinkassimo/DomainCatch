#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>

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

int grep2_tr_64(char** out1, char** out2, char* in1, char* in2, size_t len, char* str) {
	char* curLine = str;
	bool has1 = false, has2 = false;
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
				if (strstr(trimmed, in1) != NULL) {
					*out1 = realloc(*out1 , len+1);
					if (*out1 == NULL) {
						printf("realloc failed");
					}
					memcpy(*out1, trimmed, len);
					(*out1)[strlen(*out1)] = '\0';
				}
				if (strstr(trimmed, in2) != NULL) {
					*out2 = realloc(*out2 , len+1);
					if (*out2 == NULL) {
						printf("realloc failed");
					}
					memcpy(*out2, trimmed, len);
					(*out2)[strlen(*out2)] = '\0';
				}
				free(trimmed);
			}
		}
		
		if (has1 && has2) {
			return STRTOOLS_OK;
		}
		
		if (nextLine) {
			*nextLine = '\n';  // then restore newline-char, just to be tidy 
		}   
		curLine = nextLine ? (nextLine+1) : NULL;
	}
	
	if ((!has1) && (!has2)) {
		return STRTOOLS_ERR_NOT_FOUND;
	} else {
		return STRTOOLS_OK;
	}
}

// Note it does not handle consecutive delimiters
char** str_split(char* a_str, const char a_delim) {
	char** result    = 0;
	size_t count     = 0;
	char* tmp        = a_str;
	char* last_comma = 0;
	char delim[2];
	delim[0] = a_delim;
	delim[1] = 0;
	
	//printf("INPUT: %s\n", a_str);

	// Count how many elements will be extracted
	while (*tmp) {
		if (a_delim == *tmp) {
			count++;
			last_comma = tmp;
		}
		tmp++;
	}
	
	// Add space for trailing token
	count += last_comma < (a_str + strlen(a_str) - 1);

	// Add space for terminating null string so caller knows where the list of returned strings ends
	count++;

	result = malloc(sizeof(char*) * count);

	if (result) {
		size_t idx  = 0;
//		char* token = strtok(a_str, delim);

//		while (token) {
//			printf("idx: %u; count: %u;\n", idx, count);
////			if (idx < count) {
////				return NULL;
////			}
//			assert(idx < count);
//			*(result + idx++) = strdup(token);
//			token = strtok(0, delim);
//		}
		char* rest = a_str;
		char* token = strtok_r(rest, delim, &rest);
		while (token) {
			//printf("idx: %u; count: %u;\n", idx, count);
			assert(idx < count);
			*(result + idx++) = strdup(token);
			token = strtok_r(rest, delim, &rest);
		}
		//printf("END idx: %u; count: %u;\n", idx, count);
		assert(idx == count - 1);
		*(result + idx) = 0;
	}

	return result;
}