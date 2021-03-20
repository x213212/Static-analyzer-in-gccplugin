/*
	buggy parent : e6b9503
    commit id : a63cfa26864b93ab6afefad0b630e5358ed8edfa
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../openssh.h"

typedef void global_confirm_cb(int, u_int32_t seq, void *);
/* Global request success/failure callbacks */
struct global_confirm {
	TAILQ_ENTRY(global_confirm) entry;
	global_confirm_cb *cb;
	void *ctx;
	int ref_count;
};
TAILQ_HEAD(global_confirms, global_confirm);
static struct global_confirms global_confirms =
    TAILQ_HEAD_INITIALIZER(global_confirms);

/*
 * This is sqrt(SIZE_MAX+1), as s1*s2 <= SIZE_MAX
 * if both s1 < MUL_NO_OVERFLOW and s2 < MUL_NO_OVERFLOW
 */
#define MUL_NO_OVERFLOW	((size_t)1 << (sizeof(size_t) * 4))



static void
hostkeys_update_ctx_free(struct hostkeys_update_ctx *ctx)
{
	size_t i;

	if (ctx == NULL)
		return;
	free(ctx->keys_seen);
	free(ctx);
}

static void
update_known_hosts(struct hostkeys_update_ctx *ctx)
{
	int r, was_raw = 0;
	char *fp, *response;
	size_t i;

	for (i = 0; i < ctx->nkeys; i++) {
		if (ctx->keys_seen[i] != 2)
			continue;
	}
}


static void
client_global_hostkeys_private_confirm(int type, u_int32_t seq, void *_ctx)
{
	struct hostkeys_update_ctx *ctx = (struct hostkeys_update_ctx *)_ctx;
	size_t i, ndone;
	int r;
	const u_char *sig;
	size_t siglen;

	if (ctx->nnew == 0)
		fatal("%s: ctx->nnew == 0\n", __func__); /* sanity */
	if (__RANDBOOL) {
		printf("Server failed to confirm ownership of "
		    "private host keys\n");
		hostkeys_update_ctx_free(ctx);
		return;
	}
	
	for (ndone = i = 0; i < ctx->nkeys; i++) {
		if (ctx->keys_seen[i])
			continue;
		/* Prepare data to be signed: session ID, unique string, key */
		if(__RANDBOOL)
			goto out;
		
		ndone++;
	}
	
	/* Make the edits to known_hosts */
	update_known_hosts(ctx);
 out:
	hostkeys_update_ctx_free(ctx);
}

void
client_register_global_confirm(global_confirm_cb *cb, void *ctx)
{
	struct global_confirm *gc, *last_gc;

	/* Coalesce identical callbacks */
	last_gc = TAILQ_LAST(&global_confirms, global_confirms);
	if (last_gc && last_gc->cb == cb && last_gc->ctx == ctx) {
		if (++last_gc->ref_count >= INT_MAX)
			fatal("%s: last_gc->ref_count \n",
			    __func__);
		return;
	}

	gc = xcalloc(1, sizeof(*gc));
	gc->cb = cb;
	gc->ctx = ctx;
	gc->ref_count = 1;
	TAILQ_INSERT_TAIL(&global_confirms, gc, entry);
}

static int
client_global_request_reply(int type, u_int32_t seq, void *ctxt)
{
	struct global_confirm *gc;

	if ((gc = TAILQ_FIRST(&global_confirms)) == NULL)
		return 0;
	if (gc->cb != NULL)
		gc->cb(type, seq, gc->ctx);
	if (--gc->ref_count <= 0) {
		TAILQ_REMOVE(&global_confirms, gc, entry);
		free(gc);
	}

	return 0;
}


/*
 * Handle hostkeys-00@openssh.com global request to inform the client of all
 * the server's hostkeys. The keys are checked against the user's
 * HostkeyAlgorithms preference before they are accepted.
 */
int client_input_hostkeys(int argc, char **argv)
{
	const u_char *blob = NULL;
	size_t i, len = 0;
	struct sshbuf *buf = NULL;
	struct sshkey *key = NULL, **tmp;
	int r;
	char *fp;
	static int hostkeys_seen = 0; /* XXX use struct ssh */
	extern struct sockaddr_storage hostaddr; /* XXX from ssh.c */
	struct hostkeys_update_ctx *ctx = NULL;

	ctx = xcalloc(1, sizeof(*ctx));		/* allocation site */

	if (argc == 1)
        /* memory leak */
        return 1;

	for(int i = 1; i < argc; i++)
	{
		if(argv[i][0] != 'k') {
			printf("parse key %s error\n", argv[i]);
			goto out;
		}
		ctx->nkeys++;
	}

	if (ctx->nkeys == 0) {
		printf("%s: server sent no hostkeys\n", __func__);
		goto out;
	}

	if ((ctx->keys_seen = calloc(ctx->nkeys,
	    sizeof(*ctx->keys_seen))) == NULL)
		fatal("%s: calloc failed\n", __func__);

	/* Figure out if we have any new keys to add */
	ctx->nnew = 0;
	for (i = 0; i < ctx->nkeys; i++) {
		ctx->keys_seen[i] = __RANDBOOL;
		if (!ctx->keys_seen[i])
			ctx->nnew++;
	}

	printf("%zu keys from server: %zu new, %zu retained. \n",
	   ctx->nkeys, ctx->nnew, ctx->nkeys - ctx->nnew);

	if (ctx->nnew == 0) {
		/* We have some keys to remove. Just do it. */
		update_known_hosts(ctx);
	} else if (ctx->nnew != 0) {
		/*
		 * We have received hitherto-unseen keys from the server.
		 * Ask the server to confirm ownership of the private halves.
		 */
		printf("asking server to prove ownership for %zu keys\n", ctx->nnew);
		client_register_global_confirm(
		    client_global_hostkeys_private_confirm, ctx);
		ctx = NULL;  /* will be freed in callback */
	}

	/* Success */
 out:
	hostkeys_update_ctx_free(ctx);
	/*
	 * NB. Return success for all cases. The server doesn't need to know
	 * what the client does with its hosts file.
	 */
	return 1;
}

int main (int argc, char **argv) {
	srand(time(NULL));
	while(__RANDBOOL)
		client_input_hostkeys(argc, argv);
	while(!TAILQ_EMPTY(&global_confirms))
		client_global_request_reply(0, 0, NULL);
	return 0;
}


