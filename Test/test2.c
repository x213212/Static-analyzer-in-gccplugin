int   *foo(void)
{
    int *b;
    int test;
    if (test)
        b = malloc(1);
    else
        b = malloc(2);
    return b;
}