/*
    buggy parent :  7c11718 
    commit id : f4e45cb3eb6fad4570ff63eecb37bae8102992fc
*/

#include <stdio.h>
#include <stdlib.h>

#include "stdio.h"
#include "git.h"

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

struct commit_list *commit_list_insert(struct commit *item, struct commit_list **list_p)
{
	struct commit_list *new_list = xmalloc(sizeof(struct commit_list)); /* allocation site */
	new_list->item = item;
	new_list->next = *list_p;
	*list_p = new_list;
	return new_list;
}

static struct commit_list *best_bisection(struct commit_list *list, int nr)
{
	struct commit_list *p, *best;
	int best_distance = -1;

	best = list;
	for (p = list; p; p = p->next) {
		int distance;
		unsigned flags = p->item->object.flags;

		if (flags & TREESAME)
			continue;
		distance = weight(p);
		if (nr - distance < distance)
			distance = nr - distance;
		if (distance > best_distance) {
			best = p;
			best_distance = distance;
		}
	}

	return best;
}

static struct commit_list *best_bisection_sorted(struct commit_list *list, int nr)
{
	struct commit_list *p;
	struct commit_dist *array = xcalloc(nr, sizeof(*array));
	struct strbuf buf = STRBUF_INIT;
	int cnt, i;

	for (p = list, cnt = 0; p; p = p->next) {
		int distance;
		unsigned flags = p->item->object.flags;

		if (flags & TREESAME)
			continue;
		distance = weight(p);
		if (nr - distance < distance)
			distance = nr - distance;
		array[cnt].commit = p->item;
		array[cnt].distance = distance;
		cnt++;
	}
	for (p = list, i = 0; i < cnt; i++) {
		struct object *obj = &(array[i].commit->object);

		strbuf_reset(&buf);
		strbuf_addf(&buf, "dist=%d", array[i].distance);
		add_name_decoration(DECORATION_NONE, buf.buf, obj);

		p->item = array[i].commit;
		if (i < cnt - 1)
			p = p->next;
	}
	free_commit_list(p->next);
	p->next = NULL;
	strbuf_release(&buf);
	free(array);
	return list;
}


static struct commit_list *do_find_bisection(struct commit_list *list,
					     int nr, int *weights,
					     int find_all)
{
	int n, counted;
	struct commit_list *p;

	counted = 0;

	for (n = 0, p = list; p; p = p->next) {
		struct commit *commit = p->item;
		unsigned flags = commit->object.flags;

		p->item->util = &weights[n++];
		switch (count_interesting_parents(commit)) {
		case 0:
			if (!(flags & TREESAME)) {
				weight_set(p, 1);
				counted++;
				show_list("bisection 2 count one",
					  counted, nr, list);
			}
			/*
			 * otherwise, it is known not to reach any
			 * tree-changing commit and gets weight 0.
			 */
			break;
		case 1:
			weight_set(p, -1);
			break;
		default:
			weight_set(p, -2);
			break;
		}
	}

	show_list("bisection 2 initialize", counted, nr, list);

	/*
	 * If you have only one parent in the resulting set
	 * then you can reach one commit more than that parent
	 * can reach.  So we do not have to run the expensive
	 * count_distance() for single strand of pearls.
	 *
	 * However, if you have more than one parents, you cannot
	 * just add their distance and one for yourself, since
	 * they usually reach the same ancestor and you would
	 * end up counting them twice that way.
	 *
	 * So we will first count distance of merges the usual
	 * way, and then fill the blanks using cheaper algorithm.
	 */
	for (p = list; p; p = p->next) {
		if (p->item->object.flags & UNINTERESTING)
			continue;
		if (weight(p) != -2)
			continue;
		weight_set(p, count_distance(p));
		clear_distance(list);

		/* Does it happen to be at exactly half-way? */
		if (!find_all && halfway(p, nr))
			return p;
		counted++;
	}

	show_list("bisection 2 count_distance", counted, nr, list);

	while (counted < nr) {
		for (p = list; p; p = p->next) {
			struct commit_list *q;
			unsigned flags = p->item->object.flags;

			if (0 <= weight(p))
				continue;
			for (q = p->item->parents; q; q = q->next) {
				if (q->item->object.flags & UNINTERESTING)
					continue;
				if (0 <= weight(q))
					break;
			}
			if (!q)
				continue;

			/*
			 * weight for p is unknown but q is known.
			 * add one for p itself if p is to be counted,
			 * otherwise inherit it from q directly.
			 */
			if (!(flags & TREESAME)) {
				weight_set(p, weight(q)+1);
				counted++;
				show_list("bisection 2 count one",
					  counted, nr, list);
			}
			else
				weight_set(p, weight(q));

			/* Does it happen to be at exactly half-way? */
			if (!find_all && halfway(p, nr))
				return p;
		}
	}

	show_list("bisection 2 counted all", counted, nr, list);

	if (!find_all)
		return best_bisection(list, nr);
	else
		return best_bisection_sorted(list, nr);
}

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
			free(p);
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
	weights = xcalloc(on_list, sizeof(*weights));

	/* Do the real work of finding bisection commit. */
	best = do_find_bisection(list, nr, weights, find_all);
	if (best) {
		if (!find_all)
			best->next = NULL;              /* memory leak */
		*reaches = weight(best);
	}
	free(weights);
	*commit_list = best;
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


