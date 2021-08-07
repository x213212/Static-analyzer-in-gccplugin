/*
    commit id : 7e960b208fdec3205dd64747ed8b6a142f75e51e
*/

#include "common.h"
#include "binutils.h"

#define DEBUG_SUBDIRECTORY ".debug"
#define LONGEST BFD_HOST_64_BIT
#define ULONGEST BFD_HOST_U_64_BIT
char *debug_file_directory = NULL;
char *gdb_sysroot = 0;


typedef struct {} asection;
typedef double DOUBLEST;
typedef bfd_byte gdb_byte;
struct objfile{
    struct build_id *obfd;
    char *name;
};

char *
lrealpath (const char *filename)
{
  /* This system is a lost cause, just duplicate the filename.  */
  return strdup (filename); /* allocation site */
}
static char *
find_separate_debug_file (struct objfile *objfile)
{
  asection *sect;
  char *basename;
  char *dir;
  char *debugfile;
  char *name_copy;
  char *canon_name;
  bfd_size_type debuglink_size;
  unsigned long crc32;
  int i;
  struct build_id *build_id;

  build_id = build_id_bfd_get (objfile->obfd);
  if (build_id != NULL)
    {
      char *build_id_name;

      build_id_name = build_id_to_debug_filename (build_id);
      xfree (build_id);
      /* Prevent looping on a stripped .debug file.  */
      if (build_id_name != NULL && strcmp (build_id_name, objfile->name) == 0)
        {
	  warning (_("\"%s\": separate debug info file has no debug info"),
		   build_id_name);
	  xfree (build_id_name);
	}
      else if (build_id_name != NULL)
        return build_id_name;
    }

  basename = get_debug_link_info (objfile, &crc32);

  if (basename == NULL)
    return NULL;

  dir = xstrdup (objfile->name);

  /* Strip off the final filename part, leaving the directory name,
     followed by a slash.  Objfile names should always be absolute and
     tilde-expanded, so there should always be a slash in there
     somewhere.  */
  for (i = strlen(dir) - 1; i >= 0; i--)
    {
      if (IS_DIR_SEPARATOR (dir[i]))
	break;
    }
  gdb_assert (i >= 0 && IS_DIR_SEPARATOR (dir[i]));
  dir[i+1] = '\0';

  /* Set I to max (strlen (canon_name), strlen (dir)). */
  canon_name = lrealpath (dir);
  i = strlen (dir);
  if (canon_name && strlen (canon_name) > i)
    i = strlen (canon_name);

  debugfile = alloca (strlen (debug_file_directory) + 1
                      + i
                      + strlen (DEBUG_SUBDIRECTORY)
                      + strlen ("/")
                      + strlen (basename)
                      + 1);

  /* First try in the same directory as the original file.  */
  strcpy (debugfile, dir);
  strcat (debugfile, basename);

  if (separate_debug_file_exists (debugfile, crc32))
    {
      xfree (basename);
      xfree (dir);
      xfree (canon_name);
      return xstrdup (debugfile);
    }

  /* Then try in the subdirectory named DEBUG_SUBDIRECTORY.  */
  strcpy (debugfile, dir);
  strcat (debugfile, DEBUG_SUBDIRECTORY);
  strcat (debugfile, "/");
  strcat (debugfile, basename);

  if (separate_debug_file_exists (debugfile, crc32))
    {
      xfree (basename);
      xfree (dir);
      xfree (canon_name);
      return xstrdup (debugfile);
    }

  /* Then try in the global debugfile directory.  */
  strcpy (debugfile, debug_file_directory);
  strcat (debugfile, "/");
  strcat (debugfile, dir);
  strcat (debugfile, basename);

  if (separate_debug_file_exists (debugfile, crc32))
    {
      xfree (basename);
      xfree (dir);
      xfree (canon_name);
      return xstrdup (debugfile);
    }

  /* If the file is in the sysroot, try using its base path in the
     global debugfile directory.  */
  if (canon_name
      && strncmp (canon_name, gdb_sysroot, strlen (gdb_sysroot)) == 0
      && IS_DIR_SEPARATOR (canon_name[strlen (gdb_sysroot)]))
    {
      strcpy (debugfile, debug_file_directory);
      strcat (debugfile, canon_name + strlen (gdb_sysroot));
      strcat (debugfile, "/");
      strcat (debugfile, basename);

      if (separate_debug_file_exists (debugfile, crc32))
	{
	  xfree (canon_name);	/* double-free */
	  xfree (basename);
	  xfree (dir);
	  xfree (canon_name);	/* double-free */
	  return xstrdup (debugfile);
	}
    }
  
  if (canon_name)
    xfree (canon_name);

  xfree (basename);
  xfree (dir);
  return NULL;
}
int main()
{
    struct objfile objfile;
    find_separate_debug_file(&objfile);
    return 0;
}


