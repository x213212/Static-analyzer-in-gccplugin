/*
    buggy parent : 94c9fd2 
    commit id : dd1055ed594f8fef18779cce3cd921c4ac66cf9c
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../git.h"

#define active_cache (the_index.cache)
#define active_nr (the_index.cache_nr)
#define active_alloc (the_index.cache_alloc)
#define active_cache_changed (the_index.cache_changed)
#define active_cache_tree (the_index.cache_tree)

#define REFRESH_REALLY		0x0001	/* ignore_valid */
#define REFRESH_UNMERGED	0x0002	/* allow unmerged */
#define REFRESH_QUIET		0x0004	/* be quiet about it */
#define REFRESH_IGNORE_MISSING	0x0008	/* ignore non-existent */
#define REFRESH_IGNORE_SUBMODULES	0x0010	/* ignore submodules */
#define REFRESH_IN_PORCELAIN	0x0020	/* user friendly output, not "needs update" */

#define read_cache() read_index(&the_index)
#define read_cache_from(path) read_index_from(&the_index, (path))
#define read_cache_preload(pathspec) read_index_preload(&the_index, (pathspec))
#define is_cache_unborn() is_index_unborn(&the_index)
#define read_cache_unmerged() read_index_unmerged(&the_index)
#define discard_cache() discard_index(&the_index)
#define unmerged_cache() unmerged_index(&the_index)
#define cache_name_pos(name, namelen) index_name_pos(&the_index,(name),(namelen))
#define add_cache_entry(ce, option) add_index_entry(&the_index, (ce), (option))
#define rename_cache_entry_at(pos, new_name) rename_index_entry_at(&the_index, (pos), (new_name))
#define remove_cache_entry_at(pos) remove_index_entry_at(&the_index, (pos))
#define remove_file_from_cache(path) remove_file_from_index(&the_index, (path))
#define add_to_cache(path, st, flags) add_to_index(&the_index, (path), (st), (flags))
#define add_file_to_cache(path, flags) add_file_to_index(&the_index, (path), (flags))
#define chmod_cache_entry(ce, flip) chmod_index_entry(&the_index, (ce), (flip))
#define refresh_cache(flags) refresh_index(&the_index, (flags), NULL, NULL, NULL)
#define ce_match_stat(ce, st, options) ie_match_stat(&the_index, (ce), (st), (options))
#define ce_modified(ce, st, options) ie_modified(&the_index, (ce), (st), (options))
#define cache_dir_exists(name, namelen) index_dir_exists(&the_index, (name), (namelen))
#define cache_file_exists(name, namelen, igncase) index_file_exists(&the_index, (name), (namelen), (igncase))
#define cache_name_is_other(name, namelen) index_name_is_other(&the_index, (name), (namelen))
#define resolve_undo_clear() resolve_undo_clear_index(&the_index)
#define unmerge_cache_entry_at(at) unmerge_index_entry_at(&the_index, at)
#define unmerge_cache(pathspec) unmerge_index(&the_index, pathspec)
#define read_blob_data_from_cache(path, sz) read_blob_data_from_index(&the_index, (path), (sz))
static enum {
	COMMIT_AS_IS = 1,
	COMMIT_NORMAL,
	COMMIT_PARTIAL
} commit_style;


#define MAX_PARALLEL (20)
#define THREAD_COST (500)

typedef struct {} HANDLE;
typedef struct {} DWORD;
typedef struct {
	HANDLE handle;
	void *(*start_routine)(void*);
	void *arg;
	DWORD tid;
} pthread_t;

struct thread_data {
	pthread_t pthread;
	struct index_state *index;
	struct pathspec pathspec;
	int offset, nr;
};

