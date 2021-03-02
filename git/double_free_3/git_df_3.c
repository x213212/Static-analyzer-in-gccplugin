/*
    buggy parent : d90a7fd
    commit id : c2e6b6d0d1e97c5bc2db24388e247bef62faf917
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../git.h"


#define strbuf_reset(sb)  strbuf_setlen(sb, 0)
#define PACK_ID_BITS 16
#define MAX_PACK_ID ((1<<PACK_ID_BITS)-1)

struct object_entry
{
	struct object_entry *next;
	int offset;
	unsigned type : TYPE_BITS;
	unsigned pack_id : PACK_ID_BITS;
	unsigned char sha1[20];
};

struct object_entry_pool
{
	struct object_entry_pool *next_pool;
	struct object_entry *next_free;
	struct object_entry *end;
	struct object_entry entries[FLEX_ARRAY]; /* more */
};

struct mark_set
{
	union {
		struct object_entry *marked[1024];
		struct mark_set *sets[1024];
	} data;
	unsigned int shift;
};

struct last_object
{
	struct strbuf data;
	int offset;
	unsigned int depth;
	unsigned no_swap : 1;
};

struct mem_pool
{
	struct mem_pool *next_pool;
	char *next_free;
	char *end;
	char space[FLEX_ARRAY]; /* more */
};

struct atom_str
{
	struct atom_str *next_atom;
	unsigned short str_len;
	char str_dat[FLEX_ARRAY]; /* more */
};

struct tree_content;
struct tree_entry
{
	struct tree_content *tree;
	struct atom_str* name;
	struct tree_entry_ms
	{
		int mode;
		unsigned char sha1[20];
	} versions[2];
};

struct tree_content
{
	unsigned int entry_capacity; /* must match avail_tree_content */
	unsigned int entry_count;
	unsigned int delta_depth;
	struct tree_entry *entries[FLEX_ARRAY]; /* more */
};

struct avail_tree_content
{
	unsigned int entry_capacity; /* must match tree_content */
	struct avail_tree_content *next_avail;
};

struct branch
{
	struct branch *table_next_branch;
	struct branch *active_next_branch;
	const char *name;
	struct tree_entry branch_tree;
	int last_commit;
	unsigned active : 1;
	unsigned pack_id : PACK_ID_BITS;
	unsigned char sha1[20];
};

struct tag
{
	struct tag *next_tag;
	const char *name;
	unsigned int pack_id;
	unsigned char sha1[20];
};

struct hash_list
{
	struct hash_list *next;
	unsigned char sha1[20];
};

typedef enum {
	WHENSPEC_RAW = 1,
	WHENSPEC_RFC2822,
	WHENSPEC_NOW,
} whenspec_type;

struct recent_command
{
	struct recent_command *prev;
	struct recent_command *next;
	char *buf;
};

/* Configured limits on output */
static unsigned long max_depth = 10;
static off_t max_packsize = (1LL << 32) - 1;
static int force_update;

/* Stats and misc. counters */
static uintmax_t alloc_count;
static uintmax_t marks_set_count;
static uintmax_t object_count_by_type[1 << TYPE_BITS];
static uintmax_t duplicate_count_by_type[1 << TYPE_BITS];
static uintmax_t delta_count_by_type[1 << TYPE_BITS];
static unsigned long object_count;
static unsigned long branch_count;
static unsigned long branch_load_count;
static int failure;
static FILE *pack_edges;

/* Memory pools */
static size_t mem_pool_alloc = 2*1024*1024 - sizeof(struct mem_pool);
static size_t total_allocd;
static struct mem_pool *mem_pool;

/* Atom management */
static unsigned int atom_table_sz = 4451;
static unsigned int atom_cnt;
static struct atom_str **atom_table;

/* The .pack file being generated */
static unsigned int pack_id;
static struct packed_git *pack_data;
static struct packed_git **all_packs;
static unsigned long pack_size;

/* Table of objects we've written. */
static unsigned int object_entry_alloc = 5000;
static struct object_entry_pool *blocks;
static struct object_entry *object_table[1 << 16];
static struct mark_set *marks;
static const char* mark_file;

/* Our last blob */
static struct last_object last_blob = { STRBUF_INIT, 0, 0, 0 };

/* Tree management */
static unsigned int tree_entry_alloc = 1000;
static void *avail_tree_entry;
static unsigned int avail_tree_table_sz = 100;
static struct avail_tree_content **avail_tree_table;
static struct strbuf old_tree = STRBUF_INIT;
static struct strbuf new_tree = STRBUF_INIT;

