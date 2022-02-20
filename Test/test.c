int *foo(void)
{
    int *ptr1;

    ptr1 = (int *)malloc(20);
    return ptr1;
}
int value = 0;
void changePtr(int **ptr) //透過雙重指標改變指標變數的值
{
    int test;
    if (test)
    {
        int a = (int *)malloc(40);
        *ptr = a; //改變指標變數的值，即改變localPtr存放的值
    }
    else
    {
        int a2 = (int *)malloc(40);
        printf("%d\n", a2); //經過changPtr函式，localPtr指標變數改指向
        *ptr = a2;
    }
    // *ptr = a; //改變指標變數的值，即改變localPtr存放的值
}

void fun(int *fptr)
{
    *fptr = (int *)malloc(40); //指標指向的值改成5，即value變成5
    fptr[0] = 0;
}
void changePtr2(int ***ptr) //透過雙重指標改變指標變數的值
{
    int test;
    if (test)
    {
        int a = (int *)malloc(40);
        *ptr = a; //改變指標變數的值，即改變localPtr存放的值
    }
    else
    {
        int a2 = (int *)malloc(40);
        printf("%d\n", a2); //經過changPtr函式，localPtr指標變數改指向
        *ptr = a2;
    }
    // *ptr = a; //改變指標變數的值，即改變localPtr存放的值
}
int main(void)
{
    // int *a;

    // int test;

    // a = foo();
    // if (test)
    //     a[2] = 10;
    // else
    // {
    //     a = (int *)malloc(40);
    //     free(a);
    // }
    
    // 一級pointer
    int value=1;
    int *ptr0=&value;
    fun(ptr0);
    printf("%d",*ptr0);
    // 二級pointer
    int localValue = 1;
    int *localPtr = &localValue;
    changePtr(&localPtr);
    printf("%d\n", *localPtr); //經過changPtr函式，localPtr指標變數改指向
    free(localPtr);
    // 三級pointer
    int num = 100;
    int *ptr = &num;
    int **ptr2 = &ptr;
    int ***ptr3 = &ptr2;
    changePtr2(ptr3);
    // printf("透過ptr3取得到num值 \t%d\n", ***ptr3);
    // printf("ptr3指到ptr位址 \t%p\n", **ptr3);
    // printf("ptr3指到ptr的記憶體位置 \t%p\n", *ptr3);
    // printf("ptr3指到ptr2的記憶體位置 \t%p\n", ptr3);
    // printf("ptr3記憶體位址 \t%p\n\n\n", &ptr3);
    //  printf("ptr3記憶體位址 \t%p\n\n\n", ptr3);
    // free(a);
}
