#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int   *foo(void);
int main() {
        srand( 10 );
        int *p;
        int test;
        for (int i = 0;i < 10 ; i++){
                test= rand()% (10 - 0 + 1) + 0;
                p=foo();
                printf("%d\n", test);
                if(test >5)
                        goto EXIT;
                free(p);

        }
EXIT:

        return 0;
}