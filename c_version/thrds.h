#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "whois.h"
#include "strtools.h"
#include "datetools.h"

#ifndef THRDS_H

#define T_LETTER_START 0
#define T_LETTER_END 26
#define T_LETTER_NUM_START 0
#define T_LETTER_NUM_END 36
#define T_LETTER_NUM_DASH_START 0
#define T_LETTER_NUM_DASH_END 37
#define T_NUM_START 26
#define T_NUM_END 36
#define T_NUM_DASH_START 26
#define T_NUM_DASH_END 37

#define THRDS_H

typedef enum {
	T_LETTER,
	T_LETTER_NUM,
	T_LETTER_NUM_DASH,
	T_NUM,
	T_NUM_DASH
} Q_TYPE;

typedef struct {
	int id;
	int start_index;
	int end_index;
	int length;
	Q_TYPE type;
	FILE* exp_p;
	FILE* free_p;
	pthread_mutex_t* exp_lock;
	pthread_mutex_t* free_lock;
} ThreadParams;

extern const char range_chars[37];

void get_thrd_range(int*** range_list, int thrd_count, Q_TYPE type);
void* query_thrd_func(void* _params);
void query_main_body(char* prefix, int s, int e, int depth, FILE* exp_p, FILE* free_p, pthread_mutex_t* exp_lock, pthread_mutex_t* free_lock, int thrd_id);
void query_thrds(int thrd_count, int length, Q_TYPE type, FILE* exp_p, FILE* free_p, pthread_mutex_t* exp_lock, pthread_mutex_t* free_lock);

#endif