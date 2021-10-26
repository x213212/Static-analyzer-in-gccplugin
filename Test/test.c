#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#define xstrdup(a) strdup(a)
/// home/cc/gcc/ins/bin/gcc  -fplugin=/home/cc/gcc/myfile/Compiler/misra.so -I/home/cc/gcc/ins/lib/gcc/x86_64-pc-linux-gnu/7.3.0/plugin/include openssl_df_2.c  -O1  -flto  -fno-tree-dse  -fno-tree-fre -fno-tree-dce -fipa-pta   -fno-inline-functions-called-once   -o  openssl_df_2.o
// 子執行緒函數

int foo(int *z) __attribute__((noinline));
int *foo2(int z) __attribute__((noinline));

struct st
{
	int flag;
	int *f;
};
int *foo3(struct st *p)
{
	int *q;
	if (p->flag)
		q = malloc(1);
	else
		q = p->f;
	return q;
}

int foo(int *z)
{
	free(z);
}
int main()
{
	// int *q;
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
	int *q;
	int *p6 = malloc(100);
	q = p6;
	p6 = realloc(p6, 100);
	q[0] = "0";
	q[1] = "0";
	free(p6);
	// foo(p);
	// q[0] = "0";
	// struct st p;
	// int *q;
	// p.f = malloc(1);
	// q = foo3(&p);
	// // use q
	// free(q);
	// free(p.f); // double-free
	struct st *p9 = malloc(0);
	p9[3].f = malloc(1);
	free(p9);
	struct st *p = malloc(10);
	p[3].f = malloc(1);
	for (int i = 0; i < 10; i++)
		p[i].f = malloc(1);
	p[2].f = malloc(1);
	foo(p);
	// use p[i].f

	if (test)
		return 200;

	for (int i = 0; i < 10; i++)
	{
		free(p[i].f);
		free(p9[i].f);
	}

	int *p100;
	p100 = malloc(1);
	if (test)
	{
		p100 = malloc(2);
		free(p100);
	}
	free(p100);
// test2:
// return 200;

// return 0;
// return 0;

// int *p = malloc(100);
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
	// 	if(test3){
	// 		foo(p);
	// 	if(test4)
	// 	foo(p);
	// 	else
	// 	foo(p);
	// 	}
	// else
	// 	foo(p);}
	// else
	// 	foo(p);

	return 0;
}
