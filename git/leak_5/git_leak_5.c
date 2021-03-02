/*
    commit id : cb7b29eb67772d08e2365ed07ede9d954d0344c1
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../git.h"

/* Remember to update object flag allocation in object.h */
#define PARENT1		(1u<<16)
#define PARENT2		(1u<<17)
#define STALE		(1u<<18)
#define RESULT		(1u<<19)

enum object_type {
	OBJ_BAD = -1,
	OBJ_NONE = 0,
	OBJ_COMMIT = 1,
	OBJ_TREE = 2,
	OBJ_BLOB = 3,
	OBJ_TAG = 4,
	/* 5 for future expansion */
	OBJ_OFS_DELTA = 6,
	OBJ_REF_DELTA = 7,
	OBJ_ANY,
	OBJ_MAX
};


static int remove_redundant(struct commit **array, int cnt)
{
	/*
	 * Some commit in the array may be an ancestor of
	 * another commit.  Move such commit to the end of
	 * the array, and return the number of commits that
	 * are independent from each other.
	 */
	struct commit **work;
	unsigned char *redundant;
	int *filled_index;
	int i, j, filled;

	work = xcalloc(cnt, sizeof(*work));
	redundant = xcalloc(cnt, 1);
	ALLOC_ARRAY(filled_index, cnt - 1);

	for (i = 0; i < cnt; i++)
		parse_commit(array[i]);
	for (i = 0; i < cnt; i++) {
		struct commit_list *common;

		if (redundant[i])
			continue;
		for (j = filled = 0; j < cnt; j++) {
			if (i == j || redundant[j])
				continue;
			filled_index[filled] = j;
			work[filled++] = array[j];
		}
		common = paint_down_to_common(array[i], filled, work);
		if (array[i]->object.flags & PARENT2)
			redundant[i] = 1;
		for (j = 0; j < filled; j++)
			if (work[j]->object.flags & PARENT1)
				redundant[filled_index[j]] = 1;
		clear_commit_marks(array[i], all_flags);
		for (j = 0; j < filled; j++)
			clear_commit_marks(work[j], all_flags);
		free_commit_list(common);
	}

	/* Now collect the result */
	COPY_ARRAY(work, array, cnt);
	for (i = filled = 0; i < cnt; i++)
		if (!redundant[i])
			array[filled++] = work[i];
	for (j = filled, i = 0; i < cnt; i++)
		if (redundant[i])
			array[j++] = work[i];
	free(work);
	free(redundant);
	free(filled_index);
	return filled;
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

struct commit_list *reduce_heads(struct commit_list *heads)
{
	struct commit_list *p;
	struct commit_list *result = NULL, **tail = &result;
	struct commit **array;
	int num_head, i;

	if (!heads)
		return NULL;

	/* Uniquify */
	for (p = heads; p; p = p->next)
		p->item->object.flags &= ~STALE;
	for (p = heads, num_head = 0; p; p = p->next) {
		if (p->item->object.flags & STALE)
			continue;
		p->item->object.flags |= STALE;
		num_head++;
	}
	array = xcalloc(num_head, sizeof(*array)); /* allocation site */
	for (p = heads, i = 0; p; p = p->next) {
		if (p->item->object.flags & STALE) {
			array[i++] = p->item;
			p->item->object.flags &= ~STALE;
		}
	}
	num_head = remove_redundant(array, num_head);
	for (i = 0; i < num_head; i++)
		tail = &commit_list_insert(array[i], tail)->next;
	                                            /* memory leak */
	return result;
}

int main()
{
    struct commit_list *list, *reduced_list;
    int reaches, all, find_all;
    for(int i = 0; i < 10; i++)
    {
        commit_list_insert(NULL, &list);
    }
    reduced_list = reduce_heads(list);

    free_commit_list(list);
    return 0;
}






