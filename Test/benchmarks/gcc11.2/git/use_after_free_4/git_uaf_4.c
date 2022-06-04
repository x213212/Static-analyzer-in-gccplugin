/*
    buggy parent : 441c4a4
    commit id : 41284eb0f944fe2d73708bb4105a8e3ccd0297df 
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "../stdio.h"
#include "../git.h"

#define CE_STAGEMASK (0x3000)
#define CE_EXTENDED  (0x4000)
#define CE_VALID     (0x8000)
#define CE_STAGESHIFT 12

/*
 * Range 0xFFFF0FFF in ce_flags is divided into
 * two parts: in-memory flags and on-disk ones.
 * Flags in CE_EXTENDED_FLAGS will get saved on-disk
 * if you want to save a new flag, add it in
 * CE_EXTENDED_FLAGS
 *
 * In-memory only flags
 */
#define CE_UPDATE            (1 << 16)
#define CE_REMOVE            (1 << 17)
#define CE_UPTODATE          (1 << 18)
#define CE_ADDED             (1 << 19)

#define CE_HASHED            (1 << 20)
#define CE_WT_REMOVE         (1 << 22) /* remove in work directory */
#define CE_CONFLICTED        (1 << 23)

#define CE_UNPACKED          (1 << 24)
#define CE_NEW_SKIP_WORKTREE (1 << 25)

/* used to temporarily mark paths matched by pathspecs */
#define CE_MATCHED           (1 << 26)

#define CE_UPDATE_IN_BASE    (1 << 27)
#define CE_STRIP_NAME        (1 << 28)

/*
 * Extended on-disk flags
 */
#define CE_INTENT_TO_ADD     (1 << 29)
#define CE_SKIP_WORKTREE     (1 << 30)
/* CE_EXTENDED2 is for future extension */
#define CE_EXTENDED2         (1 << 31)

#define CE_EXTENDED_FLAGS (CE_INTENT_TO_ADD | CE_SKIP_WORKTREE)

#define CE_MATCH_IGNORE_VALID		01
#define CE_MATCH_RACY_IS_DIRTY		02
#define CE_MATCH_IGNORE_SKIP_WORKTREE	04
#define CE_MATCH_IGNORE_MISSING		0x08
#define CE_MATCH_REFRESH		0x10

#define ADD_CACHE_OK_TO_ADD 1		/* Ok to add */
#define ADD_CACHE_OK_TO_REPLACE 2	/* Ok to replace file/directory */
#define ADD_CACHE_SKIP_DFCHECK 4	/* Ok to skip DF conflict checks */
#define ADD_CACHE_JUST_APPEND 8		/* Append only; tree.c::read_tree() */
#define ADD_CACHE_NEW_ONLY 16		/* Do not replace existing ones */
#define ADD_CACHE_KEEP_CACHE_TREE 32	/* Do not invalidate cache-tree */

#define ADD_CACHE_VERBOSE 1
#define ADD_CACHE_PRETEND 2
#define ADD_CACHE_IGNORE_ERRORS	4
#define ADD_CACHE_IGNORE_REMOVAL 8
#define ADD_CACHE_INTENT 16

struct dir_entry {
	struct hashmap_entry ent;
	struct dir_entry *parent;
	struct cache_entry *ce;
	int nr;
	unsigned int namelen;
};

typedef int (*hashmap_cmp_fn)(const void *entry, const void *entry_or_key,
		const void *keydata);


struct hashmap {
	struct hashmap_entry **table;
	hashmap_cmp_fn cmpfn;
	unsigned int size, tablesize, grow_at, shrink_at;
};

struct hashmap_iter {
	struct hashmap *map;
	struct hashmap_entry *next;
	unsigned int tablepos;
};

extern int ignore_case;

void hashmap_free(struct hashmap *map, int free_entries)
{
	if (!map || !map->table)
		return;
	if (free_entries) {
		struct hashmap_iter iter;
		struct hashmap_entry *e;
		hashmap_iter_init(map, &iter);
		while ((e = hashmap_iter_next(&iter)))
			free(e);
	}
	free(map->table);
	memset(map, 0, sizeof(*map));
}

void free_name_hash(struct index_state *istate)
{
	if (!istate->name_hash_initialized)
		return;
	istate->name_hash_initialized = 0;

	hashmap_free(&istate->name_hash, 0);
	hashmap_free(&istate->dir_hash, 1);
}
static int dir_entry_cmp(const struct dir_entry *e1,
		const struct dir_entry *e2, const char *name)
{
	return e1->namelen != e2->namelen || strncasecmp(e1->ce->name,
			name ? name : e2->ce->name, e1->namelen);
}

static struct dir_entry *find_dir_entry(struct index_state *istate,
		const char *name, unsigned int namelen)
{
	struct dir_entry key;
	hashmap_entry_init(&key, memihash(name, namelen));
	key.namelen = namelen;
	return hashmap_get(&istate->dir_hash, &key, name);
}

static struct dir_entry *hash_dir_entry(struct index_state *istate,
		struct cache_entry *ce, int namelen)
{
	struct dir_entry *dir;

	/* get length of parent directory */
	while (namelen > 0 && !is_dir_sep(ce->name[namelen - 1]))
		namelen--;
	if (namelen <= 0)
		return NULL;
	namelen--;

	/* lookup existing entry for that directory */
	dir = find_dir_entry(istate, ce->name, namelen);
	if (!dir) {
		/* not found, create it and add to hash table */
		dir = xcalloc(1, sizeof(struct dir_entry));
		hashmap_entry_init(dir, memihash(ce->name, namelen));
		dir->namelen = namelen;
		dir->ce = ce;		/* undesirable alias */
		hashmap_add(&istate->dir_hash, dir);

		/* recursively add missing parent directories */
		dir->parent = hash_dir_entry(istate, ce, namelen);
	}
	return dir;
}

