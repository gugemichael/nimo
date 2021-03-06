#include "log.h"

#define DEFAULT_FILE_FLAG (O_CREAT|O_RDWR|O_APPEND)
#define DEFAULT_FILE_MODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)
//#define DEFAULT_DIR_MODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH|S_IXUSR|S_IXGRP|S_IXOTH)
#define DEFAULT_DIR_MODE (S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH)

#define OUTPUT_STRING_MAX 4096
#define SPLIT_MIN_SIZE (64 * 1024 * 1024)

// page size 4k 
//#define LOG_BUFFER_PAGE (1024 * 4)
#define VFS_PAGE_SIZE (getpagesize())

// log folder
#define LOG_PATH "./logs/"

#define IS_NORMAL_LOG(log) (log==NORMAL_LOG)

#include "os.h"

struct __log_t* my_log = NULL;

const char* LOG_LEVEL[] = {
	"DEBUG","INFO","WARN","ERROR","UNKOWN"
};


static void 
__get_sign(time_t raw, char* buffer, size_t len) {
	struct tm* t;
	t = localtime(&raw);
	strftime(buffer,len,"%Y-%m-%d.%H",t);
}

/*
 * write data into file
 */
static int 
__write_file(log_t *log, int type, void* buffer, size_t size) {
	int written = write(IS_NORMAL_LOG(type) ? log->normal_file : log->error_file, buffer, size);
	if (-1 != written) {
		size_t *file_size = IS_NORMAL_LOG(type) ? &log->normal_file_size
						: &log->error_file_size;
		*file_size += written;
	}
	return written;
}


static int 
__reopen(log_t *log, int type, time_t raw) {
	int fd = (IS_NORMAL_LOG(type) ? log->normal_file : log->error_file);
	
	char buf[16] = {0};
	char file_rename[FILE_NAME_MAX] = {0};
	__get_sign(raw,buf,sizeof(buf));
	snprintf(file_rename,FILE_NAME_MAX,IS_NORMAL_LOG(type)?(LOG_PATH"%s.log.%s.%lu"):(LOG_PATH"%s.log.wf.%s.%lu"),log->file_name,buf,raw); 

	int newfd = open(file_rename,log->mask,DEFAULT_FILE_MODE);
	
	// if open failed , skip
	if (-1 == newfd) {
		fprintf(stderr,"open log[%s] len[%lu] failed , err:%s",file_rename,strlen(file_rename),strerror(errno));
		return -1;
	}
	
	if (IS_NORMAL_LOG(type)) {
		log->normal_file_size = 0;
		log->normal_file = newfd;
	} else {
		log->error_file_size = 0;
		log->error_file = newfd;
	}
	
	if (close(fd) == -1)
		fprintf(stderr,"close logfd[%d] failed , err:%s",fd,strerror(errno));
	
	return newfd;
}


/*
 * fast check that we need to rotate file
 */ 
static void 
__check_file_stat(log_t *log, time_t raw) {
	if (log->split_size && log->normal_file_size>log->split_size) {
		// too big , or time up
		pthread_mutex_lock(&log->split_lock);
		// double check is for locking optimize
		if (log->normal_file_size<log->split_size) { 
			pthread_mutex_unlock(&log->split_lock);
			return;
		}
		__reopen(log, NORMAL_LOG,raw);
		pthread_mutex_unlock(&log->split_lock);
		return;
	}
	if (log->split_size && log->error_file_size>log->split_size) {
		// too big , or time up
		pthread_mutex_lock(&log->split_lock);
		// double check is for locking optimize
		if (log->error_file_size<log->split_size) { 
			pthread_mutex_unlock(&log->split_lock);
			return;
		}
		__reopen(log, ERROR_LOG,raw);
		pthread_mutex_unlock(&log->split_lock);
		return;
	}
	
	// if split_size has effected , we ignore the split_time
	if (log->split_time) {
		if (raw-log->touch_time>log->split_time) {
			pthread_mutex_lock(&log->split_lock);
			// double check is for locking optimize
			if (raw-log->touch_time<log->split_time) { 
				pthread_mutex_unlock(&log->split_lock);
				return;
			}
			// for .log
			log->normal_file = __reopen(log, NORMAL_LOG,raw);
			// for .log.wf
			log->error_file = __reopen(log, ERROR_LOG,raw);
			
			// reset
			log->touch_time = raw;
			pthread_mutex_unlock(&log->split_lock);
		}
	}
}

