#include <stdio.h> //scanf , printf
#include <string.h>    //strtok
#include <stdlib.h>    //realloc
#include <sys/socket.h>    //socket
#include <netinet/in.h> //sockaddr_in
#include <arpa/inet.h> //getsockname
#include <netdb.h> //hostent
#include <unistd.h>    //close

#include <pthread.h>

#ifndef WHOIS_H
#define WHOIS_ERR_HOSTNAME -2
#define WHOIS_ERR_IP -1
#define WHOIS_OK 0
#define WHOIS_IP "199.7.58.74"

#define WHOIS_H
int whois_query_ip(char *ip , char *query , char **response);
int whois_query_s(char *server , char *query , char **response);
int hostname_to_ip(char * hostname , char* ip);

extern int* s_thrd_socks;
extern int s_thrd_count;
extern pthread_mutex_t socket_glob_mutex;
extern struct sockaddr_in s_dest;

void setup_whois_connect(int thrd_count, char *ip);
void threaded_whois_query(int thrd_id, char *query, char **response);
void finalize_whois_connect();

#endif