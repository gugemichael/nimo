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
 * Created : 08/06/2012 04:39:25 PM
 * Filename : ub_pool.h
 * Description : A Resrouce Pool , to manage user's resource , such as fd,memory...
 * Compiler : g++ 4.6.1
 * Author : Michael LiuXin
 *
 */

#include <stdlib.h>
#include <stdio.h>

#include "list.h"

typedef void* (*destroy_handler)(void*);
typedef void* (*new_handler)();
typedef struct ub_pool_t ub_pool_t;
typedef struct res_t res_t;

struct ub_pool_t {
	/* resource collection */
	list* resource_list;
	/* error */
	const char* errmsg;
};	

struct res_t {
	/* fd , memory_ptr , socket and so on */
	void* resource;
	/* user defined how to deal the resource , such as close() */
	new_handler constructor;	
	destroy_handler destructor;	
};	

inline const char* ub_pool_error(ub_pool_t* rp)
{
	if (rp)
		return rp->errmsg;
	else
		return NULL;
}

/** 
 * Create a resource pool
 *
 * @return : pool handle , NULL if has error (errmsg will be set)
 */
ub_pool_t* ub_pool_create();

/** 
 * Destroy a resource pool
 *
 * @param [in] resource pool handle
 */
void ub_pool_destroy(ub_pool_t* rp);

/** 
 * Push a managed resource item to pool
 *
 * @param  [in] resource pool handle
 * 		   [in] user's resource
 * 		   [in] constructor function
 * 		   [in] destructor function
 */
void* ub_pool_attach(ub_pool_t* rp,void* resource, new_handler constructor,destroy_handler destructor);

