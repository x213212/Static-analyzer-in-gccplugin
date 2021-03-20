/*
	buggy parent : 51c135e
	commit id : 51eab54102adb3927f2eac41f888d4937d183717
*/

#include <stdio.h>
#include <stdlib.h>

#define WINDOW
#define WINDOW_MODE
#include "../stdio.h"
#include "../tmux.h"

/* Window structure. */
struct window {
	char		*name;
	struct event	 name_timer;

	struct window_pane *active;
	struct window_panes panes;

	int		 lastlayout;
	struct layout_cell *layout_root;

	u_int		 sx;
	u_int		 sy;

	int		 flags;
#define WINDOW_BELL 0x1
#define WINDOW_HIDDEN 0x2
#define WINDOW_ACTIVITY 0x4
#define WINDOW_CONTENT 0x8
#define WINDOW_REDRAW 0x10

	struct options	 options;

	u_int		 references;
};
ARRAY_DECL(windows, struct window *);


struct screen *window_copy_init(struct window_pane *);
void	window_copy_free(struct window_pane *);
void	window_copy_resize(struct window_pane *, u_int, u_int);
void	window_copy_key(struct window_pane *, struct client *, int);
int	window_copy_key_input(struct window_pane *, int);
int	window_copy_key_numeric_prefix(struct window_pane *, int);
void	window_copy_mouse(
	    struct window_pane *, struct client *, struct mouse_event *);

struct window_mode {
	struct screen *(*init)(struct window_pane *);
	void	(*free)(struct window_pane *);
	void	(*resize)(struct window_pane *, u_int, u_int);
	void	(*key)(struct window_pane *, struct client *, int);
	void	(*mouse)(struct window_pane *,
		    struct client *, struct mouse_event *);
	void	(*timer)(struct window_pane *);
};

struct window	*window_create1(u_int, u_int);
struct window	*window_create(const char *, const char *, const char *, const char *, u_int, u_int, u_int, char **);

struct window_pane *
window_pane_create(struct window *w, u_int sx, u_int sy, u_int hlimit);
/* Data required while mode keys are in use. */
struct mode_key_data {
	struct mode_key_tree   *tree;
	int			mode;
};
#define MODEKEY_EMACS 0
#define MODEKEY_VI 1


const struct window_mode window_copy_mode = {
	window_copy_init,
	window_copy_free,
	NULL,
	window_copy_key,
	NULL,
	NULL,
};

enum window_copy_input_type {
	WINDOW_COPY_OFF,
	WINDOW_COPY_NUMERICPREFIX,
	WINDOW_COPY_SEARCHUP,
	WINDOW_COPY_SEARCHDOWN,
	WINDOW_COPY_GOTOLINE,
};

struct window_copy_mode_data {
	struct screen	screen;

	struct mode_key_data mdata;

	u_int		oy;

	u_int		selx;
	u_int		sely;

	u_int		rectflag; /* are we in rectangle copy mode? */

	u_int		cx;
	u_int		cy;

	u_int		lastcx; /* position in last line with content */
	u_int		lastsx; /* size of last line with content */

	enum window_copy_input_type inputtype;
	const char     *inputprompt;
	char   	       *inputstr;

	u_int		numprefix;

	enum window_copy_input_type searchtype;
	char	       *searchstr;
};

/* Mode key commands. */
enum mode_key_cmd {
	MODEKEY_NONE,
	MODEKEY_OTHER,

	/* Editing keys. */
	MODEKEYEDIT_BACKSPACE,
	MODEKEYEDIT_CANCEL,
	MODEKEYEDIT_COMPLETE,
	MODEKEYEDIT_CURSORLEFT,
	MODEKEYEDIT_CURSORRIGHT,
	MODEKEYEDIT_DELETE,
	MODEKEYEDIT_DELETELINE,
	MODEKEYEDIT_DELETETOENDOFLINE,
	MODEKEYEDIT_ENDOFLINE,
	MODEKEYEDIT_ENTER,
	MODEKEYEDIT_HISTORYDOWN,
	MODEKEYEDIT_HISTORYUP,
	MODEKEYEDIT_PASTE,
	MODEKEYEDIT_STARTOFLINE,
	MODEKEYEDIT_SWITCHMODE,
	MODEKEYEDIT_SWITCHMODEAPPEND,
	MODEKEYEDIT_TRANSPOSECHARS,

