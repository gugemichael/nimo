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

#include "ub_mempool.h"

int main()
{
	ub_log_split_init("mempool",0,0,64);

	ub_log_page_buffer();

	ublog_debug("start allocating ...");

	int allocate_times = 100000;
	while(allocate_times--) {
		ublog_debug("[warn=memory] memory pool allocate faild");
	}

	return 0;
}
