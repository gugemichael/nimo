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

#define MAGIC_NR (0x1314)

/**
 * Don't allocate more than 10M space, because this mempool
 * just implement simple way of pool, it don't free anything
 * util call arena_destroy(), this feature is based 
 * on JUST USE it in an funtion or in one class
 */
void* arena_create(uint32_t size, long size_unit)
{
	if (size==0 || size>=1024*10) {
		return NULL;
	}

	// align of 4 byte
	// size += size % 4;

	int bytes = size_unit * size;

	arena_block *pool = (arena_block*) calloc(1,sizeof(*pool) + bytes);
	pool->magic_nr = MAGIC_NR;
	pool->capacity = bytes;
	pool->cursor = pool->arena;

	return pool;
}

uint32_t arena_remain_cap(void *pool) 
{
	if (NULL == pool || ((arena_block*)pool)->magic_nr != MAGIC_NR)
		return 0;	

	return ((arena_block*)pool)->capacity;
}

void* arena_alloc(void *pool, uint32_t n)
{
	arena_block* block = (arena_block*)pool;

	if (NULL == pool || block->magic_nr != MAGIC_NR)
		return NULL;

	// no space here
	if (block->cursor + n > block->arena + block->capacity)
		return NULL;

	// move the cursor
	block->cursor = (char*)block->cursor + n;

	return block->cursor;
}


void arena_destroy(void* pool)
{
	if (NULL == pool || ((arena_block*)pool)->magic_nr != MAGIC_NR)
		return;

	((arena_block*)pool)->capacity = 0;

	free(pool);
}
