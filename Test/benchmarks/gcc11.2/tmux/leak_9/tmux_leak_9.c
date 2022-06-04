/*
	buggy parent : 69b7c49 
	commit id : 6daf06b1ad61f67e9f7780d787451b9b5f82dd43
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>

#include "../stdio.h"
#include "../tmux.h"

struct options	 global_options;	/* server options */

/* Client list. */
struct clients	 clients;
struct clients	 dead_clients;

int		 server_fd;
int		 server_shutdown;
struct event	 server_ev_accept;
struct event	 server_ev_second;

struct paste_stack global_buffers;

/* Arguments tree comparison function. */
int
args_cmp(struct args_entry *a1, struct args_entry *a2)
{
    return (a1->flag - a2->flag);
}

RB_GENERATE(args_tree, args_entry, entry, args_cmp);


void *
xrealloc(void *oldptr, size_t nmemb, size_t size)
{
	size_t	 newsize = nmemb * size;
	void	*newptr;

	if (newsize == 0)
		fatalx("zero size%s", "");
	if (1 << 30 / nmemb < size)
		fatalx("nmemb * size > SIZE_MAX%s", "");
	if ((newptr = realloc(oldptr, newsize)) == NULL)
		fatal("xrealloc failed%s", "");

	return (newptr);
}

/* Get the top item on the stack. */
struct paste_buffer *
paste_get_top(struct paste_stack *ps)
{
	if (ARRAY_LENGTH(ps) == 0)
		return (NULL);
	return (ARRAY_FIRST(ps));
}

/* Get an item by its index. */
struct paste_buffer *
paste_get_index(struct paste_stack *ps, u_int idx)
{
	if (idx >= ARRAY_LENGTH(ps))
		return (NULL);
	return (ARRAY_ITEM(ps, idx));
}

/* Free the top item on the stack. */
int
paste_free_top(struct paste_stack *ps)
{
	struct paste_buffer	*pb;

	if (ARRAY_LENGTH(ps) == 0)
		return (-1);

	pb = ARRAY_FIRST(ps);
	ARRAY_REMOVE(ps, 0);

	free(pb->data);
	free(pb);

	return (0);
}

/* Free an item by index. */
int
paste_free_index(struct paste_stack *ps, u_int idx)
{
	struct paste_buffer	*pb;

	if (idx >= ARRAY_LENGTH(ps))
		return (-1);

	pb = ARRAY_ITEM(ps, idx);
	ARRAY_REMOVE(ps, idx);

	free(pb->data);
	free(pb);

	return (0);
}
/*
 * Add an item onto the top of the stack, freeing the bottom if at limit. Note
 * that the caller is responsible for allocating data.
 */
void
paste_add(struct paste_stack *ps, char *data, size_t size, u_int limit)
{
	struct paste_buffer	*pb;

	if (size == 0)
		return;

	while (ARRAY_LENGTH(ps) >= limit) {
		pb = ARRAY_LAST(ps);
		free(pb->data);
		free(pb);
		ARRAY_TRUNC(ps, 1);
	}

	pb = xmalloc(sizeof *pb);
	ARRAY_INSERT(ps, 0, pb);

	pb->data = data;
	pb->size = size;
}


/*
 * Replace an item on the stack. Note that the caller is responsible for
 * allocating data.
 */
int
paste_replace(struct paste_stack *ps, u_int idx, char *data, size_t size)
{
	struct paste_buffer	*pb;

	if (size == 0)
		return (0);				/* memory leak */

	if (idx >= ARRAY_LENGTH(ps))
		return (-1);

	pb = ARRAY_ITEM(ps, idx);
	free(pb->data);

	pb->data = data;
	pb->size = size;

	return (0);
}

void
cmd_load_buffer_callback(struct client *c, int closed, void *data)
{
	int	*buffer = data;
	char	*pdata;
	size_t	 psize;
	u_int	 limit;

	if (!closed)
		return;
	c->stdin_callback = NULL;

	c->references--;
	if (c->flags & CLIENT_DEAD)
		return;

	if (psize == 0 || (pdata = malloc(psize + 1)) == NULL) {	/* allocation site */
		free(data);
		goto out;
	}
	pdata[psize] = '\0';

	if (*buffer == -1)
		paste_add(&global_buffers, pdata, psize, limit);
	else if (paste_replace(&global_buffers, *buffer, pdata, psize) != 0) {
		printf("no buffer %d\n", *buffer); 
		/* memory leak */
	}

	free(data);

out:
	printf("cmdq_continue");
}

/* Find a flag in the arguments tree. */
struct args_entry *
args_find(struct args *args, u_char ch)
{
    struct args_entry   entry;

	entry.flag = ch;
    return (RB_FIND(args_tree, &args->tree, &entry));
}



/* Return if an argument is present. */
int
args_has(struct args *args, u_char ch) 
{
    return (args_find(args, ch) == NULL ? 0 : 1); 
}


int main (int argc, char **argv)
{
	struct client c;
	struct paste_buffer *pb;
	int closed;
	void *data;
	int buffer;

	struct args *args = calloc(argc, sizeof *args);
	for(int i = 0; i < argc; i++)
		args->argv[i] = argv[i];

	global_buffers.list = malloc(sizeof(struct paste_buffer));
	while(getchar()) 
		cmd_load_buffer_callback(&c, closed, data);

	while(getchar())
	{
		if(!args_has(args, 'b'))
			buffer = -1;
		else 
			scanf("%d", &buffer);
		
		if(buffer == -1)
			pb = paste_get_top(&global_buffers);
		else
			pb = paste_get_index(&global_buffers, buffer);
	
		if(pb != NULL)
			printf("buffer : %s\n", pb->data);

		if(args_has(args, 'd')) {
			if(buffer == -1)
				paste_free_top(&global_buffers);
			else
				paste_get_index(&global_buffers, buffer);
		}
	}
	ARRAY_FREE(&global_buffers);
	free(args);
}


