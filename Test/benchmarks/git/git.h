#define STRING_LIST_INIT_NODUP { NULL, 0, 0, 0, NULL }
#define STRING_LIST_INIT_DUP   { NULL, 0, 0, 1, NULL }

#define STRBUF_MAXLINK (2*PATH_MAX)
#define ARCHIVER_WANT_COMPRESSION_LEVELS 1
#define ARCHIVER_REMOTE 2

#define PATHSPEC_FROMTOP	(1<<0)
#define PATHSPEC_MAXDEPTH	(1<<1)
#define PATHSPEC_LITERAL	(1<<2)
#define PATHSPEC_GLOB		(1<<3)
#define PATHSPEC_ICASE		(1<<4)
#define PATHSPEC_EXCLUDE	(1<<5)
#define PATHSPEC_ATTR		(1<<6)
#define PATHSPEC_ALL_MAGIC	  \
	(PATHSPEC_FROMTOP	| \
	 PATHSPEC_MAXDEPTH	| \
	 PATHSPEC_LITERAL	| \
	 PATHSPEC_GLOB		| \
	 PATHSPEC_ICASE		| \
	 PATHSPEC_EXCLUDE	| \
	 PATHSPEC_ATTR)

#define PATHSPEC_ONESTAR 1	/* the pathspec pattern satisfies GFNM_ONESTAR */
#define PATHSPEC_PREFER_CWD (1<<0) /* No args means match cwd */
#define PATHSPEC_PREFER_FULL (1<<1) /* No args means match everything */
#define PATHSPEC_MAXDEPTH_VALID (1<<2) /* max_depth field is valid */
#define PATHSPEC_SYMLINK_LEADING_PATH (1<<3)
#define PATHSPEC_PREFIX_ORIGIN (1<<4)
#define PATHSPEC_KEEP_ORDER (1<<5)
#define PATHSPEC_LITERAL_PATH (1<<6)

#define SOMETHING_CHANGED	(1 << 0) /* unclassified changes go here */
#define CE_ENTRY_CHANGED	(1 << 1)
#define CE_ENTRY_REMOVED	(1 << 2)
#define CE_ENTRY_ADDED		(1 << 3)
#define RESOLVE_UNDO_CHANGED	(1 << 4)
#define CACHE_TREE_CHANGED	(1 << 5)
#define SPLIT_INDEX_ORDERED	(1 << 6)
#define UNTRACKED_CHANGED	(1 << 7)

#define GIT_SHA1_RAWSZ 20
#define GIT_SHA1_HEXSZ (2 * GIT_SHA1_RAWSZ)

#define GIT_MAX_RAWSZ GIT_SHA1_RAWSZ
#define GIT_MAX_HEXSZ GIT_SHA1_HEXSZ

#define SEEN		(1u<<0)
#define UNINTERESTING   (1u<<1)
#define TREESAME	(1u<<2)
#define SHOWN		(1u<<3)
#define TMP_MARK	(1u<<4) /* for isolated cases; clean after use */
#define BOUNDARY	(1u<<5)
#define CHILD_SHOWN	(1u<<6)
#define ADDED		(1u<<7)	/* Parents already parsed and added? */
#define SYMMETRIC_LEFT	(1u<<8)
#define PATCHSAME	(1u<<9)
#define BOTTOM		(1u<<10)
#define TRACK_LINEAR	(1u<<26)
#define ALL_REV_FLAGS	(((1u<<11)-1) | TRACK_LINEAR)

#define DECORATE_SHORT_REFS	1
#define DECORATE_FULL_REFS	2

#define PARENT1		(1u<<16)
#define PARENT2		(1u<<17)
#define STALE		(1u<<18)
#define RESULT		(1u<<19)

#define REVISION_WALK_WALK 0
#define REVISION_WALK_NO_WALK_SORTED 1
#define REVISION_WALK_NO_WALK_UNSORTED 2

#define OBJECT_ARRAY_INIT { 0, 0, NULL }
#define TYPE_BITS   3
#define FLAG_BITS  27

#define REF_DELETING	0x02
#define REF_ISPRUNING	0x04
#define REF_HAVE_NEW	0x08
#define REF_HAVE_OLD	0x10
#define REF_NEEDS_COMMIT 0x20
#define REF_UPDATE_VIA_HEAD 0x100
#define REF_DELETED_LOOSE 0x200
#define REF_NODEREF	0x01
#define REF_FORCE_CREATE_REFLOG 0x40

