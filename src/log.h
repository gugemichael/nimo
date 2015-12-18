/*
 * UB Library Framework For C
 *
 * Copyright 2012 , Michael LiuXin <guguemichael@gmail.com>
 *
 * Contributors can discuss or commit code at
 *
 *  http://blog.csdn.net/gugemichael (Blog)
 *  https://github.com/gugemichel/libclowf (Git)
 *
 * Version : 1.0
 * Created : 09/19/2012 09:25:52 PM
 * Filename : clowf_log.h
 * Description : log with split files
 * Compiler : g++ 4.6.1
 * Author : Michael LiuXin
 *
 */

#ifndef __CLOWF_LOG_H__
#define __CLOWF_LOG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/stat.h> 
#include <sys/types.h>
#define __USE_GNU 1
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <assert.h>

#include <sys/time.h>

#define FILE_NAME_MAX 1024

#define NORMAL_LOG (1)
#define ERROR_LOG (2)

#ifndef likely
#define likely(x) __builtin_expect(!!(x),1)
#endif
#ifndef unlikely
#define unlikely(x) __builtin_expect(!!(x),0)
#endif

#define MAX_THREADS 65535	

typedef enum __log_level {
	DEBUG = 0,
	TRACE,
	WARNNING,
	ERROR,
	UNKOWN
}log_level;


typedef struct __buffer_t {
	unsigned char* mem;
	size_t size;
	size_t cursor;
}buffer_t;	


typedef struct __log_t {
	//unsigned int level; 	/* level of log to record : ERROR , WARNNING , DEBUG , ALL */
	char file_name[FILE_NAME_MAX];
	unsigned int mask;
	int direct_io;
	int normal_file; 				/* output log file */
	int error_file; 				/* output wf file */
	int use_pagecache;
	size_t normal_file_size; 		/* file size record */
	size_t error_file_size; 		/* file size record */
	size_t split_size; 				/* one piece(file) max size with split */
	time_t touch_time; 	/* total time */
	time_t split_time;  /* one piece(file) time with split */
	struct tm *timep;
	const char* last_error;
	buffer_t *dirty_page[MAX_THREADS];
	log_level level; 			/* Log level , lower level string will ignore */
	pthread_mutex_t split_lock;
}log_t;

/**
 * Initial log handler with path , indicate use direct io or not
 * 
 * @params  [in] which file to be written . if NULL "stdout" will be set
 * @params  [in] file mask set			
 *
 * @return : pointer to the log_t structure . if failed , it's NULL
 */
log_t* clowf_log_init(const char* filepath, int direct_io);

/**
 * Initial log handler with path , direct io , and split strategy 
 * 
 * @params [in] which file to be written . if NULL "stdout" will be set
 * @params [in] file mask set			
 * @params [in] span time of split file , 0 means no split 
 * @params [in] content size of split file , 0 means no split 
 *
 * @return : pointer to the log_t structure . if failed , it's NULL
 */
log_t* clowf_log_split_init(const char* filepath, int direct_io,unsigned long long ts, size_t ss);

/**
 * Destroy log handle 
 */
void clowf_log_destroy();

/**
 * Do actual write disk (may be cached) . DON'T call it directly 
 * 
 * @params [in] specified level to be write
 * @params [in] file name from __FILE__
 * @params [in] function name from __FUNCTION__
 * @params [in] line number from __LINE__
 * @params [in][varlist] variable strings
 *
 */
void log_write(log_level level, const char* file, const char* func_name,unsigned int line, const char* __format,...);

/**
 * Reduce write() call times , if possible save into buffer
 * first . When buffer is full , will cause write()
 * 
 * @params [in] specified buffer size 
 *
 * return : 0 if buffer allocate successfully
 */
int clowf_log_page_buffer();

int clowf_log_buffer(int size);

void clowf_log_level(log_level);

/**
 * @brief :  write log with LEVEL 
 *
 * @params : [in] log : log handle 
 * @params : [in] buf : buffer to be written 
 */
#ifndef NON_USE_CLOWF_LOG
#define clowf_log_debug(buf,...) do{\
log_write(DEBUG,__FILE__,__FUNCTION__,__LINE__,buf,##__VA_ARGS__);\
}while(0)
#else 
#define clowf_log_debug(buf,...) do{}while(0)
#endif
#define LogDebug clowf_log_debug

#ifndef NON_USE_CLOWF_LOG
#define clowf_log_info(buf,...) do{\
log_write(TRACE,__FILE__,__FUNCTION__,__LINE__,buf,##__VA_ARGS__);\
}while(0)
#else 
#define clowf_log_info(buf,...) do{}while(0)
#endif
#define LogInfo clowf_log_info

#ifndef NON_USE_CLOWF_LOG
#define clowf_log_warnNING(buf,...) do{\
log_write(WARNNING,__FILE__,__FUNCTION__,__LINE__,buf,##__VA_ARGS__);\
}while(0)
#else 
#define clowf_log_warnNING(buf,...) do{}while(0)
#endif
#define LogWarn clowf_log_warnNING

#ifndef NON_USE_CLOWF_LOG
#define clowf_log_error(buf,...) do{\
log_write(ERROR,__FILE__,__FUNCTION__,__LINE__,buf,##__VA_ARGS__);\
}while(0)
#else 
#define clowf_log_error(buf,...) do{}while(0)
#endif
#define LogError clowf_log_error

void clowf_log_flush();

/**
 * @brief  : get last function called error 
 *
 * @return : error message
 */
const char* clowf_log_last_error();

#ifdef __cplusplus
}
#endif


#endif
