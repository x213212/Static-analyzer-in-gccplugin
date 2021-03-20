/*
	commit id : bad8d0fd200c7718a8f51f52b9dd2f23c74d996f
	buggy parent : 3192178
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../tmux.h"

enum notify_type {
	NOTIFY_WINDOW_LAYOUT_CHANGED,
	NOTIFY_WINDOW_UNLINKED,
	NOTIFY_WINDOW_LINKED,
	NOTIFY_WINDOW_RENAMED,
	NOTIFY_ATTACHED_SESSION_CHANGED,
	NOTIFY_SESSION_RENAMED,
	NOTIFY_SESSION_CREATED,
	NOTIFY_SESSION_CLOSED
};

struct notify_entry {
	enum notify_type	 type;

	struct client		*client;
	struct session		*session;
	struct window		*window;

	TAILQ_ENTRY(notify_entry) entry;
};
TAILQ_HEAD(, notify_entry) notify_queue = TAILQ_HEAD_INITIALIZER(notify_queue);
int	notify_enabled = 1;


void	notify_drain(void);
void	notify_add(enum notify_type, struct client *, struct session *, struct window *);

/* Global window list. */
struct windows windows;

/* Global panes tree. */
struct window_pane_tree all_window_panes;
u_int	next_window_pane_id;
u_int	next_window_id;
u_int	next_active_point;

int
window_cmp(struct window *w1, struct window *w2)
{
	return (w1->id - w2->id);
}

int
winlink_cmp(struct winlink *wl1, struct winlink *wl2)
{
	return (wl1->idx - wl2->idx);
}

int
window_pane_cmp(struct window_pane *wp1, struct window_pane *wp2)
{
	return (wp1->id - wp2->id);
}

RB_GENERATE(windows, window, entry, window_cmp);
RB_GENERATE(winlinks, winlink, entry, winlink_cmp);
RB_GENERATE(window_pane_tree, window_pane, tree_entry, window_pane_cmp);

#define UINT_MAX (1 << 30)

struct layout_cell *
layout_create_cell(struct layout_cell *lcparent)
{
	struct layout_cell	*lc;

	lc = xmalloc(sizeof *lc);	/* allocation site */
	lc->type = LAYOUT_WINDOWPANE;
	lc->parent = lcparent;

	TAILQ_INIT(&lc->cells);

	lc->sx = UINT_MAX;
	lc->sy = UINT_MAX;

	lc->xoff = UINT_MAX;
	lc->yoff = UINT_MAX;

	lc->wp = NULL;

	return (lc);
}

void
layout_set_size(
    struct layout_cell *lc, u_int sx, u_int sy, u_int xoff, u_int yoff)
{
	lc->sx = sx;
	lc->sy = sy;

	lc->xoff = xoff;
	lc->yoff = yoff;
}

void
layout_make_leaf(struct layout_cell *lc, struct window_pane *wp)
{
	lc->type = LAYOUT_WINDOWPANE;

	TAILQ_INIT(&lc->cells);

	wp->layout_cell = lc;
	lc->wp = wp;
}

void
layout_free_cell(struct layout_cell *lc)
{
	struct layout_cell	*lcchild;

	switch (lc->type) {
	case LAYOUT_LEFTRIGHT:
	case LAYOUT_TOPBOTTOM:
		while (!TAILQ_EMPTY(&lc->cells)) {
			lcchild = TAILQ_FIRST(&lc->cells);
			TAILQ_REMOVE(&lc->cells, lcchild, entry);
			layout_free_cell(lcchild);
		}
		break;
	case LAYOUT_WINDOWPANE:
		if (lc->wp != NULL)
			lc->wp->layout_cell = NULL;
		break;
	}

	free(lc);
}

void
layout_free(struct window *w)
{
	layout_free_cell(w->layout_root);
}


void
layout_init(struct window *w, struct window_pane *wp)
{
	struct layout_cell	*lc;

	lc = w->layout_root = layout_create_cell(NULL);
	layout_set_size(lc, w->sx, w->sy, 0, 0);
	layout_make_leaf(lc, wp);
}

u_int
window_count_panes(struct window *w)
{
	struct window_pane	*wp;
	u_int			 n;

	n = 0;
	TAILQ_FOREACH(wp, &w->panes, entry)
		n++;
	return (n);
}


void
notify_window_layout_changed(struct window *w)
{
	notify_add(NOTIFY_WINDOW_LAYOUT_CHANGED, NULL, NULL, w);
	notify_drain();
}