#define REF_LOG_ONLY 0x80
#define REF_ISSYMREF 0x01
#define REF_ISPACKED 0x02
#define REF_ISBROKEN 0x04
#define REF_BAD_NAME 0x08
#define REF_STORE_READ		(1 << 0)
#define REF_STORE_WRITE		(1 << 1) /* can perform update operations */
#define REF_STORE_ODB		(1 << 2) /* has access to object database */
#define REF_STORE_MAIN		(1 << 3)
#define REF_STORE_ALL_CAPS	(REF_STORE_READ | \
				 REF_STORE_WRITE | \
				 REF_STORE_ODB | \
				 REF_STORE_MAIN)
#define TRANSACTION_NAME_CONFLICT -1
/* All other errors. */
#define TRANSACTION_GENERIC_ERROR -2

#  define FLEX_ARRAY /* empty */

static inline size_t
st_mult(size_t a, size_t b)
{
	return a * b;
}

static inline size_t
alloc_nr(size_t current)
{
	return (current + 16) * 3 / 2;
}

#define REALLOC_ARRAY(x, alloc) (x) = xrealloc((x), st_mult(sizeof(*(x)), (alloc)))
#define ALLOC_GROW(x, nr, alloc) \
	do { \
		if ((nr) > alloc) { \
			if (alloc_nr(alloc) < (nr)) \
				alloc = (nr); \
			else \
				alloc = alloc_nr(alloc); \
			REALLOC_ARRAY(x, alloc); \
		} \
	} while (0)
#define BUILD_ASSERT_OR_ZERO(cond) \
	(sizeof(char [1 - 2*!(cond)]) - 1)

#define SWAP(a, b) do {						\
	void *_swap_a_ptr = &(a);				\
	void *_swap_b_ptr = &(b);				\
	unsigned char _swap_buffer[sizeof(a)];			\
	memcpy(_swap_buffer, _swap_a_ptr, sizeof(a));		\
	memcpy(_swap_a_ptr, _swap_b_ptr, sizeof(a) +		\
	       BUILD_ASSERT_OR_ZERO(sizeof(a) == sizeof(b)));	\
	memcpy(_swap_b_ptr, _swap_buffer, sizeof(a));		\
} while (0)


static const unsigned all_flags = (PARENT1 | PARENT2 | STALE | RESULT);

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



#define FREE_AND_NULL(p) do { free(p); (p) = NULL; } while (0)

#define ALLOC_ARRAY(x, alloc) (x) = xmalloc(st_mult(sizeof(*(x)), (alloc)))
#define REALLOC_ARRAY(x, alloc) (x) = xrealloc((x), st_mult(sizeof(*(x)), (alloc)))

#define COPY_ARRAY(dst, src, n) copy_array((dst), (src), (n), sizeof(*(dst)) + \
	BUILD_ASSERT_OR_ZERO(sizeof(*(dst)) == sizeof(*(src))))
static inline void copy_array(void *dst, const void *src, size_t n, size_t size)
{
	if (n)
		memcpy(dst, src, st_mult(size, n));
}

#define MOVE_ARRAY(dst, src, n) move_array((dst), (src), (n), sizeof(*(dst)) + \
	BUILD_ASSERT_OR_ZERO(sizeof(*(dst)) == sizeof(*(src))))
static inline void move_array(void *dst, const void *src, size_t n, size_t size)
{
	if (n)
		memmove(dst, src, st_mult(size, n));
}

static inline char *xstrdup_or_null(const char *str)
{
	return str ? xstrdup(str) : NULL;
}

struct ref_cache{};
struct ref_store{};
struct rev_info{};
struct packed_ref_cache{};
struct lock_file{};
struct log_info{};
struct saved_parents{};
struct split_index{};
struct cache_time{};
struct hashmap_entry{};
struct untracked_cache{};
struct stat_data{};

typedef int (*hashmap_cmp_fn)(const void *entry, const void *entry_or_key,
		const void *keydata);

struct hashmap {
	struct hashmap_entry **table;
	hashmap_cmp_fn cmpfn;
	unsigned int size, tablesize, grow_at, shrink_at;
};

