/*
 * =====================================================================================
 :A
 *
 *       Filename:  memchunk.c
 *
 *    Description:  implement of Memchunk pool
 *
 *        Version:  1.0
 *        Created:  10/18/2012 04:11:30 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Michael LiuXin
 *
 * =====================================================================================
 */
#include "slab.h"

#define CHUNK_NO_RESERVE (-1)

static inline int chunk_is_full(struct chunk_t* chunk, size_t block_size)
{
	if (NULL == chunk)	
		return 1;

	return (chunk->used*block_size >= CHUNK_ITEM_SIZE) ? 1 : 0;
}

static inline void* chunk_fetch(struct chunk_t* chunk, size_t block_size)
{
	if (NULL == chunk)	
		return NULL;

	return chunk->memory+(chunk->used++*block_size);
}

//
// SLABS_NUM
//
static inline unsigned int SLABS_NUM()
{
	unsigned int num = 4 , result = 0;
	while(num <= MAX_SLAB_CAPACITY) {
		result++; 
		num *= 2; 
	}
	return result;
}

//
// [private] __new_chunk
// 
struct chunk_t* __new_chunk()
{
	struct chunk_t* builder = (struct chunk_t*)calloc(1,sizeof(struct chunk_t));
	if (NULL == builder)
		return NULL;
	builder->used = 0;
	builder->next = NULL;

	return builder;
}

struct slab_t* __new_slab(size_t index) 
{
	struct slab_t* result = (struct slab_t*)calloc(1,sizeof(struct slab_t));
	if (NULL == result)
		return NULL;
	result->count = 1;
	// min size is 4-byte
	result->block_size = (4 << index);
	result->chunk_cursor = 0;
	// alloc chunk
	result->chunk_list = __new_chunk();
	if (NULL == result->chunk_list) {
		free(result);
		return NULL;
	} else
		return result;
}

//
// memchunk_pool_create
//
struct memchunk_t* memchunk_pool_create(int prealloc)
{
	unsigned int num = SLABS_NUM();

	struct memchunk_t* pool = (struct memchunk_t*)calloc(1,sizeof(struct memchunk_t)
			+(sizeof(struct slab_t*)*num));

	if (NULL == pool) {
		return NULL;
	}

	pool->count=num;

	if (NO_PREALLOC == prealloc) {
		return pool;
	}

	// alloacate some area firstly
	int index = 0;
	for (;index!=num;index++) {

		// alloc slab
		pool->slabs[index] = __new_slab(index);
		if (NULL == pool->slabs[index])
			goto Exception;

		// alloc chunk
		pool->slabs[index]->chunk_list = __new_chunk();
		if (NULL == pool->slabs[index]->chunk_list) {
			free(pool->slabs[index]);
			goto Exception;
		}
	}

	return pool;

Exception :


	free(pool);
	return NULL;
}


//
// memchunk_pool_destroy
//
void memchunk_pool_destroy(struct memchunk_t* memchunk)
{
	if (NULL == memchunk)
		return ;

	int i = 0;
	for (;i!=memchunk->count;i++) {
		if (NULL == memchunk->slabs[i])
			continue;
		struct chunk_t* cursor = memchunk->slabs[i]->chunk_list;
		while(cursor) {
			struct chunk_t* tmp = cursor->next;
			// delete chunk
			free(cursor);	
			cursor = tmp;
		}
		// delete slabs
		free(memchunk->slabs[i]);
		memchunk->slabs[i] = NULL;
	}

	free(memchunk);
}


void* memchunk_pool_alloc(struct memchunk_t* pool, size_t size)
{
	if (NULL == pool || 0 == size) {
		return NULL;
	}


	if (size > MAX_SLAB_CAPACITY) {
		return NULL;
	}

	// get the proper slab 
	size_t n=4,index=0;
	while((size>n)&&++index)
		n *= 2;

	struct slab_t* slab = pool->slabs[index];

	if (NULL == slab) {
		// alloc space first
		slab = pool->slabs[index] = __new_slab(index);
		if (slab == NULL)
			return NULL;
	} 

	do {

		size_t cursor = slab->chunk_cursor;
		struct chunk_t* chunk = slab->chunk_list;
		while(cursor--)
			chunk = chunk->next;

		// if no chunk , allocate
		if (chunk_is_full(chunk,slab->block_size)) {
			if (NULL != chunk->next) {
				break;
			} else {
				chunk->next = __new_chunk();
				if (NULL == chunk->next) {
					break;
				} else
					slab->chunk_cursor++;
			}
		}

		void* result = chunk_fetch(chunk,slab->block_size);
		return result;

	}while(0);

	return NULL;

}

void* memchunk_pool_realloc(struct memchunk_t* pool, void* ptr, size_t size)
{
	if (NULL == pool || NULL == ptr || 0 == size)
		return NULL;

	return NULL;
}







