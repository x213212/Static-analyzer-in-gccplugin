#include "stdlib.h"
#include "stdio.h"
#include "test2.h"
#include <pthread.h>
#include <unistd.h>
#define xstrdup(a) strdup(a)
///home/cc/gcc/ins/bin/gcc  -fplugin=/home/cc/gcc/myfile/Compiler/misra.so -I/home/cc/gcc/ins/lib/gcc/x86_64-pc-linux-gnu/7.3.0/plugin/include openssl_df_2.c  -O1  -flto  -fno-tree-dse  -fno-tree-fre -fno-tree-dce -fipa-pta   -fno-inline-functions-called-once   -o  openssl_df_2.o 

int * foo(int z);
void  foo2(int *z);
void* child(void* data);
// void *boo(int *b); 
// void *boo2(int *b);

// // 子執行緒函數
// void* child(void* data) {
//   char *str = (char*) data; // 取得輸入資料
//   int *k = malloc (10);
//   for(int i = 0;i < 3;++i) {
//     printf("%s\n", str); // 每秒輸出文字
//     sleep(1);
//   }
//   pthread_exit(NULL); // 離開子執行緒
// }



int main(){ 	
    pthread_t t; 
    //  pthread_create(&t, NULL, child, "Child"); // 建立子執行緒
    int *p; 	
    int *q; 	
    int *c ;
    

    q=foo(12);
    p=foo(10);
    // p=malloc(sizeof (int)*7);
    // c=foo(10);;
    //  q=&p;
    // p[10]=1;
    p=&q;
    //  p[0]=1;
    //  q[0]=2;
    // free(p);
    free(q);
    free(q);
    // free(c);
//   pthread_join(t, NULL); // 等待子執行緒執行完成
    // boo(c); 	
    // boo(q); 
	return 0 ;
} 

