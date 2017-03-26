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
	fflush(stdout);
	
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