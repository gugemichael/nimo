/* 
 * Implemention of nimo eio network library
 *
 * */

#include "eio.h"

#include <linux/version.h>

/** 
 * new_eio_loop 
 */
eio_loop* new_eio_loop(unsigned int max) {

	eio_loop *eio = (eio_loop*) calloc(1, sizeof(eio_loop));

	if (eio == NULL)
		return NULL;

	eio->run = 0;

	//just a hint for kernel
	eio->epfd = epoll_create(max);

	if (-1 == eio->epfd) 
		goto error;

	max = (max >= 64 ? max : 64);

	eio->max_events = max;
	eio->poll_evs = (struct epoll_event*) calloc(max, sizeof(struct epoll_event));
	eio->eio_evs = (struct eio_event*) calloc(max, sizeof(struct eio_event));

	eio->hz = DEFAULT_EIO_LOOP_HZ;

	return eio;
error:
	free(eio);
	return NULL;
}


/** 
 * eio_loop_file_event
 */
int eio_loop_file_event(eio_loop *eio, int fd, int mask, ev_file_proc proc, void* context) {

	assert(eio != NULL);
	assert(fd > 0);
	assert(eio->max_events > fd);
	assert(((mask & EIO_CLEAR) && proc == NULL) || (!(mask & EIO_CLEAR) && proc != NULL));

	struct epoll_event ev = {0};
	ev.data.fd = fd;        

	// EPOLLHUP and EPOLLERR is automatic and always associated events
	
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,17)
	ev.events |= EPOLLRDHUP;
#endif

	// use eadge trigger 
	ev.events |= EPOLLET;

	if (mask & EIO_WRITEABLE)
		ev.events |= EPOLLOUT;
	if (mask & EIO_READABLE)
		ev.events |= EPOLLIN; 


	// decide op on fd MOD or ADD
	int op = 0;
	if (EIO_CLEAR & mask)
		op = EPOLL_CTL_DEL;
	else
		op = eio->eio_evs[fd].mask ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;

	/*
	 * we discard return -1 and error. because of duplicated register
	 * and epoll_ctl an existing file event. and for syscall performance 
	 * consideration. so we return -1 directly and throw it to user
	 *
	 * Note: Kernel < 2.6.9 requires a non null event pointer even for *EPOLL_CTL_DEL*
	 *
	 */
	if (-1 == epoll_ctl(eio->epfd, op, fd, &ev))
		return -1;

	if (mask & EIO_READABLE)
		eio->eio_evs[fd].ev.file.rproc = proc;
	if (mask & EIO_WRITEABLE)
		eio->eio_evs[fd].ev.file.wproc = proc;
	if (mask & EIO_ERR)
		eio->eio_evs[fd].ev.file.errproc = proc;

	// update mask bitset
	if (mask & EIO_CLEAR) {
		eio->eio_evs[fd].mask = eio->eio_evs[fd].ev.file.ep_mask = 0;
		eio->eio_evs[fd].user_data = NULL;
		eio->eio_evs[fd].ev.file.rproc = NULL;
		eio->eio_evs[fd].ev.file.wproc = NULL;
	} else {
		eio->eio_evs[fd].mask = mask;
		eio->eio_evs[fd].ev.file.ep_mask = ev.events;
		eio->eio_evs[fd].user_data = context;
	}

	return 0;
}



/**
 * eio_loop_destroy
 */
void eio_loop_destroy(eio_loop *eio) {
	assert(eio != NULL);
	assert(eio->run != 1);

	close(eio->epfd);
	free(eio);
}


/**
 * eio_loop_use_before_proc
 */
void eio_loop_use_before_proc(eio_loop *loop, eio_loop_before proc) {
	assert(loop != NULL);

	loop->before = proc;
}


inline void eio_loop_stop(eio_loop *eio) {
	eio->run = 0;
}

static int poll_time_event(eio_loop* eio) {
	return 0;
}


static int poll_file_event(eio_loop* eio) {

	int n = epoll_wait(eio->epfd, eio->poll_evs, eio->max_events, 1000 / eio->hz);

	// if we encounter the SIGTRAP , gdb/pstack 
	if (n <= 0 && errno == EINTR)
		return 0;

	if (0 == eio->poll_evs[0].data.fd && errno == EINTR) 
		return 0;

	for (int i=0; i!=n; i++) {

		struct eio_event *event = &eio->eio_evs[eio->poll_evs[i].data.fd];

		// events may not be cared in this time
		// discard in this time !!
		if (0 == event->mask)
			continue;

		uint32_t happen = eio->poll_evs[i].events;

		if ((happen & EPOLLERR) || (happen & EPOLLHUP)
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,17)
			|| (happen & EPOLLRDHUP)
#endif
		   ) {

			// for ERROR
			if (event->ev.file.errproc != NULL)
				event->ev.file.errproc(eio, eio->poll_evs[i].data.fd, EIO_ERR, event->user_data);
		} else if ((eio->poll_evs[i].events & EPOLLIN) && event->ev.file.rproc) {
			// for READ
			event->ev.file.rproc(eio, eio->poll_evs[i].data.fd, EIO_READABLE, event->user_data);
		} else if ((eio->poll_evs[i].events & EPOLLOUT) && event->ev.file.wproc) {
			// for WRITE
			event->ev.file.wproc(eio, eio->poll_evs[i].data.fd, EIO_WRITEABLE, event->user_data);
		}
	}

	return n;
}

/**
 * eio_loop_run
 */
eio_loop* eio_loop_run(eio_loop *eio) {

	assert(eio != NULL);

	eio->run = 1;

	// do an infinite loop for epoll_wait
	while(eio->run) {

		if (eio->before != NULL)
			eio->before(eio);

		/**
		 * check the timer event
		 */
		eio->stats.timer_events += poll_time_event(eio);

		/**
		 * check the timer event
		 */
		eio->stats.file_events += poll_file_event(eio);

	}

	return eio;
}


