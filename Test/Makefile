
CC=gcc-7
INCLUDE=-I/home/cc/gcc/ins/lib/gcc/x86_64-pc-linux-gnu/7.3.0/plugin/include

FILE_NAME=$(wildcard *.c)
TEST=../Test/test2.c 

NO_OF_FILES := $(words $(FILE_NAME))

LTO=-flto
WHOLE=-fwhole-program
FLAGS=-O1 -fno-tree-dse  -fno-tree-fre -fno-tree-dce  -fipa-pta  -pthread  --param max-ssa-name-query-depth=10  
#FLAGS=-O1   -fno-tree-dse  -fno-tree-fre -fno-tree-dce -fipa-pta  -pthread   
#  -I/root/x21321219/mysql8.0/mysql-8.0.23/extra/icu/source/i18n/../common -I/root/x21321219/mysql8.0/mysql-8.0.23/extra/icu/source/i18n 
#FLAGS=-O1    -fno-tree-dse  -fno-tree-fre -fno-tree-dce -fipa-pta  -fdump-tree-all -pthread -fno-diagnostics-color 
#FLAGS=-O1    -fno-tree-dse  -fno-tree-fre -fno-tree-dce   -nostartfiles -g  -fdump-tree-all -fno-dce -fno-dse -fno-tree-dce    -fno-tree-builtin-call-dce -fipa-pta -pthread -fno-strict-aliasing
PLUGIN=-fplugin=../Compiler/misra.so 
all: test.o



test.o:  
	#   $(CC) $(PLUGIN) $(FLAGS) $(LTO)  $(FILE_NAME) -o $@ 
	$(CC) $(PLUGIN) $(FLAGS) $(LTO)   $(FILE_NAME) -o $@ 2> check.log
	@sh ./test.sh $(FILE_NAME)
	
graphviz:  
	@sh ./test.sh $(FILE_NAME)
clean:
	-rm -rf ./test ./test.dSYM *.o


