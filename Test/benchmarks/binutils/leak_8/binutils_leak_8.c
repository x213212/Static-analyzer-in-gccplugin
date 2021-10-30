/*
	buggy parent: c42608e
	commit id : 848ac659685fba46ce8816400db705f60c8040f7
*/

#include "../include/common.h"
#include "../include/binutils.h"
#include "../include/bfd.h"

void *bfd_malloc (bfd_size_type size)
{
  void *ptr;
  size_t sz = (size_t) size;

  if (size != sz
      /* This is to pacify memory checkers like valgrind.  */
      || ((signed long) sz) < 0)
    {
			printf("bfd_error_no_memory\n");
      return NULL;
    }

  ptr = malloc (sz); // allocation site
  if (ptr == NULL && sz != 0)
		printf("bfd_error_no_memory\n");

  return ptr;
}

bfd_boolean
bfd_get_section_contents (sec_ptr sec, const void *location)
{
	if (location) {
		memset (location, 0, 4);
		return TRUE;
	}


	return __RANDBOOL;
}

bfd_boolean
bfd_get_full_section_contents (sec_ptr sec, bfd_byte **ptr)
{
  bfd_byte *compressed_buffer;
	bfd_byte *p = *ptr;

	switch (sec->compress_status)
		{
			case COMPRESS_SECTION_NONE:
				if (p == NULL)
					{
						p = (bfd_byte *) bfd_malloc (4);
						if (p == NULL)
							return FALSE;
					}
			
				*p = 12;
				if (!bfd_get_section_contents (sec, p))
					{
						if (*ptr != p)
							free (p);
						return FALSE;
					}

				*ptr = p;
				return TRUE;

			case DECOMPRESS_SECTION_SIZED:
				compressed_buffer = (bfd_byte *) bfd_malloc (4);
				if (compressed_buffer == NULL)
					return FALSE;

				if (__RANDBOOL)
					goto fail_compressed;

				if (p == NULL)
					p = (bfd_byte *) bfd_malloc (4);

				if (p == NULL)
					goto fail_compressed;	

				free (compressed_buffer);
				*ptr = p;
				return TRUE;

				fail_compressed:
					free (compressed_buffer);
					return FALSE;

			case COMPRESS_SECTION_DONE:
				if (p == NULL)
					p = (bfd_byte *) bfd_malloc (4);
					if (p == NULL)
						return FALSE;

					*ptr = p;
					return TRUE;

			default:
				exit(1);
		}
}

bfd_boolean
bfd_convert_section_contents (sec_ptr isec, bfd_byte **ptr)
{
	bfd_byte *contents;
	bfd_boolean use_memmove;

	__NONDET(use_memmove, 1, 0);

	contents = *ptr;

	if (!use_memmove)
		{
			contents = (bfd_byte *) bfd_malloc (4);
			if (contents == NULL)
				return FALSE;
		}

	if (contents)
		__USE(contents);

	return __RANDBOOL;
}


bfd_boolean
bfd_set_section_contents (const void *location)
{
	if (location)
		__USE(location);
	return __RANDBOOL;
}

static void
copy_section (sec_ptr isection)
{

	bfd_byte *memhunk = NULL;

	if (__RANDBOOL)
		{
			/* 
				bfd_get_full_section_contents may return FALSE while
				allocating an object into memhunk.
			*/
			if (!bfd_get_full_section_contents (isection, &memhunk)
					|| !bfd_convert_section_contents (isection, &memhunk))
				{
					/* memory leak */
					return;
				}

			__USE (memhunk);

			if (!bfd_set_section_contents (memhunk))
				{
					/* memory leak */
					return;
				}

			free (memhunk);
		}

	else
		{
			void *memhunk = xmalloc (4);
			memset (memhunk, 0, 4);

			if (!bfd_set_section_contents (memhunk))
				{
					/* memory leak */
					return;
				}

			free (memhunk);
		}
}

int main() {
	srand(time(NULL));

	struct bfd_section sec;
	sec_ptr isection = &sec;
	sec.compress_status = rand () % 3;

	copy_section(isection);

	
}
	
	
