/*
	buggy parent : 695a591
	commit id : 7340d5adfdc8cc6d845a373f3e0d59bfd10a45d1
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../tmux.h"

#define _PATH_DEVNULL "/"
#define _PATH_BSHELL "~/"

/*
 * Executes a tmux command if a shell command returns true or false.
*/

/* Command queue entry. */
struct cmd_q_item {
	struct cmd_list		*cmdlist;

	struct mouse_event	 mouse;

	TAILQ_ENTRY(cmd_q_item)	 qentry;
};
TAILQ_HEAD(cmd_q_items, cmd_q_item);


/* Command queue. */
struct cmd_q {
	int			 references;
	int			 flags;
#define CMD_Q_DEAD 0x1

	struct client		*client;
	int			 client_exit;

	struct cmd_q_items	 queue;
	struct cmd_q_item	*item;
	struct cmd		*cmd;

	time_t			 time;
	u_int			 number;

	void			 (*emptyfn)(struct cmd_q *);
	void			*data;

	TAILQ_ENTRY(cmd_q)	 waitentry;
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
#define CMD_READONLY 0x2
	int		 flags;

	enum cmd_retval	 (*exec)(struct cmd *, struct cmd_q *);
};

void	cmd_if_shell_callback(struct job *);
void	cmd_if_shell_done(struct cmd_q *);
void	cmd_if_shell_free(void *);

struct cmd_if_shell_data {
	char			*cmd_if;
	char			*cmd_else;

	struct cmd_q		*cmdq;
	struct mouse_event	 mouse;

	int			 bflag;
	int			 references;
};

/*
 * Set of paste buffers. Note that paste buffer data is not necessarily a C
 * string!
 */

struct paste_buffer {
	char		*data;
	size_t		 size;

	char		*name;
	int		 automatic;
	u_int		 order;

	RB_ENTRY(paste_buffer) name_entry;
};

u_int	paste_next_index;
u_int	paste_next_order;
u_int	paste_num_automatic;
RB_HEAD(paste_name_tree, paste_buffer) paste_by_name;

int paste_cmp_names(const struct paste_buffer *, const struct paste_buffer *);
RB_PROTOTYPE(paste_name_tree, paste_buffer, name_entry, paste_cmp_names);
RB_GENERATE(paste_name_tree, paste_buffer, name_entry, paste_cmp_names);


struct args_entry {
	u_char			 flag;
	char			*value;
	RB_ENTRY(args_entry)	 entry;
};

/* Arguments tree comparison function. */
int
args_cmp(struct args_entry *a1, struct args_entry *a2)
{
	return (a1->flag - a2->flag);
}

RB_GENERATE(args_tree, args_entry, entry, args_cmp);

void *
xrealloc(void *oldptr, size_t newsize)
{
	void	*newptr;

	if (newsize == 0)
		fatalx("zero size%s", "");
	if ((newptr = realloc(oldptr, newsize)) == NULL)
		fatal("xrealloc failed%s", "");
	return (newptr);
}

int
paste_cmp_names(const struct paste_buffer *a, const struct paste_buffer *b)
{
	return (strcmp(a->name, b->name));
}

/* Free a paste buffer. */
void
paste_free(struct paste_buffer *pb)
{
	RB_REMOVE(paste_name_tree, &paste_by_name, pb);
	if (pb->automatic)
		paste_num_automatic--;

	free(pb->data);
	free(pb->name);
	free(pb);
}

/* Get a paste buffer by name. */
struct paste_buffer *
paste_get_name(const char *name)
{
	struct paste_buffer	pbfind;

	if (name == NULL || *name == '\0')
		return (NULL);

	pbfind.name = (char *)name;
	return (RB_FIND(paste_name_tree, &paste_by_name, &pbfind));
}

/*
 * Add an automatic buffer, freeing the oldest automatic item if at limit. Note
 * that the caller is responsible for allocating data.
 */
void
paste_add(char *data, size_t size)
{
	struct paste_buffer	*pb, *pb1;
	u_int			 limit;

	if (size == 0)
		return;

	pb = xmalloc(sizeof *pb);

	pb->name = NULL;
	do {
		free(pb->name);
		pb->name = malloc(10);
		paste_next_index++;
	} while (paste_get_name(pb->name) != NULL);

	pb->data = data;
	pb->size = size;

	pb->automatic = 1;
	paste_num_automatic++;

	pb->order = paste_next_order++;
	RB_INSERT(paste_name_tree, &paste_by_name, pb);
}

