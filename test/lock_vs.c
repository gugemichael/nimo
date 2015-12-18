#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <fcntl.h>

#include <sys/mman.h>
#include <unistd.h> 
#include <signal.h>
#include <sys/resource.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

typedef struct
{
	char key[128];
	char cluster[64];
	int role;
} ck_key;

#define MMAP_SIZE 512

inline void* fetch_mmap(char* file_path)
{
	// get the file-describer 
	int fd = open(file_path, O_RDWR | O_CREAT);
	ftruncate(fd, MMAP_SIZE);


	// use the fd for mapping
	void* mem =
		mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (mem == MAP_FAILED)
		printf("Error Mapping !");

	printf("Mapped Memory Adrress : %ld \n", (unsigned long) mem);
	close(fd);
	return mem;
}

#define LockType pthread_mutex_t
#define LOCK_INIT(t,attrib) pthread_mutex_init(t,attrib)
#define LOCK(t) pthread_mutex_lock(t)
#define UNLOCK(t) pthread_mutex_unlock(t)

//#define LockType pthread_spinlock_t
//#define LOCK_INIT(t,attrib) pthread_spin_init(t,PTHREAD_PROCESS_PRIVATE)
//#define LOCK(t) pthread_spin_lock(t)
//#define UNLOCK(t) pthread_spin_unlock(t)

//#define LockType pthread_mutex_t
//#define LOCK_INIT(t,attrib) pthread_mutex_init(t,attrib)
//#define LOCK(t) {} 
//#define UNLOCK(t) {} 

LockType lock;

uint64_t num = 0;

uint64_t MAX = 65535L*65535L*65535L;

void* thread_fun(void* c)
{
	int use_less = 1;	
	while(1) {
		LOCK(&lock);

//		num++;
//		if (counter.empty()) {
//			UNLOCK(&lock);
//			break;
//		}
//
//		int tmp = counter.front();
//		counter.pop_front();
	
		use_less++;

		UNLOCK(&lock);

		if (use_less == 100000000)	
			break;

	}

	return NULL;
}

struct fd_vector {
	uint32_t num;
};

#define THREADS 2

int main()
{
	// *******************************************
	int i = 0;
	signal(SIGUSR1, SIG_IGN);

	time_t t = time(NULL);

	LOCK_INIT(&lock,NULL);

	pthread_t tid[THREADS];
	for (;i!=THREADS;i++)
		pthread_create(&tid[i],NULL,thread_fun,NULL);

	i = 0;
	for (;i!=THREADS;i++)
		pthread_join(tid[i],NULL);
	
	// *******************************************

//	while(1) {
//		printf("%lu\n",num/100000UL);
//		sleep(1);
//	}

	printf("Use[%lus] Every This OK ...",time(NULL)-t);
	printf("\n%s\n", "Done !");

	return 0;
}