int core_preload_index = 1;
struct index_state the_index;
static int patch_interactive, add_interactive, edit_interactive;
static struct lock_file index_lock; /* real index */
static const char *author_message, *author_message_buffer;
static char *edit_message, *use_message;
static char *fixup_message, *squash_message;
static int all, also, interactive, patch_interactive, only, amend, signoff;
static int edit_flag = -1; /* unspecified */
static int quiet, verbose, no_verify, allow_empty, dry_run, renew_authorship;
static int config_commit_verbose = -1; /* unspecified */
static int no_post_rewrite, allow_empty_message;
static char *untracked_files_arg, *force_date, *ignore_submodule_arg;
static char *sign_commit;


void string_list_init(struct string_list *list, int strdup_strings)
{
	memset(list, 0, sizeof(*list));
	list->strdup_strings = strdup_strings;
}

/* returns -1-index if already exists */
static int add_entry(int insert_at, struct string_list *list, const char *string)
{
	int exact_match = 0;
	int index = insert_at != -1 ? insert_at : get_entry_index(list, string, &exact_match);

	if (exact_match)
		return -1 - index;

	ALLOC_GROW(list->items, list->nr+1, list->alloc);
	if (index < list->nr)
		MOVE_ARRAY(list->items + index + 1, list->items + index,
			   list->nr - index);
	list->items[index].string = list->strdup_strings ?
		xstrdup(string) : (char *)string; /* allocation site */
	list->items[index].util = NULL;
	list->nr++;

	return index;
}

struct string_list_item *string_list_insert(struct string_list *list, const char *string)
{
	int index = add_entry(-1, list, string);

	if (index < 0)
		index = -1 - index;

	return list->items + index;
}


/*
 * Given command line arguments and a prefix, convert the input to
 * pathspec. die() if any magic in magic_mask is used.
 */
void parse_pathspec(struct pathspec *pathspec,
		    unsigned magic_mask, unsigned flags,
		    const char *prefix, const char **argv)
{
	struct pathspec_item *item;
	const char *entry = argv ? *argv : NULL;
	int i, n, prefixlen, warn_empty_string, nr_exclude = 0;

	memset(pathspec, 0, sizeof(*pathspec));

	if (flags & PATHSPEC_MAXDEPTH_VALID)
		pathspec->magic |= PATHSPEC_MAXDEPTH;

	/* No arguments, no prefix -> no pathspec */
	if (!entry && !prefix)
		return;

	/* No arguments with prefix -> prefix pathspec */
	if (!entry) {
		if (flags & PATHSPEC_PREFER_FULL)
			return;

		pathspec->items = item = xcalloc(1, sizeof(*item)); /* allocation site */
		item->match = xstrdup(prefix);                      /* allocation site */
		item->original = xstrdup(prefix);                   /* allocation site */
		item->nowildcard_len = item->len = strlen(prefix);
		item->prefix = item->len;
		pathspec->nr = 1;
		return;
	}

	n = 0;
	warn_empty_string = 1;

	pathspec->nr = n;
	ALLOC_ARRAY(pathspec->items, n + 1);                    /* allocation site */
	item = pathspec->items;
	prefixlen = prefix ? strlen(prefix) : 0;

	for (i = 0; i < n; i++) {
		entry = argv[i];

        item[i].match = xmalloc(10);                        /* allocation site */
        item[i].original = malloc(100);                     /* allocation site */

		if (item[i].magic & PATHSPEC_EXCLUDE)
			nr_exclude++;

		if ((flags & PATHSPEC_SYMLINK_LEADING_PATH) &&
		    has_symlink_leading_path(item[i].match, item[i].len)) {
			die(_("pathspec '%s' is beyond a symbolic link"), entry);
		}

		if (item[i].nowildcard_len < item[i].len)
			pathspec->has_wildcard = 1;
		pathspec->magic |= item[i].magic;
	}

	/*
	 * If everything is an exclude pattern, add one positive pattern
	 * that matches everything. We allocated an extra one for this.
	 */
	if (nr_exclude == n) {
		int plen = (!(flags & PATHSPEC_PREFER_CWD)) ? 0 : prefixlen;
		init_pathspec_item(item + n, 0, prefix, plen, "");
		pathspec->nr++;
	}

}



