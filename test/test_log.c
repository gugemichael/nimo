/*
 * =====================================================================================
 *
 *       Filename:  test.c
 *
 *        Version:  1.0
 *        Created:  08/13/2012 07:29:28 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Michael LiuXin
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <pthread.h>

#include "nimo/log.h"

#define threads (6)

size_t logs[threads] = {0};

int fd;

// more than 4k
const char* text4k = "[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage"
					"[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage"""
					"[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage"""
					"[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage"""
					"[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage"""
					"[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage"""
					"[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage"""
					"[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage"""
					"[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage"""
					"[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage"""
					"[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage"""
					"[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage"""
					"[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage"""
					"[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage"""
					"[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage"""
					"[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage"""
					"[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage"""
					"[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage"""
					"[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage"""
					"[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage[DEBUG][32294][log_driver.c:31][10/28/2:52:22,942000] : This is c test logging , garbage""";

void* write_log(void* c) {
	size_t * log = (size_t*) c;
	fprintf(stdout,"write_log procedure\n");
	unsigned long allocate_times = 2000000000000;
	while(allocate_times-- != 0) {
		(*log)++;
		nimo_log_debug("This is c test logging , garbage");
		//(*log)+=48;
		//write(fd,text4k,4096);
	}
	return NULL;
}

void sleep_test() {
	usleep(1000000);
}

int main()
{

//	fd = open("bigfile.log",O_CREAT | O_RDWR | O_APPEND, S_IRUSR | S_IWUSR);
//
//	if (-1 == fd) {
//		printf("file create failed! %s\n",strerror(errno));
//		exit(0);
//	}

	if (NULL == nimo_log_split_init("test.logfile",0,0,0)) {
		printf("UbLog Init Failed [%s]",nimo_log_last_error());
		exit(0);
	}

	nimo_log_buffer(4096);

	pthread_t pid = 0;

	int n = 0;

	while(n != threads) {
		pthread_create(&pid,NULL,write_log,&logs[n]);
		n++;
	}

	int loop = 5000;
	while(loop--) {
		char out[128] = {0};
		size_t r = 0;
		for(int i=0; i!= threads ; i++) {
			r += logs[i];
			logs[i] = 0;
		}
		int n = snprintf(out,128,"Write %lu Logs\n",r);
		n = write(1,out,n);
		sleep_test();
	}
	return 0;
}




