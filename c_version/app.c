#include <stdio.h> //scanf , printf
#include <string.h>    //strtok
#include <stdlib.h>    //realloc
#include <sys/socket.h>    //socket
#include <netinet/in.h> //sockaddr_in
#include <arpa/inet.h> //getsockname
#include <netdb.h> //hostent
#include <unistd.h>    //close
#include <ctype.h>

#include "whois.h"
#include "strtools.h"

int main(int argc, char *argv[]) {
	char* res = NULL;
	whois_query_ip("199.7.58.74", "domain hbio.com", &res);
	
	char* in = malloc(1500);
	strcpy(in, "Expiration Date:");
	char* out = NULL;
	grep_tr_64(&out, in, 1500, res);
	//printf("%s", res);
	printf("EXTRACTED: %s", out);
}