void foo2(int *ptr)
{
    int *e = malloc(1);
    printf("%d\n", e);
    free(e);
    free(ptr);
}
