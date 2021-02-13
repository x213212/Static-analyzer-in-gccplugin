/*
	buggy parent : 52a93b9
	commit id : a26a013f22a19e2c16729e64f40ef8a7dfcc086e
*/

#include "../include/common.h"

#define FILE_ALLOC_CHUNK 5

typedef bool bfd_boolean;
typedef unsigned int bfd_size_type;

struct line_info_table
{
	unsigned int num_files;
	unsigned int num_sequences;

	struct fileinfo* files;
	struct line_sequence* sequences;
	struct line_info* lcl_head;
};

struct fileinfo
{
	char *name;
};

struct line_sequence
{
	struct line_sequence* prev_sequence;
	struct line_info* last_line;
};

struct line_info
{
	struct line_info* prev_line;
	unsigned int line;
	unsigned char end_sequence;
};


static bfd_boolean
add_line_info (struct line_info_table *table,
		unsigned int line,
		int end_sequence)
{
	bfd_size_type amt = sizeof (struct line_info);
	struct line_sequence* seq = table->sequences;
	struct line_info* info = (struct line_info *) malloc (amt);
	
	if (info == NULL)
		return FALSE;
	
	info->prev_line = NULL;
	info->line = line;
	info->end_sequence = end_sequence;

  /* We only keep the last entry with the same address and end
	sequence.  See PR ld/4986.  */
	if (seq
			&& seq->last_line->end_sequence == end_sequence)
		{
			if (table->lcl_head == seq->last_line)
				table->lcl_head = info;

			info->prev_line = seq->last_line->prev_line;
			seq->last_line = info;
		}
	else if (!seq || seq->last_line->end_sequence)
		{
			/* Start a new line sequence */
			amt = sizeof (struct line_sequence);
			seq = (struct line_sequence *) malloc (amt);	/* allocation site */
			if (seq == NULL)
				return FALSE;

			seq->prev_sequence = table->sequences;
			seq->last_line = info;
			table->lcl_head = info;
			table->sequences = seq;
			table->num_sequences++;
		}
	else
		{
			/* Normal case: add 'info' to the beginning of the current sequence.  */
			info->prev_line = seq->last_line;
			seq->last_line = info;

			/* lcl_head: initialize to head a *possible* sequence at the end.  */
			if (!table->lcl_head)
				table->lcl_head = info;
		}

		return __RANDBOOL;
}

static bfd_boolean
sort_line_sequences (struct line_info_table *table)
{
	bfd_size_type amt;
	struct line_sequence* sequences;
	struct line_sequence* seq;
	unsigned int n = 0;
	unsigned int num_sequences = table->num_sequences;

	if (num_sequences == 0)
		return TRUE;

	/* Allocate space for an array of sequences.  */
	amt = sizeof (struct line_sequence) * num_sequences;
	sequences = (struct line_sequence *) malloc (amt); 

	if (sequences == NULL)
		return FALSE;

	/* Copy the linked list into the array, freeing the original nodes.  */
  seq = table->sequences;
  for (n = 0; n < num_sequences; n++)
    {
      struct line_sequence* last_seq = seq;

			if (seq == NULL) exit (1);

      sequences[n].prev_sequence = NULL;
      sequences[n].last_line = seq->last_line;
      seq = seq->prev_sequence;
      free (last_seq);
    }

	if (seq == NULL) exit (1);

	table->sequences = sequences;
	table->num_sequences = num_sequences;
	return TRUE;
}


static struct line_info_table* decode_line_info (void)
{
	struct line_info_table *table;
	bfd_size_type amt;
	int line = 0;
	int end_sequence;

	amt = sizeof (struct line_info_table);

	table = (struct line_info_table *) malloc (amt);
	if (table == NULL)
		return NULL;

	table->num_files = 0;
	table->sequences = NULL;
	table->lcl_head = NULL;

	while (line < 5)
		{
			line++;
			__NONDET(end_sequence, 1, 0);

			if (!add_line_info(table, line, end_sequence))
				goto fail;

			if ((table->num_files % FILE_ALLOC_CHUNK) == 0)
				{
					struct fileinfo *tmp;
					amt = table->num_files + FILE_ALLOC_CHUNK;
					amt *= sizeof (struct fileinfo);
					tmp = (struct fileinfo *) realloc (table->files, amt);

					if (tmp == NULL)
						goto fail;
				}
		}

	if (sort_line_sequences (table))
		return table;

	fail:
		/* memory leak */
		if (table->sequences != NULL)
			free (table->sequences);

		if (table->files != NULL)
			free (table->files);
}

int main()
{
	time_t t;
	srand(time(&t));

	struct line_info_table *table;
	table = decode_line_info();

	
}

