#include <stdlib.h>
void freeptr(int *p){
    free(p);
}

int* foo(void){
    int* ptr1;
	int *ptr2;
    ptr1=(int*) malloc(20);
    ptr1[1]=10;

    ptr2=(int*) malloc(40);
    ptr2[2]=8;
    return ptr2;
}

int main(void){
    int *a;
    a=foo();
    a[2]=10;
    foo2(a);
    freeptr(a);

    a=foo();
    free(a);
    a[6]=4;
}
