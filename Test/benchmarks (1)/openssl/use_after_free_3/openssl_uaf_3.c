/*
	buggy parent : 4ba5e63 
	commit id : 94d1f4b0f3d262edf1cf7023a01d5404945035d5
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../openssl.h"

#define SSL_NOTHING	1
#define SSL_RECEIVED_SHUTDOWN	2
#define SSL_ST_READ_HEADER			0xF0
#define SSL_MODE_RELEASE_BUFFERS 0x00000010L

typedef struct ssl3_buffer_st {
	unsigned char *buf;     /* at least SSL3_RT_MAX_PACKET_SIZE bytes,
	                         * see ssl3_setup_buffers() */
	size_t len;             /* buffer size */
	int offset;             /* where to 'copy from' */
	int left;               /* how many bytes left */
} SSL3_BUFFER;

typedef struct ssl3_state_st {
	SSL3_BUFFER rbuf;	/* read IO goes into here */
} SSL3_STATE;

typedef struct ssl3_buf_freelist_entry_st {
	struct ssl3_buf_freelist_entry_st *next;
} SSL3_BUF_FREELIST_ENTRY;

typedef struct ssl3_buf_freelist_st {
	size_t chunklen;
	unsigned int len;
	struct ssl3_buf_freelist_entry_st *head;
} SSL3_BUF_FREELIST;

struct ssl_ctx_st {
	struct ssl3_buf_freelist_st *rbuf_freelist;
	struct ssl3_buf_freelist_st *wbuf_freelist;
};

struct ssl_st {
	struct ssl3_state_st *s3; /* SSLv3 variables */
	unsigned char *packet;
	int rstate;	/* where we are when reading */
	int rwstate;
	int shutdown;	/* we have shut things down, 0x01 sent, 0x02 * for received */
	unsigned long mode; /* API behaviour */
	SSL_CTX *ctx;
};

static void *
freelist_extract(SSL_CTX *ctx, int for_read, int sz)
	{
	SSL3_BUF_FREELIST *list;
	SSL3_BUF_FREELIST_ENTRY *ent = NULL;
	void *result = NULL;

	list = for_read ? ctx->rbuf_freelist : ctx->wbuf_freelist;
	if (list != NULL && sz == (int)list->chunklen)
		ent = list->head;
	if (ent != NULL)
		{
		list->head = ent->next;
		result = ent;
		if (--list->len == 0)
			list->chunklen = 0;
		}
	if (!result)
		result = OPENSSL_malloc(sz);	/* allocation site */
	return result;
}

static void
freelist_insert(SSL_CTX *ctx, int for_read, size_t sz, void *mem)
	{
	SSL3_BUF_FREELIST *list;
	SSL3_BUF_FREELIST_ENTRY *ent;

	list = for_read ? ctx->rbuf_freelist : ctx->wbuf_freelist;
	if (list != NULL &&
	    (sz == list->chunklen || list->chunklen == 0) &&
	    sz >= sizeof(*ent))
		{
		list->chunklen = sz;
		ent = mem;
		ent->next = list->head;
		list->head = ent;
		++list->len;
		mem = NULL;
		}

	if (mem)
		OPENSSL_free(mem);
	}

int ssl3_release_read_buffer(SSL *s)
	{
	if (s->s3->rbuf.buf != NULL)
		{
		freelist_insert(s->ctx, 1, s->s3->rbuf.len, s->s3->rbuf.buf);
		s->s3->rbuf.buf = NULL; /* still reachable */
		}
	return 1;
	}

#define SSL3_RT_MAX_MD_SIZE			64
#define SSL3_RT_MAX_PLAIN_LENGTH		16384
#define SSL3_RT_MAX_ENCRYPTED_OVERHEAD	(256 + SSL3_RT_MAX_MD_SIZE)

int ssl3_setup_read_buffer(SSL *s)
	{
	unsigned char *p;
	size_t len,align=0,headerlen;
	
	if (s->s3->rbuf.buf == NULL)
		{
		len = SSL3_RT_MAX_PLAIN_LENGTH
			+ SSL3_RT_MAX_ENCRYPTED_OVERHEAD
			+ headerlen + align;
		if ((p=freelist_extract(s->ctx, 1, len)) == NULL)
			goto err;
		s->s3->rbuf.buf = p;
		s->s3->rbuf.len = len;
		}

	s->packet= &(s->s3->rbuf.buf[0]); /* alias */
	return 1;

err:
	return 0;
	}


int ssl3_read_bytes(SSL *s, int type, unsigned char *buf, int len, int peek)
{
	int al,i,j,ret;
	unsigned int n;

	if (s->s3->rbuf.buf == NULL) /* Not initialized yet */
		if (!ssl3_setup_read_buffer(s))
			return(-1);

	/* If the other end has shut down, throw anything we read away
	 * (even in 'peek' mode) */
	if (s->shutdown & SSL_RECEIVED_SHUTDOWN)
		{
		s->rwstate=SSL_NOTHING;
		return(0);
		}

	if (len <= 0) return(len);

	n = (unsigned int)len;

	if (!peek)
		{
		s->rstate=SSL_ST_READ_HEADER;
		if (s->mode & SSL_MODE_RELEASE_BUFFERS)
			ssl3_release_read_buffer(s);
		}
	return(n);
}

int main()
{
	SSL *s = OPENSSL_malloc(sizeof(*s));;
	s->s3 = OPENSSL_malloc(sizeof(*s->s3));	
	ssl3_read_bytes(s, 0, "buf", 1, 0);
	if(s->s3->rbuf.left != 0)
		__USE(s->packet);
	OPENSSL_free(s->s3->rbuf.buf);
	OPENSSL_free(s->s3);
	OPENSSL_free(s);
	return 0;
}
	
