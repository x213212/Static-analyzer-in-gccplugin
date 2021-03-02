/*
    buggy parent : 4229f1f
    commit id : cdf690e53b5f5af1ca8679b3f3e47ea198692c18
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../git.h"

/* Flags to match_refs. */
enum match_refs_flags {
    MATCH_REFS_NONE     = 0,
    MATCH_REFS_ALL      = (1 << 0), 
    MATCH_REFS_MIRROR   = (1 << 1), 
};

struct refspec {
	unsigned force : 1;
	unsigned pattern : 1;
	unsigned matching : 1;

	char *src;
	char *dst;
};


struct ref {
	struct ref *next;
	unsigned char old_sha1[20];
	unsigned char new_sha1[20];
	char *symref;
	unsigned int force:1,
		merge:1,
		nonfastforward:1,
		deletion:1;
	enum {
		REF_STATUS_NONE = 0,
		REF_STATUS_OK,
		REF_STATUS_REJECT_NONFASTFORWARD,
		REF_STATUS_REJECT_NODELETE,
		REF_STATUS_UPTODATE,
		REF_STATUS_REMOTE_REJECT,
		REF_STATUS_EXPECTING_REPORT,
	} status;
	char *remote_status;
	struct ref *peer_ref; /* when renaming */
	char name[FLEX_ARRAY]; /* more */
};

#define REF_NORMAL	(1u << 0)
#define REF_HEADS	(1u << 1)
#define REF_TAGS	(1u << 2)

static struct ref *remote_refs, **remote_tail;

static void free_ref(struct ref *ref)
{
	if (!ref)
		return;
	free_ref(ref->peer_ref);
	free(ref->remote_status);
	free(ref->symref);
	free(ref);
}

void free_refs(struct ref *ref)
{
	struct ref *next;
	while (ref) {
		next = ref->next;
		free_ref(ref);
		ref = next;
	}
}

struct ref *find_ref_by_name(const struct ref *list, const char *name)
{
    for ( ; list; list = list->next)
        if (!strcmp(list->name, name))
            return (struct ref *)list;
    return NULL;
}

static struct ref *alloc_ref_with_prefix(const char *prefix, size_t prefixlen,
		const char *name)
{
	size_t len = strlen(name);
	struct ref *ref = xcalloc(1, sizeof(struct ref) + prefixlen + len + 1);
	memcpy(ref->name, prefix, prefixlen);
	memcpy(ref->name + prefixlen, name, len);
	return ref;
}

struct ref *alloc_ref(const char *name)
{
	return alloc_ref_with_prefix("", 0, name);
}

static struct ref *copy_ref(const struct ref *ref)
{
	struct ref *cpy;
	size_t len;
	if (!ref)
		return NULL;
	len = strlen(ref->name);
	cpy = xmalloc(sizeof(struct ref) + len + 1);
	memcpy(cpy, ref, sizeof(struct ref) + len + 1);
	cpy->next = NULL;
	cpy->symref = ref->symref ? xstrdup(ref->symref) : NULL;
	cpy->remote_status = ref->remote_status ? xstrdup(ref->remote_status) : NULL;
	cpy->peer_ref = copy_ref(ref->peer_ref);
	return cpy;
}

static struct ref *try_explicit_object_name(const char *name)
{
	unsigned char sha1[20];
	struct ref *ref;

	if (!*name) {
		ref = alloc_ref("(delete)");
		hashclr(ref->new_sha1);
		return ref;
	}
	if (get_sha1(name, sha1))
		return NULL;
	ref = alloc_ref(name);
	hashcpy(ref->new_sha1, sha1);
	return ref;
}

static void tail_link_ref(struct ref *ref, struct ref ***tail)
{
	**tail = ref;
	while (ref->next)
		ref = ref->next;
	*tail = &ref->next;
}

static struct ref *make_linked_ref(const char *name, struct ref ***tail)
{
	struct ref *ret = alloc_ref(name);
	tail_link_ref(ret, tail);
	return ret;
}

/*
 * Note. This is used only by "push"; refspec matching rules for
 * push and fetch are subtly different, so do not try to reuse it
 * without thinking.
 */
int match_refs(struct ref *src, struct ref *dst, struct ref ***dst_tail,
	       int nr_refspec, const char **refspec, int flags)
{
	struct refspec *rs;
	int send_all = flags & MATCH_REFS_ALL;
	int send_mirror = flags & MATCH_REFS_MIRROR;
	static const char *default_refspec[] = { ":", 0 };

	/* pick the remainder */
	for ( ; src; src = src->next) {
		struct ref *dst_peer;
		const struct refspec *pat = NULL;
		char *dst_name = src->name;
		if (src->peer_ref)
			continue;

		dst_peer = find_ref_by_name(dst, dst_name);
		if (dst_peer) {
			if (dst_peer->peer_ref)
				/* We're already sending something to this ref. */
				goto free_name;

		} else {
			if (pat->matching && !(send_all || send_mirror))
				/*
				 * Remote doesn't have it, and we have no
				 * explicit pattern, and we don't have
				 * --all nor --mirror.
				 */
				goto free_name;

			/* Create a new one and link it */
			dst_peer = make_linked_ref(dst_name, dst_tail);
			hashcpy(dst_peer->new_sha1, src->new_sha1);
		}
		dst_peer->peer_ref = src;
		dst_peer->force = pat->force;
	free_name:
		free(dst_name);
	}
	return 0;
}


int main (int argc, char **argv)
{
	int nr_refspec = 0;
	char **refspec = NULL;
	struct remote_lock *ref_lock = NULL;
	struct remote_lock *info_ref_lock = NULL;
	struct rev_info revs;
	int delete_branch = 0;
	int force_delete = 0;
	int objects_to_send;
	int rc = 0;
	int i;
	int new_refs;
	struct ref *ref, *local_refs;
	char *rewritten_url = NULL;

    refspec = argv;
	local_refs = get_local_heads();
	remote_tail = &remote_refs;
    
   if (match_refs(local_refs, remote_refs, &remote_tail,
		       nr_refspec, (const char **) refspec, 1)) {
		rc = -1;
		goto cleanup;
	}
   
    if (!remote_refs) {
		fprintf(stderr, "No refs in common and none specified; doing nothing.\n");
		rc = 0;
		goto cleanup;
	}

cleanup:
    free_refs(local_refs);     
    free_refs(remote_refs); /* double free */   
}


