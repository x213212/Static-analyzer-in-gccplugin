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
	int *p3;
	int *p4;
	int *p5;
	int test;
	int test2;
	int test3;
	int test4;
	int test5;
	// foo(p);
	// p[1]=10;
	int a = 10;
	int b = 22;
	int c = 22;
	int d = 22;
label2:
		foo(p5);
label3:
		foo(p5);
	switch (test5 / 10)
	{
	case 5:
		foo(p5);
		break;
	case 10:
		foo(p5);
		break;
	default:
		foo(p5);
		break;
	}
	if (test)
	{
		// if (  (test >test2) == (test3 == test4))
		// {
		// foo(p);
		// free(p);
		if (test > test2)
		{
			foo(p);
			if (test < test2)
			{
				foo(p);
			}
			else
				foo(p);
		}
		else
		{
			foo(p);
			if (test < test2)
			{
				foo(p3);
			}
			else
				foo(p5);
					goto label3;
		}
		if (test3)
		{
			foo(p);
			goto label2;
		}
	}
	else if (test == test2)
		foo(p);
	else
	{
		if (test > test2)
		{
			foo(p4);
		}
	}
	// else if (test > test2)
	// 	foo(p);
	// else if (test < test2)
	// {
	// 	// label:
	// 	// 	foo(p);
	// 	foo(p);
	// }
	// // 	if(test3){
	// // 		foo(p);
	// // 	if(test4)
	// // 	foo(p);
	// // 	else
	// // 	foo(p);
	// // 	}
	// // else
	// // 	foo(p);}
	// else
	// 	foo(p);

	return 0;
}
