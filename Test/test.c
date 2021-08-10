/*
    commit id : 3cfd3dd0956fe854a07795de12c1302ecabbd819
*/

#include "common.h"
#include "binutils.h"
#include "bfd.h"

#    define S_IREAD	00400
#    define S_IWRITE	00200
#    define S_IEXEC	00100

#    define S_IRUSR	S_IREAD			/* read, owner */
#    define S_IWUSR	S_IWRITE		/* write, owner */
#    define S_IXUSR	S_IEXEC			/* execute, owner */

#    define S_IRGRP	(S_IREAD  >> 3)		/* read, group */
#    define S_IWGRP	(S_IWRITE >> 3)		/* write, group */
#    define S_IXGRP	(S_IEXEC  >> 3)		/* execute, group */

#    define S_IROTH	(S_IREAD  >> 6)		/* read, other */
#    define S_IWOTH	(S_IWRITE >> 6)		/* write, other */
#    define S_IXOTH	(S_IEXEC  >> 6)		/* execute, other */

#  define S_IRWXU	(S_IRUSR | S_IWUSR | S_IXUSR)
#  define S_IRWXG	(S_IRGRP | S_IWGRP | S_IXGRP)
#  define S_IRWXO	(S_IROTH | S_IWOTH | S_IXOTH)

#define S_IRUGO		(S_IRUSR | S_IRGRP | S_IROTH)
#define S_IWUGO		(S_IWUSR | S_IWGRP | S_IWOTH)
#define S_IXUGO		(S_IXUSR | S_IXGRP | S_IXOTH)


typedef struct {}bfd_target;
const char *output_filename = "a.out";
static bfd_boolean make_thin_archive = FALSE;

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

bfd *bfd_openw(const char *name)
{
	__USE(name);
	
	bfd *nbfd = malloc(sizeof(bfd));
	return nbfd;
}

bfd_boolean *bfd_close(bfd *abfd)
{
	bfd_boolean ret;
	__NONDET(ret, 1, 0);

	free(abfd);
}

char *bfd_get_filename(bfd *iarch)
{
	return iarch->filename;
}

static void
write_archive (bfd *iarch)
{
  bfd *obfd;
  char *old_name, *new_name;
  bfd *contents_head = iarch->archive_next;

  old_name = (char *) xmalloc (strlen (bfd_get_filename (iarch)) + 1);
  strcpy (old_name, bfd_get_filename (iarch));
  new_name = make_tempname (old_name);  /* allocation site */

  if (new_name == NULL)
    bfd_fatal (("could not create temporary file whilst writing archive%s"), "");

  output_filename = new_name;

  obfd = bfd_openw (new_name);

  if (obfd == NULL)
    bfd_fatal ("%s", old_name);

  if (!bfd_close (obfd))
    bfd_fatal ("%s", old_name);

  output_filename = NULL;

  /* We don't care if this fails; we might be creating the archive.  */
  bfd_close (iarch);

  if (smart_rename (new_name, old_name, 0) != 0)
    exit (1);
  free (old_name);
  /* memory leak */
}   

int main()
{
        bfd iarch;
        write_archive(&iarch);
        return 0;
}



