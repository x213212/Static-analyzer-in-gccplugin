#include <stdio.h>
#include <stdlib.h>
struct person_t
{
    int *a;
    unsigned age;
};
enum VALID
{
    FALSE,
    TRUE
};
typedef struct
{
    int valid;
    int *data;
    size_t size;
} MyObject;

// int *foo(void)
// {
//     int *ptr1;

//     ptr1 = (int *)malloc(20);
//     return ptr1;
// }
int value = 0;
int changePtr(int **ptr) //透過雙重指標改變指標變數的值
{
    int test;
    int *a = (int *)malloc(40);
    // free(a);
    if (test)
    {

        *ptr = a;              //改變指標變數的值，即改變localPtr存放的值
        printf("2%d\n", a);    //經過changPtr函式，localPtr指標變數改指向
        printf("2%d\n", *ptr); //經過changPtr函式，localPtr指標變數改指向

        //     if(test){
        //          free(a);
        //          exit(a);//branch possiable have return or exit
        //   }
        //     else
        //     return 20;//fucntion exit
        free(a); // malloc def-leak warring222
    }
    else
    {
        // free(a);//malloc def-leak warring
        // a = (int *)malloc(40);
        ptr = (int *)malloc(40);
        printf("%d\n", a); //經過changPtr函式，localPtr指標變數改指向
        free(a);
    }
    free(a);
    //  free(ptr);//malloc def-leak warring222
    return 0;
    // free(a);
    // *ptr = a; //改變指標變數的值，即改變localPtr存放的值
}

// void fun(int *fptr)
// {
//     *fptr = (int *)malloc(40); //指標指向的值改成5，即value變成5
//     fptr[0] = 0;
// }
// void changePtr2(int ***ptr) //透過雙重指標改變指標變數的值
// {
//     int test;
//     if (test)
//     {
//         int a = (int *)malloc(40);
//         *ptr = a; //改變指標變數的值，即改變localPtr存放的值
//     }
//     else
//     {
//         int a2 = (int *)malloc(40);
//         printf("%d\n", a2); //經過changPtr函式，localPtr指標變數改指向
//         *ptr = a2;
//     }
//     // *ptr = a; //改變指標變數的值，即改變localPtr存放的值
// }
// int main(void)
// {
//     // int *a;

//     // int test;

//     // a = foo();
//     // if (test)
//     //     a[2] = 10;
//     // else
//     // {
//     //     a = (int *)malloc(40);
//     //     free(a);
//     // }

//     // 一級pointer
//     // int value=1;
//     // int *ptr0=&value;
//     // fun(ptr0);
//     // printf("%d",*ptr0);
//     // 二級pointer

//     // int localValue = 1;
//     // int *localPtr = &localValue;
//     // changePtr(&localPtr);
//     // struct person_t p = {(int *)malloc(40), 37};
//     // printf("%d\n", p.age); //經過changPtr函式，localPtr指標變數改指向
//     // free(p.a);
//     // int *tmp = NULL;
//     // MyObject *my1 = malloc(sizeof(MyObject));

//     // my1->valid = TRUE;
//     // my1->data = tmp;
//     // my1->size = sizeof tmp;
//     // printf("ptr3指到ptr位址 \t%p\n", my1->data);
//     // free(my1);

//     // pr。intf("%d\n", *localPtr); //經過changPtr函式，localPtr指標變數改指向
//     // free(localPtr);
//     // // 三級pointer
//     // int num = 100;
//     // int *ptr = &num;
//     // int **ptr2 = &ptr;
//     // int ***ptr3 = &ptr2;
//     // changePtr2(ptr3);
//     // printf("透過ptr3取得到num值 \t%d\n", ***ptr3);
//     // printf("ptr3指到ptr位址 \t%p\n", **ptr3);
//     // printf("ptr3指到ptr的記憶體位置 \t%p\n", *ptr3);
//     // printf("ptr3指到ptr2的記憶體位置 \t%p\n", ptr3);
//     // printf("ptr3記憶體位址 \t%p\n\n\n", &ptr3);
//     //  printf("ptr3記憶體位址 \t%p\n\n\n", ptr3);
//     // free(a);

//     // int *arr3 = (int *)realloc(NULL, sizeof(int) * 5);
//     // for (int i = 0; i < 5; i++)
//     // {
//     //     printf("arr3 %p\n", (arr3 + i));
//     //     printf("arr3[%d] = %d\n", i, *(arr3 + i));
//     // }
//     // // free(arr3);
//     // realloc(arr3, 0);
//     // realloc(arr3, 0);

//     int **arr = calloc(2, sizeof(int*));
//     arr[0] = calloc(3, sizeof(int));
//     arr[1] = calloc(5, sizeof(int));

//     for(int j = 0; j < 3; j++) {
//         printf("%d ", arr[0][j]);
//     }
//     putchar('\n');

//     for(int j = 0; j < 5; j++) {
//         printf("%d ", arr[1][j]);
//     }
//     putchar('\n');

//     for(int i = 0; i < 2; i++) {
//         free(arr[i]);
//     }
//     free(arr);
// }

int *foo(void)
{
    int *ptr1;

    ptr1 = (int *)malloc(20);
    return ptr1;
}

void foo2(int* ptr){
   int *e=malloc(1);
   printf("%d\n",e);
   free(e);
   free(ptr);

}

int main(void){
    int *a, *b, *c;
    a =foo();
    b = a;
    foo2(b);

    b = foo();
    c = b;
    foo2(c);
    printf("%d\n",&c);
    return 0;
}

 