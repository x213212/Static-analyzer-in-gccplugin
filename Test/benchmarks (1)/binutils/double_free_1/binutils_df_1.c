/*
	buggy parent : a9f42cf
	commit id : 09473be85c13eab0d794e363d898b74d66431d72
*/

#include "../include/common.h"
#define PT_LDINFO 1

typedef int ptid_t;
typedef unsigned char bfd_byte;
typedef bfd_byte gdb_byte;
typedef unsigned long ULONGEST;

enum target_xfer_status
{
  /* Some bytes are transferred.  */
  TARGET_XFER_OK = 1,

  /* No further transfer is possible.  */
  TARGET_XFER_EOF = 0,

  /* The piece of the object requested is unavailable.  */
  TARGET_XFER_UNAVAILABLE = 2,

  /* Generic I/O error.  Note that it's important that this is '-1',
     as we still have target_xfer-related code returning hardcoded
     '-1' on error.  */
  TARGET_XFER_E_IO = -1,

  /* Keep list in sync with target_xfer_status_to_string.  */
};

void *
xrealloc (void *oldmem, size_t size)
{
  void *newmem;

  if (size == 0)
    size = 1;
  if (!oldmem)
    newmem = malloc (size);
  else
    newmem = realloc (oldmem, size);
  if (!newmem)
		exit(1);

  return (newmem);
}

int remote_filename_p (const char *filename)
{
	return strncmp (filename, "remote:", 7) == 0;
}

static int
rs6000_ptrace64 (int req, int id, long long addr, int data, void *buf)
{
	int i = *((int *)addr);
	int ret;
	__NONDET(ret, 1, (-1));
	return ret;
}

static gdb_byte *rs6000_ptrace_ldinfo ()
{
  const int pid = 1;
  int ldi_size = 1024;
  void *ldi = malloc (ldi_size); /* allocation site */
  int rc = -1;

  while (1)
    {
	rc = rs6000_ptrace64 (PT_LDINFO, pid, (unsigned long) ldi, ldi_size, NULL);

      if (rc != -1)
	break; /* Success, we got the entire ld_info data.  */

      /* ldi is not big enough.  Double it and try again.  */
      ldi_size *= 2;
      ldi = xrealloc (ldi, ldi_size);
    }

  return (gdb_byte *) ldi;
}

ULONGEST
rs6000_aix_ld_info_to_xml (const gdb_byte *ldi_buf)
{
	int i = *ldi_buf;
	return i;
}

static enum target_xfer_status rs6000_xfer_shared_libraries
  (const gdb_byte *writebuf, ULONGEST *xfered_len)
{
	gdb_byte *ldi_buf;
	ULONGEST result;
	struct cleanup *cleanup;

  if (writebuf)
    return TARGET_XFER_E_IO;

  ldi_buf = rs6000_ptrace_ldinfo ();
	if (!ldi_buf) exit(1);
  cleanup = make_cleanup (free, ldi_buf);
  result = rs6000_aix_ld_info_to_xml (ldi_buf);

  free (ldi_buf);	/* double-free */
  do_cleanups (cleanup);	/* double-free */

  if (result == 0)
    return TARGET_XFER_EOF;
  else
    {
      *xfered_len = result;
      return TARGET_XFER_OK;
    }
}

int main() {
	time_t t;
	srand(time(&t));

	char buf[10];
	int len;
	gdb_byte *writebuf;
	ULONGEST *xfered_len = &len;
	
	__NONDET(writebuf, buf, NULL);

	rs6000_xfer_shared_libraries(writebuf, xfered_len);
}
