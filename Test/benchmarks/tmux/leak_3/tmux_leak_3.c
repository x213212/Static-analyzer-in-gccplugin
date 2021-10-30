/*
	buggy parent : c8ecbf3
	commit id : 2c9bdd9e326723fb392aed4d8df12cba7ef34f1f
*/

#define CMDQ

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "../stdio.h"
#include "../tmux.h"

/* Command definition. */
struct cmd_entry {
    const char      *name;
    const char      *alias;

    struct {
        const char  *template;
        int      lower;
        int      upper;
    } args;
    const char      *usage;

#define CMD_STARTSERVER 0x1
#define CMD_READONLY 0x2
#define CMD_AFTERHOOK 0x4
    int      flags;

    enum cmd_retval      (*exec)(struct cmd *, struct cmdq_item *);
};

struct cmd_state {
	struct client		*c;
	struct cmd_find_state	 tflag;
	struct cmd_find_state	 sflag;
};

struct cmdq_item {
	const char		*name;
	struct cmdq_list	*queue;
	struct cmdq_item	*next;

	struct client		*client;

	enum cmdq_type		 type;
	u_int			 group;

	u_int			 number;
	time_t			 time;

	int			 flags;
#define CMDQ_FIRED 0x1
#define CMDQ_WAITING 0x2
#define CMDQ_NOHOOKS 0x4

	struct cmdq_shared	*shared;
	struct cmd_list		*cmdlist;
	struct cmd		*cmd;

	cmdq_cb			 cb;
	void			*data;

	struct cmd_state	 state;

	TAILQ_ENTRY(cmdq_item)	 entry;
};
/* Resolve an absolute path or relative to client working directory. */
char *
server_client_get_path(struct client *c, const char *file)
{
	char	*path, resolved[PATH_MAX];

	if (*file == '/')
		path = xstrdup(file);				/* allocation site */
	if (realpath(path, resolved) == NULL)
		return (path);
	free(path);	
	return (xstrdup(resolved));				/* allocation site */
}

static enum cmd_retval
cmd_load_buffer_exec(struct cmd *self, struct cmdq_item *item)
{
	struct args			*args = self->args;
	struct cmd_load_buffer_data	*cdata;
	struct client			*c = item->client;
	FILE				*f;
	const char			*path, *bufname;
	char				*pdata, *new_pdata, *cause, *file;
	size_t				 psize;
	int				 ch, error;

	bufname = NULL;

	path = args->argv[0];

	file = server_client_get_path(c, path);			
	f = fopen(file, "rb");
	if (f == NULL) {
		printf("%s", file);
		free(file);
		return (CMD_RETURN_ERROR);
	}

	pdata = NULL;
	psize = 0;
	if (ferror(f)) {
		printf("%s: read error", file);
		goto error;
	}
	if (pdata != NULL)
		pdata[psize] = '\0';

	fclose(f);
	free(file);

	return (CMD_RETURN_NORMAL);

error:
	free(pdata);
	if (f != NULL)
		fclose(f);
	/* memory leak */
	return (CMD_RETURN_ERROR);
}

static enum cmd_retval
cmd_save_buffer_exec(struct cmd *self, struct cmdq_item *item)
{
	struct args		*args = self->args;
	struct client		*c = item->client;
	struct paste_buffer	*pb;
	const char		*path, *bufname, *bufdata, *start, *end;
	const char		*flags;
	char			*msg, *file;
	size_t			 size, used, msglen, bufsize;
	FILE			*f;

	if (strcmp(path, "-") == 0) {
		if (c == NULL) {
			return (CMD_RETURN_ERROR);
		}
		if (c->session == NULL || (c->flags & CLIENT_CONTROL))
			goto do_stdout;
		goto do_print;
	}

	flags = "wb";

	file = server_client_get_path(c, path);				
	f = fopen(file, flags);
	if (f == NULL) {
		printf("%s", file);
		free(file);
		return (CMD_RETURN_ERROR);
	}

	if (fwrite(bufdata, 1, bufsize, f) != bufsize) {
		printf("%s: write error", file);
		fclose(f);
		/* memory leak */
		return (CMD_RETURN_ERROR);
	}

	fclose(f);
	free(file);

	return (CMD_RETURN_NORMAL);

do_stdout:
	return (CMD_RETURN_NORMAL);

do_print:
	if (bufsize > (INT_MAX / 4) - 1) {
		return (CMD_RETURN_ERROR);
	}
	msg = NULL;

	used = 0;

	free(msg);
	return (CMD_RETURN_NORMAL);
}

int main()
{
	struct cmd self;
	struct cmdq_item item;
	cmd_load_buffer_exec(&self, &item);
	cmd_save_buffer_exec(&self, &item);
	return 0;
}


