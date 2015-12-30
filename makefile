# ==============================================================
# Generic Makefile
# ==============================================================

LIB_NAME = nimo

# compile relative
CC = gcc 
PROFILER = #-lprofiler
UB_OPT = -pg -g -O2 ${PROFILER}
C_FLAGS = -std=gnu99 -Wall -Werror ${UB_OPT} 
INCLUDE = 
LIBS = 
LIBS_PATH = 
ENV = ENV
TARGET = lib${LIB_NAME}.a
TARGET_PATH = ./output/

# output relative
#TARGET_INCLUDE = $(TARGET_PATH)/include/
#TARGET_LIB = $(TARGET_PATH)/lib/
TARGET_INCLUDE = $(TARGET_PATH)/${LIB_NAME}
TARGET_LIB = $(TARGET_PATH)/${LIB_NAME}

# scaned files
SRC = ./src/
SOURCES = $(wildcard src/*.c)
HEADERS = $(wildcard src/*.h)
OBJFILES = $(SOURCES:%.c=%.o)

# ctags files
TAGS = ./tags
CSCP = ./cscope.*

# command macro definition
MKDIR = mkdir -p 
RM = rm -rf 
MV = mv 
COPY = cp 
AR = ar -r

####################################################
COMPILER = $(CC) $(INCLUDE) $(LIBS_PATH) $(LIBS)
####################################################

all: $(TARGET)
	ctags -R --fields=+iaS --extra=+q
	cscope -bkqR

$(TARGET): $(ENV) $(OBJFILES)
	$(AR) $(TARGET_PATH)/$(TARGET) ${SRC}/*.o
	${COPY} ${SRC}/*.h $(TARGET_INCLUDE)
	${RM} ${SRC}/*.o 

$(OBJFILES): %.o: %.c $(HEADERS)
	${COMPILER} $(C_FLAGS) -c -o $@ $<

$(ENV):
	$(MKDIR) $(TARGET_INCLUDE)
	$(MKDIR) $(TARGET_LIB)

clean:
	$(RM) $(TARGET_PATH) ${TAGS} ${CSCP}

check:
	cppcheck --enable=all --platform=unix64 . 

