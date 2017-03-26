#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#ifndef THRDS_H
#define THRDS_H

typedef enum {
	LETTER,
	LETTER_NUM,
	LETTER_NUM_DASH,
	NUM,
	NUM_DASH
} Q_TYPE;

typedef struct {
	char start,
	char end,
	int length,
	Q_TYPE type,
	pthread_mutex_t exp_mutex,
	pthread_mutex_t free_mutex
} ThreadParams;

extern char *chars[37];

void query_range();

#endif