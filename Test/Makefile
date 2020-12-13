
CC=gcc
INCLUDE=-I/home/cc/gcc/ins/lib/gcc/x86_64-pc-linux-gnu/7.3.0/plugin/include

FILE_NAME=$(wildcard *.c)
TEST=../Test/test2.c 

NO_OF_FILES := $(words $(FILE_NAME))

LTO=-flto
WHOLE=-fwhole-program
FLAGS=-O1    -fno-tree-dse  -fno-tree-fre -fno-tree-dce   -fno-tree-pta -fdump-tree-all -fno-dce -fno-dse -fno-tree-dce    -fno-tree-builtin-call-dce -fipa-pta -pthread -fno-strict-aliasing
PLUGIN=-fplugin=../Compiler/misra.so
all: test.o


test.o:  
	$(CC) $(PLUGIN) $(FLAGS) $(LTO)  $(FILE_NAME) -o $@
clean:
	-rm -rf ./test ./test.dSYM *.o


