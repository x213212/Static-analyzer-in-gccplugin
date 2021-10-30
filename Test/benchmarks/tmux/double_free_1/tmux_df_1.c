/*
	buggy parent : 577bd45
	commit id : 765a224b3007c4ef617f22091db4c152de15dd86
*/

#include <stdio.h>
#include <stdlib.h>
#include <poll.h>

#define CLIENT
#define SESSION
#define WINDOW
#include "../stdio.h"
#include "../tmux.h"

struct winsize {
	u_int ws_col;
	u_int ws_row;
};

/* Window structure. */
struct window {
	char		*name;

	int		 fd;
	struct buffer	*in;
	struct buffer	*out;

	int		 flags;
#define WINDOW_BELL 0x1

	struct screen	 screen;

	u_int		 references;
};
ARRAY_DECL(windows, struct window *);

/* Client connection. */
struct client {
	char		*tty;

	int		 fd;
	struct buffer	*in;
	struct buffer	*out;

	u_int		 sx;
	u_int		 sy;

#define CLIENT_TERMINAL 0x1
#define CLIENT_PREFIX 0x2
#define CLIENT_HOLD 0x4
	int		 flags;

	struct session	*session;
};
ARRAY_DECL(clients, struct client *);



/* Global session list. */
struct sessions	sessions;	
struct windows windows;

/* Client session. */
struct session {
	char		*name;
	time_t		 tim;

	u_int		 sx;
	u_int		 sy;

	struct winlink *curw;
	struct winlink *lastw;
	struct winlinks windows;

	ARRAY_DECL(, struct winlink *) bells;	/* windows with bells */

#define SESSION_UNATTACHED 0x1	/* not attached to any clients */
	int		 flags;
};
ARRAY_DECL(sessions, struct session *);

struct cmd_entry {
	const char	*name;
	const char	*alias;
	const char	*usage;

#define CMD_STARTSERVER 0x1
#define CMD_NOSESSION 0x2
#define CMD_CANTNEST 0x4
	int		 flags;

	int		 (*parse)(void **, int, char **, char **);
	void		 (*exec)(void *, struct cmd_ctx *);
	void		 (*send)(void *, struct buffer *);
	void	         (*recv)(void **, struct buffer *);
	void		 (*free)(void *);
};

/* Key/command line command. */
struct cmd_ctx {
	struct client  *client;
	struct session *session;

	void		(*print)(struct cmd_ctx *, const char *, ...);
	void		(*error)(struct cmd_ctx *, const char *, ...);

#define CMD_KEY 0x1
	int		flags;
};

/* Buffer structure. */
struct buffer {
	u_char	*base;		/* buffer start */
	size_t	 space;		/* total size of buffer */

	size_t	 size;		/* size of data in buffer */
	size_t	 off;		/* offset of data in buffer */
};

struct cmd_new_window_data {
	char	*name;
	char	*cmd;
	int	 idx;
	int	 flag_detached;
};

/* Key binding. */
struct binding {
	int		 key;
	struct cmd	*cmd;
};
ARRAY_DECL(bindings, struct binding *);

int
winlink_cmp(struct winlink *wl1, struct winlink *wl2)
{
	return (wl1->idx - wl2->idx);
}

RB_GENERATE(winlinks, winlink, entry, winlink_cmp);

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

struct winlink *
winlink_find_by_index(struct winlinks *wwl, int idx)
{
	struct winlink	wl;

	if (idx < 0)
		fatalx("bad index%s", "");

	wl.idx = idx;
	return (RB_FIND(winlinks, wwl, &wl));
}

int
winlink_next_index(struct winlinks *wwl)
{
	u_int	i;

	for (i = 0; i < INT_MAX; i++) {
		if (winlink_find_by_index(wwl, i) == NULL)
			return (i);
	}
	
	fatalx("no free indexes%s", "");
}

struct winlink *
winlink_add(struct winlinks *wwl, struct window *w, int idx)
{
	struct winlink	*wl;

	if (idx == -1)
		idx = winlink_next_index(wwl);
	else if (winlink_find_by_index(wwl, idx) != NULL)
		return (NULL);

	if (idx < 0)
		fatalx("bad index%s","");

	wl = xcalloc(1, sizeof *wl);
	wl->idx = idx;
	wl->window = w;
	RB_INSERT(winlinks, wwl, wl);

	w->references++;

	return (wl);
}

void
winlink_remove(struct winlinks *wwl, struct winlink *wl)
{
	struct window	*w = wl->window;

	RB_REMOVE(winlinks, wwl, wl);
	xfree(wl);

	if (w->references == 0)
		fatal("bad reference count%s", "");
	w->references--;
}

struct winlink *
winlink_next(struct winlinks *wwl, struct winlink *wl)
{
	return (RB_NEXT(winlinks, wwl, wl));
} 

