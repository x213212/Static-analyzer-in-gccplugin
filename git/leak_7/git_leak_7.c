/*
    buggy parent : b2ccdf7
    commit id : dcb572ab94f83a1a857d276fcebff5700077f2b7
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../git.h"

#define INCOMPLETE	(1u<<10)
#define STUDYING	(1u<<11)
#define REACHABLE	(1u<<12)


#define S_IFINVALID     0030000
/*
 * A zero-length string to which object_array_entry::name can be
 * initialized without requiring a malloc/free.
 */
static char object_array_slopbuf[1];

void add_object_array_with_path(struct object *obj, const char *name,
				struct object_array *array,
				unsigned mode, const char *path)
{
	unsigned nr = array->nr;
	unsigned alloc = array->alloc;
	struct object_array_entry *objects = array->objects;
	struct object_array_entry *entry;

	if (nr >= alloc) {
		alloc = (alloc + 32) * 2;
		REALLOC_ARRAY(objects, alloc);
		array->alloc = alloc;
		array->objects = objects;
	}
	entry = &objects[nr];
	entry->item = obj;
	if (!name)
		entry->name = NULL;
	else if (!*name)
		/* Use our own empty string instead of allocating one: */
		entry->name = object_array_slopbuf;
	else
		entry->name = xstrdup(name);    /* allocation site */
	entry->mode = mode;
	if (path)
		entry->path = xstrdup(path);    /* allocation site */
	else
		entry->path = NULL;
	array->nr = ++nr;
}

void add_object_array(struct object *obj, const char *name, struct object_array *array)
{
	add_object_array_with_path(obj, name, array, S_IFINVALID, NULL);
}

static int commit_is_complete(struct commit *commit)
{
	struct object_array study;
	struct object_array found;
	int is_incomplete = 0;
	int i;

	/* early return */
	if (commit->object.flags & SEEN)
		return 1;
	if (commit->object.flags & INCOMPLETE)
		return 0;
	/*
	 * Find all commits that are reachable and are not marked as
	 * SEEN.  Then make sure the trees and blobs contained are
	 * complete.  After that, mark these commits also as SEEN.
	 * If some of the objects that are needed to complete this
	 * commit are missing, mark this commit as INCOMPLETE.
	 */
	memset(&study, 0, sizeof(study));
	memset(&found, 0, sizeof(found));
	add_object_array(&commit->object, NULL, &study);
	add_object_array(&commit->object, NULL, &found);
	commit->object.flags |= STUDYING;
	while (study.nr) {
		struct commit *c;
		struct commit_list *parent;

		c = (struct commit *)study.objects[--study.nr].item;
		if (!c->object.parsed && !parse_object(&c->object.oid))
			c->object.flags |= INCOMPLETE;

		if (c->object.flags & INCOMPLETE) {
			is_incomplete = 1;
			break;
		}
		else if (c->object.flags & SEEN)
			continue;
		for (parent = c->parents; parent; parent = parent->next) {
			struct commit *p = parent->item;
			if (p->object.flags & STUDYING)
				continue;
			p->object.flags |= STUDYING;
			add_object_array(&p->object, NULL, &study);
			add_object_array(&p->object, NULL, &found);
		}
	}
	if (!is_incomplete) {
		/*
		 * make sure all commits in "found" array have all the
		 * necessary objects.
		 */
		for (i = 0; i < found.nr; i++) {
			struct commit *c =
				(struct commit *)found.objects[i].item;
			if (!tree_is_complete(&c->tree->object.oid)) {
				is_incomplete = 1;
				c->object.flags |= INCOMPLETE;
			}
		}
		if (!is_incomplete) {
			/* mark all found commits as complete, iow SEEN */
			for (i = 0; i < found.nr; i++)
				found.objects[i].item->flags |= SEEN;
		}
	}
	/* clear flags from the objects we traversed */
	for (i = 0; i < found.nr; i++)
		found.objects[i].item->flags &= ~STUDYING;
	if (is_incomplete)
		commit->object.flags |= INCOMPLETE;
	else {
		/*
		 * If we come here, we have (1) traversed the ancestry chain
		 * from the "commit" until we reach SEEN commits (which are
		 * known to be complete), and (2) made sure that the commits
		 * encountered during the above traversal refer to trees that
		 * are complete.  Which means that we know *all* the commits
		 * we have seen during this process are complete.
		 */
		for (i = 0; i < found.nr; i++)
			found.objects[i].item->flags |= SEEN;
	}
	/* free object arrays */
    free(study.objects);    /* memory leak */
    free(found.objects);    /* memory leak */
	return !is_incomplete;
}

int main()
{
    struct commit commit;
    commit_is_complete(&commit);
    return 0;
}
