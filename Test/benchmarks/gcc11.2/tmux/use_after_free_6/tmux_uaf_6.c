/*
	buggy parent : 64b5f39
	commit id : fbb030d
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../tree.h"
#include "../array.h"

/* Window structure. */
struct window {
	char		*name;

	u_int		 sx;
	u_int		 sy;

	int		 flags;
#define WINDOW_BELL 0x1
#define WINDOW_HIDDEN 0x2
#define WINDOW_ACTIVITY 0x4
#define WINDOW_CONTENT 0x8
#define WINDOW_REDRAW 0x10

	u_int		 references;
};
ARRAY_DECL(windows, struct window *);

/* Entry on local window list. */
struct winlink {
	int		 idx;
	struct window	*window;

	RB_ENTRY(winlink) entry;
	TAILQ_ENTRY(winlink) sentry;
};
RB_HEAD(winlinks, winlink);
TAILQ_HEAD(winlink_stack, winlink);


struct session {
	char		*name;

	u_int		 sx;
	u_int		 sy;

	struct winlink	*curw;
	struct winlink_stack lastw;
	struct winlinks	 windows;

#define SESSION_UNATTACHED 0x1	/* not attached to any clients */
#define SESSION_DEAD 0x2
	int		 flags;

	int		 references;

	TAILQ_ENTRY(session) gentry;
};
ARRAY_DECL(sessions, struct session *);


/* Global session list. */
struct sessions	sessions;
struct sessions dead_sessions;

/* Global window list. */
struct windows windows;

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

struct session *
session_find(const char *name)
{
	struct session	*s;
	u_int		 i;

	for (i = 0; i < ARRAY_LENGTH(&sessions); i++) {
		s = ARRAY_ITEM(&sessions, i);
		if (s != NULL && strcmp(s->name, name) == 0)
			return (s);
	}

	return (NULL);
}


int
winlink_cmp(struct winlink *wl1, struct winlink *wl2)
{
	return (wl1->idx - wl2->idx);
}

RB_GENERATE(winlinks, winlink, entry, winlink_cmp);

struct winlink *
winlink_find_by_window(struct winlinks *wwl, struct window *w)
{
	struct winlink	*wl;

	RB_FOREACH(wl, winlinks, wwl) {
		if (wl->window == w)
			return (wl);
	}

	return (NULL);
}

struct winlink *
winlink_find_by_index(struct winlinks *wwl, int idx)
{
	struct winlink	wl;

	if (idx < 0)
		fatalx("bad index%s\n", "");

	wl.idx = idx;
	return (RB_FIND(winlinks, wwl, &wl));
}

int
winlink_next_index(struct winlinks *wwl, int idx)
{
	int	i;

	i = idx;
	do {
		if (winlink_find_by_index(wwl, i) == NULL)
			return (i);
		if (i == INT_MAX)
			i = 0;
		else
			i++;
	} while (i != idx);
	return (-1);
}

u_int
winlink_count(struct winlinks *wwl)
{
	struct winlink	*wl;
	u_int		 n;

	n = 0;
	RB_FOREACH(wl, winlinks, wwl)
		n++;

	return (n);
}

struct winlink *
winlink_add(struct winlinks *wwl, struct window *w, int idx)
{
	struct winlink	*wl;

	if (idx < 0) {
		if ((idx = winlink_next_index(wwl, -idx - 1)) == -1)
			return (NULL);
	} else if (winlink_find_by_index(wwl, idx) != NULL)
		return (NULL);

	wl = xmalloc(sizeof *wl);	/* allocation site */
	wl->idx = idx;
	wl->window = w;
	RB_INSERT(winlinks, wwl, wl);

	w->references++;

	return (wl);
}

void
window_destroy(struct window *w)
{
	u_int	i;

	if (window_index(w, &i) != 0)
		fatalx("index not found%s\n", "");
	ARRAY_SET(&windows, i, NULL);
	while (!ARRAY_EMPTY(&windows) && ARRAY_LAST(&windows) == NULL)
		ARRAY_TRUNC(&windows, 1);

	xfree(w);
}

