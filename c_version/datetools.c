#include <stdio.h>

#include "datetools.h"

bool is_time_before_now_tm(TimeStruct* t_info) {
	time_t now_time = time(NULL);
	struct tm *a_time = localtime(&now_time);
	
	int day = a_time->tm_mday;
	int month = a_time->tm_mon + 1; // Month is 0 - 11, add 1 to get a jan-dec 1-12 concept
	int year = a_time->tm_year + 1900; // Year is # years since 1900
	
	if (year < t_info->year) {
		return false;
	} else if (year > t_info->year) {
		return true;
	} else {
		if (month < t_info->month) {
			return false;
		} else if (month > t_info->month) {
			return true;
		} else {
			if (day < t_info->day) {
				return false;
			} else if (day > t_info->day) {
				return true;
			} else {
				return false;
			}
		}
	}
}

TimeStruct* convert_raw_time(char* raw_time) {
	// Format: 12-nov-2018
	char** t_arr = str_split(raw_time, '-');
	if (t_arr == NULL) return NULL;
	
	TimeStruct* ts = (TimeStruct*) malloc(sizeof(TimeStruct));
	
	ts->year = atoi(t_arr[2]);
	
	char* cand_month = t_arr[1];
	if (strcmp(cand_month, "jan") == 0) {
		ts->month = 1;
	} else if (strcmp(cand_month, "feb") == 0) {
		ts->month = 2;
	} else if (strcmp(cand_month, "mar") == 0) {
		ts->month = 3;
	} else if (strcmp(cand_month, "apr") == 0) {
		ts->month = 4;
	} else if (strcmp(cand_month, "may") == 0) {
		ts->month = 5;
	} else if (strcmp(cand_month, "jun") == 0) {
		ts->month = 6;
	} else if (strcmp(cand_month, "jul") == 0) {
		ts->month = 7;
	} else if (strcmp(cand_month, "aug") == 0) {
		ts->month = 8;
	} else if (strcmp(cand_month, "sep") == 0) {
		ts->month = 9;
	} else if (strcmp(cand_month, "oct") == 0) {
		ts->month = 10;
	} else if (strcmp(cand_month, "nov") == 0) {
		ts->month = 11;
	} else if (strcmp(cand_month, "dec") == 0) {
		ts->month = 12;
	} else {
		fprintf(stderr, "ERROR: convert_raw_time: bad month name\n");
		// Bad format
		ts->month = 13;
	}
	
	ts->day = atoi(t_arr[0]);
	
	//FREE MEMORY!!!
	int i;
	for (i = 0; i < 3; i++) {
		free(t_arr[i]);
	}
	free(t_arr);
	
	//printf("Current: %d %d %d\n", ts->day, ts->month, ts->year);
	return ts;
}

bool is_time_before_now(char* raw_time) {
	TimeStruct* t_info = convert_raw_time(raw_time);
	if (t_info == NULL) {
		return false;
	}
	bool result = is_time_before_now_tm(t_info);
	free(t_info);
	return result;
}