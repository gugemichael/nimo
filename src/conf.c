/*
 * =====================================================================================
 *
 *       Filename:  confloader.c
 *
 *    Description:  Load User Configure File , And Split to KV
 *
 *        Version:  1.0
 *       Compiler:  gcc
 *
 *         Author:  Michael LiuXin
 *
 * =====================================================================================
 */
#include "conf.h" 

#define CONFIG_INIT_LEN (64)
#define CONFIG_MAX_LEN (1024*1024)

//
// clowf_config_load_from_file
//
config_t* clowf_config_load_from_file(const char* fpath)
{
	if (NULL == fpath)
		return NULL;

	FILE* conf = fopen(fpath,"r");
	// open failed
	if (NULL == conf)
		return NULL;

	size_t file_len = 1024, len = 0;
	char*  buf = (char*)calloc(1,file_len);
	// keep reading until EOF
	while(!feof(conf)) {
		int n = fread(buf+len,1,1024,conf);
		len += n;
		if (len >= file_len) {
			file_len <<= 1;
			char *old = buf;
			buf = realloc(buf,file_len);
			if (buf == NULL)
				free(old);
		}
	}

	config_t* back = clowf_config_load(buf);

	free(buf);
	fclose(conf);
	return back;
}

//
// clowf_config_load
//
config_t* clowf_config_load(const char* cfg)
{
	if (NULL == cfg)
		return NULL;

	int len = strlen(cfg);
	if (len < 3)
		return NULL;

	config_t* config = (config_t*)calloc(1,sizeof(config_t));
	if (NULL==config)
		goto check_exception;

	// alloc kv pairs
	config->kv_list = (kv_t*)calloc(CONFIG_INIT_LEN,sizeof(kv_t));
	if (NULL == config->kv_list)
		goto cleanup;

	config->count = CONFIG_INIT_LEN;

	const char* line = cfg;
	const char* cur = cfg;

	// iterator char string
	while((line-cfg)!=len+1) { 
		// still in same line
		if (*(line)!='\n' && (*line)!='\0') {
			line++;
			continue;
		} else {

			// first check the cacpacity
			if (config->used >= config->count) {
				// extends space 
				config->kv_list = (kv_t*)realloc(config->kv_list,sizeof(kv_t)*config->count*2);
				if (NULL == config->kv_list)
					goto cleanup;
				else {
					config->count *= 2;
				}
			}

			kv_t* kv_pair =	&config->kv_list[config->used];

			/**
			 * For Key 
			 */
			// skip blank sapce
			while(*cur==' ' || *cur=='\t')
				cur++;
			// ignore the comment (start with "#")
			if (*cur == '=' || *cur == '#' || *cur == '\n' || *cur == '\r')
				goto keepgo;

			const char* key = cur;
			while(*key!=' '&&*key!='=') {
				if (key == line)
					goto keepgo;
				key++;
			}

			// alloc len(key) + '\0' space
			char* tmp_key = (char*) malloc(key-cur+1);
			if (!tmp_key)
				goto keepgo;
			memcpy(tmp_key,cur,key-cur);

			while(*cur++!='=') {
				// no value
				if (cur>=line) {
					free(tmp_key);
					goto keepgo;
				}
			}

			/**
			 * For Value
			 */
			// skip blank space after ": "
			while(*cur==' '||*cur=='\t')
				cur++;
			// oops , bad string
			if (cur>=line) {
				free(tmp_key);
				goto keepgo;
			}
			const char* value = cur;
			while(value<=line) {
				if (*(value+1)==' ' || value+1==line)
					break;
				else
					value++;
			}
			char* tmp_value = (char*)calloc(1,value-cur+2);
			// don't copy "\n"
			memcpy(tmp_value,cur,value-cur+1);

			// kv ok
			config->used++;
			kv_pair->k = tmp_key;
			kv_pair->v = tmp_value;

keepgo:
			line++;
			cur = line;

		}
	}

	return config;

cleanup :
	free(config);
check_exception :
	return NULL;
}

//
// clowf_config_get_integer
//
long clowf_config_integer(config_t* cfg, const char* key)
{
	const char* v =	clowf_config_string(cfg,key);

#define CARRY (1024L)

	if (v!=NULL) {
		int last = strlen(v) - 1;
		long unit = 1;
		switch (v[last]) {
			case 't' :
			case 'T' :
				unit *= CARRY;
			case 'g' :
			case 'G' :
				unit *= CARRY;
			case 'm' :
			case 'M' :
				unit *= CARRY;
			case 'k' :
			case 'K' :
				unit *= CARRY;
				break;
			default :
				return atol(v);
		}
		char num[last+1];
		memcpy(num,v,last);
		return atol(num) * unit;
	}

#undef CARRY

	// TODO : ***return 0 is not good for bad number***
	return 0L;
}

//
// clowf_config_get_string 
//
const char* clowf_config_string(config_t* cfg, const char* key)
{
	if (NULL == cfg)
		return NULL;

	for (int i=0;i!=cfg->used;i++) {
		if (0 == strcmp(key,cfg->kv_list[i].k)) {
			// find it
			return cfg->kv_list[i].v;
		}   
	}   

	return NULL;
}

//
// clowf_config_release
//
void clowf_config_release(config_t* cfg)
{
	if (cfg!=NULL) {
		for (int i=0;i!=cfg->used;i++) {
			free(cfg->kv_list[i].k);
			free(cfg->kv_list[i].v);
		}
		free(cfg->kv_list);
		free(cfg);
	}
}

//
// clowf_config_print
//
void clowf_config_print(config_t* cfg)
{
	if (NULL==cfg)
		return;

	for (int i=0;i!=cfg->used;i++) {
		printf("[ %s = %s ]\n",cfg->kv_list[i].k,cfg->kv_list[i].v);
	}

}

