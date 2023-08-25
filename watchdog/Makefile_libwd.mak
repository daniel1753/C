# make clean/all CONFIG=Debug/Release ARCH=linux64/raspi ECHO=0/1

# the parrent dir of "arch" dir
ROOT_DIR = ../..

#PROJ_SRCS = $(wildcard *.c)
PROJ_SRCS = wdlib.c 247.c

# list of directories containing header files
#PROJ_INC_DIR = $(ROOT_DIR)/include
PROJ_INC_DIR = $(ROOT_DIR)/ds/include

# the executable file
PROJ_OUT = libwd.so

# list of ADDITIONAL compiler flags (prepended with "-" sign)
#PROJ_CFLAGS = -Wunused-but-set-variable
PROJ_CFLAGS =
PROJ_CPPFLAGS =

#PROJ_MACROS = _BSD_SOURCE
PROJ_MACROS =

# additional object files (created by other projects)
PROJ_ADD_OBJS = scheduler.o uid.o pq_heap.o task.o heap.o vector.o

# additional libraries
PROJ_LIBS =

# linker flags
PROJ_LDFLAGS = 

# list of projects (makefiles) this project depends on
PROJ_DEPENDS = $(ROOT_DIR)/ds/src/scheduler/Makefile\
$(ROOT_DIR)/ds/src/uid/Makefile\
$(ROOT_DIR)/ds/src/task/Makefile\
$(ROOT_DIR)/ds/src/pq_heap/Makefile\
$(ROOT_DIR)/ds/src/vector/Makefile\
$(ROOT_DIR)/ds/src/heap/Makefile


include $(ROOT_DIR)/make/Project.mak 
