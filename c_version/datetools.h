#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include "strtools.h"

#ifndef DATETOOLS_H

#define DATETOOLS_H

typedef struct {
	int year;
	int month;
	int day;
} TimeStruct;

bool is_time_before_now_tm(TimeStruct* t_info);
TimeStruct* convert_raw_time(char* raw_time);
bool is_time_before_now(char* raw_time);

#endif