/* Branch data */
static unsigned long max_active_branches = 5;
static unsigned long cur_active_branches;
static unsigned long branch_table_sz = 1039;
static struct branch **branch_table;
static struct branch *active_branches;

/* Tag data */
static struct tag *first_tag;
static struct tag *last_tag;

/* Input stream parsing */
static whenspec_type whenspec = WHENSPEC_RAW;
static struct strbuf command_buf = STRBUF_INIT;
static int unread_command_buf;
static struct recent_command cmd_hist = {&cmd_hist, &cmd_hist, NULL};
static struct recent_command *cmd_tail = &cmd_hist;
static struct recent_command *rc_free;
static unsigned int cmd_save = 100;
static uintmax_t next_mark;
static struct strbuf new_data = STRBUF_INIT;

void strbuf_grow(struct strbuf *sb, size_t extra)
{
	if (sb->len + extra + 1 <= sb->len)
		die("you want to use way too much memory%s", "");
	if (!sb->alloc)
		sb->buf = NULL;
	ALLOC_GROW(sb->buf, sb->len + extra + 1, sb->alloc);
}

void strbuf_init(struct strbuf *sb, size_t hint)
{
	sb->alloc = sb->len = 0;
	sb->buf = strbuf_slopbuf;
	if (hint)
		strbuf_grow(sb, hint);
}

char *strbuf_detach(struct strbuf *sb, size_t *sz)
{
	char *res = sb->alloc ? sb->buf : NULL;
	if (sz)
		*sz = sb->len;
	strbuf_init(sb, 0);
	return res;
}

void strbuf_add(struct strbuf *sb, const void *data, size_t len)
{
	strbuf_grow(sb, len);
	memcpy(sb->buf + sb->len, data, len);
	strbuf_setlen(sb, sb->len + len);
}

static inline void strbuf_setlen(struct strbuf *sb, size_t len) {
	if (!sb->alloc)
		strbuf_grow(sb, 0);
	assert(len < sb->alloc);
	sb->len = len;
	sb->buf[len] = '\0';
}

static inline void strbuf_addstr(struct strbuf *sb, const char *s) {
	strbuf_add(sb, s, strlen(s));
}
static inline void strbuf_addbuf(struct strbuf *sb, struct strbuf *sb2) {
	strbuf_add(sb, sb2->buf, sb2->len);
}

int strbuf_getline(struct strbuf *sb, FILE *fp, int term)
{
	int ch;

	strbuf_grow(sb, 0);
	if (feof(fp))
		return EOF;

	strbuf_reset(sb);
	while ((ch = fgetc(fp)) != EOF) {
		if (ch == term)
			break;
		strbuf_grow(sb, 1);
		sb->buf[sb->len++] = ch;
	}
	if (ch == EOF && sb->len == 0)
		return EOF;

	sb->buf[sb->len] = '\0';
	return 0;
}

static int read_next_command(void)
{
	static int stdin_eof = 0;

	if (stdin_eof) {
		unread_command_buf = 0;
		return EOF;
	}

	do {
		if (unread_command_buf) {
			unread_command_buf = 0;
		} else {
			struct recent_command *rc;

			strbuf_detach(&command_buf, NULL);
			stdin_eof = strbuf_getline(&command_buf, stdin, '\n'); /* allocation site */
			if (stdin_eof)
				return EOF;

			rc = rc_free;
			if (rc)
				rc_free = rc->next;
			else {
				rc = cmd_hist.next;
				cmd_hist.next = rc->next;
				cmd_hist.next->prev = &cmd_hist;
				free(rc->buf);
			}

			rc->buf = command_buf.buf;
			rc->prev = cmd_tail;
			rc->next = cmd_hist.prev;
			rc->prev->next = rc;
			cmd_tail = rc;
		}
	} while (command_buf.buf[0] == '#');

	return 0;
}

static void cmd_mark(void)
{
	if (!prefixcmp(command_buf.buf, "mark :")) {
		next_mark = strtoumax(command_buf.buf + 6, NULL, 10);
		read_next_command();
	}
	else
		next_mark = 0;
}

static void cmd_new_blob(void)
{
	static struct strbuf buf = STRBUF_INIT;

	read_next_command();
	cmd_mark();
	cmd_data(&buf);
	store_object(OBJ_BLOB, &buf, &last_blob, NULL, next_mark);
}