struct winlink *
winlink_previous(struct winlinks *wwl, struct winlink *wl)
{
	struct winlink	*wk;
	int		 idx = wl->idx;

	wk = NULL;
	wl = RB_MIN(winlinks, wwl);
	while (wl != NULL && wl->idx < idx) {
		wk = wl;
		wl = RB_NEXT(winlinks, wwl, wl);
	}
	if (wl == NULL)
		return (NULL);
	return (wk);
} 

/* Find session by name. */
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

/* Destroy a session. */
void
session_destroy(struct session *s)
{
	struct winlink	*wl;
	u_int		 i;

	if (session_index(s, &i) != 0)
		fatalx("session not found%s", "");
	ARRAY_SET(&sessions, i, NULL);
	while (!ARRAY_EMPTY(&sessions) && ARRAY_LAST(&sessions) == NULL)
		ARRAY_TRUNC(&sessions, 1);
	
	while (!RB_EMPTY(&s->windows)) {
		wl = RB_ROOT(&s->windows);
		RB_REMOVE(winlinks, &s->windows, wl);
		winlink_remove(&s->windows, wl);
	}

	xfree(s->name);
	xfree(s);
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

/* Attach a window to a session. */
struct winlink *
session_attach(struct session *s, struct window *w, int idx)
{
	return (winlink_add(&s->windows, w, idx));
}


/* Create a new window on a session. */
struct winlink *
session_new(struct session *s, const char *name, const char *cmd, int idx)
{
	struct window	*w;
	const char	*env[] = { NULL, "TERM=screen", NULL };
	char		 buf[256];
	u_int		 i;

	w = malloc(sizeof *w);
	if (session_index(s, &i) != 0)
		fatalx("session not found%s", "");
	env[0] = buf;
	
	return (session_attach(s, w, idx));
}

/* Detach a window from a session. */
int
session_detach(struct session *s, struct winlink *wl)
{
	if (s->curw == wl && session_last(s) != 0 && session_previous(s) != 0)
		session_next(s);
	if (s->lastw == wl)
		s->lastw = NULL;

	winlink_remove(&s->windows, wl);
	if (RB_EMPTY(&s->windows)) {
		session_destroy(s);
		return (1);
	}
	return (0);
}

/* Return if session has window. */
int
session_has(struct session *s, struct window *w)
{
	struct winlink	*wl;

	RB_FOREACH(wl, winlinks, &s->windows) {
		if (wl->window == w)
			return (1);
	}
	return (0);
}

/* Move session to next window. */
int
session_next(struct session *s)
{
	struct winlink	*wl;

	if (s->curw == NULL)
		return (-1);

	wl = winlink_next(&s->windows, s->curw);
	if (wl == NULL)
		wl = RB_MIN(winlinks, &s->windows);
	if (wl == s->curw)
		return (1);
	s->lastw = s->curw;
	s->curw = wl;
	return (0);
}

/* Move session to previous window. */ 
int
session_previous(struct session *s)
{
	struct winlink	*wl;

	if (s->curw == NULL)
		return (-1);

	wl = winlink_previous(&s->windows, s->curw);
	if (wl == NULL)
		wl = RB_MAX(winlinks, &s->windows);
	if (wl == s->curw)
		return (1);
	s->lastw = s->curw;
	s->curw = wl;
	return (0);
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
	s->lastw = s->curw;
	s->curw = wl;
	return (0);
}

/* Move session to last used window. */ 
int
session_last(struct session *s)
{
	struct winlink	*wl;

	wl = s->lastw;
	if (wl == NULL)
		return (-1);
	if (wl == s->curw) 
		return (1);

	s->lastw = s->curw;
	s->curw = wl;
	return (0);
}


/* Create a new session. */
struct session *
session_create(const char *name, const char *cmd, u_int sx, u_int sy)
{
	struct session	*s;
	u_int		 i;

	s = xmalloc(sizeof *s);
	s->tim = time(NULL);
	s->curw = s->lastw = NULL;
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

	s->name = xstrdup(name);
	if (session_new(s, NULL, cmd, -1) == NULL) {
		session_destroy(s);
		return (NULL);
	}
	session_select(s, 0);

	return (s);
}

int main (int argc, char **argv)
{
	struct session *s;
	struct winlink *wl;
	char *name, cmd;
	u_int sx, sy;
	int i = 0;
	int idx;

	s = session_create(argv[0], "cmd", sx, sy); 
	for(i = 1; i < argc; i++)	
	{
		session_new(s, argv[i], cmd, idx);
		session_next(s);
	}

  	wl = winlink_find_by_index(& s->windows, idx);
  	wl->idx = idx;
	session_destroy(s);
}



