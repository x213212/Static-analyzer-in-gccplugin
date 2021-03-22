#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#define xstrdup(a) strdup(a)
///home/cc/gcc/ins/bin/gcc  -fplugin=/home/cc/gcc/myfile/Compiler/misra.so -I/home/cc/gcc/ins/lib/gcc/x86_64-pc-linux-gnu/7.3.0/plugin/include openssl_df_2.c  -O1  -flto  -fno-tree-dse  -fno-tree-fre -fno-tree-dce -fipa-pta   -fno-inline-functions-called-once   -o  openssl_df_2.o
// 子執行緒函數
pthread_mutex_t mLock;
int *test;
int *foo(int z);
int *foo2(int z);
void foo3(int *z);
void test22(int *k);
void test33(int *k);
void test44(int *k);
// int *foo(int z);
int *foo4(int z);
// int *foo2(int z);
// void foo3(int *z);

void foo3(int *z)
{
	// int *p2;
	// p2=foo2(2);
	// p2[0]=10;
	///error
	// p2=10;
	//printf("%d",p2);
	*z = malloc(1);
}
int *foo2(int z)
{

	int *a2 = malloc(z);
	int *p3 = malloc(z);
	int *p4 = malloc(z);
	int tmp;
	a2[0] = 10;
	//  free(a2);
	if (tmp > 10)
	{
		pthread_mutex_unlock(&mLock);
		p3[0] = 10;
		return p3;
	}
	else
	{
		p4[0] = 10;
		return p4;
	}

	//char tmp ;
	// return (char)tmp;
	return a2;
}
int *foo4(int z)
{
	int *b = malloc(1);
	int *p2 = malloc(1);

	// b=2;
	//  p2=foo2(2);
	b[0] = 2;
	p2[0] = 1;
	p2 = &b;

	free(p2);
	//int *a=malloc(1);
	// for(int i = 0;i < 3;++i) {
	// printf("qwdwqd%d\n", i);
	//  }
	//   free(a);
	return p2;
}
int *foo(int z)
{
	// int *b = malloc(1);
	int *p2 = malloc(1);
	p2[0] = 1;
	// free(p2);
	// b=2;
	//  p2=foo2(2);
	// b[0] = 2;
	// p2[0] = 1;
	// p2 = &b;

	// free(p2);
	//int *a=malloc(1);
	// for(int i = 0;i < 3;++i) {
	// printf("qwdwqd%d\n", i);
	//  }
	//   free(a);

	return foo2(z);
}
void *child(void *data)
{
	pthread_mutex_t mLock2;
		pthread_mutex_lock(&mLock2);
			pthread_mutex_lock(&mLock2);
	//a1
	//   pthread_mutex_lock(&mLock);
	//   //   char *str = (char*) data; // 取得輸入資料
	//   // int *p;e
	//   // p=foo(2);
	//   int *b = malloc(2);
	//   int *p2 = malloc(2);
	//   // p2 = &b;
	// //   MEM[(int *)&b] = 1;
	// //  <mem_ref 0x7fbe6f4e27d0
	//   // b[0] = 2;
	//   p2[0] = 1;
	//   p2[1] = 1;

	//   free(p2);
	//   // free(test);
	//   // 	str=malloc(5);
	//   pthread_mutex_unlock(&mLock);
	//   pthread_exit(NULL); // 離開子執行緒
	//a2
	pthread_mutex_lock(&mLock);
	pthread_mutex_lock(&mLock);
	int **ppData = malloc(10);
	int *pData = malloc(20);
	int *a = foo(1);
	int data2 = 0;

	ppData = &pData;
	pData = &data2;
	*ppData[0] = 10;
	*ppData[1] = 10;
	*ppData[2] = 10;
	**ppData = 12;
	free(pData);
	// free(ppData);
	pthread_mutex_unlock(&mLock);
		free(ppData);
	//  pthread_exit(NULL); // 離開子執行緒

	//a3
	// pthread_mutex_lock(&mLock);
	// int a = 10;
	// int *ptr1 = &a;
	// int **ptr2 = &ptr1;
	// int ***ptr3 = &ptr2;
	// pthread_mutex_unlock(&mLock);
	// pthread_exit(NULL); // 離開子執行緒
}
int *foo(int z);
void boo(int *b)
{
	free(b);
	printf("asdda\n");
}
void test44(int *k)
{
	free(k);
}
void test33(int *k)
{
	test44(k);
}
void test22(int *k)
{
	test22(k);
	free(k);
}
int main()
{
	int *p;
	int *p2;
	int *p3;
	// test= malloc (sizeof (int ) * 10);
	// foo3(p);
	p3 = foo(2);
	p3[0] = 1;
	free(p3);
	p = foo2(2);
	p[0] = 2;
	p2 = foo2(2);
	p2[0] = 4;
	// test22(p);
	// test22(p);
	// test33(p2);
	free(p);
	free(p2);
	char buff[50];
	int *q = malloc(5);
	q[0] = 10;
	test22(q);
	// child((int)q);
	int n;
	pthread_t t; // 宣告 pthread 變數
	pthread_mutex_destroy(&mLock);
	pthread_create(&t, NULL, child, buff); // 建立子執行緒

	// 主執行緒工作
	//   for(int i = 0;i < 3;++i) {
	//     printf("Master\n"); // 每秒輸出文字
	//     sleep(1);
	//   }

	pthread_join(t, NULL); // 等待子執行緒執行完成
	pthread_mutex_destroy(&mLock);
	// scanf("%d",n);
	// p=foo(2);
	// if(n)
	// 	free(p);
	// else
	// 	boo(p);
	// q=p;
	// free(q);
	// printf("%d",q);
	return 0;
}