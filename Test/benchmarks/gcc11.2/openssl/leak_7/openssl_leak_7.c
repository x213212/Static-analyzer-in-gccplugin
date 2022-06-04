/*
	buggy parent: 9b57977
	commit id: af6de400b49c011600cfd557319d1142da6e5cbd
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../openssl.h"

#define OPENSSL_CRYPTO_THREAD_LOCAL_KEY_MAX 256

struct thread_local_inits_st {
    int async;
    int err_state;
};
# define OPENSSL_INIT_THREAD_ASYNC           0x01
# define OPENSSL_INIT_THREAD_ERR_STATE       0x02



typedef int CRYPTO_THREAD_LOCAL;
static CRYPTO_THREAD_LOCAL threadstopkey;

static void *thread_local_storage[OPENSSL_CRYPTO_THREAD_LOCAL_KEY_MAX];

int CRYPTO_THREAD_init_local(CRYPTO_THREAD_LOCAL *key, void (*cleanup)(void *))
{
    static unsigned int thread_local_key = 0;

    if (thread_local_key >= OPENSSL_CRYPTO_THREAD_LOCAL_KEY_MAX)
        return 0;

    *key = thread_local_key++;

    thread_local_storage[*key] = NULL;

    return 1;
}

void *CRYPTO_THREAD_get_local(CRYPTO_THREAD_LOCAL *key)
{
    if (*key >= OPENSSL_CRYPTO_THREAD_LOCAL_KEY_MAX)
        return NULL;

    return thread_local_storage[*key];
}

int CRYPTO_THREAD_set_local(CRYPTO_THREAD_LOCAL *key, void *val)
{
    if (*key >= OPENSSL_CRYPTO_THREAD_LOCAL_KEY_MAX)
        return 0;

    thread_local_storage[*key] = val;

    return 1;
}

static struct thread_local_inits_st *ossl_init_get_thread_local(int alloc)
{
    struct thread_local_inits_st *local =
        CRYPTO_THREAD_get_local(&threadstopkey);

    if (local == NULL && alloc) {
        local = OPENSSL_zalloc(sizeof *local);	/* allocation site */
        CRYPTO_THREAD_set_local(&threadstopkey, local);
    }
    if (!alloc) {
        CRYPTO_THREAD_set_local(&threadstopkey, NULL);
    }

    return local;
}

int ossl_init_thread_start(uint64_t opts)
{
    struct thread_local_inits_st *locals = ossl_init_get_thread_local(1);

    if (locals == NULL)
        return 0;

    if (opts & OPENSSL_INIT_THREAD_ASYNC) {
#ifdef OPENSSL_INIT_DEBUG
        fprintf(stderr, "OPENSSL_INIT: ossl_init_thread_start: "
                        "marking thread for async\n");
#endif
        locals->async = 1;
    }

    if (opts & OPENSSL_INIT_THREAD_ERR_STATE) {
#ifdef OPENSSL_INIT_DEBUG
        fprintf(stderr, "OPENSSL_INIT: ossl_init_thread_start: "
                        "marking thread for err_state\n");
#endif
        locals->err_state = 1;
    }

    return 1;
}

int main (void)
{
	for(int i = 0; i < 10; i++)
	{
		CRYPTO_THREAD_init_local(&threadstopkey, NULL);
		ossl_init_thread_start(OPENSSL_INIT_THREAD_ERR_STATE);
	}
	for(int i = 0; i < 10; i++) {
		printf("%d\n", thread_local_storage[i]);
		free(thread_local_storage[i]);	
	}
	return 0;
}
