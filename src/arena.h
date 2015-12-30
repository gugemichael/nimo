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

#define	ARENA_UNIT_IN_MB (1024L * 1024L)
#define	ARENA_UNIT_IN_KB (1024L)
#define	ARENA_UNIT_IN_BYTE (1L)

typedef struct _arena_block {
	uint16_t magic_nr; 
	uint32_t capacity;		// memory pool total size (kb)
	char* cursor;			// allocated cursor
	char arena[];
} arena_block;
//__attribute__((__aligned__((4))));

/**
 * Don't allocate more than 10M space, because this mempool
 * just implement simple way of pool, it don't free anything
 * util call arena_destroy(), this feature is based 
 * on JUST USE it in an funtion or in one class
 */
void* arena_create(uint32_t size, long size_unit);

/**
 * Get the remaining capacity
 */
uint32_t arena_remain_cap(void *pool);

/**
 * Alloc aligned memroy
 */
void* arena_alloc(void *pool, uint32_t n);

/*
 * The last free all previous alloced memory which
 * don't free ! only call this function at the end 
 * of using simple_pool
 */
void arena_destroy(void* pool);

#endif     
