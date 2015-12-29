/*
 *
 * Copyright 2012 , Michael LiuXin <guguemichael@gmail.com>
 *
 * Contributors can discuss or commit code at
 *
 *  http://blog.csdn.net/gugemichael (Blog)
 *  https://github.com/gugemichel/libub (Git)
 *
 * Version : 1.0
 * Created : 01/21/2013 03:26:23 PM
 * Filename : ub_arena_mem.h
 * Description : A Pooled Memory pool
 * Compiler : g++ 4.6.1
 * Author : Michael LiuXin
 *
 */

#ifndef  __NIMO_ARENA_H__
#define __NIMO_ARENA_H__

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

typedef struct _arena_mem _arena_mem_t;

struct _arena_mem {
	uint32_t size;		// memory pool total size (kb)
	void *area;
	void *cursor;		// indicate the current position of pool
};

/**
 * Don't allocate more than 10M space, because this mempool
 * just implement simple way of pool, it don't free anything
 * util call arena_destroy(), this feature is based 
 * on JUST USE it in an funtion or in one class
 */
void* arena_create(uint32_t size);

/**
 * Get the remaining capacity
 */
uint32_t arena_remain_cap(_arena_mem_t *pool);

/**
 * Alloc aligned memroy
 */
void* arena_alloc(_arena_mem_t* pool, uint32_t n);

/*
 * The last free all previous alloced memory which
 * don't free ! only call this function at the end 
 * of using simple_pool
 */
void arena_destroy(_arena_mem_t* pool);

#endif     
