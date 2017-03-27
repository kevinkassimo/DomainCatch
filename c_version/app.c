#include <stdio.h> //scanf , printf
#include <string.h>    //strtok
#include <stdlib.h>    //realloc
#include <sys/socket.h>    //socket
#include <netinet/in.h> //sockaddr_in
#include <arpa/inet.h> //getsockname
#include <netdb.h> //hostent
#include <unistd.h>    //close
#include <ctype.h>
#include <signal.h>

#include "includes.h"

void test_ip() {
	char* res = NULL;
	whois_query_ip("199.7.58.74", "domain hbio.com", &res);
	
	char* in = malloc(1500);
	strcpy(in, "Expiration Date:");
	char* out = NULL;
	grep_tr_64(&out, in, 1500, res);
	//printf("%s", res);
	printf("EXTRACTED: %s", out);	
}

void test_range() {
	int** lst = NULL;
	get_thrd_range(&lst, 4, T_LETTER);
		
	if (lst == NULL) {
		printf("Failed to assign");
	}
		
	int i;
	for (i = 0; i < 4; i++) {
		//printf("%d", sizeof(lst));
		printf("#%d Start: %c\n", i, range_chars[lst[i][0]]);
		printf("#%d end: %c\n", i, range_chars[lst[i][1]]);
			
		free(lst[i]);
	}
	free(lst);
}

void test_date() {
	char test_date_1[1024] = "12-nov-2018";
	if (is_time_before_now(test_date_1)) {
		printf("WRONG!");
	} else {
		printf("CORRECT!");
	}
	char test_date_2[1024] = "12-nov-2017";
	if (is_time_before_now(test_date_2)) {
		printf("WRONG!");
	} else {
		printf("CORRECT!");
	}
	char test_date_3[1024] = "27-mar-2017";
	if (is_time_before_now(test_date_3)) {
		printf("WRONG!");
	} else {
		printf("CORRECT!");
	}
	char test_date_4[1024] = "26-mar-2017";
	if (is_time_before_now(test_date_4)) {
		printf("WRONG!");
	} else {
		printf("CORRECT!");
	}
}

void test_thrds() {
	FILE* exp_p = fopen("expired.txt", "w");
	FILE* free_p = fopen("free.txt", "w");
	
	pthread_mutex_t exp_lock = PTHREAD_MUTEX_INITIALIZER, free_lock = PTHREAD_MUTEX_INITIALIZER;
	
	
	query_thrds(6, 4, T_LETTER, exp_p, free_p, &exp_lock, &free_lock);
}

void sigpipe_handler() {
	//close(socket_p);
	//start_whois_connect(WHOIS_IP);
}

int main(int argc, char *argv[]) {
	signal(SIGPIPE, sigpipe_handler);
	
	//test_date();
	test_thrds();
	printf("Done");
}