/*
 * Take a union of paths in the index and the named tree (typically, "HEAD"),
 * and return the paths that match the given pattern in list.
 */
static int list_paths(struct string_list *list, const char *with_tree,
		      const char *prefix, const struct pathspec *pattern)
{
	int i, ret;
	char *m;

	if (!pattern->nr)
		return 0;

	if (with_tree) {
		char *max_prefix = common_prefix(pattern);
		overlay_tree_on_index(&the_index, with_tree,
				      max_prefix ? max_prefix : prefix);
		free(max_prefix);
	}

	for (i = 0; i < active_nr; i++) {
		const struct cache_entry *ce = active_cache[i];
		struct string_list_item *item;

		item = string_list_insert(list, ce->name);
		if (ce_skip_worktree(ce))
			item->util = item; /* better a valid pointer than a fake one */
	}

	ret = report_path_error(m, pattern, prefix);
	free(m);
	return ret;
}


static const char *prepare_index(int argc, const char **argv, const char *prefix,
				 const struct commit *current_head, int is_status)
{
	struct string_list partial;
	struct pathspec pathspec;
	int refresh_flags = REFRESH_QUIET;
	const char *ret;

	if (is_status)
		refresh_flags |= REFRESH_UNMERGED;
	parse_pathspec(&pathspec, 0,                
		       PATHSPEC_PREFER_FULL,
		       prefix, argv);

	if (read_cache_preload(&pathspec) < 0)
		die(_("index file corrupt%s"), "");

	if (interactive) {
		char *old_index_env = NULL;

		if (interactive_add(argc, argv, prefix, patch_interactive) != 0)
			die(_("interactive add failed%s"), "");

		read_cache_from(get_lock_file_path(&index_lock));
		commit_style = COMMIT_NORMAL;
		return get_lock_file_path(&index_lock);
	}

	/*
	 * Non partial, non as-is commit.
	 *
	 * (1) get the real index;
	 * (2) update the_index as necessary;
	 * (3) write the_index out to the real index (still locked);
	 * (4) return the name of the locked index file.
	 *
	 * The caller should run hooks on the locked real index, and
	 * (A) if all goes well, commit the real index;
	 * (B) on failure, rollback the real index.
	 */
	if (all || (also && pathspec.nr)) {
		add_files_to_cache(also ? prefix : NULL, &pathspec, 0);
		commit_style = COMMIT_NORMAL;
		return get_lock_file_path(&index_lock);
	}

	/*
	 * As-is commit.
	 *
	 * (1) return the name of the real index file.
	 *
	 * The caller should run hooks on the real index,
	 * and create commit from the_index.
	 * We still need to refresh the index here.
	 */
	if (!only && !pathspec.nr) {
		if (active_cache_changed) {
				die(_("unable to write new_index file%s"), "");
		} else {
			rollback_lock_file(&index_lock);
		}
		commit_style = COMMIT_AS_IS;
		return get_index_file();
	}

	/*
	 * A partial commit.
	 *
	 * (0) find the set of affected paths;
	 * (1) get lock on the real index file;
	 * (2) update the_index with the given paths;
	 * (3) write the_index out to the real index (still locked);
	 * (4) get lock on the false index file;
	 * (5) reset the_index from HEAD;
	 * (6) update the_index the same way as (2);
	 * (7) write the_index out to the false index file;
	 * (8) return the name of the false index file (still locked);
	 *
	 * The caller should run hooks on the locked false index, and
	 * create commit from it.  Then
	 * (A) if all goes well, commit the real index;
	 * (B) on failure, rollback the real index;
	 * In either case, rollback the false index.
	 */
	commit_style = COMMIT_PARTIAL;

	string_list_init(&partial, 1);
	if (list_paths(&partial, !current_head ? NULL : "HEAD", prefix, &pathspec))
		exit(1);

	discard_cache();
	return ret;
}

int main(int argc, char **argv)
{
    struct commit *current_head;
    int is_status;
    prepare_index(argc, argv, "prefix", current_head, is_status);
    return 0;
}


