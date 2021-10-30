/*
	buggy parent : f17c8bf
	commit id : c91550fc5d8dae5f1140bca649690fa13e5276e9
*/

#include "../include/common.h"
#include "../include/binutils.h"

#define SO_NAME_MAX_PATH_SIZE 512	/* FIXME: Should be dynamic */
#define XCNEW(T)		((T *) xcalloc (1, sizeof (T)))

typedef int CORE_ADDR;

struct svr4_info
{
  /* Load map address for the main executable.  */
  CORE_ADDR main_lm_addr;
};

struct lm_info
  {
    CORE_ADDR l_addr, l_addr_inferior;
    unsigned int l_addr_p : 1;
    CORE_ADDR lm_addr;
    CORE_ADDR l_ld, l_next, l_prev, l_name;
  };

struct so_list
  {
    struct so_list *next;	/* next structure in linked list */
    struct lm_info *lm_info;
    char so_original_name[SO_NAME_MAX_PATH_SIZE];
    char so_name[SO_NAME_MAX_PATH_SIZE];
  };

void
free_so (struct so_list *so)
{
  xfree (so);
}

static void
do_free_so (void *arg)
{
  struct so_list *so = arg;

  free_so (so);
}

struct cleanup *
make_cleanup_free_so (struct so_list *so)
{
  return make_cleanup (do_free_so, so);
}

static int
svr4_read_so_list (CORE_ADDR lm, CORE_ADDR prev_lm,
		   struct so_list ***link_ptr_ptr, int ignore_first)
{
  struct so_list *first = NULL;
  CORE_ADDR next_lm;

  for (; lm != 0; prev_lm = lm, lm = next_lm)
    {
      struct so_list *new;
      struct cleanup *old_chain;
      int errcode;
      char *buffer;

      new = XCNEW (struct so_list);		/* allocation site */
      old_chain = make_cleanup_free_so (new);

      new->lm_info = lm_info_read (lm);
      if (new->lm_info == NULL)
	{
	  do_cleanups (old_chain);
	  return 0;
	}

      next_lm = new->lm_info->l_next;

      if (new->lm_info->l_prev != prev_lm)
	{
	  warning (_("Corrupted shared library list: %s != %s"),
		   paddress (target_gdbarch (), prev_lm),
		   paddress (target_gdbarch (), new->lm_info->l_prev));
	  do_cleanups (old_chain);
	  return 0;
	}

      /* For SVR4 versions, the first entry in the link map is for the
         inferior executable, so we must ignore it.  For some versions of
         SVR4, it has no name.  For others (Solaris 2.3 for example), it
         does have a name, so we can no longer use a missing name to
         decide when to ignore it.  */
      if (ignore_first && new->lm_info->l_prev == 0)
	{
	  struct svr4_info *info = get_svr4_info ();

	  first = new;
	  info->main_lm_addr = new->lm_info->lm_addr;
	  do_cleanups (old_chain);	/* freed */
	  continue;
	}

      /* Extract this shared object's name.  */
      target_read_string (new->lm_info->l_name, &buffer,
			  SO_NAME_MAX_PATH_SIZE - 1, &errcode);
      if (errcode != 0)
	{
	  /* If this entry's l_name address matches that of the
	     inferior executable, then this is not a normal shared
	     object, but (most likely) a vDSO.  In this case, silently
	     skip it; otherwise emit a warning. */
	  if (first == NULL
	      || new->lm_info->l_name != first->lm_info->l_name)	/* use-after-free */
	    warning (_("Can't read pathname for load map: %s."),
		     safe_strerror (errcode));
	  do_cleanups (old_chain);
	  continue;
	}

      strncpy (new->so_name, buffer, SO_NAME_MAX_PATH_SIZE - 1);
      new->so_name[SO_NAME_MAX_PATH_SIZE - 1] = '\0';
      strcpy (new->so_original_name, new->so_name);
      xfree (buffer);

      /* If this entry has no name, or its name matches the name
	 for the main executable, don't include it in the list.  */
      if (! new->so_name[0] || match_main (new->so_name))
	{
	  do_cleanups (old_chain);
	  continue;
	}

      discard_cleanups (old_chain);
      new->next = 0;
      **link_ptr_ptr = new;
      *link_ptr_ptr = &new->next;
    }

  return 1;
}

int main(int argc, char **argv)
{
	struct so_list **link_ptr;
	svr4_read_so_list(argc, argc - 1, &link_ptr, 1); 	
	return 0;
}

