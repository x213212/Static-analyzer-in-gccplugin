/*
    buggy parent : e9ce897
    commit id : afbb8838b7d4d1887da1e1871f8e9ccd01ae1138
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../git.h"

#define N_(msgid) msgid

static const char * const git_reset_usage[] = {
	N_("git reset [--mixed | --soft | --hard | --merge | --keep] [-q] [<commit>]"),
	N_("git reset [-q] [<tree-ish>] [--] <paths>..."),
	N_("git reset --patch [<tree-ish>] [--] [<paths>...]"),
	NULL
};

enum reset_type { MIXED, SOFT, HARD, MERGE, KEEP, NONE };
static const char *reset_type_names[] = {
	N_("mixed"), N_("soft"), N_("hard"), N_("merge"), N_("keep"), NULL
};
const char *tree_type = "tree";

#define MAX_UNPACK_TREES 8

struct unpack_trees_options;
struct exclude_list;

typedef int (*merge_fn_t)(const struct cache_entry * const *src,
		struct unpack_trees_options *options);

enum unpack_trees_error_types {
	ERROR_WOULD_OVERWRITE = 0,
	ERROR_NOT_UPTODATE_FILE,
	ERROR_NOT_UPTODATE_DIR,
	ERROR_WOULD_LOSE_UNTRACKED_OVERWRITTEN,
	ERROR_WOULD_LOSE_UNTRACKED_REMOVED,
	ERROR_BIND_OVERLAP,
	ERROR_SPARSE_NOT_UPTODATE_FILE,
	ERROR_WOULD_LOSE_ORPHANED_OVERWRITTEN,
	ERROR_WOULD_LOSE_ORPHANED_REMOVED,
	ERROR_WOULD_LOSE_SUBMODULE,
	NB_UNPACK_TREES_ERROR_TYPES
};

/*
 * Sets the list of user-friendly error messages to be used by the
 * command "cmd" (either merge or checkout), and show_all_errors to 1.
 */
void setup_unpack_trees_porcelain(struct unpack_trees_options *opts,
				  const char *cmd);

int oneway_merge(const struct cache_entry * const *src,
		 struct unpack_trees_options *o);

int twoway_merge(const struct cache_entry * const *src,
		 struct unpack_trees_options *o);

struct name_entry {
	const struct object_id *oid;
	const char *path;
	unsigned int mode;
};

struct tree_desc {
	const void *buffer;
	struct name_entry entry;
	unsigned int size;
};

struct unpack_trees_options {
	unsigned int reset,
		     merge,
		     update,
		     index_only,
		     nontrivial_merge,
		     trivial_merges_only,
		     verbose_update,
		     aggressive,
		     skip_unmerged,
		     initial_checkout,
		     diff_index_cached,
		     debug_unpack,
		     skip_sparse_checkout,
		     gently,
		     exiting_early,
		     show_all_errors,
		     dry_run;
	const char *prefix;
	int cache_bottom;
	struct dir_struct *dir;
	struct pathspec *pathspec;
	merge_fn_t fn;
	const char *msgs[NB_UNPACK_TREES_ERROR_TYPES];
	/*
	 * Store error messages in an array, each case
	 * corresponding to a error message type
	 */
	struct string_list unpack_rejects[NB_UNPACK_TREES_ERROR_TYPES];

	int head_idx;
	int merge_size;

	struct cache_entry *df_conflict_entry;
	void *unpack_data;

	struct index_state *dst_index;
	struct index_state *src_index;
	struct index_state result;

	struct exclude_list *el; /* for internal use */
};

struct tree_desc_skip {
	struct tree_desc_skip *prev;
	const void *ptr;
};

struct tree_desc_x {
	struct tree_desc d;
	struct tree_desc_skip *skip;
};

static int update_tree_entry_internal(struct tree_desc *desc, struct strbuf *err)
{
	const void *buf = desc->buffer;
	const unsigned char *end = desc->entry.oid->hash + 20;
	unsigned long size = desc->size;
	unsigned long len = end - (const unsigned char *)buf;

	if (size < len)
		die("too-short tree file%s", buf);
	buf = end;
	size -= len;
	desc->buffer = buf;
	desc->size = size;
	if (size)
		return decode_tree_entry(desc, buf, size, err);
	return 0;
}

