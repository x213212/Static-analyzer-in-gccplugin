/*
	buggy parent: 9d2cdc8
	commit id : aba19b625f34fb3d61263fe8044cf0c6d8804570
*/
#include <stdio.h>
#include <stdlib.h>
int *foo(void);
int main()
{
	int test;
	int test2;
	int *a = malloc(1);
	// test = 1;
	// test2 = 1;
	if (test)
	{ // bb2
		printf("%d\n", a);

		if (test2)
		{ // bb3
			free(a);
			printf("%d\n", a);
			exit(0);
		}
		else
		{ // 4
			printf("%d\n", a);
			exit(0);
			// free(a);
		}
	}
	else
	{ // bb5
		printf("%d\n", a);
		// free(a);
	}
	free(a);
	return 0;
}
