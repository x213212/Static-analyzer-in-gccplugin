/*
	buggy parent : 9c98104
	commit id : 978b9495b78054b76052a09064cae8c94a58b93e
*/

#include "../include/common.h"
#include "../include/binutils.h"

struct attach_command_continuation_args
{
  char *args;
  int from_tty;
  int async_exec;
};

char *
savestring (const char *ptr, size_t len)
{
  char *p = (char *) xmalloc (len + 1);	/* allocation site */

  memcpy (p, ptr, len);
  p[len] = 0;
  return p;
}

static char *
strip_bg_char (const char *args, int *bg_char_p)
{
  const char *p;

  if (args == NULL || *args == '\0')
    {
      *bg_char_p = 0;
      return NULL;
    }

  p = args + strlen (args);
  if (p[-1] == '&')
    {
      p--;
      while (p > args && isspace (p[-1]))
	p--;

      *bg_char_p = 1;
      if (p != args)
	return savestring (args, p - args);
      else
	return NULL;
    }

  *bg_char_p = 0;
  return xstrdup (args);	/* allocation site */
}

void
attach_command (char *args, int from_tty)
{
  int async_exec;
  struct cleanup *args_chain;
  struct target_ops *attach_target;

  /* Clean up any leftovers from other runs.  Some other things from
     this function should probably be moved into target_pre_inferior.  */

  args = strip_bg_char (args, &async_exec);
  args_chain = make_cleanup (free, args);

  attach_target = find_attach_target ();

  /* to_attach should push the target, so after this point we
     shouldn't refer to attach_target again.  */
  attach_target = NULL;

  /* Done with ARGS.  */
  do_cleanups (args_chain);	/* freed */

  /* Some system don't generate traps when attaching to inferior.
     E.g. Mach 3 or GNU hurd.  */
  if (target_can_async_p ())
	{
	  /* sync_execution mode.  Wait for stop.  */
	  struct attach_command_continuation_args *a;

	  a = xmalloc (sizeof (*a));
	  a->args = xstrdup (args);	/* use-after-free */
	  a->from_tty = from_tty;
	  a->async_exec = async_exec;
	  return;
	}

  attach_command_post_wait (args, from_tty, async_exec);
}

int main(int argc, char **argv)
{
	for(int i = 1; i < argc; i++)
		attach_command (argv[i], i);
	return 0;
}

