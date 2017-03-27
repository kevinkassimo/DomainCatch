#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#include "thrds.h"

const char range_chars[37] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-'};

// *** since we need to get the pointer to the intended 2D array
void get_thrd_range(int*** range_list, int thrd_count, Q_TYPE type) {
	// INVALID THREADS
	if (thrd_count <= 0) {
		fprintf(stderr, "ERROR: get_thrd_range: bad thread number\n");
		return;
	}
	
	*range_list = (int**) realloc(*range_list, sizeof(int*) * thrd_count);
	int i;
	//sizeof 2 == start + end
	for (i = 0; i < thrd_count; i++) {
		(*range_list)[i] = (int*) malloc(sizeof(int) * 2);
	}
	int s, e; // s is always smaller than e
	switch (type) {
		case T_LETTER:
			s = 0; e = 26; break;
		case T_LETTER_NUM:
			s = 0; e = 36; break;
		case T_LETTER_NUM_DASH:
			s = 0; e = 37; break;
		case T_NUM:
			s = 26; e = 36; break;
		case T_NUM_DASH:
			s = 26; e = 37; break;
	}
	
	int base_cut = (e - s) / thrd_count;
	int rest = (e - s) % thrd_count;
		
	int acc = s;
	for (i = 0; i < thrd_count; i++) {
		(*range_list)[i][0] = acc;
		acc += base_cut;
		if (rest > 0) {
			rest--;
			acc++;
		}
		(*range_list)[i][1] = acc;
	}
}

void* query_thrd_func(void* _params) {
	ThreadParams* params = (ThreadParams*) _params;
	
	int s, e;
	
	switch (params->type) {
		case T_LETTER:
			s = 0; e = 26; break;
		case T_LETTER_NUM:
			s = 0; e = 36; break;
		case T_LETTER_NUM_DASH:
			s = 0; e = 37; break;
		case T_NUM:
			s = 26; e = 36; break;
		case T_NUM_DASH:
			s = 26; e = 37; break;
	}
	
	int i;
	for (i = params->start_index; i < params->end_index; i++) {
		if (i == 36) {continue;} // No head dash
		
		char c = range_chars[i];
		char* str = (char*) malloc(2);
		str[0] = c; str[1] = '\0';
		
		query_main_body(str, s, e, (params->length)-1, params->exp_p, params->free_p, params->exp_lock, params->free_lock, params->id);
		// Final clearup as previous code is blocking
		free(str);
	}
	return NULL;
}

void query_main_body(char* prefix, int s, int e, int depth, FILE* exp_p, FILE* free_p, pthread_mutex_t* exp_lock, pthread_mutex_t* free_lock, int thrd_id) {
	int i;
	for (i = s; i < e; i++) {
		char c = range_chars[i];
		
		if ((c == '-') && (prefix[strlen(prefix)-1] == '-' || depth == 1)) { 
			// bad domain syntax skip
			continue;
		}
		
		char* str = (char*) malloc(strlen(prefix) + 2);
		memset(str, '\0', strlen(prefix) + 2);
		strcat(str, prefix);
		str[strlen(prefix)] = c;
		
		// Reaches the end of depth, perform actual operations
		if (depth == 1) {
			char* res = NULL;
			
			char* temp_dom_str = (char*) malloc(1024);
			memset(temp_dom_str, 0, 1024);
			strcat(temp_dom_str, "domain ");
			strcat(&(temp_dom_str[strlen(temp_dom_str)]), str);
			strcat(&(temp_dom_str[strlen(temp_dom_str)]), ".com");
			
			
			printf("%s\n", temp_dom_str);
			
			//whois_query_ip(WHOIS_IP, temp_dom_str, &res);
			
			//connected_whois_query(temp_dom_str, &res);
			threaded_whois_query(thrd_id, temp_dom_str, &res);
			
			free(temp_dom_str);
			
			if (res == NULL) {
				fprintf(stderr, "ERROR: query_main_body: whois query no response");
			} else {
				char* exp_msg = NULL;
				char* free_msg = NULL;
				
				// Get messages
				grep2_tr_64(&exp_msg, &free_msg, "Expiration Date:", "No match for domain", 100, res);
				free(res);
				// grep_tr_64(&exp_msg, "Expiration Date:", 1024, res)
				// grep_tr_64(&free_msg, "No match for domain", 1024, res);
				
				// Handle exp_msg
				if (exp_msg) {
					char* start_ptr = &(exp_msg[17]);
					char* raw_date_str = strdup(start_ptr);
					
					if (is_time_before_now(raw_date_str)) {
						char* domain_str = (char*) malloc(1024);
						strcat(domain_str, str);
						strcat(&(domain_str[strlen(domain_str)]), ".com");
						
						// simply write to file
						pthread_mutex_lock(exp_lock);
						
						fwrite(domain_str, 1, sizeof(domain_str), exp_p);
						fwrite("\n", sizeof(char), 1, exp_p);
						fflush(exp_p);
						
						printf("EXPIRING: %s\n", domain_str);
						
						pthread_mutex_unlock(exp_lock);
						
						free(domain_str);
						free(exp_msg);
					} else {
						free(exp_msg);
					}
					free(raw_date_str);
				}
				
				// Handle free_msg
				if (free_msg) {
					char* domain_str = (char*) malloc(1024);
					strcat(domain_str, str);
					strcat(&(domain_str[strlen(domain_str)]), ".com");
					
					pthread_mutex_lock(free_lock);
					// simply write to file
					fwrite(domain_str, 1, sizeof(domain_str), free_p);
					fwrite("\n", sizeof(char), 1, free_p);
					fflush(free_p);
					
					pthread_mutex_unlock(free_lock);
					
					free(domain_str);
					free(free_msg);
				}
				
				//free(res);
			}
			
		} else {
			query_main_body(str, s, e, depth-1, exp_p, free_p, exp_lock, free_lock, thrd_id);
		}
		
		// Final clear up as previous code is blocking
		free(str);
	}
}

void query_thrds(int thrd_count, int length, Q_TYPE type, FILE* exp_p, FILE* free_p, pthread_mutex_t* exp_lock, pthread_mutex_t* free_lock) {
	
	setup_whois_connect(thrd_count, WHOIS_IP);
	
	
	int** range_map = NULL;
	
	get_thrd_range(&range_map, thrd_count, type);
	if (range_map == NULL) {
		fprintf(stderr, "ERROR: query_thrds: bad range map\n");
		return;
	} else {
		int i;
		ThreadParams** tps = (ThreadParams**) malloc(sizeof(ThreadParams*) * thrd_count);
		pthread_t threads[thrd_count];
		
		for (i = 0; i < thrd_count; i++) {
			ThreadParams* tp = (ThreadParams*) malloc(sizeof(ThreadParams));
			tps[i] = tp;
			
			tp->id = i;
			tp->start_index = range_map[i][0];
			tp->end_index = range_map[i][1];
			tp->length = length;
			tp->type = type;
			tp->exp_p = exp_p;
			tp->free_p = free_p;
			tp->exp_lock = exp_lock;
			tp->free_lock = free_lock;
			
			// Free range map entry
			free(range_map[i]);
			
			pthread_create(&threads[i], NULL, query_thrd_func, (void*) tp);
		}
		// Free range map self
		free(range_map);
		
		// Join threads
		for (i = 0; i < thrd_count; i++) {
			pthread_join(threads[i], NULL);
		}
		
		// Free each ThreadParams
		for (i = 0; i < thrd_count; i++) {
			free(tps[i]);
		}
		free(tps);
	}
	
	finalize_whois_connect();
}


