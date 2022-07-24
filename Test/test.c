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
                p[1]=1;
                p[2]=1;
                p[3]=1;
                p[4]=1;
                p[5]=1;
                p[6]=1;
                p[7]=1;
                p[8]=1;
                p[9]=1;
                printf("%d\n", p);
                if(test >5)
                        goto EXIT;
                free(p);

        }
EXIT:
free(p);

        return 0;
}