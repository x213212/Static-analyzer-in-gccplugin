/*
	parent : 540f0b3
	commit id : 189017c078b7870c18ced485c1fd99f65fcc4801
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../tmux.h"

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
	const char		*input_prompt;
	char			*input_str;

	void 			(*callbackfn)(struct window_choose_data *);
};

/* Command definition. */
struct cmd_entry {
	const char	*name;
	const char	*alias;

	const char	*args_template;
	int		 args_lower;
	int		 args_upper;

	const char	*usage;

#define CMD_STARTSERVER 0x1
#define CMD_CANTNEST 0x2
#define CMD_READONLY 0x4
	int		 flags;

	void		 (*key_binding)(struct cmd *, int);
	enum cmd_retval	 (*exec)(struct cmd *, struct cmd_q *);
};

/*
 * Find window containing text.
 */

/* Flags for determining matching behavior. */
#define CMD_FIND_WINDOW_BY_TITLE   0x1
#define CMD_FIND_WINDOW_BY_CONTENT 0x2
#define CMD_FIND_WINDOW_BY_NAME    0x4

#define CMD_FIND_WINDOW_ALL		\
	(CMD_FIND_WINDOW_BY_TITLE |	\
	 CMD_FIND_WINDOW_BY_CONTENT |	\
	 CMD_FIND_WINDOW_BY_NAME)

struct cmd_find_window_data {
	struct winlink	*wl;
	char		*list_ctx;
	u_int		 pane_id;
};

ARRAY_DECL(cmd_find_window_data_list, struct cmd_find_window_data);

/* Tree of format entries. */
struct format_entry {
	char		       *key;
	char		       *value;

	RB_ENTRY(format_entry)	entry;
};
RB_HEAD(format_tree, format_entry);

/* Format tree comparison function. */
int
format_cmp(struct format_entry *fe1, struct format_entry *fe2)
{
	return (strcmp(fe1->key, fe2->key));
}

/* Format key-value replacement entry. */
RB_GENERATE(format_tree, format_entry, entry, format_cmp);

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

void
window_choose_add(struct window_pane *wp, struct window_choose_data *wcd)
{
	struct window_choose_mode_data	*data = wp->modedata;
	struct window_choose_mode_item	*item;
	char				 tmp[10];

	ARRAY_EXPAND(&data->list, 1);
	item = &ARRAY_LAST(&data->list);

	item->wcd = wcd;
	item->pos = ARRAY_LENGTH(&data->list) - 1;
	item->state = 0;
}

struct window_choose_data *
window_choose_data_create(int type, struct client *c, struct session *s)
{
	struct window_choose_data	*wcd;

	wcd = xmalloc(sizeof *wcd);
	wcd->type = type;

	wcd->ft = malloc(sizeof(*(wcd->ft))); 
	wcd->ft_template = NULL;

	wcd->command = NULL;

	wcd->wl = NULL;
	wcd->pane_id = -1;
	wcd->idx = -1;

	wcd->tree_session = NULL;

	wcd->start_client = c;
	wcd->start_client->references++;
	wcd->start_session = s;
	wcd->start_session->references++;

	return (wcd);
}

void
cmd_find_window_match(struct cmd_find_window_data_list *find_list,
    int match_flags, struct winlink *wl, const char *str,
    const char *searchstr)
{
	struct cmd_find_window_data	 find_data;
	struct window_pane		*wp;
	u_int				 i, line;
	char				*sres;

	memset(&find_data, 0, sizeof find_data);

	i = 0;
	TAILQ_FOREACH(wp, &wl->window->panes, entry) {
		i++;

		if ((match_flags & CMD_FIND_WINDOW_BY_NAME) &&
		    fnmatch(searchstr, wl->window->name, 0) == 0) {
			find_data.list_ctx = xstrdup("");				/* allocation site */
			break;
		}

		if ((match_flags & CMD_FIND_WINDOW_BY_TITLE) &&
		    fnmatch(searchstr, wp->base.title, 0) == 0) {
			find_data.list_ctx = strdup(wp->base.title);	/* allocation site */
			break;
		}
	}
	if (find_data.list_ctx != NULL) {
		find_data.wl = wl;
		find_data.pane_id = i - 1;
		ARRAY_ADD(find_list, find_data);
	}
}

enum cmd_retval cmd_find_window_exec(struct cmd *self, struct cmd_q *cmdq)
{
	struct args			*args = self->args;
	struct client			*c;
	struct window_choose_data	*cdata;
	struct session			*s;
	struct winlink			*wl, *wm;
	struct cmd_find_window_data_list find_list;
	char				*str, *searchstr;
	u_int				 i, match_flags;

	str = args->argv[0];

	ARRAY_INIT(&find_list);

	RB_FOREACH(wm, winlinks, &s->windows)
	    cmd_find_window_match (&find_list, match_flags, wm, str, searchstr);
	free(searchstr);

	if (ARRAY_LENGTH(&find_list) == 0) {
		ARRAY_FREE(&find_list);
		return (CMD_RETURN_ERROR);
	}

	if (ARRAY_LENGTH(&find_list) == 1) {
		goto out;
	}

	for (i = 0; i < ARRAY_LENGTH(&find_list); i++) {
		wm = ARRAY_ITEM(&find_list, i).wl;

		cdata = window_choose_data_create(TREE_OTHER, c, c->session);
		cdata->idx = wm->idx;
		cdata->wl = wm;

		cdata->pane_id = ARRAY_ITEM(&find_list, i).pane_id;
		*ARRAY_ITEM(&find_list, i).list_ctx = 0;
		
		window_choose_add(wl->window->active, cdata);
	}

out:
	/* memory leak */
//	for (i = 0; i < ARRAY_LENGTH(&find_list); i++)
//		free(ARRAY_ITEM(&find_list, i).list_ctx);
	ARRAY_FREE(&find_list);
	return (CMD_RETURN_NORMAL);
}

int main()
{
	struct cmd_q *cmdq;
	struct cmd self;
	cmd_find_window_exec(&self, cmdq);
	return 0;
}
