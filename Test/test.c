#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int *foo(void);
#include <stdio.h>
#include <stdlib.h>

typedef struct Node
{
        int data;
        struct Node *next;
} Node;

Node *createNode(int value)
{
        Node *newNode = (Node *)malloc(sizeof(Node));
        if (!newNode)
        {
                printf("malloc failed\n");
                exit(EXIT_FAILURE);
        }
        newNode->data = value;
        newNode->next = NULL;
        return newNode;
}

int main()
{
        Node *head = NULL;
        Node *temp = NULL;

        for (int i = 1; i <= 5; i++)
        {
                Node *node = createNode(i);
                if (head == NULL)
                {

                        head = node;
                        temp = head;
                }
                else
                {

                        temp->next = node;
                        temp = node;
                }
        }

        temp = head;
        while (temp != NULL)
        {
                printf("%d ", temp->data);
                temp = temp->next;
        }
        printf("\n");

        temp = head;
        while (temp != NULL)
        {
                Node *next = temp->next;
                free(temp);
                temp = next;
        }
        srand(10);
        int *p;
        int test;
        for (int i = 0; i < 10; i++)
        {
                test = rand() % (10 - 0 + 1) + 0;
                p = (int *)malloc(sizeof(int) * 10);
                p[0] = 1;
                p[1] = 2;
                p[2] = 3;
                p[3] = 4;
                p[4] = 5;
                p[5] = 6;
                p[6] = 7;
                p[7] = 8;
                p[8] = 9;
                p[9] = 10;
                // printf("%d\n",p[0]);
                if (test > 5)
                        goto EXIT;
                free(p);
        }
EXIT:
        free(p);

        return 0;
}