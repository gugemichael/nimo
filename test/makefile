FLAGS = -pg --coverage -g
CC = gcc -std=gnu99 -Wall -Werror ${FLAGS} 
PROFILER = #-lprofiler
INCLUDE = ../output/
INCLUDE_LIB = ../output/
LIBS = -L${INCLUDE_LIB} -lclowf -lrt -lpthread ${PROFILER}
RM = rm -rf 

all : 
	${CC} ${FLAGS} -I${INCLUDE} -L${INCLUDE_LIB} ${LIBS} test.c -o test

clean :
	${RM} *.core gmon.out logs