static void cmd_data(struct strbuf *sb)
{
	strbuf_reset(sb);

	if (prefixcmp(command_buf.buf, "data "))
		die("Expected 'data n' command, found: %s", command_buf.buf);

	if (!prefixcmp(command_buf.buf + 5, "<<")) {
		char *term = xstrdup(command_buf.buf + 5 + 2);
		size_t term_len = command_buf.len - 5 - 2;

		for (;;) {
			if (strbuf_getline(&command_buf, stdin, '\n') == EOF)           /* command_buf.buf may be freed */
				die("EOF in data (terminator '%s' not found)", term);
			if (term_len == command_buf.len
				&& !strcmp(term, command_buf.buf))
				break;
			strbuf_addbuf(sb, &command_buf);
			strbuf_addch(sb, '\n');
		}
		free(term);
	}
	else {
		size_t n = 0, length;
		length = strtoul(command_buf.buf + 5, NULL, 10);
	}
}

static void cmd_new_commit(void)
{
	static struct strbuf msg = STRBUF_INIT;
	struct branch *b;
	char *sp;
	char *author = NULL;
	char *committer = NULL;
	struct hash_list *merge_list = NULL;
	unsigned int merge_count;

	/* Obtain the branch name from the rest of our command */
	sp = strchr(command_buf.buf, ' ') + 1;
	b = lookup_branch(sp);
	if (!b)
		b = new_branch(sp);

	read_next_command();
	cmd_mark();
	if (!prefixcmp(command_buf.buf, "author ")) {
		author = parse_ident(command_buf.buf + 7);
		read_next_command();
	}
	if (!prefixcmp(command_buf.buf, "committer ")) {
		committer = parse_ident(command_buf.buf + 10);
		read_next_command();
	}
	if (!committer)
		die("Expected committer but didn't get one%s", "");
	cmd_data(&msg);
	read_next_command(); /* double-free */
	cmd_from(b);
	merge_list = cmd_merge(&merge_count);

	/* ensure the branch is active/loaded */
	if (!b->branch_tree.tree || !max_active_branches) {
		unload_one_branch();
		load_branch(b);
	}

	/* file_change* */
	while (command_buf.len > 0) {
		if (!prefixcmp(command_buf.buf, "M "))
			file_change_m(b);
		else if (!prefixcmp(command_buf.buf, "D "))
			file_change_d(b);
		else if (!prefixcmp(command_buf.buf, "R "))
			file_change_cr(b, 1);
		else if (!prefixcmp(command_buf.buf, "C "))
			file_change_cr(b, 0);
		else if (!strcmp("deleteall", command_buf.buf))
			file_change_deleteall(b);
		else {
			unread_command_buf = 1;
			break;
		}
		if (read_next_command() == EOF)
			break;
	}

	/* build the tree and the commit */
	store_tree(&b->branch_tree);
	hashcpy(b->branch_tree.versions[0].sha1,
		b->branch_tree.versions[1].sha1);

	strbuf_reset(&new_data);
	strbuf_addf(&new_data, "tree %s\n",
		sha1_to_hex(b->branch_tree.versions[1].sha1));
	if (!is_null_sha1(b->sha1))
		strbuf_addf(&new_data, "parent %s\n", sha1_to_hex(b->sha1));
	while (merge_list) {
		struct hash_list *next = merge_list->next;
		strbuf_addf(&new_data, "parent %s\n", sha1_to_hex(merge_list->sha1));
		free(merge_list);
		merge_list = next;
	}
	strbuf_addf(&new_data,
		"author %s\n"
		"committer %s\n"
		"\n",
		author ? author : committer, committer);
	strbuf_addbuf(&new_data, &msg);
	free(author);
	free(committer);

	if (!store_object(OBJ_COMMIT, &new_data, NULL, b->sha1, next_mark))
		b->pack_id = pack_id;
	b->last_commit = object_count_by_type[OBJ_COMMIT];
}


int main(int argc, const char **argv)
{
	unsigned int i, show_stats = 1;

	strbuf_init(&command_buf, 0);

	rc_free = pool_alloc(cmd_save * sizeof(*rc_free));
	for (i = 0; i < (cmd_save - 1); i++)
		rc_free[i].next = &rc_free[i + 1];
	rc_free[cmd_save - 1].next = NULL;

	while (read_next_command() != EOF) {
		if (!strcmp("blob", command_buf.buf))
			cmd_new_blob();
		else if (!prefixcmp(command_buf.buf, "commit "))
			cmd_new_commit();
		else if (!prefixcmp(command_buf.buf, "tag "))
			cmd_new_tag();
		else
			die("Unsupported command: %s", command_buf.buf);
	}
	return failure ? 1 : 0;
}


