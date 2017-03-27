#include <stdio.h> //scanf , printf
#include <string.h>    //strtok
#include <stdlib.h>    //realloc
#include <sys/socket.h>    //socket
#include <netinet/in.h> //sockaddr_in
#include <arpa/inet.h> //getsockname
#include <netdb.h> //hostent
#include <unistd.h>    //close

#include "whois.h"

int hostname_to_ip(char* hostname, char* ip){
	struct hostent *he;
	struct in_addr **addr_list;
	int i; 
	if ((he = gethostbyname(hostname)) == NULL) {
		// get the host info
		herror("gethostbyname");
		return WHOIS_ERR_HOSTNAME;
	}
	addr_list = (struct in_addr **) he->h_addr_list;
	for (i = 0; addr_list[i] != NULL; i++)  {
		//Return the first one;
		strcpy(ip , inet_ntoa(*addr_list[i]) );
		return WHOIS_OK;
	}
	return WHOIS_OK;
}

int whois_query_ip(char *ip , char *query , char **response) {
	char message[100], buffer[1500];
	int sock, read_size, total_size = 0;
	struct sockaddr_in dest;
	
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	  
	//Prepare connection structures :)
	memset(&dest, 0, sizeof(dest));
	dest.sin_family = AF_INET;
	   
	dest.sin_addr.s_addr = inet_addr(ip);
	dest.sin_port = htons(43);
 
	//Now connect to remote server
	if (connect(sock, (const struct sockaddr*) &dest, sizeof(dest)) < 0) {
		perror("connect failed");
	}
	 
	//Now send some data or message
	sprintf(message, "%s\r\n", query);
	if (send(sock, message, strlen(message), 0) < 0) {
		perror("send failed");
	}
	 
	//Now receive the response
	while ((read_size = recv(sock, buffer, sizeof(buffer), 0))) {
		*response = realloc(*response, read_size + total_size);
		if (*response == NULL) {
			printf("realloc failed");
		}
		memcpy(*response + total_size, buffer, read_size);
		total_size += read_size;
	}
	//printf("Done");
	//fflush(stdout);
	
	*response = realloc(*response , total_size + 1);
	*(*response + total_size) = '\0';
	 
	close(sock);
	return WHOIS_OK;
}

int whois_query_s(char *server , char *query , char **response) {
	char ip[32], message[100], buffer[1500];
	int sock, read_size, total_size = 0;
	struct sockaddr_in dest;
	
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	  
	//Prepare connection structures :)
	memset(&dest, 0, sizeof(dest));
	dest.sin_family = AF_INET;
	  
	//printf("\nResolving %s..." , server);
	if (hostname_to_ip(server, ip)) {
		printf("Failed");
		return WHOIS_ERR_IP;
	}
	printf("%s" , ip);    
	dest.sin_addr.s_addr = inet_addr( ip );
	dest.sin_port = htons( 43 );
 
	//Now connect to remote server
	if (connect(sock, (const struct sockaddr*) &dest, sizeof(dest)) < 0) {
		perror("connect failed");
	}
	 
	//Now send some data or message
	sprintf(message, "%s\r\n", query);
	if (send(sock, message, strlen(message), 0) < 0) {
		perror("send failed");
	}
	 
	//Now receive the response
	while ((read_size = recv(sock, buffer, sizeof(buffer), 0))) {
		*response = realloc(*response , read_size + total_size);
		if (*response == NULL) {
			printf("realloc failed");
		}
		memcpy(*response + total_size , buffer , read_size);
		total_size += read_size;
	}
	//printf("Done");
	fflush(stdout);
	 
	*response = realloc(*response , total_size + 1);
	*(*response + total_size) = '\0';
	 
	close(sock);
	return WHOIS_OK;
}

int socket_p;

int* s_thrd_socks;
int s_thrd_count;
pthread_mutex_t socket_glob_mutex = PTHREAD_MUTEX_INITIALIZER;
struct sockaddr_in s_dest;

void setup_whois_connect(int thrd_count, char *ip) {
	s_thrd_count = thrd_count;
	s_thrd_socks = (int*) malloc(sizeof(int) * thrd_count);
	
	memset(&s_dest, 0, sizeof(s_dest));
	s_dest.sin_family = AF_INET;
	s_dest.sin_addr.s_addr = inet_addr(ip);
	s_dest.sin_port = htons(43);
}

