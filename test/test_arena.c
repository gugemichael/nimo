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
#include "nimo/arena.h"

#define threads (6)

size_t logs[threads] = {0};

uint32_t allocated = 0L;

void sleep_test() {
	usleep(1000000);
}

int main()
{

	nimo_log_init(NULL);

	void* block = arena_create(2, ARENA_UNIT_IN_MB);

	while(NULL != arena_alloc(block,44))
		allocated += 44;

	nimo_log_debug("total : %lu bytes , allocated %lu bytes", 2 * ARENA_UNIT_IN_MB, allocated);

	arena_destroy(block);

	nimo_log_destroy();


	return 0;
}




