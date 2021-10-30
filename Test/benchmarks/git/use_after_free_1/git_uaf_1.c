/*
    buggy parent : 0730dd4
    commit id : 882add136fa8319832ef373b8797ef58edb80efc
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../git.h"

struct checkout {
	struct index_state *istate;
	const char *base_dir;
	int base_dir_len;
	unsigned force:1,
		 quiet:1,
		 not_new:1,
		 refresh_cache:1;
};
#define CHECKOUT_INIT { NULL, "" }


char strbuf_slopbuf[1];

void strbuf_init(struct strbuf *sb, size_t hint)
{
	sb->alloc = sb->len = 0;
	sb->buf = strbuf_slopbuf;
	if (hint)
		strbuf_grow(sb, hint);
}

void strbuf_release(struct strbuf *sb)
{
	if (sb->alloc) {
		free(sb->buf);
		strbuf_init(sb, 0);
	}
}

static inline void strbuf_setlen(struct strbuf *sb, size_t len)
{
	if (len > (sb->alloc ? sb->alloc - 1 : 0))
		die("BUG: strbuf_setlen() beyond buffer%s", "");
	sb->len = len;
	sb->buf[len] = '\0';
}

char *strbuf_detach(struct strbuf *sb, size_t *sz)
{
	char *res;
	strbuf_grow(sb, 0);
	res = sb->buf;
	if (sz)
		*sz = sb->len;
	strbuf_init(sb, 0);
	return res;
}

void strbuf_attach(struct strbuf *sb, void *buf, size_t len, size_t alloc)
{
	strbuf_release(sb);
	sb->buf   = buf;
	sb->len   = len;
	sb->alloc = alloc;
	strbuf_grow(sb, 0);
	sb->buf[sb->len] = '\0';
}

void strbuf_grow(struct strbuf *sb, size_t extra)
{
	int new_buf = !sb->alloc;
	if (unsigned_add_overflows(extra, 1) ||
	    unsigned_add_overflows(sb->len, extra + 1))
		die("you want to use way too much memory%s", "");
	if (new_buf)
		sb->buf = NULL;
	ALLOC_GROW(sb->buf, sb->len + extra + 1, sb->alloc);	/* allocation site */
	if (new_buf)
		sb->buf[0] = '\0';
}

void strbuf_vaddf(struct strbuf *sb, const char *fmt, const char *ap)
{
	int len;
    const char *cp = ap;
	if (!strbuf_avail(sb))
		strbuf_grow(sb, 64);
	strbuf_setlen(sb, sb->len + len);
}

void strbuf_addf(struct strbuf *sb, const char *fmt, const char *ap)
{
	strbuf_vaddf(sb, fmt, ap);
}

static void add_path(struct strbuf *buf, size_t base_len, const char *path)
{
	strbuf_setlen(buf, base_len);
	if (buf->len && buf->buf[buf->len - 1] != '/')
		strbuf_addch(buf, '/');
	strbuf_addstr(buf, path);
}

int checkout_entry(struct cache_entry *ce,
		   const struct checkout *state, char *topath)
{
	static struct strbuf path = STRBUF_INIT;
	struct stat st;

	if (topath)
		return write_entry(ce, topath, state, 1);

	strbuf_reset(&path);
	strbuf_add(&path, state->base_dir, state->base_dir_len);

	create_directories(path.buf, path.len, state);
	return write_entry(ce, path.buf, state, 0);
}
static int checkout_path(unsigned mode, struct object_id *oid,
			 const char *path, const struct checkout *state)
{
	struct cache_entry *ce;
	int ret;

	ce = make_cache_entry(mode, oid->hash, path, 0, 0);
	ret = checkout_entry(ce, state, NULL);

	free(ce);
	return ret;
}


