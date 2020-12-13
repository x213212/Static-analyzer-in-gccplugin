#include "stdlib.h"
#include "stdio.h"

int *foo(int z){

	int *a=malloc(z);
	for(int i = 0;i < 3;++i) {
    printf("qwdwqd%d\n", i);
	 }

	return a;
}
void  foo2(int *z){
	free(z);
}
void* child(void* data) {
  char *str = (char*) data; // 取得輸入資料
  int *k = malloc (10);
  for(int i = 0;i < 3;++i) {
    // printf("%s\n", str); // 每秒輸出文字
    sleep(1);
  }
  pthread_exit(NULL); // 離開子執行緒
  free(k);
}

// void *boo(int *b){ 	free(b); 	printf("asdda\n"); } 
// void *boo2(int *b){ 	free(b); 	printf("asdda\n"); }