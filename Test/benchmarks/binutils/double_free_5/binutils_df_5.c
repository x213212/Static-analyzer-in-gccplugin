/*
	buggy parent: e4b0fe2
	commit id: ec32282347dc5dbd1d58cbc16427e2e24ca6b14c
*/

#include "../include/common.h"

typedef char symbol;

struct badness_vector
{
	int length;
	int *rank;
};

static int
find_oload_champ (struct symbol **oload_syms,
		struct badness_vector **oload_champ_bv)
{
	int ret;
	__USE(oload_syms)

	struct badness_vector *bv ;

	bv = malloc(sizeof(struct badness_vector));
	*oload_champ_bv = bv;

	ret = -1;

	return ret;
}

static int
find_oload_champ_namespace_loop (
		const char *qualified_name,
		int namespace_len,
		struct symbol ***oload_syms,
		struct badness_vector **oload_champ_bv,
		int *oload_champ)
{
	int next_namespace_len = namespace_len;
	int searched_deeper = 0;
	static int static_calls;
	int new_oload_champ;
  struct cleanup *old_cleanups;

  struct symbol **new_oload_syms;
  struct badness_vector *new_oload_champ_bv;

	static int static_call_num;
	static_call_num++;
	int call_num = static_call_num;

	int name_len = strlen(qualified_name);
	
  /* Initialize these to values that can safely be xfree'd.  */
  *oload_syms = NULL;
  *oload_champ_bv = NULL;

	if ((next_namespace_len < name_len) && qualified_name[next_namespace_len] == ':')
	{
		next_namespace_len += 3;
		searched_deeper = 1;

		if (find_oload_champ_namespace_loop (qualified_name, next_namespace_len,
					oload_syms, oload_champ_bv,
					oload_champ))
			{
				return 1;
			}
	}

  old_cleanups = make_cleanup (free, *oload_syms);			/* double-free */
  old_cleanups = make_cleanup (free, *oload_champ_bv);

	new_oload_syms = malloc(100 * sizeof(struct symbol *));	/* allocation site */

	if (!new_oload_syms)
		exit(1);
	new_oload_syms[0] = NULL;

  new_oload_champ = find_oload_champ (new_oload_syms, &new_oload_champ_bv);

	if (new_oload_champ != -1 || call_num == 1)
		{
			*oload_syms = new_oload_syms;
			*oload_champ = new_oload_champ;
			*oload_champ_bv = new_oload_champ_bv;
			do_cleanups (old_cleanups);
			return 1;
		}
	else if (searched_deeper)
		{
			free (new_oload_syms);							
			free (new_oload_champ_bv);
			discard_cleanups (old_cleanups);
			return 0;
		}
	else
		{
      *oload_syms = new_oload_syms;
      *oload_champ = new_oload_champ;
      *oload_champ_bv = new_oload_champ_bv;
			discard_cleanups (old_cleanups);
			return 0;
		}
}

int main() {
	symbol **symvec[10];
	struct badness_vector *bvvec[10];
	int oload_champ;
	const char *namespace = "A::B::C";

	find_oload_champ_namespace_loop(namespace, 1, &symvec, &bvvec, &oload_champ);
	do_cleanups(NULL);
}
	
