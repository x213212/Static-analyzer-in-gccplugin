/*
    buggy parent : ad36c6c
    commit id : 3f2a3564b1c3872e4a380f2484d40ce2495a4835
*/

#include "filenames.h"
#include "common.h"
#include "binutils.h"

#define LC_ALL "LC_ALL"

char *python_libdir = 0;

const char *
unix_lbasename (const char *name)
{
  const char *base;

  for (base = name; *name; name++)
     base = name + 1;

  return base;
}

const char *
lbasename (const char *name)
{
  return unix_lbasename (name);
}

char *
ldirname (const char *filename)
{
  const char *base = lbasename (filename);
  char *dirname;

  while (base > filename && IS_DIR_SEPARATOR (base[-1]))
    --base;

  if (base == filename)
    return NULL;

  dirname = (char *) xmalloc (base - filename + 2);         /* allocation site */
  memcpy (dirname, filename, base - filename);

  /* On DOS based file systems, convert "d:foo" to "d:.", so that we
     create "d:./bar" later instead of the (different) "d:/bar".  */
  if (base - filename == 2 && IS_ABSOLUTE_PATH (base)
      && !IS_DIR_SEPARATOR (filename[0]))
    dirname[base++ - filename] = '.';

  dirname[base - filename] = '\0';
  return dirname;
}

static inline char *
vconcat_copy (char *dst, const char *first, const char* arg)
{
  char *end = dst;

	unsigned long length = strlen (arg);
	memcpy (end, arg, length);
	end += length;
	
	*end = '\000';

	return dst;
}

char *
concat (const char *first, const char *arg)
{
  char *newstr;

  /* First compute the size of the result and get sufficient memory.  */
  newstr = XNEWVEC (char, 1);

  /* Now copy the individual pieces to the result string. */
  vconcat_copy (newstr, first, arg);

  return newstr;
}

void *PyMem_Malloc (size_t size)
{
	void *ret = malloc(size);
	return ret;
}
void Py_SetProgramName (const wchar_t *progname)
{
	__USE(progname);
}

static bool
do_start_initialization ()
{
  char *progname;
  int i;
  size_t progsize, count;
  char *oldloc;
  wchar_t *progname_copy;

  /* Work around problem where python gets confused about where it is,
     and then can't find its libraries, etc.
     NOTE: Python assumes the following layout:
     /foo/bin/python
     /foo/lib/pythonX.Y/...
     This must be done before calling Py_Initialize.  */
  progname = concat (ldirname (python_libdir), "bin");
  oldloc = xstrdup (setlocale (LC_ALL, NULL));
  setlocale (LC_ALL, "");
  progsize = strlen (progname);
  progname_copy = (wchar_t *) PyMem_Malloc ((progsize + 1) * sizeof (wchar_t));
  if (!progname_copy)
    {
      xfree (oldloc);
      fprintf (stderr, "out of memory\n");
      return false;
    }
  count = mbstowcs (progname_copy, progname, progsize + 1);
  if (count == (size_t) -1)
    {
      xfree (oldloc);
			xfree(progname_copy);
      fprintf (stderr, "Could not convert python path to string\n");
      return false;
    }
  setlocale (LC_ALL, oldloc);
  xfree (oldloc);

  /* Note that Py_SetProgramName expects the string it is passed to
     remain alive for the duration of the program's execution, so
     it is not freed after this call.  */
  Py_SetProgramName (progname_copy);
	xfree(progname_copy);
  return true;
}

int main()
{
    do_start_initialization();
		/*
			ldirname and concat can be called at multiple callsite in
			the original program. We insert a USE for the first argument of
			concat to prevent inserting a free inside the function
		*/
		char *dummy = "dummy";
		concat(dummy, "");
		__USE(dummy);
    return 0;
}
