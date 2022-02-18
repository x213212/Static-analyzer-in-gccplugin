int* foo(void){
    int* ptr1;

    ptr1=(int*) malloc(20);
    return ptr1;
}

int main(void){
    int *a;
    int test;

    a=foo();
    if (test)
       a=a+2;
    else {
       a=(int*) malloc(40);
       free(a);
    }

    free(a);
}
