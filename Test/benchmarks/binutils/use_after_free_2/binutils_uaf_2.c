/*
	buggy parent: 62a5222
	commit id : 909e4e716c4d77e33357bbe9bc902bfaf2e1af24
*/

#include "../include/common.h"
#include "../include/binutils.h"

#define ARMAG  "!<arch>\012"	/* For COFF and a.out archives.  */
#define ARMAGB "!<bout>\012"	/* For b.out archives.  */
#define ARMAGT "!<thin>\012"	/* For thin archives.  */
#define SARMAG 8
#define ARFMAG "`\012"

#define bfd_is_thin_archive(abfd) ((abfd)->is_thin_archive)
typedef struct bfd_target
{
  char *name;
}bfd_target;

typedef enum bfd_format
{
  bfd_unknown = 0,	/* File format is unknown.  */
  bfd_object,		/* Linker/assembler/compiler output.  */
  bfd_archive,		/* Object archive file.  */
  bfd_core,		/* Core dump.  */
  bfd_type_end		/* Marks the end; don't use it!  */
}
bfd_format;

struct bfd
{
  const char *filename;
  const struct bfd_target *xvec;
  void *iostream;
  int format;
  unsigned int opened_once : 1;
  unsigned int is_thin_archive : 1;
  struct bfd *my_archive;      /* The containing archive BFD.  */
  struct bfd *archive_next;    /* The next BFD in the archive.  */
  struct bfd *archive_head;    /* The first BFD in the archive.  */
  struct bfd *nested_archives; 
  unsigned int target_defaulted : 1;
};

const bfd_target *
bfd_generic_archive_p (bfd *abfd)
{
  struct artdata *tdata_hold;
  char armag[SARMAG + 1];
  bfd_size_type amt;

  if (bfd_bread (armag, SARMAG, abfd) != SARMAG)
    {
      if (bfd_get_error () != bfd_error_system_call)
	bfd_set_error (bfd_error_wrong_format);
      return NULL;
    }

  bfd_is_thin_archive (abfd) = (strncmp (armag, ARMAGT, SARMAG) == 0);

  if (strncmp (armag, ARMAG, SARMAG) != 0
      && strncmp (armag, ARMAGB, SARMAG) != 0
      && ! bfd_is_thin_archive (abfd))
    return NULL;

  return abfd->xvec;
}

bfd *
bfd_fopen (const char *filename, const char *target, const char *mode, int fd)
{
  bfd *nbfd;
  const bfd_target *target_vec;

  nbfd = _bfd_new_bfd ();	/* allocation site */ 
  if (nbfd == NULL)
    {
      return NULL;
    }

  target_vec = bfd_find_target (target, nbfd);
  if (target_vec == NULL)
    {
      if (fd != -1)
	close (fd);
      _bfd_delete_bfd (nbfd);
      return NULL;
    }

  /* OK, put everything where it belongs.  */

  nbfd->filename = xstrdup (filename);

  return nbfd;
}


bfd *
bfd_openr (const char *filename, const char *target)
{
  return bfd_fopen (filename, target, "r", -1);
}

bfd_boolean
_bfd_archive_close_and_cleanup (bfd *abfd)
{
  if (bfd_read_p (abfd) && abfd->format == bfd_archive)
    {
      bfd *nbfd;
      bfd *next;

      /* Close nested archives (if this bfd is a thin archive).  */
      for (nbfd = abfd->nested_archives; nbfd; nbfd = next)
	{
	  next = nbfd->archive_next;
	  bfd_close (nbfd);
	}

    }
  return TRUE;
}
static bfd *
open_nested_file (const char *filename, bfd *archive)
{
  const char *target;
  bfd *n_bfd;

  target = NULL;
  if (!archive->target_defaulted)
    target = archive->xvec->name;
  n_bfd = bfd_openr (filename, target);
  if (n_bfd != NULL)
    {
      n_bfd->my_archive = archive;
    }
  return n_bfd;
}

static bfd *
find_nested_archive (const char *filename, bfd *arch_bfd)
{
  bfd *abfd;

  /* PR 15140: Don't allow a nested archive pointing to itself.  */
  if (filename_cmp (filename, arch_bfd->filename) == 0)
    {
      bfd_set_error (bfd_error_malformed_archive);
      return NULL;
    }

  for (abfd = arch_bfd->nested_archives;
       abfd != NULL;
       abfd = abfd->archive_next)
    {
      if (filename_cmp (filename, abfd->filename) == 0)
	return abfd;
    }
  abfd = open_nested_file (filename, arch_bfd);
  if (abfd)
    {
      abfd->archive_next = arch_bfd->nested_archives;
      arch_bfd->nested_archives = abfd;
    }
  return abfd;
}

int main (int argc, char **argv)
{
  bfd *archive, *nbfd;
  bfd *last, *next;

  if (argc != 2)
    die ("usage: bfdtest1 <archive>%s", "");

  archive = bfd_openr (argv[0], NULL);
  if (archive == NULL)
    die ("no such archive%s", "");
  
  for(int i = 1; i < argc; i++)
  {
	nbfd = find_nested_archive(argv[i], archive);
	__USE(nbfd);
  }
	
  if (!bfd_check_format (archive, bfd_archive))
    {
      bfd_close (archive);
      die ("bfd_check_format%s", "");
    }

  _bfd_archive_close_and_cleanup (archive);

  bfd_generic_archive_p (archive);

  if(archive->format != bfd_unknown)
  {
    bfd *next;
  	for (nbfd = archive->nested_archives; nbfd; nbfd = nbfd->archive_next)
	  __USE(nbfd);
  }	  
  if (!bfd_close (archive))
    die ("bfd_close%s", "");

  return 0;
}
