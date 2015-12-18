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
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>

#include "ub_memchunk.h"

int main()
{
	ub_log_split_init("mempool",0,0,64);

	ublog_debug("start allocating ...");

	srand((unsigned int)time(NULL));

	struct memchunk_t* pool_tmp = memchunk_pool_create(NO_PREALLOC);
	memchunk_pool_destroy(pool_tmp);
	struct memchunk_t* pool = memchunk_pool_create(USE_PREALLOC);

	//int allocate_times = 40000;
	int allocate_times = 10;
	unsigned long max = 4*1024*1024*2;
	while(allocate_times--) {
		//size_t s = rand() % 32;
		size_t s = rand() % max;
		if (allocate_times % 1000 == 0)
			s = 16*1024*1023;
		long long *tmp = memchunk_pool_alloc(pool,s);
		if (NULL == tmp) {
			ublog_warn("[warn=memory] memory pool allocate faild size[%lu]",s);
		} else {
			//ublog_debug("[ok=memchunk] [%x]",tmp);
		}
	}
	printf("Done\n");
	getchar();
	memchunk_pool_destroy(pool);
	printf("Destroy Done\n");

	return 0;
}
