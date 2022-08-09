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
                p[0]=1;
                p[1]=2;
                p[2]=3;
                p[3]=4;
                p[4]=5;
                p[5]=6;
                p[6]=7;
                p[7]=8;
                p[8]=9;
                p[9]=10;
                printf("%d\n", p);
                if(test >5)
                        goto EXIT;
                free(p);

        }
EXIT:
free(p);

        return 0;
}