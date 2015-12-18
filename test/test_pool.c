#define NO_USE_UB_LOG
#include "ub_pool.h"
#include "ub_log.h"
#undef NO_USE_UB_LOG

void* fd_close(void* param)
{
	int* fd = (int*)param;
	ublog_trace("User Defined Func");
	int err = close(*fd);
	if (-1 == err) {
		char buf[1024] = {0};
		snprintf(buf,1024,"[err=system] [errno=%d] [errmsg=%s] [fd=%d]",errno,strerror(errno),*fd);
		ublog_fatal(buf);
	} else 
		ublog_debug("[ok=system] close ok");

	return NULL;
}

void* fd_create(void* param)
{
	int* fd = (int*)calloc(1,sizeof(int));
	*fd = open("./test.c",O_RDWR);
	if (*fd==-1)
		ublog_fatal("[err=system] open file faild");
	return fd;
}

int main()
{
	ub_log_split_init(NULL,0,0,64);


	ub_pool_t* rp = ub_pool_create();
	//int fd = open("./test.c",O_RDWR);
	//ub_pool_attach(rp,&fd,NULL,fd_close);
	ub_pool_attach(rp,NULL,fd_create,fd_close);

	ublog_trace("Init & Attach ok");
	ub_pool_destroy(rp);


	return 0;
}
