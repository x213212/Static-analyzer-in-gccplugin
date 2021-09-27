#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#define xstrdup(a) strdup(a)
///home/cc/gcc/ins/bin/gcc  -fplugin=/home/cc/gcc/myfile/Compiler/misra.so -I/home/cc/gcc/ins/lib/gcc/x86_64-pc-linux-gnu/7.3.0/plugin/include openssl_df_2.c  -O1  -flto  -fno-tree-dse  -fno-tree-fre -fno-tree-dce -fipa-pta   -fno-inline-functions-called-once   -o  openssl_df_2.o
// 子執行緒函數

int foo(int *z) __attribute__((noinline));
int *foo2(int z) __attribute__((noinline));

int foo(int *z)
{
	*z = malloc(10);
	
}
int main()
{
	int *p;
	int *p2;
	int test;
	foo(p);
	if (test)
	{
		foo(p);
		free(p);
	}
	free(p);

	return 0;
}
