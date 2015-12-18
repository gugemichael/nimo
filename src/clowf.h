/*
 * UB Library Framework For C
 *
 * Copyright 2012 , Michael LiuXin <guguemichael@gmail.com>
 *
 * Contributors can discuss or commit code at
 *
 *  http://blog.csdn.net/gugemichael (Blog)
 *  https://github.com/gugemichel/libub (Git)
 *
 * Version : 1.0
 * Created : 11/22/2012 10:43:00 AM
 * Filename : ub.h
 * Description : UB c library general tools
 * Compiler : g++ 4.6.1
 * Author : Michael LiuXin
 *
 */

#ifndef __CLOWF_H__
#define __CLOWF_H__

#define MICAHEL "Michael"
#define MICAHEL_LIUXIN "Michael LiuXin"

#ifdef __cplusplus
extern "C" {
#endif

// branch predict
#define likely(x) __builtin_expect(!!(x),1)
#define unlikely(x) __builtin_expect(!!(x),0)

// useless code comment
#define CLOWF_NON_USE(expr) do {(void)(expr);} while (0)

// CLOWF_DEBUG dependency assert
#ifdef CLOWF_DEBUG
#define clowf_assert(x) assert(x);
#else
#define clowf_assert(x) ;
#endif

#ifndef clowf_ok
#define clowf_ok (0)
#define clowf_fail (!(clowf_ok))
#endif


#if HAVE_STDBOOL_H                                                                                              
#include <stdbool.h>                                                                                            
#else
#define bool char                                                                                               
#define false 0                                                                                                 
#define true 1                                                                                                  
#endif 


/**
 * @Description : make daemon process
 * @Params 		: void
 * @Return  		: void
 */
#define clowf_daemonize() do { \
    int fd; \
    if (fork() != 0) \
		exit(0); /* parent exits */ \
    setsid(); /* create a new session */ \
    if ((fd = open("/dev/null", O_RDWR, 0)) != -1) { \
        dup2(fd, STDIN_FILENO); \
        dup2(fd, STDOUT_FILENO); \
        dup2(fd, STDERR_FILENO); \
        if (fd > STDERR_FILENO) close(fd); \
    } \
}while(0); \

/**
 * @brief 	: get current milliseconds
 *
 * @params 	: void
 *
 * @return 	: void
 */
#define clowf_ustime() do{ \
	struct timeval time; \
	gettimeofday(&time, 0x0L);\
	return time.tv_sec * 1000 + time.tv_usec;\
}while(0)


#ifdef __cplusplus
}
#endif

#endif

