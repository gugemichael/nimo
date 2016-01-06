/*
 * =====================================================================================
 *
 *       Filename:  epoll.c
 *
 *    Description:  A example for Linux epoll
 *
 *        Version:  1.0
 *        Created:  03/28/2012 03:40:37 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <sys/socket.h>   
#include <sys/epoll.h>   
#include <sys/time.h>
#include <sys/resource.h>
#include <netinet/in.h>   
#include <arpa/inet.h>   
#include <fcntl.h>   
#include <unistd.h>   
#include <stdio.h>   
#include <errno.h>   
#include <stdlib.h>
#include <assert.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <stddef.h>
#include <inttypes.h>

#include <netinet/tcp.h>
#include <string.h>
#include <strings.h>

#include <error.h>

#include <sys/epoll.h>

#include "nimo/log.h"
#include "nimo/eio.h"

#define MAX_CONNS 4096 
#define EPOLL_WAIT_TIMEOUT (-1)

#define EVENT_READABLE 1
#define EVENT_WRITEABLE 2

#define IP_UNKNOWN ("unknown")
#define PORT_UNKNOWN (0)

#define HEART_BEAT_INTERVAL (1)
#define HEART_BEAT_KEEPLIVE (4)

#define BACKLOG 1024

#define SESSION_MONITOR

void tcp_accept(eio_loop* eio, int server_socket, int mask, void* context);
void tcp_write(eio_loop* eio, int server_socket, int mask, void* context);
void tcp_read(eio_loop* eio, int server_socket, int mask, void* context);
void tcp_close(eio_loop* loop, int clientfd);


int LIMIT = 0;

void tcp_accept(eio_loop* eio, int server_socket, int mask, void* context)
{
	// the server must be accepted
	struct sockaddr_in client;
	socklen_t client_size = sizeof(struct sockaddr_in);

	int cfd = accept(server_socket,(struct sockaddr*)&client,&client_size);

	nimo_log_debug("[ok=accept] fd[%d] connected", cfd);

	if (cfd != -1) {
		int flag = fcntl(cfd,F_GETFL,0);
		// nonblocking
		flag |= O_NONBLOCK;
		if (-1 == fcntl(cfd,F_SETFL,flag))
			nimo_log_error("[warn=socket] socket set O_NONBLOCK faild");
		// no delay (without nagle)
		int nodelay = 1;
		if (-1 == setsockopt(cfd,IPPROTO_TCP,TCP_NODELAY,&nodelay,sizeof(nodelay)))
			nimo_log_error("[warn=socket] socket TCP_NODELAY set faild");

		// add the client_fd to epoll loop
		eio_loop_file_event(eio, cfd, EIO_READABLE, tcp_read, NULL);

	} else {
		nimo_log_error("[err=socket] socket accept faild , errcode : %d, errmsg : %s",errno,strerror(errno));
	}

}


void tcp_close(eio_loop* loop, int clientfd)
{
	nimo_log_debug("[ok=socket] Client socket close");

	// clean up event in epoll
	eio_loop_file_event(loop, clientfd, EIO_CLEAR, NULL, NULL);

	close(clientfd);
}

void tcp_write(eio_loop* eio, int clientfd, int mask, void* context)
{
	//const char* echo = "nimo_eio_server\n";
	const char* echo = "+OK\r\n";
	size_t len = strlen(echo);
	while(1) {
		int n = write(clientfd,echo,len);
		nimo_log_debug("[ok=write] write [%d] bytes",n);
		if (-1==n && errno==EAGAIN) {
			nimo_log_debug("[ok=socket] Write Socket Pause");
			break;
		}
		len -= n;
		if (len <= 0)
			break;
	}

	eio_loop_file_event(eio, clientfd, EIO_READABLE, tcp_read, NULL);
}

void tcp_read(eio_loop* eio, int clientfd, int mask, void* context)
{
	/**
	 * this method is called by epoll_wait callback if there has 
	 * something to read in buffer
	 */
	int ret = -1;
	char buffer[1024] = {0};
	while(1) {
		ret = read(clientfd,buffer,1024);
		if (LIMIT++ == 1000000)
			; // exit(0);
		if (0 == ret) {
			nimo_log_debug("[ok=socket] socket fd[%d] closed",clientfd);
			tcp_close(eio, clientfd);
			break;
		} else if (-1 == ret) { 
			if (errno == EAGAIN) {
				// would blocking
				eio_loop_file_event(eio, clientfd, EIO_WRITEABLE, tcp_write, NULL);
			} else {
				nimo_log_error("[err=socket] read error[%s] , tcp close",strerror(errno));
				tcp_close(eio, clientfd);
			}
			break;
		} else {
			// keep reading 
			nimo_log_debug("[ok=socket] fd[%d] read [%d] bytes",clientfd,ret);
		}
	}
}


#define ut_main main
int ut_main() 
{
	log_t *log = nimo_log_init(NULL);

	nimo_log_level(log, DEBUG);

	// setup a socket
	int server_socket = socket(AF_INET,SOCK_STREAM,0);
	if (-1 == server_socket) {
		nimo_log_error("[err=socket] socket create faild");
		return -1;
	}
	else
		nimo_log_debug("[ok=socket] socket create success");

	struct sockaddr_in server_addr;
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htons(INADDR_ANY);
	server_addr.sin_port = htons(6789);

	int flag=1,len=sizeof(flag);

	// we can reuse the port
	setsockopt(server_socket,SOL_SOCKET,SO_REUSEADDR,&flag,len);
	if (-1 == setsockopt(server_socket,IPPROTO_TCP,TCP_NODELAY,&flag,sizeof(flag)))
		nimo_log_error("[warn=socket] socket set SO_REUSEADDR faild");

	// bind ip/port
	if (-1 == bind(server_socket,(struct sockaddr*) &server_addr, sizeof(server_addr))) {
		nimo_log_error("[err=socket] socket bind faild");
		return -1; 
	} else
		nimo_log_debug("[ok=socket] socket bind on port[%d] success",6789);
	if (-1 == listen(server_socket,BACKLOG)) {
		nimo_log_error("[err=socket] socket listen faild");
	} else	
		nimo_log_debug("[ok=socket] socket listen backlog[%d] success",BACKLOG);


	// create a epoll server handle
	eio_loop *loop = new_eio_loop(1024);

	//eio_loop_set_event_hz(loop, 10);

	if (loop == NULL) {
		nimo_log_debug("create eio server error ! ");
		exit(0);
	}

	// firstly listen the server's socket with ACCEPT
	eio_loop_file_event(loop, server_socket, EIO_READABLE, tcp_accept, NULL);

	printf("eio server run backgroud\n");

	// do event loop 
	eio_loop_run(loop);

	return 0;
}





