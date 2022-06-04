/*
	buggy parent : e4adad9
	commit id : 0a3452520fe4cd6871ae8b7c4199c6d5d4efe912
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../openssl.h"

#  define openssl_fdset(a,b) FD_SET(a, b)
#define OSSL_ASYNC_FD       int
#define OSSL_BAD_ASYNC_FD   -1

#define ASYNC_ERR      0
#define ASYNC_NO_JOBS  1
#define ASYNC_PAUSE    2
#define ASYNC_FINISH   3

typedef struct async_job_st ASYNC_JOB;
typedef struct async_wait_ctx_st ASYNC_WAIT_CTX;

typedef struct async_fibre_st {
    int dummy;
} async_fibre;

struct async_ctx_st {
    async_fibre dispatcher;
    ASYNC_JOB *currjob;
    unsigned int blocked;
};

struct ssl_st {
	ASYNC_WAIT_CTX *waitctx;
};

struct async_job_st {
    async_fibre fibrectx;
    int (*func) (void *);
    void *funcargs;
    int ret;
    int status;
    ASYNC_WAIT_CTX *waitctx;
};

struct fd_lookup_st {
    const void *key;
    OSSL_ASYNC_FD fd;
    void *custom_data;
    void (*cleanup)(ASYNC_WAIT_CTX *, const void *, OSSL_ASYNC_FD, void *);
    int add;
    int del;
    struct fd_lookup_st *next;
};

struct async_wait_ctx_st {
    struct fd_lookup_st *fds;
    size_t numadd;
    size_t numdel;
};

struct async_pool_st {
    size_t curr_size;
    size_t max_size;
};


int ASYNC_WAIT_CTX_get_all_fds(ASYNC_WAIT_CTX *ctx, OSSL_ASYNC_FD *fd,
                               size_t *numfds)
{
    struct fd_lookup_st *curr;

    curr = ctx->fds;
    *numfds = 0;
    while (curr != NULL) {
        if (curr->del) {
            /* This one has been marked deleted so do nothing */
            curr = curr->next;
            continue;
        }
        if (fd != NULL) {
            *fd = curr->fd;
            fd++;
        }
        (*numfds)++;
        curr = curr->next;
    }
    return 1;
}

int SSL_get_all_async_fds(SSL *s, OSSL_ASYNC_FD *fds, size_t *numfds)
{
    ASYNC_WAIT_CTX *ctx = s->waitctx;

    if (ctx == NULL)
        return 0;
    return ASYNC_WAIT_CTX_get_all_fds(ctx, fds, numfds);
}

void wait_for_async(SSL *s)
{
    /* On Windows select only works for sockets, so we simply don't wait  */
    int width = 0;
    fd_set asyncfds;
    OSSL_ASYNC_FD *fds;
    size_t numfds;

    if (!SSL_get_all_async_fds(s, NULL, &numfds))
        return;
    if (numfds == 0)
        return;
    fds = app_malloc(sizeof(OSSL_ASYNC_FD) * numfds, "allocate async fds"); /* allocation site */	
    if (!SSL_get_all_async_fds(s, fds, &numfds)) {
        OPENSSL_free(fds);
    }

	/* use-after-free */
    while (numfds > 0) {
        if (width <= (int)*fds)
            width = (int)*fds + 1;
        openssl_fdset((int)*fds, &asyncfds);
        numfds--;
        fds++;
    }
    select(width, (void *)&asyncfds, NULL, NULL, NULL);
	/* memory leak */
}

int main()
{
	SSL s;
	wait_for_async(&s);
	return 0;
}





