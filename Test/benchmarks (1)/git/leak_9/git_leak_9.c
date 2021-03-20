/*
    buggy parent : c299468
    commit id : 851e1fbd01250f56a6e479e1addada220a56e1f7
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>

#include "../stdio.h"
#include "../git.h"

struct ref_lock {
	char *ref_name;
	struct lock_file *lk;
	struct object_id old_oid;
};

struct files_ref_store {
	struct ref_store base;
	unsigned int store_flags;

	char *gitdir;
	char *gitcommondir;
	char *packed_refs_path;

	struct ref_cache *loose;
	struct packed_ref_cache *packed;

	/*
	 * Lock used for the "packed-refs" file. Note that this (and
	 * thus the enclosing `files_ref_store`) must not be freed.
	 */
	struct lock_file packed_refs_lock;
};

struct ref_update {

	/*
	 * If (flags & REF_HAVE_NEW), set the reference to this value:
	 */
	struct object_id new_oid;

	/*
	 * If (flags & REF_HAVE_OLD), check that the reference
	 * previously had this value:
	 */
	struct object_id old_oid;

	/*
	 * One or more of REF_HAVE_NEW, REF_HAVE_OLD, REF_NODEREF,
	 * REF_DELETING, REF_ISPRUNING, REF_LOG_ONLY,
	 * REF_UPDATE_VIA_HEAD, REF_NEEDS_COMMIT, and
	 * REF_DELETED_LOOSE:
	 */
	unsigned int flags;

	void *backend_data;
	unsigned int type;
	char *msg;

	/*
	 * If this ref_update was split off of a symref update via
	 * split_symref_update(), then this member points at that
	 * update. This is used for two purposes:
	 * 1. When reporting errors, we report the refname under which
	 *    the update was originally requested.
	 * 2. When we read the old value of this reference, we
	 *    propagate it back to its parent update for recording in
	 *    the latter's reflog.
	 */
	struct ref_update *parent_update;

	const char refname[FLEX_ARRAY];
};

enum ref_transaction_state {
	REF_TRANSACTION_OPEN     = 0,
	REF_TRANSACTION_PREPARED = 1,
	REF_TRANSACTION_CLOSED   = 2
};

/*
 * Data structure for holding a reference transaction, which can
 * consist of checks and updates to multiple references, carried out
 * as atomically as possible.  This structure is opaque to callers.
 */
struct ref_transaction {
	struct ref_store *ref_store;
	struct ref_update **updates;
	size_t alloc;
	size_t nr;
	enum ref_transaction_state state;
};



void strbuf_grow(struct strbuf *sb, size_t extra)
{
	int new_buf = !sb->alloc;
	if (unsigned_add_overflows(extra, 1) ||
	    unsigned_add_overflows(sb->len, extra + 1))
		die("you want to use way too much memory%s", "");
	if (new_buf)
		sb->buf = NULL;
	ALLOC_GROW(sb->buf, sb->len + extra + 1, sb->alloc); /* allocation site */
	if (new_buf)
		sb->buf[0] = '\0';
}


void strbuf_release(struct strbuf *sb)
{
	if (sb->alloc) {
		free(sb->buf);
		strbuf_init(sb, 0);
	}
}

int strbuf_readlink(struct strbuf *sb, const char *path, size_t hint)
{
	size_t oldalloc = sb->alloc;

	if (hint < 32)
		hint = 32;

	while (hint < STRBUF_MAXLINK) {
		int len;

		strbuf_grow(sb, hint);
		len = readlink(path, sb->buf, hint);
		if (len < 0) {
			if (errno != ERANGE)
				break;
		} else if (len < hint) {
			strbuf_setlen(sb, len);
			return 0;
		}

		/* .. the buffer was too small - try again */
		hint *= 2;
	}
	if (oldalloc == 0)
		strbuf_release(sb);
	return -1;
}

static int files_read_raw_ref(struct ref_store *ref_store,
			      const char *refname, unsigned char *sha1,
			      struct strbuf *referent, unsigned int *type)
{
	struct files_ref_store *refs =
		files_downcast(ref_store, REF_STORE_READ, "read_raw_ref");
	struct strbuf sb_contents = STRBUF_INIT;
	struct strbuf sb_path = STRBUF_INIT;
	const char *path;
	const char *buf;
	int fd;
	int ret = -1;
	int save_errno;
	int remaining_retries = 3;
    struct stat st;
	*type = 0;
	strbuf_reset(&sb_path);

