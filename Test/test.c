/*
	buggy parent: a72f95f
	commit id: b55ec8b676ed05d93ee49d6c79ae0403616c4fb0
*/

#include "./common.h"

#define PTR void *
#define ABBREV_HASH_SIZE 64
#define CHUNK_SIZE (4096 -32)
#define CHUNK_HEADER_SIZE 32

typedef bool bfd_boolean;

struct bfd_hash_entry
{
	struct bfd_hash_entry *next;

	/* String being hashed.  */
  const char *string;

  /* Hash code.  This is the full hash code, not the index into the
     table.  */
  unsigned long hash;
};

struct bfd_hash_table
{
	struct bfd_hash_entry **table;

	/* An objalloc for this hash table.  This is a struct objalloc *,
		but we use void * to avoid requiring the inclusion of objalloc.h.  */
	void *memory;
	/* The number of slots in the hash table.  */
  unsigned int size;
  /* The number of entries in the hash table.  */
  unsigned int count;
  /* The size of elements.  */
  unsigned int entsize;
};

struct objalloc
{
	char *current_ptr;
	unsigned int current_space;
	void *chunks;
};

struct objalloc_chunk
{
	struct objalloc_chunk *next;
	char *current_ptr;
};

struct objalloc *
objalloc_create (void)
{
	struct objalloc *ret;
	struct objalloc_chunk *chunk;

	ret = (struct objalloc *) malloc (sizeof *ret); /* allocation site */

	if (ret == NULL)
		return NULL;

	ret->chunks = (PTR) malloc (CHUNK_SIZE); /* allocation site */
	if (ret->chunks == NULL)
		{
			free (ret);
			return NULL;
		}

	chunk = (struct objalloc_chunk *) ret->chunks;
	chunk->next = NULL;
	chunk->current_ptr = NULL;
	chunk->next = NULL;

	ret->current_ptr = (char *) chunk + CHUNK_HEADER_SIZE;
	ret->current_space = CHUNK_SIZE - CHUNK_HEADER_SIZE;

	return ret;
}

PTR
objalloc_alloc (struct objalloc *o, unsigned long len)
{
	char *ret;
	struct objalloc_chunk *chunk;
	
	/* We don't care this case in modelled program */
	if (len > CHUNK_SIZE)
		exit (1);

	chunk = (struct objalloc_chunk *) malloc (CHUNK_SIZE); /* allocation site */
	if (chunk == NULL)
		return NULL;

	chunk->next = (struct objalloc_chunk *) o->chunks;
	chunk->current_ptr = NULL;

	o->current_ptr = (char *) chunk + CHUNK_HEADER_SIZE;
	o->current_space = CHUNK_SIZE - CHUNK_HEADER_SIZE;

	o->chunks = (PTR) chunk;

	return (PTR) (chunk + CHUNK_HEADER_SIZE);
}

void
objalloc_free (struct objalloc *o)
{
	struct objalloc_chunk *l;
	void *data;

	l = (struct objalloc_chunk *) o->chunks;
	while (l != NULL)
		{
			data = l + CHUNK_HEADER_SIZE;
			struct objalloc_chunk *next;

			next = l->next;
			free(l);
			l = next;
		}
	free (o);
}

bfd_boolean
bfd_hash_table_init_n (struct bfd_hash_table *table,
		unsigned int entsize,
		unsigned int size)
{
	unsigned long alloc;

	alloc = size;
	alloc *= sizeof (struct bfd_hash_entry *);
	table->memory = (void *) objalloc_create ();
	if (table->memory == NULL)
		{
			return FALSE;
		}
	
  table->table = (struct bfd_hash_entry **)
      objalloc_alloc ((struct objalloc *) table->memory, alloc);

	if (table->table == NULL)
		{
			objalloc_free(table);
			return FALSE;
		}

  memset ((void *) table->table, 0, alloc);
  table->size = size;
  table->entsize = entsize;
  table->count = 0;

	return TRUE;
}

struct bfd_hash_entry *
bfd_hash_insert (struct bfd_hash_table *table,
		 const char *string,
		 unsigned long hash)
{
  struct bfd_hash_entry *hashp;
	unsigned int _index;

	hashp = (struct bfd_hash_entry *) objalloc_alloc (table->memory, sizeof (*hashp));
	hashp->hash = hash;
	hashp->string = string;
	_index = hash % table->size;
	table->table[_index] = hashp;
	table->count++;
	
	return hashp;
}

void bfd_dwarf2_cleanup_debug_info(struct bfd_hash_table *table)
{
	
	objalloc_free(table->memory);
}


int main()
{
	time_t t;

	struct bfd_hash_table table;
	struct bfd_hash_entry *entry;
	unsigned int entsize = 10;
	unsigned int size = 10;
	unsigned int cnt = 0;
	const char *hash[10] =
			{ "a", "b", "c", "d", "e", "f", "g", "h", "i", "j" };

	srand(time(&t));

	if (! bfd_hash_table_init_n (&table, entsize, size))
		return;

	while ((cnt < entsize))
		{
			bfd_hash_insert (&table, hash[cnt], cnt);
			cnt++;
		}

	
	bfd_dwarf2_cleanup_debug_info(&table);
	memset (&table, 0, sizeof(struct bfd_hash_table)); /* memory leak */
	
}