struct pathspec {
	int nr;
	unsigned int has_wildcard:1;
	unsigned int recursive:1;
	unsigned magic;
	int max_depth;
	struct pathspec_item {
		char *match;
		char *original;
		unsigned magic;
		int len, prefix;
		int nowildcard_len;
		int flags;
		int attr_match_nr;
		struct attr_match {
			char *value;
			enum attr_match_mode {
				MATCH_SET,
				MATCH_UNSET,
				MATCH_VALUE,
				MATCH_UNSPECIFIED
			} match_mode;
		} *attr_match;
		struct attr_check *attr_check;
	} *items;
};

typedef unsigned long long uintmax_t;
typedef uintmax_t timestamp_t;
typedef int (*compare_strings_fn)(const char *, const char *);

struct string_list_item {
	char *string;
	void *util;
};

struct string_list {
	struct string_list_item *items;
	unsigned int nr, alloc;
	unsigned int strdup_strings:1;
	compare_strings_fn cmp; /* NULL uses strcmp() */
};

struct archiver {
	const char *name;
	int (*write_archive)(const struct archiver *, struct archiver_args *);
	unsigned flags;
	void *data;
};

struct strbuf {
	size_t alloc;
	size_t len;
	char *buf;
};

struct cache_tree_sub {
	struct cache_tree *cache_tree;
	int count;		/* internally used by update_one() */
	int namelen;
	int used;
	char name[FLEX_ARRAY];
};

struct object_id {
	unsigned char hash[GIT_MAX_RAWSZ];
};

struct cache_tree {
	int entry_count; /* negative means "invalid" */
	struct object_id oid;
	int subtree_nr;
	int subtree_alloc;
	struct cache_tree_sub **down;
};

struct object {
	unsigned parsed : 1;
	unsigned type : TYPE_BITS;
	unsigned flags : FLAG_BITS;
	struct object_id oid;
};

struct tree {
	struct object object;
	void *buffer;
	unsigned long size;
};

struct cache_entry {
	struct hashmap_entry ent;
	struct stat_data ce_stat_data;
	unsigned int ce_mode;
	unsigned int ce_flags;
	unsigned int ce_namelen;
	unsigned int index;	/* for link extension */
	struct object_id oid;
	char name[FLEX_ARRAY]; /* more */
};

struct index_state {
	struct cache_entry **cache;
	unsigned int version;
	unsigned int cache_nr, cache_alloc, cache_changed;
	struct string_list *resolve_undo;
	struct cache_tree *cache_tree;
	struct split_index *split_index;
	struct cache_time timestamp;
	unsigned name_hash_initialized : 1,
		 initialized : 1;
	struct hashmap name_hash;
	struct hashmap dir_hash;
	unsigned char sha1[20];
	struct untracked_cache *untracked;
};

struct commit_list {
	struct commit *item;
	struct commit_list *next;
};

struct commit {
	struct object object;
	void *util;
	unsigned int index;
	timestamp_t date;
	struct commit_list *parents;
	struct tree *tree;
};

struct commit_dist {
	struct commit *commit;
	int distance;
};
struct name_decoration {
	struct name_decoration *next;
	int type;
	char name[FLEX_ARRAY];
};

enum decoration_type {
	DECORATION_NONE = 0,
	DECORATION_REF_LOCAL,
	DECORATION_REF_REMOTE,
	DECORATION_REF_TAG,
	DECORATION_REF_STASH,
	DECORATION_REF_HEAD,
	DECORATION_GRAFTED,
};

struct rev_cmdline_info {
	unsigned int nr;
	unsigned int alloc;
	struct rev_cmdline_entry {
		struct object *item;
		const char *name;
		enum {
			REV_CMD_REF,
			REV_CMD_PARENTS_ONLY,
			REV_CMD_LEFT,
			REV_CMD_RIGHT,
			REV_CMD_MERGE_BASE,
			REV_CMD_REV
		} whence;
		unsigned flags;
	} *rev;
};

struct object_list {
	struct object *item;
	struct object_list *next;
};

struct object_array {
	unsigned int nr;
	unsigned int alloc;
	struct object_array_entry {
		struct object *item;
		char *name;
		char *path;
		unsigned mode;
	} *objects;
};

