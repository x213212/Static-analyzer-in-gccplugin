/*
    buggy parent : 24d707f
    commit id : fc5c40bb2bb1921f3bdfa55c1d846dc080c356b2
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../git.h"

void find_bisection(struct commit_list **commit_list, int *reaches,
		    int *all, int find_all)
{
	int nr, on_list;
	struct commit_list *list, *p, *best, *next, *last;
	int *weights;

	show_list("bisection 2 entry", 0, 0, *commit_list);

	/*
	 * Count the number of total and tree-changing items on the
	 * list, while reversing the list.
	 */
	for (nr = on_list = 0, last = NULL, p = *commit_list;
	     p;
	     p = next) {
		unsigned flags = p->item->object.flags;

		next = p->next;
		if (flags & UNINTERESTING) {
			/* memory leak */
			continue;
		}
		p->next = last;
		last = p;
		if (!(flags & TREESAME))
			nr++;
		on_list++;
	}
	list = last;
	show_list("bisection 2 sorted", 0, nr, list);

	*all = nr;

	*commit_list = list;
}

struct commit_list *commit_list_insert(struct commit *item, struct commit_list **list_p)
{
	struct commit_list *new_list = xmalloc(sizeof(struct commit_list));
	new_list->item = item;
	new_list->next = *list_p;
	*list_p = new_list;
	return new_list;
}

struct commit *pop_commit(struct commit_list **stack)
{
	struct commit_list *top = *stack;
	struct commit *item = top ? top->item : NULL;

	if (top) {
		*stack = top->next;
		free(top);
	}
	return item;
}


void free_commit_list(struct commit_list *list)
{
	while (list)
		pop_commit(&list);
}

int main()
{
    struct commit_list *list;
    int reaches, all, find_all;
    for(int i = 0; i < 10; i++)
    {
        commit_list_insert(NULL, &list);
    }
    find_bisection(&list, &reaches, &all, find_all);
        
    free_commit_list(list);
    return 0;
}