int
window_zoom(struct window_pane *wp)
{
	struct window		*w = wp->window;
	struct window_pane	*wp1;

	if (w->flags & WINDOW_ZOOMED)
		return (-1);

	if (!window_pane_visible(wp))
		return (-1);

	if (window_count_panes(w) == 1)
		return (-1);

	TAILQ_FOREACH(wp1, &w->panes, entry) {
		wp1->saved_layout_cell = wp1->layout_cell;
		wp1->layout_cell = NULL;
	}

	w->saved_layout_root = w->layout_root;
	layout_init(w, wp);
	w->flags |= WINDOW_ZOOMED;
	notify_window_layout_changed(w);

	return (0);
}
int
window_unzoom(struct window *w)
{
	struct window_pane	*wp;

	if (!(w->flags & WINDOW_ZOOMED))
		return (-1);

	w->flags &= ~WINDOW_ZOOMED;
	layout_free(w);
	w->layout_root = w->saved_layout_root;
	w->saved_layout_root = NULL;

	TAILQ_FOREACH(wp, &w->panes, entry) {
		wp->layout_cell = wp->saved_layout_cell;
		wp->saved_layout_cell = NULL;
	}
	notify_window_layout_changed(w);
	return (0);
}

void
window_destroy(struct window *w)
{
	window_unzoom(w);	/* w will be freed if its ref is already 0 */

	RB_REMOVE(windows, &windows, w);

	if (w->layout_root != NULL)
		layout_free(w);	/* double-free */
	
	free(w->name);		/* double-free */
	free(w);			/* double-free */
}

void
window_remove_ref(struct window *w)
{
	if (w->references == 0)
		fatal("bad reference count%s", "");
	w->references--;
	if (w->references == 0)
		window_destroy(w);
}

void
notify_add(enum notify_type type, struct client *c, struct session *s,
    struct window *w)
{
	struct notify_entry	*ne;

	ne = xcalloc(1, sizeof *ne);
	ne->type = type;
	ne->client = c;
	ne->session = s;
	ne->window = w;
	TAILQ_INSERT_TAIL(&notify_queue, ne, entry);

	if (c != NULL)
		c->references++;
	if (s != NULL)
		s->references++;
	if (w != NULL)
		w->references++;
}

void
notify_drain(void)
{
	struct notify_entry	*ne, *ne1;

	if (!notify_enabled)
		return;

	TAILQ_FOREACH(ne, &notify_queue, entry) {
		if (ne->window != NULL)
			window_remove_ref(ne->window);

		TAILQ_REMOVE(&notify_queue, ne, entry);
		free(ne);
	}
}
int
window_pane_visible(struct window_pane *wp)
{
	struct window	*w = wp->window;

	if (wp->layout_cell == NULL)
		return (0);
	if (wp->xoff >= w->sx || wp->yoff >= w->sy)
		return (0);
	if (wp->xoff + wp->sx > w->sx || wp->yoff + wp->sy > w->sy)
		return (0);
	return (1);
}

struct window_pane *
window_get_active_at(struct window *w, u_int x, u_int y)
{
	struct window_pane	*wp;

	TAILQ_FOREACH(wp, &w->panes, entry) {
		if (!window_pane_visible(wp))
			continue;
		if (x < wp->xoff || x > wp->xoff + wp->sx)
			continue;
		if (y < wp->yoff || y > wp->yoff + wp->sy)
			continue;
		return (wp);
	}
	return (NULL);
}

struct window *
window_create1(u_int sx, u_int sy)
{
	struct window	*w;

	w = xcalloc(1, sizeof *w);	/* allocation site */
	w->name = NULL;
	w->flags = 0;

	TAILQ_INIT(&w->panes);
	w->active = NULL;

	w->lastlayout = -1;
	w->layout_root = NULL;

	w->sx = sx;
	w->sy = sy;

	w->references = 0;

	w->id = next_window_id++;
	RB_INSERT(windows, &windows, w);

	return (w);
}

struct window *
window_create(const char *name, int argc, char **argv, const char *path, u_int sx, u_int sy, u_int hlimit)
{
	struct window		*w;
	struct window_pane	*wp;

	w = window_create1(sx, sy);
	layout_init(w, wp);

	w->active = TAILQ_FIRST(&w->panes);
	w->name = xstrdup(name);	/* allocation site */

	return (w);
}

int main(int argc, char **argv)
{
	struct window *w;
	u_int sx, sy, hlimit;
	w = window_create("name", argc, argv, "path", sx, sy, hlimit);	
	window_zoom(w->active);
	window_count_panes(w);
	window_destroy(w);
}

