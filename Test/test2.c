int *foo2(void){
    int *d;
    d=malloc(10);
    return d;

}
int   *foo(void)
{
    int *b;
    int test;
    if (test)
        b = foo2();
    else
        b = foo2();
    return b;
}