	files_ref_path(refs, &sb_path, refname);

	path = sb_path.buf;

stat_ref:
	/*
	 * We might have to loop back here to avoid a race
	 * condition: first we lstat() the file, then we try
	 * to read it as a link or as a file.  But if somebody
	 * changes the type of the file (file <-> directory
	 * <-> symlink) between the lstat() and reading, then
	 * we don't want to report that as an error but rather
	 * try again starting with the lstat().
	 *
	 * We'll keep a count of the retries, though, just to avoid
	 * any confusing situation sending us into an infinite loop.
	 */

	if (remaining_retries-- <= 0)
		goto out;

	if (lstat(path, &st) < 0) {
		if (errno != ENOENT)
			goto out;
		if (resolve_packed_ref(refs, refname, sha1, type)) {
			errno = ENOENT;
			goto out;
		}
		ret = 0;
		goto out;
	}

	/* Follow "normalized" - ie "refs/.." symlinks by hand */
    strbuf_reset(&sb_contents);
    if (strbuf_readlink(&sb_contents, path, 0) < 0) {
        if (errno == ENOENT || errno == EINVAL)
            /* inconsistent with lstat; retry */
            goto stat_ref;
      else
           goto out;
    }
    if (starts_with(sb_contents.buf, "refs/") &&
       !check_refname_format(sb_contents.buf, 0)) {
      strbuf_swap(&sb_contents, referent);
      *type |= REF_ISSYMREF;
     ret = 0;
     goto out;
    }
 
	strbuf_reset(&sb_contents);
	if (strbuf_read(&sb_contents, fd, 256) < 0) {
		int save_errno = errno;
		close(fd);
		errno = save_errno;
		goto out;
	}
	strbuf_rtrim(&sb_contents);
	buf = sb_contents.buf;
	if (starts_with(buf, "ref:")) {
		buf += 4;
		while (isspace(*buf))
			buf++;

		strbuf_reset(referent);
		strbuf_addstr(referent, buf);
		*type |= REF_ISSYMREF;
		ret = 0;
		goto out;
	}

	/*
	 * Please note that FETCH_HEAD has additional
	 * data after the sha.
	 */
	if (get_sha1_hex(buf, sha1) ||
	    (buf[40] != '\0' && !isspace(buf[40]))) {
		*type |= REF_ISBROKEN;
		errno = EINVAL;
		goto out;
	}

	ret = 0;

out:
	save_errno = errno;
	strbuf_release(&sb_path);
	strbuf_release(&sb_contents);
	errno = save_errno;
	return ret;
}

static int lock_raw_ref(struct files_ref_store *refs,
			const char *refname, int mustexist,
			const struct string_list *extras,
			const struct string_list *skip,
			struct ref_lock **lock_p,
			struct strbuf *referent,
			unsigned int *type,
			struct strbuf *err)
{
	struct ref_lock *lock;
	struct strbuf ref_file = STRBUF_INIT;
	int attempts_remaining = 3;
	int ret = TRANSACTION_GENERIC_ERROR;

	*type = 0;

	*lock_p = lock = xcalloc(1, sizeof(*lock));

	lock->ref_name = xstrdup(refname);
	files_ref_path(refs, &ref_file, refname);

    /*
	 * Now we hold the lock and can read the reference without
	 * fear that its value will change.
	 */

	if (files_read_raw_ref(&refs->base, refname,
			       lock->old_oid.hash, referent, type)) {
		if (errno == ENOENT) {
			if (mustexist) {
				/* Garden variety missing reference. */
				strbuf_addf(err, "unable to resolve reference '%s'",
					    refname);
				goto error_return;
			} 
		}
	}

	ret = 0;
	goto out;

error_return:
	unlock_ref(lock);
	*lock_p = NULL;

out:
	strbuf_release(&ref_file);
	return ret;
}


