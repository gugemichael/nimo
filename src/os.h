#ifndef __NIMO__SYSCALL_H__
#define __NIMO__SYSCALL_H__


/* #define os_fdatasync(fd) __compile_error__ */

// gettid
#include <sys/syscall.h>

#ifdef __linux__
	#define os_gettid() syscall(SYS_gettid)
#elif (defined(__APPLE__) && defined(__MACH__))
	#define os_gettid() syscall(SYS_gettid)
#else
	#define os_gettid __compile_error__
#endif

// fsync
#ifdef __linux__
	#define os_fdatasync fdatasync
#elif (defined(__APPLE__) && defined(__MACH__))
	#define os_fdatasync fsync
#else
	#define os_fdatasync __compile_error__
#endif

// O_DIRECT and O_SYNC
#ifndef __linux__
	#ifndef O_DIRECT
		#define O_DIRECT 0
	#endif 
	#ifndef O_SYNC
		#define O_SYNC 0
	#endif 
#endif

#endif