	/* Menu (choice) keys. */
	MODEKEYCHOICE_CANCEL,
	MODEKEYCHOICE_CHOOSE,
	MODEKEYCHOICE_DOWN,
	MODEKEYCHOICE_PAGEDOWN,
	MODEKEYCHOICE_PAGEUP,
	MODEKEYCHOICE_SCROLLDOWN,
	MODEKEYCHOICE_SCROLLUP,
	MODEKEYCHOICE_UP,

	/* Copy keys. */
	MODEKEYCOPY_BACKTOINDENTATION,
	MODEKEYCOPY_BOTTOMLINE,
	MODEKEYCOPY_CANCEL,
	MODEKEYCOPY_CLEARSELECTION,
	MODEKEYCOPY_COPYSELECTION,
	MODEKEYCOPY_DOWN,
	MODEKEYCOPY_ENDOFLINE,
	MODEKEYCOPY_GOTOLINE,
	MODEKEYCOPY_HALFPAGEDOWN,
	MODEKEYCOPY_HALFPAGEUP,
	MODEKEYCOPY_HISTORYBOTTOM,
	MODEKEYCOPY_HISTORYTOP,
	MODEKEYCOPY_LEFT,
	MODEKEYCOPY_MIDDLELINE,
	MODEKEYCOPY_NEXTPAGE,
	MODEKEYCOPY_NEXTSPACE,
	MODEKEYCOPY_NEXTSPACEEND,
	MODEKEYCOPY_NEXTWORD,
	MODEKEYCOPY_NEXTWORDEND,
	MODEKEYCOPY_PREVIOUSPAGE,
	MODEKEYCOPY_PREVIOUSSPACE,
	MODEKEYCOPY_PREVIOUSWORD,
	MODEKEYCOPY_RECTANGLETOGGLE,
	MODEKEYCOPY_RIGHT,
	MODEKEYCOPY_SCROLLDOWN,
	MODEKEYCOPY_SCROLLUP,
	MODEKEYCOPY_SEARCHAGAIN,
	MODEKEYCOPY_SEARCHDOWN,
	MODEKEYCOPY_SEARCHREVERSE,
	MODEKEYCOPY_SEARCHUP,
	MODEKEYCOPY_STARTNUMBERPREFIX,
	MODEKEYCOPY_STARTOFLINE,
	MODEKEYCOPY_STARTSELECTION,
	MODEKEYCOPY_TOPLINE,
	MODEKEYCOPY_UP,
};

struct windows windows;
void
mode_key_init(struct mode_key_data *mdata, struct mode_key_tree *mtree)
{
	mdata->tree = mtree;
	mdata->mode = 0;
}

#define PANE_REDRAW 0x1
#define PANE_FREEZE 0x2
int
window_pane_set_mode(struct window_pane *wp, const struct window_mode *mode)
{
	struct screen	*s;

	if (wp->mode != NULL)
		return (1);
	wp->mode = mode;

	if ((s = wp->mode->init(wp)) != NULL)
		wp->screen = s;
	wp->flags |= PANE_REDRAW;
	return (0);
}

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

void
window_pane_reset_mode(struct window_pane *wp)
{
	if (wp->mode == NULL)
		return;

	wp->mode->free(wp);
	wp->mode = NULL;

	wp->screen = &wp->base;
	wp->flags |= PANE_REDRAW;
}

struct screen *
window_copy_init(struct window_pane *wp)
{
	struct window_copy_mode_data	*data;
	struct screen			*s;
	struct screen_write_ctx	 	 ctx;
	u_int				 i;
	int				 keys;

	wp->modedata = data = xmalloc(sizeof *data);	/* allocation site */
	data->oy = 0;
	data->cx = wp->base.cx;
	data->cy = wp->base.cy;

	data->lastcx = 0;
	data->lastsx = 0;

	data->rectflag = 0;

	data->inputtype = WINDOW_COPY_OFF;
	data->inputprompt = NULL;
	data->inputstr = xstrdup("");
	data->numprefix = 0;

	data->searchtype = WINDOW_COPY_OFF;
	data->searchstr = NULL;

	wp->flags |= PANE_FREEZE;

	s = &data->screen;


	s->cx = data->cx;
	s->cy = data->cy;

	return (s);
}

