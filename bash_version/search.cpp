#include <iostream>
#include <string>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <sys/ioctl.h>
#include <cstdio>
#include <stdio.h>
#include <pthread.h>
using namespace std;

int length = 3;
bool letter_only;
string suffix = ".com";
FILE* expire_p;
FILE* noreg_p;

pthread_mutex_t expire_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t noreg_mutex = PTHREAD_MUTEX_INITIALIZER;


void searchLetter(string prefix = "", int depth = 1);

void* singleSearchThread(void* ptrs) {
	char* chars = (char*) ptrs;
	char start = chars[0];
	char end = chars[1];
		
	for (char c = start; c < end; c++) {
		string new_str = "";
		new_str += c;
		searchLetter(new_str, 1);
	}
	pthread_exit(NULL);
}

void multipleSearchLetter(string prefix = "", int depth = 1) {
		
	pthread_t threads[4];
	int rc;
		
	for (int i = 0; i < 4; i++) {
		char* chars = (char*) malloc(2);
			
		switch (i) {
			case 0:
				chars[0] = 'a';
				chars[1] = 'h';
				break;
			case 1:
				chars[0] = 'h';
				chars[1] = 'o';
				break;
			case 2:
				chars[0] = 'o';
				chars[1] = 'u';
				break;
			case 3:
				chars[0] = 'u';
				chars[1] = '{';
				break;
		}
		
		rc = pthread_create(&threads[i], NULL, singleSearchThread, (void*) chars);
	}
	pthread_exit(NULL);
}

void searchLetter(string prefix, int depth) {
	for (char c = 'a'; c <= 'z'; c++) {
		string new_str = prefix;
		new_str += c;
		if (depth >= length) {
			char cmd[1024];
			bzero(cmd, 1024);
			strcat(cmd, "./whois.sh ");
			strcat(&cmd[strlen(cmd)-1], new_str.c_str());
			strcat(&cmd[strlen(cmd)-1], suffix.c_str());
			//cout << cmd << endl;
			FILE* whois_p = popen(cmd, "r");
			if (!whois_p) {
				fprintf(stderr, "ERROR: cannot run whois command\n");
				exit(1);
			}
			
			char buf[5];
			char* RET_CODE;
			while ((RET_CODE = fgets(buf, sizeof(buf), whois_p)) == NULL) {
				//loop
			}
			pclose(whois_p);
			int code = atoi(RET_CODE);
			switch (code) {
				case 0:
					pthread_mutex_lock(&noreg_mutex);
					fwrite(new_str.c_str(), sizeof(char), sizeof(new_str.c_str()), noreg_p);
					fwrite("\n", 1, 1, noreg_p);
					fflush(noreg_p);
					pthread_mutex_unlock(&noreg_mutex);
					cout << "#Free: " + new_str << endl;
					break;
				case 1:
					pthread_mutex_lock(&expire_mutex);
					fwrite(new_str.c_str(), sizeof(char), sizeof(new_str.c_str()), expire_p);
					fwrite("\n", 1, 1, expire_p);
					fflush(expire_p);
					pthread_mutex_unlock(&expire_mutex);
					cout << "#Expired: " + new_str << endl;
					break;
			}
		} else {
			searchLetter(new_str, depth + 1);
		}
	}
}



int main() {
	expire_p = fopen("expired.txt", "w");
	noreg_p = fopen("not_registered.txt", "w");
	
	multipleSearchLetter();
}