void
winlink_remove(struct winlinks *wwl, struct winlink *wl)
{
	struct window	*w = wl->window;

	RB_REMOVE(winlinks, wwl, wl);
	xfree(wl);

	if (w->references == 0)
		fatal("bad reference count%s\n", "");
	w->references--;
	if (w->references == 0)
		window_destroy(w);
}

struct winlink *
winlink_next(struct winlinks *wwl, struct winlink *wl)
{
	return (RB_NEXT(winlinks, wwl, wl));
}

struct winlink *
winlink_previous(struct winlinks *wwl, struct winlink *wl)
{
	return (RB_PREV(winlinks, wwl, wl));
}

void
winlink_stack_remove(struct winlink_stack *stack, struct winlink *wl)
{
	struct winlink	*wl2;

	if (wl == NULL)
		return;
	
	TAILQ_FOREACH(wl2, stack, sentry) {
		if (wl2 == wl) {
			TAILQ_REMOVE(stack, wl, sentry);
			return;
		}
	}
}
void
winlink_stack_push(struct winlink_stack *stack, struct winlink *wl)
{
	if (wl == NULL)
		return;

	winlink_stack_remove(stack, wl);
	TAILQ_INSERT_HEAD(stack, wl, sentry);
}

int
window_index(struct window *s, u_int *i)
{
	for (*i = 0; *i < ARRAY_LENGTH(&windows); (*i)++) {
		if (s == ARRAY_ITEM(&windows, *i))
			return (0);
	}
	return (-1);
}

/* Find session index. */
int
session_index(struct session *s, u_int *i)
{
	for (*i = 0; *i < ARRAY_LENGTH(&sessions); (*i)++) {
		if (s == ARRAY_ITEM(&sessions, *i))
			return (0);
	}
	return (-1);
}

/* Move session to next window. */
int
session_next(struct session *s, int activity)
{
	struct winlink	*wl;

	if (s->curw == NULL)
		return (-1);

	wl = winlink_next(&s->windows, s->curw);
	
	if (wl == NULL) {
		wl = RB_MIN(winlinks, &s->windows);
	}
	if (wl == s->curw)
		return (1);
	winlink_stack_remove(&s->lastw, wl);
	winlink_stack_push(&s->lastw, s->curw);
	s->curw = wl;
	return (0);
}

/* Destroy a session. */
void
session_destroy(struct session *s)
{
	u_int	i;

	printf("session %s destroyed\n", s->name);

	if (session_index(s, &i) != 0)
		fatalx("session not found%s\n", "");
	ARRAY_SET(&sessions, i, NULL);
	while (!ARRAY_EMPTY(&sessions) && ARRAY_LAST(&sessions) == NULL)
		ARRAY_TRUNC(&sessions, 1);

	while (!TAILQ_EMPTY(&s->lastw))
		winlink_stack_remove(&s->lastw, TAILQ_FIRST(&s->lastw));
	while (!RB_EMPTY(&s->windows))
		winlink_remove(&s->windows, RB_ROOT(&s->windows));

	xfree(s->name);
	xfree(s);
}

struct window *
window_create1(u_int sx, u_int sy)
{
	struct window	*w;
	u_int		 i;

	w = xmalloc(sizeof *w);
	w->name = NULL;
	w->flags = 0;

	w->sx = sx;
	w->sy = sy;

	for (i = 0; i < ARRAY_LENGTH(&windows); i++) {
		if (ARRAY_ITEM(&windows, i) == NULL) {
			ARRAY_SET(&windows, i, w);
			break;
		}
	}
	if (i == ARRAY_LENGTH(&windows))
		ARRAY_ADD(&windows, w);
	w->references = 0;

	return (w);
}

/* Attach a window to a session. */
struct winlink *
session_attach(struct session *s, struct window *w, int idx, char **cause)
{
	struct winlink	*wl;

	if ((wl = winlink_add(&s->windows, w, idx)) == NULL)
		printf("index in use: %d\n", idx);
	return (wl);
}

/* Create a new window on a session. */
struct winlink *
session_new(struct session *s,
    const char *name, const char *cmd, int idx, char **cause)
{
	struct window	*w;
	const char	*shell;
	
	u_int		 hlimit;

	w = window_create1(s->sx, s->sy);
	if (w == NULL) {
		return (NULL);
	}
	return (session_attach(s, w, idx, cause));
}

