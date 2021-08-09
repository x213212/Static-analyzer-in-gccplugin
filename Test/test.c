/*
	buggy parent : 2f5404b
	commit id : f978cb06dbfbd93dbd52bd39d992f8644b0c639e
*/

#include "common.h"
#include "vec.h"

struct other_sections
{
	char *name;
	int secindex;
};

struct section_addr_info
{
	size_t num_sections;
	struct other_sections other[1];
};

struct sect_opt
{
	const char *name;
	const char *value;
};

void
null_cleanup (void *arg)
{
}

void *
xrealloc (void *oldmem, size_t size)
{
  void *newmem;

  if (size == 0)
    size = 1;

  newmem = realloc (oldmem, size); /* allocation site */

  if (!newmem)
		exit(1);

  return (newmem);
}
	
static void
add_symbol_file_command (char **args)
{
	int section_index = 0;
  int sec_num = 0;
  char *arg;
	int expecting_sec_addr = 0;
  int expecting_sec_name = 0;
  int argcnt = 0;
	int i;

	struct cleanup *my_cleanups = make_cleanup (null_cleanup, NULL);

	struct section_addr_info *section_addrs;
	struct sect_opt *sect_opts = NULL;
	size_t num_sect_opts = 0;

	num_sect_opts = 4;
	sect_opts = xmalloc (sizeof(struct sect_opt) * num_sect_opts); /* allocation site */


	if (args == NULL) exit (1);

	for (arg = args[0]; arg != NULL; arg = args[++argcnt])
		{
			if (argcnt == 0)
				continue;

			else if (argcnt == 1)
				{
					sect_opts[section_index].name = ".text";
					sect_opts[section_index].value = arg;
					if (++section_index >= num_sect_opts)
						{
							num_sect_opts *= 2;
							sect_opts = ((struct sect_opt *)
										xrealloc (sect_opts, num_sect_opts
									* sizeof (struct sect_opt)));
						}
				}
			else
				{
					if (expecting_sec_name)
						{
							sect_opts[section_index].value = arg;
							expecting_sec_addr = 0;
						}
					else if (expecting_sec_addr)
						{
							sect_opts[section_index].value = arg;
							expecting_sec_addr = 0;

							if (++section_index >= num_sect_opts)
								{
									num_sect_opts *= 2;
									sect_opts = ((struct sect_opt *)
												 xrealloc (sect_opts, num_sect_opts
											 * sizeof (struct sect_opt)));
								}
						}
					else if (strcmp (arg, "-s") == 0)
						{
							expecting_sec_name = 1;
							expecting_sec_addr = 1;
						}
					else
						exit (1);
				}
		}
	
		if (section_index < 1)
			exit (1);

		section_addrs = malloc (sizeof(struct section_addr_info));
		make_cleanup (free, section_addrs);
		
		for (i = 0; i < section_index; i++)
			{
				const char *val = sect_opts[i].value;
				const char *sec = sect_opts[i].name;

				/* Here we store the section offsets in the order they were
					 entered on the command line.  */
				section_addrs->other[sec_num].name = (char *) sec;
				sec_num++;
				/* The object's sections are initialized when a
		 call is made to build_objfile_section_table (objfile).
		 This happens in reread_symbols.
		 At this point, we don't know what file type this is,
		 so we can't determine what section names are valid.  */
			}
		section_addrs->num_sections = sec_num;

		do_cleanups (my_cleanups);

		return; /* memory leak */
}

int main ()
{
	const char *args[6] = {"prog", "v", "-s", "idx", "addr", NULL};
	add_symbol_file_command(args);

}