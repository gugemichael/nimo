/*
 * UB Library Framework For C
 *
 * Copyright 2012 , Michael LiuXin <guguemichael@gmail.com>
 *
 * Contributors can discuss or commit code at
 *
 *  http://blog.csdn.net/gugemichael (Blog)
 *  https://github.com/gugemichel/libub (Git)
 *
 * Version : 1.0
 * Created : 10/18/2012 03:26:23 PM
 * Filename : ub_memchunk.h
 * Description : A Complex Slab-Chunk Memory pool
 * Compiler : g++ 4.6.1
 * Author : Michael LiuXin
 *
 */
#ifndef __CLOWF_SLAB_H__
#define __CLOWF_SLAB_H__

#include <stdlib.h>

#define MEMCHUNK_OK 0
/* error : system internal error */
#define MEMCHUNK_SYSTEM_ERROR 1
/* error : no enough memory */
#define MEMCHUNK_NOENOUGH_MEMORY 2

/* allocate chunks' space when created*/
#define USE_PREALLOC 1 	
/* don't allocate when created */
#define NO_PREALLOC 0 	

/* one slab's max size , default is 16M */
#define	MAX_SLAB_CAPACITY (1024*1024*16)
/* chunk size is 4M */
#define	CHUNK_ITEM_SIZE (1024*1024*4)

/**
 * Chunk , A big memory area allocated sequencely
 */
struct chunk_t {
	unsigned char memory[CHUNK_ITEM_SIZE];
	size_t used; 		/* next cell index wich can be used */
	struct chunk_t* next; 	/* chunk linker */
};

/*
 * free memory to record
 */
struct free_t {
	void* ptr;
	struct free_t* next;
};

/**
 * Slab , A list of chunk
 */
struct slab_t {
	size_t count;
	size_t block_size;
	size_t chunk_cursor;
	struct chunk_t* chunk_list;	
	//struct free_t* free_list;	
};

/**
 * Mempool with 4,8,16,32 ... (slabs)
 */
struct memchunk_t {
	size_t count;
	struct slab_t* slabs[]; 
};

/**
 * @brief : create a memchunk pool
 * 
 * @param : [in] if the mempool should alloc some firstlly
 *
 * @return : the memchunk handle
 */
struct memchunk_t* memchunk_pool_create(int prealloc);

/**
 * @brief : destroy memchunk pool
 */
void memchunk_pool_destroy(struct memchunk_t*);

/**
 * @brief : allocate memroy from pool 
 * 
 * @param : [in] memchunk_t* 
 * 			[in] how many bytes to alloc , it will be align
 *
 * @return : the memchunk handle
 */
void* memchunk_pool_alloc(struct memchunk_t*, size_t size);

void* memchunk_pool_realloc(struct memchunk_t*, void* ptr, size_t size);

#endif