static buffer_t* 
__alloc_threadlocal_buf(log_t *log, int thread_specific) {
	assert(log != NULL);
	log->dirty_page[thread_specific] = calloc(1,sizeof(buffer_t) + log->buffer_page_cap);
	if (log->dirty_page[thread_specific] == NULL)
		return NULL;

	log->dirty_page[thread_specific]->size = log->buffer_page_cap;
	log->dirty_page[thread_specific]->cursor = 0L;
	
	return log->dirty_page[thread_specific];
}


const char *nimo_log_last_error(log_t *log) {
	if (log != NULL)
		return log->last_error;
	return "";
}


log_t* nimo_log_init(const char* filepath) {
	if (my_log)
		return my_log;

	static const char* normal = LOG_PATH"%s.log";
	static const char* err = LOG_PATH"%s.log.wf";
	
	char file_name[2][FILE_NAME_MAX] = {"",""};
	my_log = (log_t*)calloc(1, sizeof(log_t));
	log_t* feedback = my_log;
	
	if (NULL == feedback) {
		fprintf(stderr,"malloc nimo log resource failed!\n");
		return NULL;
	}
	
	feedback->level = DEBUG;
	pthread_mutex_init(&feedback->split_lock,NULL);

	if (filepath) {
		feedback->mask = DEFAULT_FILE_FLAG; 
		struct stat buf = {0};
		// LOG_PATH is not exist
		if (0!=stat(LOG_PATH,&buf) && ENOENT == errno)
			if (-1==mkdir(LOG_PATH,DEFAULT_DIR_MODE) && errno!=EEXIST)
				goto exception;
		snprintf(feedback->file_name,FILE_NAME_MAX,"%s",filepath);
		snprintf(file_name[0],FILE_NAME_MAX,normal,filepath); 
		snprintf(file_name[1],FILE_NAME_MAX,err,filepath); 
		feedback->normal_file = open(file_name[0],feedback->mask,DEFAULT_FILE_MODE);
		if (-1 == feedback->normal_file) 
			goto exception;
		feedback->error_file = open(file_name[1],feedback->mask,DEFAULT_FILE_MODE);
		if (-1 == feedback->error_file) {
			close(feedback->normal_file);
			goto exception;
		}
	} else {
		feedback->normal_file = 1;
		feedback->error_file = 2;  // stdout
	}
	
	// OK
	return feedback;
	
exception : 
	fprintf(stderr, "initial nimo log failed : %s", strerror(errno));
	free(feedback);
	my_log = NULL;
	return NULL;
}

log_t* nimo_log_split_init(const char* filepath, unsigned long long ts, size_t ss) {
	if (my_log)
		return my_log;
	
	my_log= nimo_log_init(filepath);
	
	if (NULL == my_log)
		return NULL;
	
	struct stat filestat = {0};
	
	// get initial size 
	fstat(my_log->normal_file,&filestat);
	my_log->normal_file_size = filestat.st_size; 
	fstat(my_log->error_file,&filestat);
	my_log->error_file_size = filestat.st_size; 
	my_log->split_size = (0==ss) ? 0 : (ss<SPLIT_MIN_SIZE ? SPLIT_MIN_SIZE : (ss * 1024 * 1024));
	my_log->split_time = ts;
	my_log->touch_time = (unsigned long long)time(NULL);
	
	return my_log;
}

void nimo_log_destroy() {
	if (my_log) {
		// release lock resource
		pthread_mutex_destroy(&my_log->split_lock);
		// flush dirty content
		if (my_log->use_pagecache) {
			for (int i=MAX_THREADS;i!=0;i--)
				if (my_log->dirty_page[i])
					__write_file(my_log, NORMAL_LOG,my_log->dirty_page[i]->mem, my_log->dirty_page[i]->cursor);
		}
		close(my_log->normal_file);
		close(my_log->error_file);
		for (int i=0;i!=MAX_THREADS;i++) {
			if (my_log->dirty_page[i] != NULL) {
				free(my_log->dirty_page[i]);
				my_log->dirty_page[i] = NULL;
			}
		}

		free(my_log);

		my_log = NULL;
	}
}