/* Move session to specific window. */
int
session_select(struct session *s, int idx)
{
	struct winlink	*wl;

	wl = winlink_find_by_index(&s->windows, idx);
	if (wl == NULL)
		return (-1);
	if (wl == s->curw)
		return (1);
	winlink_stack_remove(&s->lastw, wl);
	winlink_stack_push(&s->lastw, s->curw);	/* use-after-free */
	s->curw = wl;
	return (0);
}

struct session *
session_create(const char *name, const char *cmd, int idx, u_int sx, u_int sy, char **cause)
{
	struct session	*s;
	u_int		 i;

	s = xmalloc(sizeof *s);
	s->references = 0;
	s->flags = 0;

	s->curw = NULL;
	TAILQ_INIT(&s->lastw);
	RB_INIT(&s->windows);

	s->sx = sx;
	s->sy = sy;

	for (i = 0; i < ARRAY_LENGTH(&sessions); i++) {
		if (ARRAY_ITEM(&sessions, i) == NULL) {
			ARRAY_SET(&sessions, i, s);
			break;
		}
	}
	if (i == ARRAY_LENGTH(&sessions))
		ARRAY_ADD(&sessions, s);
	if(name == NULL)
		fatal("no name%s\n", "");
	s->name = xstrdup(name);

	if (cmd != NULL) {
		if (session_new(s, NULL, cmd, idx, cause) == NULL) {
			session_destroy(s);
			return (NULL);
		}
		session_select(s, RB_ROOT(&s->windows)->idx);
	}
	printf("session %s created\n", s->name);

	return (s);
}

/* Free dead, unreferenced clients and sessions. */
void
server_clean_dead(void)
{
	struct session	*s;
	struct client	*c;
	u_int		 i;

	for (i = 0; i < ARRAY_LENGTH(&dead_sessions); i++) {
		s = ARRAY_ITEM(&dead_sessions, i);
		if (s == NULL || s->references != 0)
			continue;
		ARRAY_SET(&dead_sessions, i, NULL);
		xfree(s);
	}
}

int
server_link_window(struct session *src, struct winlink *srcwl,
    struct session *dst, int dstidx, int killflag, int selectflag, char **cause)
{
	struct winlink		*dstwl;
	struct session_group	*srcsg, *dstsg;

	dstwl = NULL;
	if (dstidx != -1)
		dstwl = winlink_find_by_index(&dst->windows, dstidx);
	if (dstwl != NULL) {
		if (dstwl->window == srcwl->window)
			return (0);
		if (killflag) {
			/*
			 * Can't use session_detach as it will destroy session
			 * if this makes it empty.
			 */
			winlink_stack_remove(&dst->lastw, dstwl);
			winlink_remove(&dst->windows, dstwl);

			/* Force select/redraw if current. */
			if (dstwl == dst->curw) {
				selectflag = 1;
			
			//	dst->curw = NULL; /* developer fix */
			}
		}
	}
	dstidx = dstidx + 1;
	dstwl = session_attach(dst, srcwl->window, dstidx, cause);
	if (dstwl == NULL)
		return (-1);
	
	if (selectflag)
		session_select(dst, dstwl->idx);
	return (0);
}


int main(int argc, char **argv)
{
	struct session *src, *dst;
	struct window *w;
	char *name, *cause;
	int 	i;

	ARRAY_INIT(&windows);
	ARRAY_INIT(&sessions);
	ARRAY_INIT(&dead_sessions);

	src = session_create(argv[0], "cmd", 0, 80, 40, &cause);
	
	if(argc == 0)
		dst = NULL;
	else {
		name = strdup(argv[1]);
		dst = session_create(argv[1], "cmd", 1, 80, 40, &cause);
	}	
	
	if(argc != 0)
		server_link_window(src, src->curw, dst, 1, 1, 0, &cause);
	
	for(i = 0; i < ARRAY_LENGTH(&sessions); i++)
		session_destroy(ARRAY_ITEM(&sessions, i));	

	server_clean_dead();
}




