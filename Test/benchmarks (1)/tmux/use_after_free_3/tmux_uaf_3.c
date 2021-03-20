/*
	buggy parent : 0e87430 
	commit id : 6589120667587f2d5c6f59f9faf4bda584a9f602
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../tmux.h"

/* Key/command line command. */
struct cmd_ctx {
	/*
	 * curclient is the client where this command was executed if inside
	 * tmux. This is NULL if the command came from the command-line.
	 *
	 * cmdclient is the client which sent the MSG_COMMAND to the server, if
	 * any. This is NULL unless the command came from the command-line.
	 *
	 * cmdclient and curclient may both be NULL if the command is in the
	 * configuration file.
	 */
	struct client  *curclient;
	struct client  *cmdclient;

	struct msg_command_data	*msgdata;

	void (*print)(struct cmd_ctx *, const char *, ...);
	void (*info)(struct cmd_ctx *, const char *, ...);
	void (*error)(struct cmd_ctx *, const char *, ...);
};

struct screen *window_choose_init(struct window_pane *);
void	window_choose_free(struct window_pane *);
void	window_choose_key(struct window_pane *, struct session *, int);
void	window_choose_mouse(
	    struct window_pane *, struct session *, struct mouse_event *);

void	window_choose_fire_callback(
	    struct window_pane *, struct window_choose_data *);
void	window_choose_write_line(
	    struct window_pane *, struct screen_write_ctx *, u_int);

void	window_choose_scroll_up(struct window_pane *);
void	window_choose_scroll_down(struct window_pane *);

void	window_choose_collapse(struct window_pane *, struct session *);
void	window_choose_expand(struct window_pane *, struct session *, u_int);
void	window_choose_collapse_all(struct window_pane *);
void	window_choose_expand_all(struct window_pane *);

enum window_choose_input_type {
	WINDOW_CHOOSE_NORMAL = -1,
	WINDOW_CHOOSE_GOTO_ITEM,
};

struct window_choose_mode_item {
	struct window_choose_data	*wcd;
	char				*name;
	int				 pos;
	int				 state;
#define TREE_EXPANDED 0x1
};

struct window_choose_mode_data {
	struct screen	        screen;

	ARRAY_DECL(, struct window_choose_mode_item) list;
	ARRAY_DECL(, struct window_choose_mode_item) old_list;
	int			width;
	u_int			top;
	u_int			selected;
	enum window_choose_input_type input_type;
	const char		*input_prompt;
	char			*input_str;

	void 			(*callbackfn)(struct window_choose_data *);
	void			(*freefn)(struct window_choose_data *);
};

int     window_choose_key_index(struct window_choose_mode_data *, u_int);
int     window_choose_index_key(struct window_choose_mode_data *, int);
void	window_choose_prompt_input(enum window_choose_input_type,
	    const char *, struct window_pane *, int);


int
session_cmp(struct session *s1, struct session *s2)
{
    return (strcmp(s1->name, s2->name));
}

RB_GENERATE(sessions, session, entry, session_cmp);
struct sessions sessions;

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
window_choose_add(struct window_pane *wp, struct window_choose_data *wcd)
{
	struct window_choose_mode_data	*data = wp->modedata;
	struct window_choose_mode_item	*item;
	char				 tmp[10];

	ARRAY_EXPAND(&data->list, 1);	/* allocation site */
	item = &ARRAY_LAST(&data->list);

	item->name = malloc(10);		/* allocation site */
	item->wcd = wcd;
	item->pos = ARRAY_LENGTH(&data->list) - 1;
	item->state = 0;

}

struct screen *
window_choose_init(struct window_pane *wp)
{
	struct window_choose_mode_data	*data;
	struct screen			*s;
	int				 keys;

	wp->modedata = data = xmalloc(sizeof *data);	/* allocation site */

	data->callbackfn = NULL;
	data->freefn = NULL;
	data->input_type = WINDOW_CHOOSE_NORMAL;
	data->input_str = xstrdup("");
	data->input_prompt = NULL;

	ARRAY_INIT(&data->list);
	ARRAY_INIT(&data->old_list);
	data->top = 0;

	s = &data->screen;
	s->mode &= ~MODE_CURSOR;

	return (s);
}

struct window_choose_data *
window_choose_data_create(struct cmd_ctx *ctx)
{
	struct window_choose_data	*wcd;

	wcd = xmalloc(sizeof *wcd);
	wcd->command = NULL;
	wcd->wl = NULL;
	wcd->tree_session = NULL;
	wcd->session = ctx->curclient->session;
	wcd->idx = -1;
	wcd->type = 0;

	return (wcd);
}

void
window_choose_free(struct window_pane *wp)
{
	struct window_choose_mode_data	*data = wp->modedata;
	struct window_choose_mode_item	*item;
	u_int				 i;

	for (i = 0; i < ARRAY_LENGTH(&data->list); i++) {
		item = &ARRAY_ITEM(&data->list, i);
		if (data->freefn != NULL && item->wcd != NULL)
			data->freefn(item->wcd);
		free(item->name);
	}
	ARRAY_FREE(&data->list);
	ARRAY_FREE(&data->old_list);
	free(data->input_str);

	free(data);
}