struct merge_parents {
	int alloc, nr;
	struct merge_parent {
		struct object_id given;
		struct object_id commit;
		unsigned char used;
	} *item;
};


extern struct index_state the_index;
extern char strbuf_slopbuf[];
#define STRBUF_INIT  { 0, 0, strbuf_slopbuf }
extern int save_commit_buffer;
extern const char *commit_type;

struct archiver_args {
	const char *base;
	size_t baselen;
	struct tree *tree;
	const unsigned char *commit_sha1;
	const struct commit *commit;
	timestamp_t time;
	struct pathspec pathspec;
	unsigned int verbose : 1;
	unsigned int worktree_attributes : 1;
	unsigned int convert : 1;
	int compression_level;
};

static inline void strbuf_swap(struct strbuf *a, struct strbuf *b)
{
	SWAP(*a, *b);
}

void *xrealloc(void *ptr, size_t size)
{
	void *ret;

	ret = realloc(ptr, size);
	if (!ret && !size)
		ret = realloc(ptr, 1);
	if (!ret) {
		ret = realloc(ptr, size);
		if (!ret && !size)
			ret = realloc(ptr, 1);
		if (!ret)
			die("Out of memory, realloc failed%s", "");
	}
	return ret;
}

/* Project API declarations retained by the reduced benchmark extracts. */
struct active_request_slot;
struct branch;
struct dir_entry;
struct files_ref_store;
struct hash_list;
struct pretty_print_context;
struct ref;

void strbuf_addch(struct strbuf *sb, int character);
void strbuf_init(struct strbuf *sb, size_t hint);
void strbuf_grow(struct strbuf *sb, size_t extra);
void strbuf_add(struct strbuf *sb, const void *data, size_t len);
void strbuf_reset(struct strbuf *sb);
void strbuf_release(struct strbuf *sb);
void strbuf_rtrim(struct strbuf *sb);
void strbuf_addf();
size_t strbuf_avail(const struct strbuf *sb);
char *strbuf_detach(struct strbuf *sb, size_t *length);
int strbuf_getline_nul(struct strbuf *sb, FILE *stream);
ssize_t strbuf_read(struct strbuf *sb, int fd, size_t hint);

int parse_commit(struct commit *commit);
struct commit_list *paint_down_to_common(struct commit *one, int count,
					 struct commit **others);
void clear_commit_marks(struct commit *commit, unsigned int flags);
void clear_commit_marks_many(int count, struct commit **commits,
			     unsigned int flags);
void free_commit_list(struct commit_list *list);
struct commit *pop_commit(struct commit_list **list);
int commit_list_count(const struct commit_list *list);
struct object *parse_object(const struct object_id *oid);
struct object *peel_to_type();
struct commit *lookup_commit(const struct object_id *oid);
struct commit *get_commit_reference(const char *name);
struct commit_list *reduce_heads(struct commit_list *heads);
int tree_is_complete(const struct object_id *oid);

int get_oid_hex(const char *hex, struct object_id *oid);
int get_oid(const char *name, struct object_id *oid);
int get_sha1_hex(const char *hex, unsigned char *sha1);
int get_sha1(const char *name, unsigned char *sha1);
int oidcmp(const struct object_id *left, const struct object_id *right);
const char *oid_to_hex(const struct object_id *oid);
char *sha1_to_hex(const unsigned char *sha1);
int is_null_oid(const struct object_id *oid);
int is_null_sha1(const unsigned char *sha1);
void hashclr(unsigned char *sha1);
void hashcpy(unsigned char *destination, const unsigned char *source);

struct active_request_slot *get_active_slot(void);
int curl_easy_setopt();
int start_active_slot(struct active_request_slot *slot);
int run_active_slot(struct active_request_slot *slot);

int prefixcmp(const char *text, const char *prefix);
uintmax_t strtoumax(const char *text, char **end, int base);
int store_object();
struct branch *lookup_branch(const char *name);
struct branch *new_branch(const char *name);
char *parse_ident(const char *text);
void cmd_from(struct branch *branch);
struct hash_list *cmd_merge(unsigned int *count);
void unload_one_branch(void);
void load_branch(struct branch *branch);
void file_change_m(struct branch *branch);
void file_change_d(struct branch *branch);
void file_change_cr(struct branch *branch, int rename);
void file_change_deleteall(struct branch *branch);
void store_tree(void *tree);
void *pool_alloc(size_t size);
void cmd_new_tag(void);

