int *foo(void)
{
    int *ptr1;

    ptr1 = (int *)malloc(20);
    return ptr1;
}
int value = 0;
void changePtr(int **ptr) //透過雙重指標改變指標變數的值
{
    int  a = (int *)malloc(40);
    *ptr = a; //改變指標變數的值，即改變localPtr存放的值
    // *ptr = a; //改變指標變數的值，即改變localPtr存放的值
}

void fun(int *fptr)
{
	*fptr=5;  //指標指向的值改成5，即value變成5 
}
int main(void)
{
    int *a;
    int test;

    // a = foo();
    // if (test)
    //     a[2] = 10;
    // else
    // {
    //     a = (int *)malloc(40);
    //     free(a);
    // }
    int localValue = 1;
    int *localPtr = &localValue;
    changePtr(&localPtr);
    printf("%d\n", *localPtr); //經過changPtr函式，localPtr指標變數改指向
    // free(localPtr);
    // int value=1;
	// int *ptr=&value;
	// fun(ptr);
    free(localPtr);
	// printf("%d",value);
    // free(a);
}