void
window_choose_collapse(struct window_pane *wp, struct session *s)
{
	struct window_choose_mode_data	*data = wp->modedata;
	struct window_choose_mode_item	*item, *chosen;
	struct window_choose_data	*wcd;
	u_int				 i, pos;

	ARRAY_DECL(, struct window_choose_mode_item) list_copy;
	ARRAY_INIT(&list_copy);

	pos = data->selected;

	chosen = &ARRAY_ITEM(&data->list, pos);
	chosen->state &= ~TREE_EXPANDED;

	/*
	 * Trying to mangle the &data->list in-place has lots of problems, so
	 * assign the actual result we want to render and copy the new one over
	 * the top of it.
	 */
	for (i = 0; i < ARRAY_LENGTH(&data->list); i++)
	{
		item = &ARRAY_ITEM(&data->list, i);
		wcd = item->wcd;

		if (s == wcd->tree_session) {
			/* We only show the session when collapsed. */
			if (wcd->type & TREE_SESSION) {
				item->state &= ~TREE_EXPANDED;

				ARRAY_ADD(&list_copy,
						ARRAY_ITEM(&data->list, i));
				/*
				 * Update the selection to this session item so
				 * we don't end up highlighting a non-existent
				 * item.
				 */
				data->selected = i;
			}
		} else
			ARRAY_ADD(&list_copy, ARRAY_ITEM(&data->list, i));
	}

	if (!ARRAY_EMPTY(&list_copy)) {
		ARRAY_FREE(&data->list);
		ARRAY_CONCAT(&data->list, &list_copy);
	}
}

void
window_choose_collapse_all(struct window_pane *wp)
{
	struct window_choose_mode_data	*data = wp->modedata;
	struct window_choose_mode_item	*item, *chosen;
	struct session			*s;
	u_int				 i;

	chosen = &ARRAY_ITEM(&data->list, data->selected);

	RB_FOREACH(s, sessions, &sessions)
		window_choose_collapse(wp, s);	/* chosen can be freed */

	/* Reset the selection back to the starting session. */
	for (i = 0; i < ARRAY_LENGTH(&data->list); i++) {
		item = &ARRAY_ITEM(&data->list, i);

		if (chosen->wcd->session != item->wcd->tree_session) /* use-after-free */
			continue;

		if (item->wcd->type & TREE_SESSION)
			data->selected = i;
	}
}

void
window_choose_expand_all(struct window_pane *wp)
{
	struct window_choose_mode_data	*data = wp->modedata;
	struct window_choose_mode_item	*item;
	struct session			*s;
	u_int				 i;

	RB_FOREACH(s, sessions, &sessions) {
		for (i = 0; i < ARRAY_LENGTH(&data->list); i++) {
			item = &ARRAY_ITEM(&data->list, i);

			if (s != item->wcd->tree_session)
				continue;

			if (item->wcd->type & TREE_SESSION)
				window_choose_expand(wp, s, i);
		}
	}

}

void
window_choose_expand(struct window_pane *wp, struct session *s, u_int pos)
{
	struct window_choose_mode_data	*data = wp->modedata;
	struct window_choose_mode_item	*item, *chosen;
	struct window_choose_data	*wcd;
	u_int				 i, items;

	chosen = &ARRAY_ITEM(&data->list, pos);
	items = ARRAY_LENGTH(&data->old_list) - 1;

	/* It's not possible to expand anything other than sessions. */
	if (!(chosen->wcd->type & TREE_SESSION))
		return;

	/* Don't re-expand a session which is already expanded. */
	if (chosen->state & TREE_EXPANDED)
		return;

	/* Mark the session entry as expanded. */
	chosen->state |= TREE_EXPANDED;

	/*
	 * Go back through the original list of all sessions and windows, and
	 * pull out the windows where the session matches the selection chosen
	 * to expand.
	 */
	for (i = items; i > 0; i--) {
		item = &ARRAY_ITEM(&data->old_list, i);
		item->state |= TREE_EXPANDED;
		wcd = item->wcd;

		if (s == wcd->tree_session) {
			/*
			 * Since the session is already displayed, we only care
			 * to add back in window for it.
			 */
			if (wcd->type & TREE_WINDOW) {
				/*
				 * If the insertion point for adding the
				 * windows to the session falls inside the
				 * range of the list, then we insert these
				 * entries in order *AFTER* the selected
				 * session.
				 */
				if (pos < i ) {
					ARRAY_INSERT(&data->list,
					    pos + 1,
					    ARRAY_ITEM(&data->old_list,
					    i));
				} else {
					/* Ran out of room, add to the end. */
					ARRAY_ADD(&data->list,
					    ARRAY_ITEM(&data->old_list,
					    i));
				}
			}
		}
	}
}

struct window_choose_data *
window_choose_add_item(struct window_pane *wp, struct cmd_ctx *ctx,
    struct winlink *wl, const char *template, char *action, u_int idx)
{
	struct window_choose_data	*wcd;
	char				*action_data;

	wcd = window_choose_data_create(ctx);
	wcd->idx = wl->idx;

	wcd->session->references++;

	window_choose_add(wp, wcd);

	/*
	 * Interpolate action_data here, since the data we pass back is the
	 * expanded template itself.
	 */
	free(action_data);

	return (wcd);

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

	wp->screen = &wp->base;

	return (wp);
}

void
window_pane_destroy(struct window_pane *wp)
{
	free(wp->cwd);
	free(wp->shell);
	free(wp);
}


int main()
{
	struct cmd_ctx ctx;
	struct window_pane *wp;
	struct window_choose_data *wcd;
	int sx, sy, hlimit;

	wp = window_pane_create(NULL, sx, sy, hlimit);
	window_choose_init(wp);
	while(getchar())
	{
		wcd = window_choose_data_create(&ctx);
		window_choose_add(wp, wcd);
	}
	window_choose_expand_all(wp);	
	window_choose_collapse_all(wp);
	window_choose_free(wp);
	window_pane_destroy(wp);
}



