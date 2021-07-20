/*
    commit id : be74fad95edc8827516e144cf38d135b503249cd
*/

#include "../include/common.h"
#include "../include/binutils.h"

static int mri_mode;

/* This flag distinguishes between ar and ranlib:
   1 means this is 'ranlib'; 0 means this is 'ar'.
   -1 means if we should use argv[0] to decide.  */
extern int is_ranlib;

/* Nonzero means don't warn about creating the archive file if necessary.  */
int silent_create = 0;

/* Nonzero means describe each action performed.  */
int verbose = 0;

/* Nonzero means preserve dates of members when extracting them.  */
int preserve_dates = 0;

/* Nonzero means don't replace existing members whose dates are more recent
   than the corresponding files.  */
int newer_only = 0;

/* Controls the writing of an archive symbol table (in BSD: a __.SYMDEF
   member).  -1 means we've been explicitly asked to not write a symbol table;
   +1 means we've been explicitly asked to write it;
   0 is the default.
   Traditionally, the default in BSD has been to not write the table.
   However, for POSIX.2 compliance the default is now to write a symbol table
   if any of the members are object files.  */
int write_armap = 0;

/* Operate in deterministic mode: write zero for timestamps, uids,
   and gids for archive members and the archive symbol table, and write
   consistent file modes.  */
int deterministic = -1;			/* Determinism indeterminate.  */

/* Nonzero means it's the name of an existing member; position new or moved
   files with respect to this one.  */
char *posname = NULL;

/* Sez how to use `posname': pos_before means position before that member.
   pos_after means position after that member. pos_end means always at end.
   pos_default means default appropriately. For the latter two, `posname'
   should also be zero.  */
enum pos
  {
    pos_default, pos_before, pos_after, pos_end
  } postype = pos_default;

enum operations
  {
    none = 0, del, replace, print_table,
    print_files, extract, move, quick_append
  } operation = none;



/* List of sections to add to the output BFD.  */
static struct section_add *add_sections;

/* List of sections to update in the output BFD.  */
static struct section_add *update_sections;

/* List of sections to dump from the output BFD.  */
static struct section_add *dump_sections;

/* If non-NULL the argument to --add-gnu-debuglink.
   This should be the filename to store in the .gnu_debuglink section.  */
static const char * gnu_debuglink_filename = NULL;

/* Whether to convert debugging information.  */
static bfd_boolean convert_debugging = FALSE;

/* Whether to compress/decompress DWARF debug sections.  */
static enum
{
  nothing = 0,
  compress = 1 << 0,
  compress_zlib = compress | 1 << 1,
  compress_gnu_zlib = compress | 1 << 2,
  compress_gabi_zlib = compress | 1 << 3,
  decompress = 1 << 4
} do_debug_sections = nothing;

/* Whether to change the leading character in symbol names.  */
static bfd_boolean change_leading_char = FALSE;

/* Whether to remove the leading character from global symbol names.  */
static bfd_boolean remove_leading_char = FALSE;

/* Whether to permit wildcard in symbol comparison.  */
static bfd_boolean wildcard = FALSE;

/* True if --localize-hidden is in effect.  */
static bfd_boolean localize_hidden = FALSE;


FILE *saved_script_handle = NULL;
FILE *previous_script_handle = NULL;
bfd_boolean force_make_executable = FALSE;

char *default_target;
const char *output_filename = "a.out";

/* Name this program was invoked by.  */
char *program_name;

/* The prefix for system library directories.  */
const char *ld_sysroot;

/* The canonical representation of ld_sysroot.  */
char *ld_canon_sysroot;
int ld_canon_sysroot_len;

/* Set by -G argument, for targets like MIPS ELF.  */
int g_switch_value = 8;

int status = 0;
int optind = 1;


int filename_cmp (const char *s1, const char *s2)
{
#if !defined(HAVE_DOS_BASED_FILE_SYSTEM) \
    && !defined(HAVE_CASE_INSENSITIVE_FILE_SYSTEM)
  return strcmp(s1, s2);
#else
  for (;;)
    {
      int c1 = *s1;
      int c2 = *s2;

#if defined (HAVE_CASE_INSENSITIVE_FILE_SYSTEM)
      c1 = TOLOWER (c1);
      c2 = TOLOWER (c2);
#endif

#if defined (HAVE_DOS_BASED_FILE_SYSTEM)
      /* On DOS-based file systems, the '/' and the '\' are equivalent.  */
      if (c1 == '/')
        c1 = '\\';
      if (c2 == '/')
        c2 = '\\';
#endif

      if (c1 != c2)
        return (c1 - c2);

      if (c1 == '\0')
        return 0;

      s1++;
      s2++;
    }
#endif
}

/* Return a path for a new temporary file in the same directory
   as file PATH.  */

static char *
template_in_dir (const char *path)
{
#define template "stXXXXXX"
  const char *slash = strrchr (path, '/');
  char *tmpname;
  size_t len;

  if (slash != (char *) NULL)
    {
      len = slash - path;
      tmpname = (char *) xmalloc (len + sizeof (template) + 2);	/* allocation site */
      memcpy (tmpname, path, len);
      tmpname[len++] = '/';
    }
  else
    {
      tmpname = (char *) xmalloc (sizeof (template));	/* allocation site */
      len = 0;
    }

  memcpy (tmpname + len, template, sizeof (template));
  return tmpname;
}

