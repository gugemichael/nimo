/*
 *
 * Copyright 2012 , Michael LiuXin <guguemichael@gmail.com>
 *
 * Contributors can discuss or commit code at
 *
 *  http://blog.csdn.net/gugemichael (Blog)
 *  https://github.com/gugemichel/libnimo (Git)
 *
 * Version : 1.0
 * Created : 01/20/2013 04:15:32 PM
 * Filename : nimo_conf.h
 * Description : Load User Configure File , And Split to KV
 * Compiler : g++ 4.6.1
 * Author : Michael LiuXin
 *
 */

#ifndef  __NIMO_CONF_LOADER_H__
#define	 __NIMO_CONF_LOADER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* kv paire */
typedef struct __kv_t {
	char* k;
	char* v;
} kv_t;

/* Loaded config data */
typedef struct __config_t {
	kv_t* kv_list;	
	size_t cap;
	size_t count;
} config_t;

/* Load configure string from char* , and parse. 
 * string format is like "key = value"
 *
 * @params configure, conf in key value format string
 *
 * @return configure structure
 *
 * */
config_t* nimo_config_load(const char* configure);

/* Load configure string from file (path) , 
 * this will invoke nimo_config_load
 *
 * @params file, absolutely file path
 *
 * @return configure structure or NULL if file not exist
 */
config_t* nimo_config_load_from_file(const char* file);

/* Get number from spicified key
 * 		  
 * @params cfg, configure structrue
 * @params key, key name
 *
 * @return key's value , if value isn't a number or 
 * 		   not exist, "0" will be returned.
 *
 * */
long nimo_config_integer(config_t* cfg, const char* key);

/**
 * Get const char string from spicified key
 *
 * @params cfg, configure structrue
 * @params key, key name
 *
 * @return : key's value , if value isn't exist, 
 * 			 NULL will be returned
 */
const char* nimo_config_string(config_t* cfg, const char* key);

/**
 * Release configure structure , failed impossible 
 *
 * @cfg configure structrue to be released
 */
void nimo_config_release(config_t* cfg);

size_t nimo_config_count(config_t*);

void nimo_config_print(config_t*);

#ifdef __cplusplus
}
#endif

#endif     

