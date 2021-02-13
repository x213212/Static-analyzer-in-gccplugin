#include "stdlib.h"
#include "stdio.h"
#include <pthread.h>
#include <unistd.h>
#define xstrdup(a) strdup(a)
///home/cc/gcc/ins/bin/gcc  -fplugin=/home/cc/gcc/myfile/Compiler/misra.so -I/home/cc/gcc/ins/lib/gcc/x86_64-pc-linux-gnu/7.3.0/plugin/include openssl_df_2.c  -O1  -flto  -fno-tree-dse  -fno-tree-fre -fno-tree-dce -fipa-pta   -fno-inline-functions-called-once   -o  openssl_df_2.o 
// 子執行緒函數
pthread_mutex_t mLock;

int *foo(int z)  __attribute__((noinline));
int *foo2(int z)  __attribute__((noinline));
void* child(void* data)   __attribute__((noinline));
int *foo2(int z) {

	  int *a2=malloc(z);
    int *p3=malloc(z);
    int *p4=malloc(z);
    // int a = 10;
int **ppData = NULL;
int *pData = NULL;
int data = 0;

ppData = &pData;
pData = &data;
**ppData = 10;
int a =10;
    int *ptr = &a;

    int **ptr2 = &ptr; 
    **ptr2=20;
    int tmp ;
    if(tmp   > 10){
      p3[0] = 10;
      a2[0] = 9;
      *(int*)(a2 + sizeof(int) * 0) = 1;
      
      return p3;
    }else{
      free(a2);
      p4[0] = 12;
      return p4;
    }
    a2[0] = 10;
  
  free(a2);
  //char tmp ;
  // return (char)tmp;
	return a2;
}
int *foo(int z) {
    int *p2;
    p2=foo2(2);
    // p2[0]=10;
    ///error
    // p2=10;
   //printf("%d",p2);
	//int *a=malloc(1);
	// for(int i = 0;i < 3;++i) {
    // printf("qwdwqd%d\n", i);
	//  }
  // free(p2);
	return p2;
}
void* child(void* data) {
  pthread_mutex_lock(&mLock);
//   char *str = (char*) data; // 取得輸入資料
    int *p22;
    int *b =malloc(454);
    // int test;
    p22=foo(2);
    // printf("%d",p22);
    // b=10;
    
    p22=&b;
    free(p22);
// 	str=malloc(5);
  pthread_mutex_unlock(&mLock);
  pthread_exit(NULL); // 離開子執行緒
}
int * foo(int z);
void boo(int *b){
	free(b);
	printf("asdda\n");
}
int main(){
	int *p;
    // p=foo(2);
    char buff[50];
	// int *q=malloc(5);
	// int n;
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
	return 0 ;
}