void
window_copy_free(struct window_pane *wp)
{
	struct window_copy_mode_data	*data = wp->modedata;

	wp->flags &= ~PANE_FREEZE;

	if (data->searchstr != NULL)
		xfree(data->searchstr);
	xfree(data->inputstr);


	xfree(data);
}
void
window_copy_key(struct window_pane *wp, struct client *c, int key)
{
	const char			*word_separators;
	struct window_copy_mode_data	*data = wp->modedata;
	struct screen			*s = &data->screen;
	u_int				 n, np;
	int				 keys;
	enum mode_key_cmd		 cmd;

	np = data->numprefix;
	if (np == 0)
		np = 1;

	if (data->inputtype == WINDOW_COPY_NUMERICPREFIX) {
		data->inputtype = WINDOW_COPY_OFF;
	} else if (data->inputtype != WINDOW_COPY_OFF) {
		if (getchar())
			goto input_off;
		return;
	}

	switch (cmd) {
	case MODEKEYCOPY_CANCEL:
		for (; np != 0; np--)
			window_pane_reset_mode(wp);
		break;
	case MODEKEYCOPY_GOTOLINE:
		*data->inputstr = '\0';
		goto input_on;
	default:
		break;
	}

	data->numprefix = 0;
	return;

input_on:
	data->inputtype = WINDOW_COPY_GOTOLINE;
	data->inputprompt = "Goto Line";
	
	return;

input_off:

	data->inputtype = WINDOW_COPY_OFF;
	data->inputprompt = NULL;

}

void
window_pane_destroy(struct window_pane *wp)
{
	window_pane_reset_mode(wp);

	if (wp->cwd != NULL)
		xfree(wp->cwd);
	if (wp->shell != NULL)
		xfree(wp->shell);
	xfree(wp);
}


void
window_destroy_panes(struct window *w)
{
	struct window_pane	*wp;

	while (!TAILQ_EMPTY(&w->panes)) {
		wp = TAILQ_FIRST(&w->panes);
		TAILQ_REMOVE(&w->panes, wp, entry);
		window_pane_destroy(wp);
	}
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

void
window_destroy(struct window *w)
{
	u_int	i;

	if (window_index(w, &i) != 0)
		fatalx("index not found%s", "");
	ARRAY_SET(&windows, i, NULL);
	while (!ARRAY_EMPTY(&windows) && ARRAY_LAST(&windows) == NULL)
		ARRAY_TRUNC(&windows, 1);

	window_destroy_panes(w);

	if (w->name != NULL)
		xfree(w->name);
	xfree(w);
}


struct window *
window_create1(u_int sx, u_int sy)
{
	struct window	*w;
	u_int		 i;

	w = xmalloc(sizeof *w);
	w->name = NULL;
	w->flags = 0;

	TAILQ_INIT(&w->panes);
	w->active = NULL;

	w->lastlayout = -1;
	w->layout_root = NULL;

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

struct window_pane *
window_add_pane(struct window *w, u_int hlimit)
{
	struct window_pane	*wp;

	wp = window_pane_create(w, w->sx, w->sy, hlimit);
	if (TAILQ_EMPTY(&w->panes))
		TAILQ_INSERT_HEAD(&w->panes, wp, entry);
	else
		TAILQ_INSERT_AFTER(&w->panes, w->active, wp, entry);
	return (wp);
}

struct window *
window_create(const char *name, const char *cmd, const char *shell,
    const char *cwd, u_int sx, u_int sy, u_int hlimit,char **cause)
{
	struct window		*w;
	struct window_pane	*wp;

	w = window_create1(sx, sy);
	wp = window_add_pane(w, hlimit);
	
	w->active = TAILQ_FIRST(&w->panes);
	w->name = xstrdup(name);
	return (w);
}


struct window_pane *
window_pane_create(struct window *w, u_int sx, u_int sy, u_int hlimit)
{
	struct window_pane	*wp;

	wp = xcalloc(1, sizeof *wp);
	wp->window = w;

	wp->shell = NULL;
	wp->cwd = NULL;

	wp->fd = -1;
	wp->event = NULL;

	wp->mode = NULL;

	wp->layout_cell = NULL;

	wp->xoff = 0;
	wp->yoff = 0;

	wp->sx = sx;
	wp->sy = sy;

	wp->pipe_fd = -1;
	wp->pipe_off = 0;
	wp->pipe_event = NULL;

	wp->saved_grid = NULL;

	return (wp);
}


int main()
{
	struct window *w;
	struct window_pane *wp;
	struct client c;
	int key;
	u_int sx, sy, hlimit;
	char *env;

	w = window_create("name", "cmd", "shell", "cwd", sx, sy, hlimit, NULL);	
	wp = w->active;

	window_pane_set_mode(wp, &window_copy_mode);
	
	while(getchar())
	{
		scanf("%d", &key);	
		wp->mode->key(wp, &c, key);
	}
	wp->mode->free(wp);
	window_destroy(w);
	return 0;
}

