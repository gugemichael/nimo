FLAGS = -pg --coverage -g
CC = gcc -std=gnu99 -Wall -Werror ${FLAGS} 
PROFILER = #-lprofiler
INCLUDE = ../output/
INCLUDE_LIB = ../output/
LIBS = -L${INCLUDE_LIB} -lnimo -lpthread ${PROFILER}
RM = rm -rf 

all : 
	${CC} test.c ${FLAGS} -I${INCLUDE} -L${INCLUDE_LIB} ${LIBS} -o test

clean :
	${RM} *.core core.* gmon.out logs *.gcno *.gcda