static int run_dir_diff(const char *extcmd, int symlinks, const char *prefix,
			int argc, const char **argv)
{
	char tmpdir[PATH_MAX];
	struct strbuf info = STRBUF_INIT, lpath = STRBUF_INIT;
	struct strbuf rpath = STRBUF_INIT, buf = STRBUF_INIT;
	struct strbuf ldir = STRBUF_INIT, rdir = STRBUF_INIT;
	struct strbuf wtdir = STRBUF_INIT;
	size_t ldir_len, rdir_len, wtdir_len;
	const char *workdir, *tmp;
	int ret = 0, i;
	FILE *fp;
	struct hashmap working_tree_dups, submodules, symlinks2;
	struct hashmap_iter iter;
	struct pair_entry *entry;
	struct index_state wtindex;
	struct checkout lstate, rstate;
	const char *helper_argv[] = { "difftool--helper", NULL, NULL, NULL };
	struct hashmap wt_modified, tmp_modified;
	int indices_loaded = 0;

	workdir = get_git_work_tree();

	/* Setup temp directories */
	tmp = getenv("TMPDIR");
	xsnprintf(tmpdir, sizeof(tmpdir), "%s/git-difftool.XXXXXX", tmp ? tmp : "/tmp");
	if (!mkdtemp(tmpdir))
		return error("could not create '%s'", tmpdir);
	strbuf_addf(&ldir, "%s/left/", tmpdir);
	strbuf_addf(&rdir, "%s/right/", tmpdir);
	strbuf_addstr(&wtdir, workdir);
	if (!wtdir.len || !is_dir_sep(wtdir.buf[wtdir.len - 1]))
		strbuf_addch(&wtdir, '/');
	mkdir(ldir.buf, 0700);
	mkdir(rdir.buf, 0700);

	memset(&wtindex, 0, sizeof(wtindex));

	memset(&lstate, 0, sizeof(lstate));
	lstate.base_dir = ldir.buf;                             /* can be freed when ldir.buf is realloced */
	lstate.base_dir_len = ldir.len;
	lstate.force = 1;
	memset(&rstate, 0, sizeof(rstate));
	rstate.base_dir = rdir.buf;								/* can be freed when rdir.buf is realloced */
	rstate.base_dir_len = rdir.len;
	rstate.force = 1;

	ldir_len = ldir.len;
	rdir_len = rdir.len;
	wtdir_len = wtdir.len;

	/* Build index info for left and right sides of the diff */
	i = 0;
	while (!strbuf_getline_nul(&info, fp)) {
		int lmode, rmode;
		struct object_id loid, roid;
		char status;
		const char *src_path, *dst_path;

		if (lmode && status != 'C') {
			if (checkout_path(lmode, &loid, src_path, &lstate))
				return error("could not write '%s'", src_path);
		}

		if (rmode && !S_ISLNK(rmode)) {
			struct working_tree_entry *entry;

			if (!use_wt_file(workdir, dst_path, &roid)) {
				if (checkout_path(rmode, &roid, dst_path, &rstate))
					return error("could not write '%s'",
						     dst_path);
			} else if (!is_null_oid(&roid)) {
				/*
				 * Changes in the working tree need special
				 * treatment since they are not part of the
				 * index.
				 */
				struct cache_entry *ce2 =
					make_cache_entry(rmode, roid.hash,
							 dst_path, 0, 0);

				add_path(&rdir, rdir_len, dst_path);
				if (symlinks) {
					if (symlink(wtdir.buf, rdir.buf)) {
						ret = error_errno("could not symlink '%s' to '%s'", wtdir.buf, rdir.buf);
						goto finish;
					}
				} 
			}	
			else {
				goto finish;
			
			}
		}
	}

finish:
	strbuf_release(&ldir);
	strbuf_release(&rdir);
	strbuf_release(&wtdir);
	strbuf_release(&buf);

	return ret;
}

int main(int argc, char **argv)
{
    int symlinks;
    run_dir_diff("extcmd", symlinks, "prefix", argc, argv);
    return 0;
}