/* Return the name of a created temporary file in the same directory
   as FILENAME.  */

char *
make_tempname (char *filename)
{
  char *tmpname = template_in_dir (filename);
  int fd;

  fd = mkstemp (tmpname);
  if (fd == -1)
    {
      free (tmpname);
      return NULL;
    }
  close (fd);
  return tmpname;
}

void
set_times (const char *destination, const struct stat *statbuf)
{
  int result;

  {
#ifdef HAVE_GOOD_UTIME_H
    struct utimbuf tb;

    tb.actime = statbuf->st_atime;
    tb.modtime = statbuf->st_mtime;
    result = utime (destination, &tb);
#else /* ! HAVE_GOOD_UTIME_H */
#ifndef HAVE_UTIMES
    long tb[2];

    tb[0] = statbuf->st_atime;
    tb[1] = statbuf->st_mtime;
    result = utime (destination, tb);
#else /* HAVE_UTIMES */
    struct timeval tv[2];

    tv[0].tv_sec = statbuf->st_atime;
    tv[0].tv_usec = 0;
    tv[1].tv_sec = statbuf->st_mtime;
    tv[1].tv_usec = 0;
    result = utimes (destination, tv);
#endif /* HAVE_UTIMES */
#endif /* ! HAVE_GOOD_UTIME_H */
  }

  if (result != 0)
    non_fatal (("%s: cannot set time:"), destination);
}

int
smart_rename (const char *from, const char *to, int preserve_dates)
{
  bfd_boolean exists;
  struct stat s;
  int ret = 0;

  exists = lstat (to, &s) == 0;

  /* Use rename only if TO is not a symbolic link and has
     only one hard link, and we have permission to write to it.  */
  if (! exists
      || (!S_ISLNK (s.st_mode)
	  && S_ISREG (s.st_mode)
	  && (s.st_mode & S_IWUSR)
	  && s.st_nlink == 1)
      )
    {
      ret = rename (from, to);
      if (ret == 0)
	{
	  if (exists)
	    {
	      /* Try to preserve the permission bits and ownership of
		 TO.  First get the mode right except for the setuid
		 bit.  Then change the ownership.  Then fix the setuid
		 bit.  We do the chmod before the chown because if the
		 chown succeeds, and we are a normal user, we won't be
		 able to do the chmod afterward.  We don't bother to
		 fix the setuid bit first because that might introduce
		 a fleeting security problem, and because the chown
		 will clear the setuid bit anyhow.  We only fix the
		 setuid bit if the chown succeeds, because we don't
		 want to introduce an unexpected setuid file owned by
		 the user running objcopy.  */
	      chmod (to, s.st_mode & 0777);
	      if (chown (to, s.st_uid, s.st_gid) >= 0)
		chmod (to, s.st_mode & 07777);
	    }
	}
      else
	{
	  /* We have to clean up here.  */
	  non_fatal (("unable to rename '%s';"), to);
	  unlink (from);
	}
    }
  else
    {
      if (ret != 0)
				non_fatal (("unable to copy file '%s';"), to);

      if (preserve_dates)
				set_times (to, &s);
      unlink (from);
    }

  return ret;
}

void unlink_if_ordinary (const char *name)
{
	__USE(name);
}

/* The top-level control.  */

static void
copy_file (const char *input_filename, const char *output_filename,
	   const char *input_target,   const char *output_target,
	   const bfd_arch_info_type *input_arch)
{
  bfd *ibfd;
  char **obj_matching;
  char **core_matching;
  off_t size;

	__NONDET(size, 1, 0);

  if (size < 1)
    {
      if (size == 0)
				non_fatal (("error: the input file '%s' is empty"), input_filename);
      status = 1;
      return;
    }
}
/* Load the file specified in PA, allocating memory to hold the file
           contents, and store a pointer to the allocated memory in the contents
   field of PA.  The size field of PA is also updated.  All errors call
   FATAL.  */

static int
copy_main (int argc, char *argv[])
{
  char *input_filename = NULL;
  char *output_filename = NULL;
  char *tmpname;
  char *input_target = NULL;
  char *output_target = NULL;
  bfd_boolean show_version = FALSE;
  bfd_boolean change_warn = TRUE;
  bfd_boolean formats_info = FALSE;
  int c;
  int statbuf;
  const bfd_arch_info_type *input_arch = NULL;

  input_filename = argv[optind];
  if (optind + 1 < argc)
    output_filename = argv[optind + 1];

  /* If there is no destination file, or the source and destination files
     are the same, then create a temp and rename the result into the input.  */
  if (output_filename == NULL)
    tmpname = make_tempname (input_filename);
  else
    tmpname = output_filename;                  /* stack object */


  if (tmpname == NULL)
    fatal ("warning: could not create temporary file whilst copying '%s'",
	   input_filename);

  copy_file (input_filename, tmpname, input_target, output_target, input_arch);
  if (status == 0)
    {
      if (preserve_dates)
				set_times (tmpname, &statbuf);
      if (tmpname != output_filename)
				status = (smart_rename (tmpname, input_filename, preserve_dates) != 0);
		}
	else
		unlink_if_ordinary (tmpname);

    return 0;                                 /* memory leak */
}

int
main (int argc, char *argv[])
{
  program_name = argv[0];

  copy_main (argc, argv);

  return status;
}

