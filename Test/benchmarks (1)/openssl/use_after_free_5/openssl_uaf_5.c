/*
	buggy parent : 7234632
	commit id : d1e1aeef8f434fa3caf5e27faf82ec629b794f3a
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../openssl.h"

typedef struct _pitem
	{
	unsigned char priority[8]; /* 64-bit value in big-endian encoding */
	void *data;
	struct _pitem *next;
	} pitem;

typedef struct _pqueue
	{
	pitem *items;
	int count;
	} pqueue_s;

typedef struct _pqueue *pqueue;
typedef struct record_pqueue_st
	{
	unsigned short epoch;
	pqueue q;
	} record_pqueue;

typedef struct ssl3_record_st
	{
/*r */	int type;               /* type of record */
/*rw*/	unsigned int length;    /* How many bytes available */
/*rw*/	unsigned int orig_len;  /* How many bytes were available before padding
				   was removed? This is used to implement the
				   MAC check in constant time for CBC records.
				 */
/*r */	unsigned int off;       /* read/write offset into 'buf' */
/*rw*/	unsigned char *data;    /* pointer to the record data */
/*rw*/	unsigned char *input;   /* where the decode bytes are */
/*r */	unsigned char *comp;    /* only used with decompression - malloc()ed */
/*r */  unsigned long epoch;    /* epoch number, needed by DTLS1 */
/*r */  unsigned char seq_num[8]; /* sequence number, needed by DTLS1 */
	} SSL3_RECORD;

typedef struct ssl3_buffer_st
	{
	unsigned char *buf;     /* at least SSL3_RT_MAX_PACKET_SIZE bytes,
	                         * see ssl3_setup_buffers() */
	size_t len;             /* buffer size */
	int offset;             /* where to 'copy from' */
	int left;               /* how many bytes left */
	} SSL3_BUFFER;

typedef struct ssl3_state_st
	{
	SSL3_BUFFER rbuf;	/* read IO goes into here */
	SSL3_RECORD rrec;	/* each decoded record goes in here */
	} SSL3_STATE;
struct ssl_st
	{
	struct ssl3_state_st *s3;
	unsigned char *packet;
	unsigned int packet_length;
	};

typedef struct dtls1_record_data_st
	{
	unsigned char *packet;
	unsigned int   packet_length;
	SSL3_BUFFER    rbuf;
	SSL3_RECORD    rrec;
	} DTLS1_RECORD_DATA;

pitem *
pitem_new(unsigned char *prio64be, void *data)
	{
	pitem *item = (pitem *) OPENSSL_malloc(sizeof(pitem));	/* allocation site */
	if (item == NULL) return NULL;

	memcpy(item->priority,prio64be,sizeof(item->priority));

	item->data = data;
	item->next = NULL;

	return item;
	}

void
pitem_free(pitem *item)
	{
	if (item == NULL) return;

	OPENSSL_free(item);
	}

pqueue_s *
pqueue_new()
	{
	pqueue_s *pq = (pqueue_s *) OPENSSL_malloc(sizeof(pqueue_s));
	if (pq == NULL) return NULL;

	memset(pq, 0x00, sizeof(pqueue_s));
	return pq;
	}

void
pqueue_free(pqueue_s *pq)
	{
	if (pq == NULL) return;

	OPENSSL_free(pq);
	}

pitem *
pqueue_insert(pqueue_s *pq, pitem *item)
	{
	pitem *curr, *next;

	if (pq->items == NULL)
		{
		pq->items = item;
		return item;
		}

	for(curr = NULL, next = pq->items; 
		next != NULL;
		curr = next, next = next->next)
		{
		/* we can compare 64-bit value in big-endian encoding
		 * with memcmp:-) */
		int cmp = memcmp(next->priority, item->priority,8);
		if (cmp > 0)		/* next > item */
			{
			item->next = next;

			if (curr == NULL) 
				pq->items = item;
			else  
				curr->next = item;

			return item;
			}
		
		else if (cmp == 0)	/* duplicates not allowed */
			return NULL;
		}

	item->next = NULL;
	curr->next = item;

	return item;
	}

