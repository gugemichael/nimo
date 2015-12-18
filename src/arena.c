/*
 * Copyright 2012 , Michael LiuXin <guguemichael@gmail.com>
 *
 * Contributors can discuss or commit code at
 *
 *  http://blog.csdn.net/gugemichael (Blog)
 *  https://github.com/gugemichel/libub (Git)
 *
 * Version : 1.0
 * Created : 01/21/2013 03:26:23 PM
 * Filename : ub_mempool.h
 * Description : A Pooled Memory pool
 * Compiler : g++ 4.6.1
 * Author : Michael LiuXin
 *
 */

#include "arena.h"

/**
 * Don't allocate more than 10M space, because this mempool
 * just implement simple way of pool, it don't free anything
 * util call arena_destroy(), this feature is based 
 * on JUST USE it in an funtion or in one class
 */
void* arena_create(uint32_t size)
{
	if (size==0 || size>=1024*10) {
		return NULL;
	}

	// align of 4 byte
	// size += size % 4;

	_arena_mem_t *pool = (_arena_mem_t*)calloc(1,sizeof(_arena_mem_t));
	pool->size = 1024*size;
	pool->area = calloc(1,1024*size);
	pool->cursor = pool->area;


	return pool;
}

uint32_t arena_remain_cap(_arena_mem_t *pool) 
{
	if (NULL == pool)
		return -1;	
	else return pool->size - (pool->cursor - pool->area);
}

void* arena_alloc(_arena_mem_t* pool, uint32_t n)
{
	// no space here
	if (NULL==pool || NULL==pool->area) {
		return NULL;
	}

	void* ret = pool->cursor;
	// move the cursor
	pool->cursor = (void*)((char*)pool->cursor + n);

	return ret;
}


void arena_destroy(_arena_mem_t* pool)
{
	if (NULL == pool)
		return;

	free(pool->area);
	pool->cursor = pool->area = NULL;	
	pool->size = 0;
}