struct cache_entry *index_dir_exists(struct index_state *istate, const char *name, int namelen)
{
	struct cache_entry *ce;
	struct dir_entry *dir;

	lazy_init_name_hash(istate);
	dir = find_dir_entry(istate, name, namelen);
	if (dir && dir->nr)
		return dir->ce;

	return NULL;
}

static void add_dir_entry(struct index_state *istate, struct cache_entry *ce)
{
	/* Add reference to the directory entry (and parents if 0). */
	struct dir_entry *dir = hash_dir_entry(istate, ce, ce_namelen(ce));
	while (dir && !(dir->nr++))
		dir = dir->parent;
}

static void hash_index_entry(struct index_state *istate, struct cache_entry *ce)
{
	if (ce->ce_flags & CE_HASHED)
		return;
	ce->ce_flags |= CE_HASHED;
	hashmap_entry_init(ce, memihash(ce->name, ce_namelen(ce)));
	hashmap_add(&istate->name_hash, ce);

	if (ignore_case)
		add_dir_entry(istate, ce);
}

static int cache_entry_cmp(const struct cache_entry *ce1,
		const struct cache_entry *ce2, const void *remove)
{
	/*
	 * For remove_name_hash, find the exact entry (pointer equality); for
	 * index_file_exists, find all entries with matching hash code and
	 * decide whether the entry matches in same_name.
	 */
	return remove ? !(ce1 == ce2) : 0;
}

void add_name_hash(struct index_state *istate, struct cache_entry *ce)
{
	if (istate->name_hash_initialized)
		hash_index_entry(istate, ce);
}

int add_to_index(struct index_state *istate, const char *path, struct stat *st, int flags)
{
	int size, namelen, was_same;
	mode_t st_mode = st->st_mode;
	struct cache_entry *ce, *alias;
	unsigned ce_option = CE_MATCH_IGNORE_VALID|CE_MATCH_IGNORE_SKIP_WORKTREE|CE_MATCH_RACY_IS_DIRTY;
	int verbose = flags & (ADD_CACHE_VERBOSE | ADD_CACHE_PRETEND);
	int pretend = flags & ADD_CACHE_PRETEND;
	int intent_only = flags & ADD_CACHE_INTENT;
	int add_option = (ADD_CACHE_OK_TO_ADD|ADD_CACHE_OK_TO_REPLACE|
			  (intent_only ? ADD_CACHE_NEW_ONLY : 0));

	if (!S_ISREG(st_mode) && !S_ISLNK(st_mode) && !S_ISDIR(st_mode))
		return error("%s: can only add regular files, symbolic links or git-directories", path);

	namelen = strlen(path);
	if (S_ISDIR(st_mode)) {
		while (namelen && path[namelen-1] == '/')
			namelen--;
	}
	size = cache_entry_size(namelen);
	ce = xcalloc(1, size);
	memcpy(ce->name, path, namelen);
	ce->ce_namelen = namelen;
	if (!intent_only)
		fill_stat_cache_info(ce, st);
	else
		ce->ce_flags |= CE_INTENT_TO_ADD;

	/* When core.ignorecase=true, determine if a directory of the same name but differing
	 * case already exists within the Git repository.  If it does, ensure the directory
	 * case of the file being added to the repository matches (is folded into) the existing
	 * entry's directory case.
	 */
	if (ignore_case) {
		const char *startPtr = ce->name;
		const char *ptr = startPtr;
		while (*ptr) {
			while (*ptr && *ptr != '/')
				++ptr;
			if (*ptr == '/') {
				struct cache_entry *foundce;
				++ptr;
				foundce = index_dir_exists(istate, ce->name, ptr - ce->name - 1);
				if (foundce) {
					memcpy((void *)startPtr, foundce->name + (startPtr - ce->name), ptr - startPtr);
					startPtr = ptr;
				}
			}
		}
	}

	alias = index_file_exists(istate, ce->name, ce_namelen(ce), ignore_case);
	if (alias && !ce_stage(alias) && !ie_match_stat(istate, alias, st, ce_option)) {
		/* Nothing changed, really */
		free(ce);
		if (!S_ISGITLINK(alias->ce_mode))
			ce_mark_uptodate(alias);
		alias->ce_flags |= CE_ADDED;
		return 0;
	}

	if (ignore_case && alias && different_name(ce, alias))
		ce = create_alias_ce(istate, ce, alias);
	ce->ce_flags |= CE_ADDED;

	if (pretend)
		;
	else if (add_index_entry(istate, ce, add_option))
		return error("unable to add %s to index",path);
	if (verbose && !was_same)
		printf("add '%s'\n", path);
	return 0;
}

static void set_index_entry(struct index_state *istate, int nr, struct cache_entry *ce)
{
	istate->cache[nr] = ce;
	add_name_hash(istate, ce);
}

int main(int argc, char **argv)
{
	struct index_state istate;
	struct cache_entry *ce;
	struct stat st;
	int flag;
	
	istate.cache = xcalloc(argc, sizeof(istate.cache));
	/* create cache hash map */
	for(int i = 0; i < argc; i++)
	{
		ce = malloc(sizeof(struct cache_entry)); /* allocation site */
		set_index_entry(&istate, i, ce); 
	}

	/* free hash map */
	for (int i = 0; i < argc; i++) 
		free(istate.cache[i]);

	free(istate.cache);	

	/* may find and use freed cache entry */
	add_to_index(&istate, "path", &st, flag);

	free_name_hash(&istate);
}
