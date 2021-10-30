/*
	buggy parent : 565ba00
	commit id : 5cb483680f608b49ab6068bce0d7b006af1dbbfd
*/

#include <stdio.h>
#include <stdlib.h>

#define WINDOW

#include "../stdio.h"
#include "../tmux.h"

#define MAXNAMELEN	32
#define MAXTITLELEN	192

/* Buffer macros. */
#define BUFFER_USED(b) ((b)->size)
#define BUFFER_FREE(b) ((b)->space - (b)->off - (b)->size)
#define BUFFER_IN(b) ((b)->base + (b)->off + (b)->size)
#define BUFFER_OUT(b) ((b)->base + (b)->off)

/* Buffer structure. */
struct buffer {
	u_char	*base;		/* buffer start */
	size_t	 space;		/* total size of buffer */

	size_t	 size;		/* size of data in buffer */
	size_t	 off;		/* offset of data in buffer */
};

/* Window structure. */
struct window {
	char		 name[MAXNAMELEN];

	int		 fd;
	struct buffer	*in;
	struct buffer	*out;

	u_int		 references;

	struct screen	 screen;
};
ARRAY_DECL(windows, struct window *);

/* Global window list. */
struct windows	windows;

void window_destroy(struct window *w);

#define SIZE_MAX (1 << 30)
void *
xrealloc(void *oldptr, size_t nmemb, size_t size)
{
	size_t	 newsize = nmemb * size;
	void	*newptr;

	if (newsize == 0)
                fatalx("zero size%s", "");
        if (SIZE_MAX / nmemb < size)
                fatalx("nmemb * size > SIZE_MAX%s", "");
        if ((newptr = realloc(oldptr, newsize)) == NULL)
		fatal("xrealloc failed%s", "");

        return (newptr);
}

/* Create a buffer. */
struct buffer *
buffer_create(size_t size)
{
	struct buffer	*b;

	if (size == 0)
		fatalx("zero size%s", "");

	b = xcalloc(1, sizeof *b);

	b->base = xmalloc(size);
	b->space = size;

	return (b);
}

/* Destroy a buffer. */
void
buffer_destroy(struct buffer *b)
{
	xfree(b->base);
	xfree(b);
}

/* Find window index in list. */
int
window_index(struct windows *ww, struct window *w, u_int *i)
{
	for (*i = 0; *i < ARRAY_LENGTH(ww); (*i)++) {
		if (w == ARRAY_ITEM(ww, *i))
			return (0);
	}
	return (-1);
}

/* Remove a window from a list. */
void
window_remove(struct windows *ww, struct window *w)
{
	u_int	i;

	if (window_index(ww, w, &i) != 0)
		fatalx("window not found%s", "");
	ARRAY_REMOVE(ww, i);

	w->references--;
	if (w->references == 0) {
		window_destroy(w);			/* freed */
		window_remove(&windows, w);	/* use-after-free */
	}
}

/* Destroy a window. */
void
window_destroy(struct window *w)
{
	close(w->fd);

	buffer_destroy(w->in);
	buffer_destroy(w->out);
	xfree(w);
}

/* Add a window to a list. */
void
window_add(struct windows *ww, struct window *w)
{
	u_int	i;

	if (window_index(ww, NULL, &i) != 0)
		ARRAY_ADD(ww, w);
	else
		ARRAY_SET(ww, i, w);

	w->references++;
}


/* Create a new window. */
struct window *
window_create(const char *cmd, u_int sx, u_int sy)
{
	struct window	*w;
	struct termios	 tio;
	int		 fd, mode;
	char		 pid[16], *ptr, *name;

	w = xmalloc(sizeof *w);
	w->fd = fd;
	w->in = buffer_create(BUFSIZ);
	w->out = buffer_create(BUFSIZ);

	name = xstrdup(cmd);
	if ((ptr = strchr(name, ' ')) != NULL) {
		if (ptr != name && ptr[-1] != '\\')
			*ptr = '\0';
		else {
			while ((ptr = strchr(ptr + 1, ' ')) != NULL) {
				if (ptr[-1] != '\\') {
					*ptr = '\0';
					break;
				}
			}
		}
	}
	strcpy(w->name, name);
	xfree(name);

	window_add(&windows, w);
	w->references = 0;

	return (w);
}

int main()
{
	struct window *w;
	u_int sx;
	u_int sy;
	w = window_create("cmd", sx, sy);
	window_add(&windows, w);

	window_remove(&windows, w);
}
