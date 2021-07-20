#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#define FALSE 0
#define TRUE 1
#define __USE(exp) {printf("%d\n", *(int *)exp);}
#define __NONDET(ret, X, Y) {ret = (rand() % 2 == 1 ? X : Y);}
#define __RANDBOOL ((rand() % 2 == 1) ? 1 : 0)

#define die_errno(a, b) die(a, b)
#define die(a, b) fatal(a, b)

#define fatalx(a, b) fatal(a, b)
#define fatal(a, b) { printf(a, b); exit(1); }
#define as_fatal fatal
#define non_fatal(a, b) { printf(a, b); }

#define bfd_fatal(a, b) fatal(a, b)

#define xmalloc(a) malloc(a)
#define xstrdup(a) strdup(a)
#define xcalloc(a, b) calloc(a, b)
#define xfree(a) free(a)

#define reallocarray(ptr, b, c) realloc(ptr, b * c)

typedef unsigned long long uint64_t;
typedef int bool;

bool true = 1;
bool false = 0;

typedef void (make_cleanup_ftype) (void *);

struct cleanup
{
  struct cleanup *next;
  void (*function) (void *);
  void (*free_arg) (void *);
  void *arg;
};

static const struct cleanup sentinel_cleanup = { 0, 0, 0, 0 };

/* Handy macro to use when referring to sentinel_cleanup.  */
#define SENTINEL_CLEANUP ((struct cleanup *) &sentinel_cleanup)

static struct cleanup *cleanup_chain = SENTINEL_CLEANUP;
static struct cleanup *final_cleanup_chain;	/* cleaned up when gdb exits */

void discard_cleanups (struct cleanup *old_chain);
void discard_my_cleanups (struct cleanup **pmy_chain,
		struct cleanup *old_chain);
void discard_final_cleanups (struct cleanup *old_chain);
static void do_my_cleanups (struct cleanup **pmy_chain,
		struct cleanup *old_chain);
void do_cleanups (struct cleanup *old_chain);
static struct cleanup *
make_my_cleanup2 (struct cleanup **pmy_chain, make_cleanup_ftype *function,
		  void *arg,  void (*free_arg) (void *));
static struct cleanup *
make_my_cleanup (struct cleanup **pmy_chain, make_cleanup_ftype *function,
		 void *arg);
struct cleanup *
make_cleanup (make_cleanup_ftype *function, void *arg);

void
discard_cleanups (struct cleanup *old_chain)
{
  discard_my_cleanups (&cleanup_chain, old_chain);
}

void
discard_final_cleanups (struct cleanup *old_chain)
{
  discard_my_cleanups (&final_cleanup_chain, old_chain);
}

void
discard_my_cleanups (struct cleanup **pmy_chain,
		     struct cleanup *old_chain)
{
  struct cleanup *ptr;

  while ((ptr = *pmy_chain) != old_chain)
    {
      *pmy_chain = ptr->next;
      if (ptr->free_arg)
	(*ptr->free_arg) (ptr->arg);
      free (ptr);
    }

		printf("freearg: %x\n", ptr->arg);
}

static void
do_my_cleanups (struct cleanup **pmy_chain,
		struct cleanup *old_chain)
{
  struct cleanup *ptr;

	while ((ptr = *pmy_chain) != old_chain)
    {
      *pmy_chain = ptr->next;	/* Do this first in case of recursion.  */
      (*ptr->function) (ptr->arg);
			printf("cleanup!!!!! =========\n");
			printf("%x\n", ptr->arg);
      if (ptr->free_arg)
				(*ptr->free_arg) (ptr->arg);

			free (ptr);
    }
}

void do_cleanups (struct cleanup *old_chain)
{
  do_my_cleanups (&cleanup_chain, old_chain);
}

static struct cleanup *
make_my_cleanup2 (struct cleanup **pmy_chain, make_cleanup_ftype *function,
		  void *arg,  void (*free_arg) (void *))
{
  struct cleanup *new
    = (struct cleanup *) malloc (sizeof (struct cleanup));
  struct cleanup *old_chain = *pmy_chain;

  new->next = *pmy_chain;
  new->function = function;
  new->free_arg = free_arg;
  new->arg = arg;
  *pmy_chain = new;

	if (old_chain == NULL)
		exit (1);

  return old_chain;
}

static struct cleanup *
make_my_cleanup (struct cleanup **pmy_chain, make_cleanup_ftype *function,
		 void *arg)
{
  return make_my_cleanup2 (pmy_chain, function, arg, NULL);
}

struct cleanup *
make_cleanup (make_cleanup_ftype *function, void *arg)
{
  return make_my_cleanup (&cleanup_chain, function, arg);
}

