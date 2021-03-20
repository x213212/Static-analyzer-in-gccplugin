/*
	gdb/symfile.c
	buggy parent: 8733d1b
	commit id: 7e960b208fdec3205dd64747ed8b6a142f75e51e
*/

#include "../include/common.h"


#define bfd_set_cacheable(abfd,bool) (((abfd)->cacheable = bool), TRUE)

typedef int bfd;

int remote_filename_p (const char *filename)
{
	return strncmp (filename, "remote:", 7) == 0;
}

int
openp (const char *path, const char *name,
		char **filename_opened)
{
	if (filename_opened)
		{
			*filename_opened = strdup(name); /* allocation site */
		}
		
	return __RANDBOOL;
}


void error (const char *msg, const char *s) {
	printf("%s: %s\n", msg, s);
}

bfd* gdb_bfd_open(const char *name) {
	__USE(name);

	if (__RANDBOOL) {
		bfd *abfd = malloc(sizeof(bfd));
		return abfd;
	} else {
		return NULL;
	}
}

bfd* remote_bfd_open(const char *name) {
	return gdb_bfd_open(name);
}
	
bfd* symfile_bfd_open (char *name)
{
	bfd *sym_bfd;
	char *absolute_name;
	int desc;

	if (remote_filename_p (name))
		{
			sym_bfd = remote_bfd_open (name);
			if (!sym_bfd)
				error (("can't open to read symbols"), name);

			return sym_bfd;
		}

	name = strdup (name);	/* allocation site */

	desc = openp (getenv ("PATH"), name, &absolute_name);

	if (desc < 0)
		{
			make_cleanup (free, name);
			error ("", name);
		}

	free (name);
	name = absolute_name;
	make_cleanup (free, name);			/* double-free */
	
	sym_bfd = gdb_bfd_open (name);
	if (!sym_bfd)
		{
			/* when sym_bfd is NULL, absolute_name freed twice at cleanups */
			make_cleanup (free, name);	/* double-free */
			error (("can't open to read symbols"), name);
		}

	/* This statement may incur null dereference of sym_bfd */
	//bfd_set_cacheable (sym_bfd, 1);

	return sym_bfd;
}
			
int main() {
	srand(time(NULL));

	struct cleanup *old_chain = SENTINEL_CLEANUP;
	symfile_bfd_open("test");

	do_cleanups(old_chain); 
}