/*
 * Add or replace an item in the store. Note that the caller is responsible for
 * allocating data.
 */
int
paste_set(char *data, size_t size, const char *name, char **cause)
{
	struct paste_buffer	*pb, *old;

	if (cause != NULL)
		*cause = NULL;

	if (size == 0) {
		free(data);
		return (0);
	}
	if (name == NULL) {
		paste_add(data, size);
		return (0);
	}

	if (*name == '\0') {
		if (cause != NULL)
			*cause = xstrdup("empty buffer name");
		return (-1);
	}

	pb = xmalloc(sizeof *pb);

	pb->name = xstrdup(name);

	pb->data = data;
	pb->size = size;

	pb->automatic = 0;
	pb->order = paste_next_order++;

	if ((old = paste_get_name(name)) != NULL)
		paste_free(old);

	RB_INSERT(paste_name_tree, &paste_by_name, pb);

	return (0);
}

/* Find a flag in the arguments tree. */
struct args_entry *
args_find(struct args *args, u_char ch)
{
	struct args_entry	entry;

	entry.flag = ch;
	return (RB_FIND(args_tree, &args->tree, &entry));
}

/* Return if an argument is present. */
int
args_has(struct args *args, u_char ch)
{
	return (args_find(args, ch) == NULL ? 0 : 1);
}

char *
cmd_capture_pane_append(char *buf, size_t *len, char *line, size_t linelen)
{
	buf = xrealloc(buf, *len + linelen + 1);
	memcpy(buf + *len, line, linelen);
	*len += linelen;
	return (buf);
}

char *
cmd_capture_pane_pending(struct args *args, struct window_pane *wp,
    size_t *len)
{
	int 		*pending;
	char		*buf, *line, tmp[5];
	size_t		 linelen;
	u_int		 i;

	if (pending == NULL)
		return (xstrdup(""));									/* allocation site */

	buf = xstrdup("");											/* allocation site */
	if (args_has(args, 'C')) {
		for (i = 0; i < linelen; i++) {
			buf = cmd_capture_pane_append(buf, len, tmp,
			    strlen(tmp));
		}
	} else
		buf = cmd_capture_pane_append(buf, len, line, linelen);
	return (buf);
}

char *
cmd_capture_pane_history(struct args *args, struct cmd_q *cmdq,
    struct window_pane *wp, size_t *len)
{
	struct grid		*gd;
	int			 n, with_codes, escape_c0, join_lines;
	u_int			 i, sx, top, bottom, tmp;
	char			*cause, *buf, *line;
	size_t			 linelen;

	if (args_has(args, 'a')) {
		if (gd == NULL) {
			if (!args_has(args, 'q')) {
				return (NULL);
			}
			return (xstrdup(""));							/* allocation site */
		}
	}

	buf = NULL;
	for (i = top; i <= bottom; i++) {
		buf = cmd_capture_pane_append(buf, len, line, linelen);
		buf[(*len)++] = '\n';

		free(line);
	}
	return (buf);
}
enum cmd_retval
cmd_capture_pane_exec(struct cmd *self, struct cmd_q *cmdq)
{
	struct args		*args = self->args;
	struct client		*c;
	struct window_pane	*wp;
	char			*buf, *cause;
	const char		*bufname;
	size_t			 len;

	len = 0;
	if (args_has(args, 'P'))
		buf = cmd_capture_pane_pending(args, wp, &len);		
	else
		buf = cmd_capture_pane_history(args, cmdq, wp, &len);	
	if (buf == NULL)
		return (CMD_RETURN_ERROR);

	if (args_has(args, 'p')) {
		c = cmdq->client;
		if (c == NULL ||
		    (c->session != NULL && !(c->flags & CLIENT_CONTROL))) {
			/* memory leak */
			return (CMD_RETURN_ERROR);
		}
		bufname = NULL;
		if (paste_set(buf, len, bufname, &cause) != 0) {
			free(buf);
			free(cause);
			return (CMD_RETURN_ERROR);
		}
	}

	/* false memory leak */
	return (CMD_RETURN_NORMAL);
}

int main()
{
	struct cmd self;
	struct cmd_q cmdq;
	cmd_capture_pane_exec(&self, &cmdq);
	struct paste_buffer *p;
	char *b;
	RB_FOREACH(p, paste_name_tree, &paste_by_name)
		paste_free(p);
	return 0;
}
