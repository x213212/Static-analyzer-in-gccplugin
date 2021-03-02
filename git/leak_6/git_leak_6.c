/*
    buggy parent : cb5918a
    commit id : a452d0f4bae99c9acef6f7db75f6f1d922618732
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../git.h"

struct commit_list * commit_list_insert_by_date(struct commit *item, struct commit_list **list)
{
	struct commit_list **pp = list;
	struct commit_list *p;
	while ((p = *pp) != NULL) {
		if (p->item->date < item->date) {
			break;
		}
		pp = &p->next;
	}
	return commit_list_insert(item, pp);
}

static struct commit_list *merge_bases_many(struct commit *one, int n, struct commit **twos)
{
	struct commit_list *list = NULL;
	struct commit_list *result = NULL;
	int i;

	for (i = 0; i < n; i++) {
		if (one == twos[i])
			/*
			 * We do not mark this even with RESULT so we do not
			 * have to clean it up.
			 */
			return commit_list_insert(one, &result);
	}

	if (parse_commit(one))
		return NULL;
	for (i = 0; i < n; i++) {
		if (parse_commit(twos[i]))
			return NULL;
	}

	list = paint_down_to_common(one, n, twos);

	while (list) {
		struct commit *commit = pop_commit(&list);
		if (!(commit->object.flags & STALE))
			commit_list_insert_by_date(commit, &result);
	}
	return result;
}

struct commit_list *commit_list_insert(struct commit *item, struct commit_list **list_p)
{
	struct commit_list *new_list = xmalloc(sizeof(struct commit_list)); /* allocation site */
	new_list->item = item;
	new_list->next = *list_p;
	*list_p = new_list;
	return new_list;
}

static struct commit_list *get_merge_bases_many_0(struct commit *one,
						  int n,
						  struct commit **twos,
						  int cleanup)
{
	struct commit_list *list;
	struct commit **rslt;
	struct commit_list *result;
	int cnt, i;

	result = merge_bases_many(one, n, twos);
	for (i = 0; i < n; i++) {
		if (one == twos[i])
			return result;
	}
	if (!result || !result->next) {
		if (cleanup) {
			clear_commit_marks(one, all_flags);
			clear_commit_marks_many(n, twos, all_flags);
		}
		return result;
	}

	/* There are more than one */
	cnt = commit_list_count(result);
	rslt = xcalloc(cnt, sizeof(*rslt));
	for (list = result, i = 0; list; list = list->next)
		rslt[i++] = list->item;
	free_commit_list(result);

	clear_commit_marks(one, all_flags);
	clear_commit_marks_many(n, twos, all_flags);

	cnt = remove_redundant(rslt, cnt);
	result = NULL;
	for (i = 0; i < cnt; i++)
		commit_list_insert_by_date(rslt[i], &result);
	free(rslt);
	return result;
}

struct commit_list *get_merge_bases_many_dirty(struct commit *one,
					       int n,
					       struct commit **twos)
{
	return get_merge_bases_many_0(one, n, twos, 0);
}

static int show_merge_base(struct commit **rev, int rev_nr, int show_all)
{
	struct commit_list *result;

	result = get_merge_bases_many_dirty(rev[0], rev_nr - 1, rev + 1);

	if (!result)
		return 1;

	while (result) {
		printf("%s\n", oid_to_hex(&result->item->object.oid));
		if (!show_all)
			return 0;
		result = result->next;
	}
    /* memory leak */
	return 0;
}

static int handle_independent(int count, const char **args)
{
	struct commit_list *revs = NULL;
	struct commit_list *result;
	int i;

	for (i = count - 1; i >= 0; i--)
		commit_list_insert(get_commit_reference(args[i]), &revs);

	result = reduce_heads(revs);
	if (!result)
		return 1;

	while (result) {
		printf("%s\n", oid_to_hex(&result->item->object.oid));
		result = result->next;
	}
    /* memory leak */
	return 0;
}

int main (int argc, char **argv)
{
    struct commit *rev;
    int show_all, rev_nr;
    show_merge_base(&rev, rev_nr, show_all);
    handle_independent(argc, argv);
    return 0;
}





