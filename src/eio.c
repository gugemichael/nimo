/* 
 * Implemention of nimo eio network library
 *
 * */

#include "eio.h"

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
	assert(mask != EIO_CLEAR || proc == NULL);
	assert(fd > 0);

	struct epoll_event ev = {0};
	ev.data.fd = fd;        

	if (mask & EIO_WRITE)
		ev.events |= EPOLLOUT;
	if (mask & EIO_READ)
		ev.events |= EPOLLIN; 

	// decide op on fd MOD or ADD
	int op = 0;
	if (EIO_CLEAR != mask)
		op = eio->eio_evs[fd].mask ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
	else
		op = EPOLL_CTL_DEL;

	if (-1 == epoll_ctl(eio->epfd, op, fd, &ev))
		return -1;

	mask & EIO_READ ? eio->eio_evs[fd].rproc = proc : 0;
	mask & EIO_WRITE ? eio->eio_evs[fd].wproc = proc : 0;

	// update mask bitset
	if (mask & EIO_CLEAR) 
		eio->eio_evs[fd].mask = eio->eio_evs[fd].ep_mask = 0;
	else {
		eio->eio_evs[fd].mask = mask;
		eio->eio_evs[fd].ep_mask = ev.events;
	}

	eio->eio_evs[fd].user_data = context;

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


static int check_timer_and_run() {
	return 0;
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

		// check the timer event
		int time_events = check_timer_and_run();

		eio->stats.timer_events += time_events;

		int n = epoll_wait(eio->epfd, eio->poll_evs, eio->max_events, 1000 / eio->hz);

		if (n <= 0 || errno == EINTR)
			continue;

		// if we encounter the SIGTRAP , gdb/pstack 
		if (0 == eio->poll_evs[0].data.fd && errno == EINTR) 
			continue;

		for (int i=0; i!=n; i++) {

			struct eio_event *ev = &eio->eio_evs[eio->poll_evs[i].data.fd];

			// events may not be cared in this time
			if (0 == ev->mask)
				continue;

			if ((eio->poll_evs[i].events & EPOLLIN) && ev->rproc)
				ev->rproc(eio, eio->poll_evs[i].data.fd, EIO_READ, ev->user_data);

			if ((eio->poll_evs[i].events & EPOLLOUT) && ev->wproc)
				ev->wproc(eio, eio->poll_evs[i].data.fd, EIO_WRITE, ev->user_data);
		}
	}

	return eio;
}


inline void eio_loop_set_event_hz(eio_loop* eio, unsigned int hz) {
	eio->hz = hz; 
}


inline void eio_loop_stop(eio_loop *eio) {
	eio->run = 0;
}
