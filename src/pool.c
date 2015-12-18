/*
 * =====================================================================================
 *
 *       Filename:  resourepool.c
 *
 *    Description:  resource pool implements
 *
 *        Version:  1.0
 *        Created:  08/06/2012 05:07:22 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Michael LiuXin
 *
 * =====================================================================================
 */
#include "pool.h"

ub_pool_t* ub_pool_create()
{
	ub_pool_t* rp = (ub_pool_t*)calloc(1,sizeof(ub_pool_t));
	if (NULL == rp) {
		return NULL;
	}

	rp->resource_list = list_create();
	if (NULL == rp->resource_list) {
		goto exception;
	}

	return rp;

exception :
	free(rp);
	return NULL;
}

void ub_pool_destroy(ub_pool_t* rp)
{
	if (!rp || !rp->resource_list) {
		return ;
	}


	list_iter* itr = NULL;
	if (NULL==(itr=list_get_iterator(rp->resource_list,AL_START_HEAD))) {
	} else {
		list_node* node = NULL; 
		/* iterate the list and call destructor */
		while(NULL != (node=list_next(itr))) {
			if (node) {
				res_t* r = (res_t*)node->value;
				if (r) {
					if (NULL != r->destructor) {
						// call it
						r->destructor(r->resource);
					}
				} 
			} 
		}
	}

	list_release(rp->resource_list);
	rp->resource_list = NULL;
	free(rp);

}

void* ub_pool_attach(ub_pool_t* rp,void* r, new_handler constructor,destroy_handler destructor)
{
	if (!rp || !rp->resource_list) {
		return NULL;
	}

	void* feedback = r;

	res_t* resource = (res_t*)calloc(1,sizeof(res_t));
	if (NULL==resource) {
		return NULL;
	} else {
		resource->resource = r;
		// user has constructor , so we help constructing it
		if (NULL==r && NULL!=(resource->constructor=constructor)) {
			feedback = resource->constructor();	
			resource->resource = feedback;
		}
		resource->destructor = destructor;
		list_add_tail(rp->resource_list,resource);
	}

	return feedback;

}