void update_tree_entry(struct tree_desc *desc)
{
	struct strbuf err = STRBUF_INIT;
	if (update_tree_entry_internal(desc, &err))
		die("%s", err.buf);
	strbuf_release(&err);
}

static void extended_entry_extract(struct tree_desc_x *t,
				   struct name_entry *a,
				   const char *first,
				   int first_len)
{
	const char *path;
	int len;
	struct tree_desc probe;
	struct tree_desc_skip *skip;

	/*
	 * Extract the first entry from the tree_desc, but skip the
	 * ones that we already returned in earlier rounds.
	 */
	while (1) {
		if (!t->d.size) {
			entry_clear(a);
			break; /* not found */
		}
		entry_extract(&t->d, a);
		for (skip = t->skip; skip; skip = skip->prev)
			if (a->path == skip->ptr)
				break; /* found */
		if (!skip)
			break;
		/* We have processed this entry already. */
		update_tree_entry(&t->d);
	}
}
int traverse_trees(int n, struct tree_desc *t, struct traverse_info *info)
{
	int i, n;
	struct tree_desc_x *tx = xcalloc(n, sizeof(*tx));
	struct name_entry *entry = xmalloc(n*sizeof(*entry));
	struct name_entry *e = NULL;
	for (i = 0; i < n; i++)
		tx[i].d = t[i];
	for (i = 0; i < n; i++) {
		e = entry + i;
		extended_entry_extract(tx + i, e, NULL, 0);
	}
	free(tx);
	return 1;
}

int unpack_trees(unsigned len, struct tree_desc *t, struct unpack_trees_options *o)
{
	traverse_trees(len, t, NULL);
	return 0;
}


static int init_tree_desc_internal(struct tree_desc *desc, const void *buffer, unsigned long size, struct strbuf *err)
{
	desc->buffer = buffer;
	desc->size = size;
	if (size)
		return decode_tree_entry(desc, buffer, size, err);
	return 0;
}

void init_tree_desc(struct tree_desc *desc, const void *buffer, unsigned long size)
{
	struct strbuf err = STRBUF_INIT;
	if (init_tree_desc_internal(desc, buffer, size, &err))
		die("%s", err.buf);
	strbuf_release(&err);
}

void fill_tree_descriptor(struct tree_desc *desc, const struct object_id *oid)
{
	unsigned long size = 0;
	void *buf = NULL;

	if (oid) {
		buf = malloc(100); /* allocation site */
		if (!buf)
			die("unable to read tree %s", oid_to_hex(oid));
	}
	init_tree_desc(desc, buf, size);
}

static int reset_index(const struct object_id *oid, int reset_type, int quiet)
{
	int nr = 0;
	struct tree_desc desc[2];
	struct tree *tree;
	struct unpack_trees_options opts;

	memset(&opts, 0, sizeof(opts));
	opts.head_idx = 1;
	opts.src_index = &the_index;
	opts.dst_index = &the_index;
	opts.fn = oneway_merge;
	opts.merge = 1;
	if (!quiet)
		opts.verbose_update = 1;
	switch (reset_type) {
	case KEEP:
	case MERGE:
		opts.update = 1;
		break;
	case HARD:
		opts.update = 1;
		/* fallthrough */
	default:
		opts.reset = 1;
	}

	read_cache_unmerged();

	if (reset_type == KEEP) {
		struct object_id head_oid;
		if (get_oid("HEAD", &head_oid))
			return error(_("You do not have a valid HEAD."));
		fill_tree_descriptor(desc + nr, &head_oid);
		nr++;
		opts.fn = twoway_merge;
	}

	fill_tree_descriptor(desc + nr, oid);
	nr++;

    __USE(desc[nr].buffer);
	if (unpack_trees(nr, desc, &opts))
		return -1;

	if (reset_type == MIXED || reset_type == HARD) {
		tree = parse_tree_indirect(oid);
		prime_cache_tree(&the_index, tree);
	}

	return 0;
}

int main ()
{
    struct object_id oid;
    int type, quiet;
    reset_index(&oid, type, quiet);
    return 0;
}
