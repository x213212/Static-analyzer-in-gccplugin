/*
	commit id : ad1541913f902dad3f6c4b79543b6c380a8058f
*/

#include <stdio.h>
#include <stdlib.h>

#define WINDOW_MODE
#define WINDOW
#include "../stdio.h"
#include "../tmux.h"

typedef unsigned int uint8_t;
/* Buffer structure. */
struct buffer {
	u_char	*base;		/* buffer start */
	size_t	 space;		/* total size of buffer */

	size_t	 size;		/* size of data in buffer */
	size_t	 off;		/* offset of data in buffer */
};

/*
 * Window mode. Windows can be in several modes and this is used to call the
 * right function to handle input and output.
 */
struct window_mode {
	void	(*init)(struct window *);
	void	(*resize)(struct window *, u_int, u_int);
	void	(*draw)(struct window *, struct buffer *, u_int, u_int);
	void	(*key)(struct window *, int);
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

	const struct window_mode *mode;
	void		*modedata;

	u_int		 references;
};
ARRAY_DECL(windows, struct window *);

void	window_scroll_init(struct window *);
void	window_scroll_resize(struct window *, u_int, u_int);
void	window_scroll_draw(struct window *, struct buffer *, u_int, u_int);
void	window_scroll_key(struct window *, int);

const struct window_mode window_scroll_mode = {
	window_scroll_init,
	NULL,
	NULL,
	window_scroll_key
};

struct window_scroll_mode_data {
	u_int	off;
	u_int	size;
};

struct session {
	char		*name;

	u_int		 sx;
	u_int		 sy;

	struct winlink	*curw;
	struct winlinks	 windows;

#define SESSION_UNATTACHED 0x1	/* not attached to any clients */
#define SESSION_DEAD 0x2
};

struct windows windows;

int
winlink_cmp(struct winlink *wl1, struct winlink *wl2)
{
	return (wl1->idx - wl2->idx);
}

RB_GENERATE(winlinks, winlink, entry, winlink_cmp);

void
window_scroll_init(struct window *w)
{
	struct window_scroll_mode_data	*data;

	w->modedata = data = xmalloc(sizeof *data);	/* allocation site */
	data->off = 0;
	data->size = w->screen.hsize;
}

void
window_scroll_key(struct window *w, int key)
{
	struct window_scroll_mode_data	*data = w->modedata;
	u_int				 off, sy = screen_size_y(&w->screen);

	off = data->off;
	switch (key) {
	case 'Q':
	case 'q':
		w->mode = NULL;
		xfree(w->modedata);

		break;
	case 'k':
	case 'K':
	case KEYC_UP:
		if (data->off < data->size)
			data->off++;
		break;
	case 'j':
	case 'J':
	case KEYC_DOWN:
		if (data->off > 0)
			data->off--;
		break;
	case '\025':
	case KEYC_PPAGE:
		if (data->off + sy > data->size)
			data->off = data->size;
		else
			data->off += sy;
		break;
	case '\006':
	case KEYC_NPAGE:
		if (data->off < sy)
			data->off = 0;
		else
			data->off -= sy;
		break;
	}
	if (off != data->off)		/* use-after-free */
		return;
}

#define BUFFER_IN(b) ((b)->base + (b)->off + (b)->size)
void
input_store8(struct buffer *b, uint8_t n)
{	
	BUFFER_IN(b)[0] = n;
	b->size += 1;
}

/* Translate a key code from client into an output key sequence. */
void
input_key(struct buffer *b, int key)
{
	u_int	i;

	if (key != KEYC_NONE && key >= 0) {
		input_store8(b, key);
		return;
	}
}
void
window_key(struct window *w, int key)
{
	if (w->mode != NULL)
		w->mode->key(w, key);
	else
		input_key(w->out, key);	
}

struct window *
window_create(
    const char *name, const char *cmd, const char **env, u_int sx, u_int sy)
{
	struct window	*w;

	w = xmalloc(sizeof *w);
	w->mode = NULL;

	if(__RANDBOOL)
	{
		w->mode = &window_scroll_mode;
		w->mode->init(w);
	}


	return (w);
}

void
window_destroy(struct window *w)
{
	xfree(w);
}

struct winlink *
winlink_add(struct winlinks *wwl, struct window *w, int idx)
{
	struct winlink	*wl;

	wl = xcalloc(1, sizeof *wl);	/* allocation site */
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
		fatal("bad reference count%s\n", "");
	w->references--;
	if (w->references == 0)
		window_destroy(w);
}

/* Destroy a session. */
void
session_destroy(struct session *s)
{
	u_int	i;

	printf("session %s destroyed\n", s->name);

	while (!RB_EMPTY(&s->windows))
		winlink_remove(&s->windows, RB_ROOT(&s->windows));

	xfree(s->name);
	xfree(s);
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
    const char *name, const char *cmd, const char *cwd, int idx, char **cause)
{
	struct window	*w;
	const char	*shell;
	
	u_int		 hlimit;

	w = window_create(name, cmd, NULL, s->sx, s->sy);
	if (w == NULL) {
		return (NULL);
	}

	return (session_attach(s, w, idx, cause));
}
/* Create a new session. */
struct session *
session_create(const char *name, const char *cmd, const char *cwd,
    struct environ *env, struct termios *tio, int idx, u_int sx, u_int sy,
    char **cause)
{
	struct session	*s;
	struct winlink *wl;
	u_int		 i;

	s = xmalloc(sizeof *s);

	s->curw = NULL;
	RB_INIT(&s->windows);

	s->sx = sx;
	s->sy = sy;

	if(name == NULL)
		fatal("no name%s\n", "");
	s->name = xstrdup(name);

	if (cmd != NULL) {
		if ((wl = session_new(s, NULL, cmd, cwd, idx, cause)) == NULL) {
			session_destroy(s);
			return (NULL);
		}
		s->curw = wl; 
	}
	return (s);
}

int main(int argc, char **argv)
{
	struct session *s;
	struct window *w;
	int key;
	uint8_t sx, sy;
	char *cause, env;

	if(argc == 1)
		return 1;
	
	s = session_create("name", argv[0], "cwd", NULL, NULL, 0, sx, sy, &cause);
	w = window_create(argv[1], "cwd", &env, sx, sy);
	s->curw = session_attach(s, w, 0, &cause);

	while(1) {
		if(s->curw == NULL)
			break;
		w = s->curw->window;

		while(scanf("%d", &key) != EOF)
			window_key(w, key);

		winlink_remove(&s->windows, s->curw);
	}	
	session_destroy(s);	
	return 0;
}