/*
 * Prepare for carrying out update:
 * - Lock the reference referred to by update.
 * - Read the reference under lock.
 * - Check that its old SHA-1 value (if specified) is correct, and in
 *   any case record it in update->lock->old_oid for later use when
 *   writing the reflog.
 * - If it is a symref update without REF_NODEREF, split it up into a
 *   REF_LOG_ONLY update of the symref and add a separate update for
 *   the referent to transaction.
 * - If it is an update of head_ref, add a corresponding REF_LOG_ONLY
 *   update of HEAD.
 */
static int lock_ref_for_update(struct files_ref_store *refs,
			       struct ref_update *update,
			       struct ref_transaction *transaction,
			       const char *head_ref,
			       struct string_list *affected_refnames,
			       struct strbuf *err)
{
	struct strbuf referent = STRBUF_INIT;
	int mustexist; 
	int ret;
	struct ref_lock *lock;

	if (head_ref) {
		ret = split_head_update(update, transaction, head_ref,
					affected_refnames, err);
		if (ret)
			return ret;
	}

	ret = lock_raw_ref(refs, update->refname, mustexist,
			   affected_refnames, NULL,
			   &lock, &referent,
			   &update->type, err);
	if (ret) {
		char *reason;

		reason = strbuf_detach(err, NULL);
		strbuf_addf(err, "cannot lock ref '%s': %s",
			    original_update_refname(update), reason);
		free(reason);
		return ret;                                 /* memory leak */
	}

	update->backend_data = lock;

	if (update->type & REF_ISSYMREF) {
		if (update->flags & REF_NODEREF) {
			/*
			 * We won't be reading the referent as part of
			 * the transaction, so we have to read it here
			 * to record and possibly check old_sha1:
			 */
			if (refs_read_ref_full(&refs->base,
					       referent.buf, 0,
					       lock->old_oid.hash, NULL)) {
				if (update->flags & REF_HAVE_OLD) {
					strbuf_addf(err, "cannot lock ref '%s': "
						    "error reading reference",
						    original_update_refname(update));
					return -1;                      /* memory leak */
				}
			} else if (check_old_oid(update, &lock->old_oid, err)) {
				return TRANSACTION_GENERIC_ERROR;   /* memory leak */
			}
		} else {
			/*
			 * Create a new update for the reference this
			 * symref is pointing at. Also, we will record
			 * and verify old_sha1 for this update as part
			 * of processing the split-off update, so we
			 * don't have to do it here.
			 */
			ret = split_symref_update(refs, update,
						  referent.buf, transaction,
						  affected_refnames, err);
			if (ret)
				return ret;                         /* memory leak */
		}
	} else {
		struct ref_update *parent_update;

		if (check_old_oid(update, &lock->old_oid, err))
			return TRANSACTION_GENERIC_ERROR;       /* memory leak */

		}

	if ((update->flags & REF_HAVE_NEW) &&
	    !(update->flags & REF_DELETING) &&
	    !(update->flags & REF_LOG_ONLY)) {
		if (!(update->type & REF_ISSYMREF) &&
		    !oidcmp(&lock->old_oid, &update->new_oid)) {
			/*
			 * The reference already has the desired
			 * value, so we don't need to write it.
			 */
		} else if (write_ref_to_lockfile(lock, &update->new_oid,
						 err)) {
			char *write_err = strbuf_detach(err, NULL);

			/*
			 * The lock was freed upon failure of
			 * write_ref_to_lockfile():
			 */
			update->backend_data = NULL;
			return TRANSACTION_GENERIC_ERROR;       /* memory leak */
		} else {
			update->flags |= REF_NEEDS_COMMIT;
		}
	}
	if (!(update->flags & REF_NEEDS_COMMIT)) {
		/*
		 * We didn't call write_ref_to_lockfile(), so
		 * the lockfile is still open. Close it to
		 * free up the file descriptor:
		 */
		if (close_ref(lock)) {
			return TRANSACTION_GENERIC_ERROR;       /* memory leak */
		}
	}
	return 0;
}

int main()
{
    struct files_ref_store refs;
    struct ref_update update;
    struct ref_transaction transaction;
    char *head_ref;
    struct string_list affected_refnames;
    struct strbuf err;
    lock_ref_for_update(&refs, &update, &transaction, head_ref, &affected_refnames, &err);
    return 0;

}


