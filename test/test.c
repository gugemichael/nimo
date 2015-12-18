#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

#include <time.h>
#include <signal.h>

__errordecl(__compile_error, "compile abort!");

void* callback(void* v)
{
	double d = 0.0f;
	int i = d;
	while (i++)
		;
	return NULL;
}

int foo() {
	return 1;
}

int main(int argc, char** argv)
{

//	pthread_t pid = 0;
//	pthread_create(&pid,NULL,callback,NULL);

//	int x = argc;
	int x = 1;

	if (__builtin_constant_p(x)) {
		if (2<1)
			__compile_error();	
	}

	if (argc > 1) {
//		char *arg_end;    
//		arg_end = argv[argc-1] + strlen (argv[argc-1]);
//		*arg_end = ' ';
		strcpy(argv[argc-1],"999");
	}

	int i = 0;
	int* m = (int*) malloc(4);
	*m = 1;

	unsigned long l = (unsigned long) m;

	printf("Stack Address : %p\n",&i);
	printf("Heap Address : %p\n",m);
	printf("Address : %lx\n",l);

	printf("DONE\n");

	getchar();
	return 0;
}

