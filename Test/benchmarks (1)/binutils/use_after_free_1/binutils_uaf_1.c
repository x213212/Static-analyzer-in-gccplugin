/*
    commit id : 242f30e
    gas/config/tc-risvc.c
*/

#include "../include/common.h"
#include "../include/binutils.h"

struct riscv_subset
{
  const char *name;

  struct riscv_subset *next;
};

static struct riscv_subset *riscv_subsets;

static unsigned xlen = 0; /* width of an x-register */
static unsigned abi_xlen = 0; /* width of a pointer in the ABI */

#define LOAD_ADDRESS_INSN (abi_xlen == 64 ? "ld" : "lw")
#define ADD32_INSN (xlen == 64 ? "addiw" : "addi")

static unsigned elf_flags = 0;

/* This is the set of options which the .option pseudo-op may modify.  */

struct riscv_set_options
{
  int pic; /* Generate position-independent code.  */
  int rvc; /* Generate RVC code.  */
  int relax; /* Emit relocs the linker is allowed to relax.  */
};

static struct riscv_set_options riscv_opts =
{
  0,	/* pic */
  0,	/* rvc */
  1,	/* relax */
};


static void
riscv_add_subset (const char *subset)
{
    char c = *subset;
    struct riscv_subset *s = xmalloc (sizeof *s);
    s->name = xstrdup (subset);
    s->next = riscv_subsets;
    riscv_subsets = s;
}

static void
riscv_set_arch (const char *s)
{
  const char *all_subsets = "imafdc";
  const char *extension = NULL;
  const char *p = s;

  if (strncmp (p, "rv32", 4) == 0)
    {
      xlen = 32;
      p += 4;
    }
  else if (strncmp (p, "rv64", 4) == 0)
    {
      xlen = 64;
      p += 4;
    }
  else
    as_fatal ("-march=%s: ISA string must begin with rv32 or rv64", s);

  switch (*p)
    {
      case 'i':
	break;

      case 'g':
	p++;
	for ( ; *all_subsets != 'c'; all_subsets++)
	  {
	    const char subset[] = {*all_subsets, '\0'};
	    riscv_add_subset (subset);
	  }
	break;

      default:
	as_fatal ("-march=%s: first ISA subset must be `i' or `g'", s);
    }

  while (*p)
    {
      if (*p == 'x')
	{
	  char *subset = xstrdup (p), *q = subset;  /* allocation site */

	  while (*++q != '\0' && *q != '_')
	    ;
	  *q = '\0';

	  if (extension) {
	    as_fatal ("-march=%s: only one non-standard extension is supported", extension); /* use-after-free */
		}

	  extension = subset;
	  riscv_add_subset (subset);
	  p += strlen (subset);
	  free (subset);
	}
      else if (*p == '_')
	p++;
      else if ((all_subsets = strchr (all_subsets, *p)) != NULL)
	{
	  const char subset[] = {*p, 0};
	  riscv_add_subset (subset);
	  all_subsets++;
	  p++;
	}
      else if (*p == 'q')
	{
	  const char subset[] = {*p, 0};
	  riscv_add_subset (subset);
	  p++;
	}
      else
	as_fatal ("-march=%s: unsupported ISA subset `%c'", *p);
    }
}

int getopt(char *optargv)
{
        if(*optargv == '0')
                return 0;
        return 1;
}

int main(int argc, char **argv)
{
	srand(time(NULL));

	while (__RANDBOOL) {
		char *string = "rv32ix_x";
		riscv_set_arch(string);
	}
}