pitem *
pqueue_peek(pqueue_s *pq)
	{
	return pq->items;
	}

pitem *
pqueue_pop(pqueue_s *pq)
	{
	pitem *item = pq->items;

	if (pq->items != NULL)
		pq->items = pq->items->next;

	return item;
	}

pitem *
pqueue_find(pqueue_s *pq, unsigned char *prio64be)
	{
	pitem *next;
	pitem *found = NULL;

	if ( pq->items == NULL)
		return NULL;

	for ( next = pq->items; next->next != NULL; next = next->next)
		{
		if ( memcmp(next->priority, prio64be,8) == 0)
			{
			found = next;
			break;
			}
		}
	
	/* check the one last node */
	if ( memcmp(next->priority, prio64be,8) ==0)
		found = next;

	if ( ! found)
		return NULL;

#if 0 /* find works in peek mode */
	if ( prev == NULL)
		pq->items = next->next;
	else
		prev->next = next->next;
#endif

	return found;
	}

void
pqueue_print(pqueue_s *pq)
	{
	pitem *item = pq->items;

	while(item != NULL)
		{
		printf("item\t%02x%02x%02x%02x%02x%02x%02x%02x\n",
			item->priority[0],item->priority[1],
			item->priority[2],item->priority[3],
			item->priority[4],item->priority[5],
			item->priority[6],item->priority[7]);
		item = item->next;
		}
	}

pitem *
pqueue_iterator(pqueue_s *pq)
	{
	return pqueue_peek(pq);
	}

pitem *
pqueue_next(pitem **item)
	{
	pitem *ret;

	if ( item == NULL || *item == NULL)
		return NULL;


	/* *item != NULL */
	ret = *item;
	*item = (*item)->next;

	return ret;
	}

int
pqueue_size(pqueue_s *pq)
{
	pitem *item = pq->items;
	int count = 0;
	
	while(item != NULL)
	{
		count++;
		item = item->next;
	}
	return count;
}

static int
dtls1_buffer_record(SSL *s, record_pqueue *queue, unsigned char *priority)
	{
	DTLS1_RECORD_DATA *rdata;
	pitem *item;

	/* Limit the size of the queue to prevent DOS attacks */
	if (pqueue_size(queue->q) >= 100)
		return 0;
		
	rdata = OPENSSL_malloc(sizeof(DTLS1_RECORD_DATA));	/* allocation site */
	item = pitem_new(priority, rdata);					/* allocation site */
	if (rdata == NULL || item == NULL)
		{
		if (rdata != NULL) OPENSSL_free(rdata);
		if (item != NULL) pitem_free(item);
		
		return(0);
		}
	
	rdata->packet = s->packet;
	rdata->packet_length = s->packet_length;
	memcpy(&(rdata->rbuf), &(s->s3->rbuf), sizeof(SSL3_BUFFER));
	memcpy(&(rdata->rrec), &(s->s3->rrec), sizeof(SSL3_RECORD));

	item->data = rdata;

	/* insert should not fail, since duplicates are dropped */
	if (pqueue_insert(queue->q, item) == NULL)
		{
		OPENSSL_free(rdata);
		pitem_free(item);
		return(0);
		}

	s->packet = NULL;
	s->packet_length = 0;
	memset(&(s->s3->rbuf), 0, sizeof(SSL3_BUFFER));
	memset(&(s->s3->rrec), 0, sizeof(SSL3_RECORD));
	
	if (!ssl3_setup_buffers(s))
		{
		OPENSSL_free(rdata);	/* freed */
		pitem_free(item);		/* freed */
		return(0);
		}
	
	return(1);
	}



int main()
{
	record_pqueue queue;
	pitem *item;
	queue.q = pqueue_new(); 
	dtls1_buffer_record(NULL, &queue, 0);
	dtls1_buffer_record(NULL, &queue, 1);
	dtls1_buffer_record(NULL, &queue, 2);
	while(!(item = pqueue_pop(queue.q))){
		__USE(item); // use-after-free
		pitem_free(item);
	}
	pqueue_free(queue.q);
	return 0;
}
	