struct ref *get_local_heads(void);
struct tree *parse_tree_indirect(const struct object_id *oid);
int prime_cache_tree(struct index_state *istate, struct tree *tree);
struct string_list_item *string_list_lookup(struct string_list *list,
					     const char *string);

int error(const char *format, ...);
int error_errno(const char *format, ...);
int starts_with(const char *text, const char *prefix);
char *xgetcwd(void);
void *xmemdupz(const void *data, size_t len);
const char *real_path_internal(const char *path, int die_on_error);

int verify_path(const char *path);
int remove_file_from_cache(const char *path);
int add_cacheinfo();
int get_entry_index(struct string_list *list, const char *string,
		    int *exact_match);
int has_symlink_leading_path(const char *path, int length);
void init_pathspec_item();
char *common_prefix(const struct pathspec *pathspec);
int overlay_tree_on_index();
int ce_skip_worktree(const struct cache_entry *entry);
int report_path_error();
int read_index_preload();
int interactive_add();
int read_index_from();
const char *get_lock_file_path(const struct lock_file *lock);
int add_files_to_cache();
void rollback_lock_file(struct lock_file *lock);
char *get_index_file(void);
void discard_index(struct index_state *istate);

int decode_tree_entry();
void entry_clear();
void entry_extract();
int read_cache_unmerged(void);

int weight(struct commit_list *entry);
void add_name_decoration();
int count_interesting_parents(struct commit *commit);
void weight_set(struct commit_list *entry, int weight_value);
void show_list();
int count_distance(struct commit_list *entry);
void clear_distance(struct commit_list *list);
int halfway(struct commit_list *entry, int count);
void add_merge_parent();

int ce_stage(const struct cache_entry *entry);
void diff_free_filepair(void *pair);
int git_merge_trees();
int unmerged_index(struct index_state *istate);
void get_files_dirs();
void record_df_conflict_files();
int process_renames();
int process_entry();

int unsigned_add_overflows(size_t left, size_t right);
struct files_ref_store *files_downcast();
void files_ref_path();
int resolve_packed_ref();
int check_refname_format(const char *refname, int flags);
void unlock_ref(void *lock);
int split_head_update();
const char *original_update_refname();
int refs_read_ref_full();
int check_old_oid();
int split_symref_update();
int write_ref_to_lockfile();
int close_ref(void *lock);

int write_entry();
void create_directories();
struct cache_entry *make_cache_entry();
const char *get_git_work_tree(void);
int xsnprintf(char *buffer, size_t size, const char *format, ...);
int use_wt_file();

int online_cpus(void);
void trace_printf(const char *format, ...);

void setup_git_directory_gently(int *nongit_ok);
char *alias_lookup(const char *alias);
void commit_pager_choice(void);
int run_command_v_opt();
int split_cmdline(char *command, const char ***argv);
int handle_options();
void trace_argv_printf();

struct hashmap_iter;
void hashmap_iter_init(struct hashmap *map, struct hashmap_iter *iter);
struct hashmap_entry *hashmap_iter_next(struct hashmap_iter *iter);
void hashmap_entry_init(void *entry, unsigned int hash);
unsigned int memihash(const void *data, size_t length);
void *hashmap_get();
void hashmap_add(struct hashmap *map, void *entry);
void lazy_init_name_hash(struct index_state *istate);
unsigned int ce_namelen(const struct cache_entry *entry);
size_t cache_entry_size(size_t name_length);
void fill_stat_cache_info(struct cache_entry *entry, const struct stat *st);
struct cache_entry *index_file_exists();
int ie_match_stat();
void ce_mark_uptodate(struct cache_entry *entry);
int different_name(const struct cache_entry *left,
		   const struct cache_entry *right);
struct cache_entry *create_alias_ce();
int add_index_entry();

ssize_t read_in_full(int fd, void *buffer, size_t count);
int is_absolute_path(const char *path);
void update_linked_gitdir(const char *gitfile, const char *gitdir);
int cache_name_pos(const char *name, int name_length);
