#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#include "thrds.h"

char *chars[37] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-'};

void get_thrd_range(int** range_list, int thrd_count, Q_TYPE type) {
	
}

void* query_thrd_func(void* _params) {
	ThreadParams* params = (ThreadParams*) _params;
	
}

void query_main() {
	
}

void query_thrds(int thrd_count, Q_TYPE type, pthread_mutex_t exp_lock, pthread_mutex_t free_lock) {
	switch (type) {
		case LETTER:
			int** ranges = NULL;
			get_thrd_range()
			break;
		case LETTER_NUM:
			break;
		case LETTER_NUM_DASH:
			break;
		case NUM:
			break;
		case NUM_DASH:
			break;
	}
}


