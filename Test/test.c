int* foo(void){
    int* ptr1;

    ptr1=(int*) malloc(20);
	ptr1[0]=1;
    return ptr1;
}

int main(void){
    int *a;
	int test;
    a=foo();
	//printf(stderr , "%d\n",a[]);
    if (test)
       a[2]=10;
    else
       a=(int*) malloc(40);

    free(a);
}
