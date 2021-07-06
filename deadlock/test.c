#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "./test2.h"

pthread_mutex_t lock1, lock2,lock3,lock4;
void *resource1(){

    pthread_mutex_lock(&lock1);
   // printf("Job started in resource1..\n");
    pthread_mutex_lock(&lock2);
    pthread_mutex_unlock(&lock2);
    pthread_mutex_unlock(&lock1);
   // printf("Job started lock1 lock2 free..\n");
    //pthread_mutex_lock(&lock3);
    //pthread_mutex_lock(&lock4);
    //pthread_mutex_unlock(&lock4);
   // pthread_mutex_unlock(&lock3);

    printf("Job finished in resource1..\n");
    pthread_exit(NULL);

}

void *resource2(){

    pthread_mutex_lock(&lock2);
    printf("Job started in resource2..\n");
    pthread_mutex_lock(&lock3);

    pthread_mutex_unlock(&lock3);
    pthread_mutex_unlock(&lock2);

    printf("Job finished in resource2..\n");
    pthread_exit(NULL);
}
void *resource3(){

    pthread_mutex_lock(&lock4);
    printf("Job started in resource3..\n");
    pthread_mutex_lock(&lock1);
    pthread_mutex_unlock(&lock1);
    pthread_mutex_unlock(&lock4);

    printf("Job finished in resource3..\n");
    pthread_exit(NULL);
}

int main() {

    pthread_mutex_init(&lock1,NULL);
    pthread_mutex_init(&lock2,NULL);
    pthread_mutex_init(&lock3,NULL);
    pthread_mutex_init(&lock4,NULL);
    pthread_t t1,t2,t3;
 //   pthread_create(&t2,NULL,resource2,NULL);
    pthread_create(&t1,NULL,resource1,NULL);
    pthread_create(&t2,NULL,resource2,NULL);
    pthread_create(&t3,NULL,resource3,NULL);
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    pthread_join(t3,NULL);
    test();
    for(int i = 0; i < 10 ; i ++){
	    printf("%d\n" , i);
    }
    return 0;

}
