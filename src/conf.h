/*
 * UB Library Framework For C
 *
 * Copyright 2012 , Michael LiuXin <guguemichael@gmail.com>
 *
 * Contributors can discuss or commit code at
 *
 *  http://blog.csdn.net/gugemichael (Blog)
 *  https://github.com/gugemichel/libclowf (Git)
 *
 * Version : 1.0
 * Created : 01/20/2013 04:15:32 PM
 * Filename : clowf_conf.h
 * Description : Load User Configure File , And Split to KV
 * Compiler : g++ 4.6.1
 * Author : Michael LiuXin
 *
 */

#ifndef  __CLOWF_CONF_LOADER_H__
#define	 __CLOWF_CONF_LOADER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* kv paire */
typedef struct kv_t {
	char* k;
	char* v;
}kv_t;

/* Loaded config data */
typedef struct config_t{
	kv_t* kv_list;	
	size_t count;
	size_t used;
}config_t;

/**
 * Load configure string from char* , and parse. 
 * string MUST like "key:value"
 *
 * @configure conf in kv string format
 *
 * return configure structure
 */
config_t* clowf_config_load(const char* configure);

/**
 * Load configure string from file (path) , 
 * this will invoke clowf_config_load()
 *
 * @file absolutely file path
 *
 * return configure structure
 */
config_t* clowf_config_load_from_file(const char* file);

/**
 * Get number from spicified key
 * 		  
 * @cfg configure structrue
 * @key
 *
 * return key's value , if value isn't a number or 
 * 		  not exist, "0" will be returned.
 */
long clowf_config_integer(config_t* cfg, const char* key);

/**
 * Get CONST char string from spicified key
 *
 * @cfg configure structrue
 * @key
 *
 * @return : key's value , if value isn't exist , 
 * 			 "NULL" will be returned
 */
const char* clowf_config_string(config_t* cfg, const char* key);

/**
 * Release configure structure , failed impossible 
 *
 * @cfg configure structrue to be released
 */
void clowf_config_release(config_t* cfg);

size_t clowf_config_used(config_t*);
void clowf_config_print(config_t*);

#ifdef __cplusplus
}
#endif

#endif     