void log_write(log_level level, const char* file, const char* func_name,unsigned int line, const char* __format, ...) {
	if (level < my_log->level || !my_log || !__format)
		return;
	
	// get ms
	struct timeval time_ms;
	gettimeofday(&time_ms,NULL);

	// optimiz ! we can using cached tm struct
	struct tm *p = my_log->timep;
	if (unlikely(time_ms.tv_sec > my_log->touch_time || !p || 0 == my_log->touch_time)) {
		time_t timep;
		time(&timep);
		p = my_log->timep = gmtime(&timep);
	}
	
	// stdout ignore
	if ((my_log->normal_file!=1) && (my_log->split_size!=0 || my_log->split_time!=0))
		__check_file_stat(my_log, time_ms.tv_sec);
	
	int fd = level <= INFO && level >= DEBUG ? my_log->normal_file : my_log->error_file;
	int type = (fd == my_log->normal_file) ? NORMAL_LOG : ERROR_LOG;
	
	char tmp[OUTPUT_STRING_MAX + 1] = {0};
	char *output = tmp;
	
	int output_len = 0 , valist_len = 0;
	pid_t thread_specific = os_gettid();
	
	const char* print_level = (level >= DEBUG && level <= ERROR) ? LOG_LEVEL[level] : LOG_LEVEL[UNKOWN];
	
	__builtin_va_list __local_argv; 
	__builtin_va_start(__local_argv, __format);
	
	output_len = snprintf(output,OUTPUT_STRING_MAX,"[%s][%d][%s:%u][%d/%d/%d:%d:%d,%ld] : ",print_level,thread_specific,
					file,line,/*func_name,*/(1+p->tm_mon),p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec,(long)time_ms.tv_usec);
	// keep appending
	valist_len = vsnprintf(output + output_len,OUTPUT_STRING_MAX - output_len, __format, __local_argv);
	// append a "\n" at the end
	output[output_len + valist_len] = '\n';
	size_t len = output_len + valist_len + 1;
	
	/* we would use buffer on NORMAL file only */
	if (IS_NORMAL_LOG(type) && my_log->use_pagecache) {
		// acquire thread's log buffer
		buffer_t *flush_page = my_log->dirty_page[thread_specific];
		if (unlikely(flush_page==NULL)) {
			flush_page = __alloc_threadlocal_buf(my_log, thread_specific);
			if (flush_page == NULL)
				return;
		}
		size_t cursor = flush_page->cursor;
		if ((cursor + len ) > flush_page->size) {
			/*
			 * No space to save buffer , need flushing :
			 * 1. make new buffer page (other thread can use)
			 * 2. reset my_log buffer page and cursor
			 * 3. release lock
			 * 4. do slowly write
			 */
			if (-1 != __write_file(my_log, type,flush_page->mem,cursor)) {
				// flush buffer ok and reset cursor without clear memory
				memcpy(flush_page->mem,output,len);
				flush_page->cursor = len;
			} else
				fprintf(stderr,"nimo flush log write failed , log message ignore : %s\n",my_log->last_error = strerror(errno)); // flush failed
		} else if (cursor!=0 && time_ms.tv_sec>my_log->touch_time) {
			__write_file(my_log, type,flush_page->mem,cursor);
		} else {
			/**
			 * only copy string in memory
			 */
			memcpy(flush_page->mem+cursor,output,len);
			flush_page->cursor += len;
		}
	} else
		__write_file(my_log, type,output,len);

	// update log time
	my_log->touch_time = time_ms.tv_sec;
}

/*
 * use buffered loggig
 */
int nimo_log_buffer(log_t* log, int size) {
	if (!log)
		return -1;

#ifdef  __linux__
	log->buffer_page_cap = size;
	log->use_pagecache = 1;
#endif     
	
	return 0;
}

void nimo_log_flush(log_t* log) {
	// only sync the file content and size metadata
	if (log) {
		os_fdatasync(log->error_file);
		os_fdatasync(log->normal_file);
	}
}

void nimo_log_level(log_t* log, log_level level) {
	if (log)
		log->level = level;
}