void threaded_whois_query(int thrd_id, char *query, char **response) {
	int read_size, total_size = 0;
	char message[100], buffer[1500];
	
	*response = realloc(*response, 1500);
	
	s_thrd_socks[thrd_id] = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	if (connect(s_thrd_socks[thrd_id], (const struct sockaddr*) &s_dest, sizeof(s_dest)) < 0) {
		perror("Connection failed");
		usleep(500);
	}
	//Now send some data or message
	sprintf(message, "%s\r\n", query);
	if (send(s_thrd_socks[thrd_id], message, strlen(message), 0) < 0) {
		perror("send failed");
	}
	//Now receive the response
	while ((read_size = recv(s_thrd_socks[thrd_id], buffer, sizeof(buffer), 0)) > 0) {
		if (read_size + total_size > 1499) {
			//printf("read_size: %d, total_size: %d\n", read_size, total_size);
			memcpy(*response + total_size, buffer, (1498-total_size));
			total_size = 1499;
			break;
		} else {
			memcpy(*response + total_size, buffer, read_size);
			total_size += read_size;
		}
		
//		if (read_size + total_size < 0) {
//			fprintf(stderr, "Weird BUG: %d, as read_size = \n", read_size+total_size);
//		}
//		
//		*response = realloc(*response, read_size + total_size);
//		if (*response == NULL) {
//			printf("realloc failed");
//			break;
//		} else {
//			memcpy(*response + total_size, buffer, read_size);
//			total_size += read_size;
//		}
	}
	
	//printf("TOT SIZE: %d\n", total_size);
	//*response = realloc(*response , total_size + 1);
	*(*response + total_size) = '\0';
	
	//printf("res: %s\n", *response);
	 
	close(s_thrd_socks[thrd_id]);
}

void finalize_whois_connect() {
	free(s_thrd_socks);
}
//
//void start_whois_connect(char *ip) {
//	struct sockaddr_in dest;
//	//close(socket_p);
//	
//	socket_p = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//	
//	//Prepare connection structures :)
//	memset(&dest, 0, sizeof(dest));
//	dest.sin_family = AF_INET;
//	   
//	dest.sin_addr.s_addr = inet_addr(ip);
//	dest.sin_port = htons(43);
// 
//	//Now connect to remote server
//	while (connect(socket_p, (const struct sockaddr*) &dest, sizeof(dest)) < 0) {
//		perror("1st connect failed");
//		
//		//pthread_mutex_lock(&socket_glob_mutex);
//		if (pthread_mutex_trylock(&socket_glob_mutex)) {
//			close(socket_p);
//			sleep(2);
//			socket_p = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//			pthread_mutex_unlock(&socket_glob_mutex);
//		} else {
//			sleep(2);
//		}
//		//pthread_mutex_unlock(&socket_glob_mutex);
//	}
//}
//
//void connected_whois_query(char *query , char **response) {
//	int read_size, total_size = 0;
//	char message[100], buffer[1500];
//	//Now send some data or message
//	sprintf(message, "%s\r\n", query);
//	while (send(socket_p, message, strlen(message), 0) < 0) {
//		perror("send failed");
//		//pthread_mutex_lock(&socket_glob_mutex);
//		if (pthread_mutex_trylock(&socket_glob_mutex)) {
//			close(socket_p);
//			sleep(2);
//			struct sockaddr_in dest;
//			//close(socket_p);
//			
//			socket_p = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//			
//			//Prepare connection structures :)
//			memset(&dest, 0, sizeof(dest));
//			dest.sin_family = AF_INET;
//			   
//			dest.sin_addr.s_addr = inet_addr(WHOIS_IP);
//			dest.sin_port = htons(43);
//			
//			while (connect(socket_p, (const struct sockaddr*) &dest, sizeof(dest)) < 0) {
//				perror("2nd connect failed");
//				sleep(1);
//				//pthread_mutex_unlock(&socket_glob_mutex);
//			}
//			
//			pthread_mutex_unlock(&socket_glob_mutex);
//		} else {
//			sleep(2);
//		}
//		//pthread_mutex_unlock(&socket_glob_mutex);
//	}
//	 
//	//Now receive the response
//	while ((read_size = recv(socket_p, buffer, sizeof(buffer), 0))) {
//		*response = realloc(*response, read_size + total_size);
//		if (*response == NULL) {
//			printf("realloc failed");
//		}
//		memcpy(*response + total_size, buffer, read_size);
//		total_size += read_size;
//	}
//	//printf("Done");
//	//fflush(stdout);
//	
//	*response = realloc(*response , total_size + 1);
//	*(*response + total_size) = '\0';
//}
//
//void close_whois_connect() {
//	close(socket_p);
//}