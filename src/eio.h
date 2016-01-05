/*
 * Library Framework For C
 *
 * Copyright 2012 , Michael LiuXin <guguemichael@gmail.com>
 *
 * Contributors can discuss or commit code at
 *
 *  http://blog.csdn.net/gugemichael (Blog)
 *  https://github.com/gugemichel/libnimo (Git)
 *
 * Version : 1.0
 * Created : 09/19/2012 09:25:52 PM
 * Filename : nimo_log.h
 * Description : log with split files
 * Compiler : g++ 4.6.1
 * Author : Michael LiuXin
 *
 */

#ifndef __NIMO_EIO_H__
#define __NIMO_EIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>

#include <unistd.h>

#include <sys/epoll.h>


struct eio_event;
struct eio_stats;

typedef struct __eio_loop eio_loop;

enum EIO_EVENT {
	EIO_CLEAR = 0,
	EIO_READ = 1,
	EIO_WRITE = 2,
	TIMER = 4,
	EIO_CLOSE = 8
};


#define	DEFAULT_EIO_LOOP_HZ 100

/**
 * event handlers 
 */
typedef void (*eio_loop_before) (eio_loop *eventLoop);

typedef void (*ev_file_proc) (eio_loop *eventLoop, int fd, int event, void *context);
typedef int (*ev_time_proc) (eio_loop *eventLoop, void *context);


/* Event descriptor
 *
 * */
struct eio_event {
	uint32_t mask;
	uint32_t ep_mask;
	ev_file_proc rproc;
	ev_file_proc wproc;
	void* user_data;
};


/* Event loop stats information
 *
 * */
struct eio_stats {
	unsigned long conns;
	unsigned long timer_events;
};


/* Event loop server structure
 *
 * */
struct __eio_loop {

	int run; 		// running flag
	int epfd; 		// polling resource descriptor

	int hz; 		// frequence of timer event check. implemention by epoll_wait timeout on period time
					// so we suggest with 100hz one second times (means every 10ms)

	struct eio_stats stats; 	/* stats of loop */	

	int max_events;
	struct eio_event *eio_evs; 		/* register client event array */
	struct epoll_event *poll_evs;  	/* polling resource array */

	eio_loop_before before;
};



/* create eio base loop and initial it ! this handle 
 * is using for register file or timed event on following
 * statement and running infinitely
 *
 * @params max, max timed and file event in this loop
 *
 * @return new loop instance or null on failed
 * 
 * */
eio_loop* new_eio_loop(unsigned int max);

void eio_loop_set_event_hz(eio_loop* eio, unsigned int hz);

/* register events
 * 
 * @params eio , eio_loop handle
 * @params fd  , file descriptor 
 * @params mask, file event register mask flags in EIO_EVENT 
 * @params proc, file event handler on event happening
 * @params proc, user data
 *
 * @return zero on success or -1 on failed
 *
 * */
int eio_loop_file_event(eio_loop *eio, int fd, int mask, ev_file_proc proc, void* context);


/* stop the loop. before the eio_loop_destroy
 * 
 * @params eio , eio_loop handle
 *
 * */
void eio_loop_stop(eio_loop *eio);


/* destroy the loop and free the resource. you need to
 * invoke eio_loop_stop first and then destroy it !
 * 
 * @params eio , eio_loop handle
 *
 * */
void eio_loop_destroy(eio_loop *eio);


/* set procedure called before every event polling
 * trigger. DON't supply a few heavy task in it
 * 
 * */
void eio_loop_use_before_proc(eio_loop *loop, eio_loop_before proc);


/* keep running the reactor !
 * 
 * */
eio_loop* eio_loop_run(eio_loop *eio);


#ifdef __cplusplus
}
#endif


#endif
