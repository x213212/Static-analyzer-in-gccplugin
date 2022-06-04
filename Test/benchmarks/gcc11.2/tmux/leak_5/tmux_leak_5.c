/*
	buggy parent : 40fefe2
	commit id : 933929cd622478bb43afe590670613da2e9ff359
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

/*
 * Job scheduling. Run queued commands in the background and record their
 * output.
 */

void	job_callback(struct bufferevent *, short, void *);
void	job_write_callback(struct bufferevent *, void *);

/* Start a job running, if it isn't already. */
struct job *
job_run(const char *cmd, struct session *s, const char *cwd, void *data)
{
	struct job	*job;
	struct environ	*env;
	pid_t		 pid;
	int		 nullfd, out[2];
	const char	*home;

	switch (pid = fork()) {
	case -1:
		close(out[0]);
		close(out[1]);
		return (NULL);
	case 0:		/* child */

		execl(_PATH_BSHELL, "sh", "-c", cmd, (char *) NULL);
		fatal("execl failed%s", "");
	}

	/* parent */
	close(out[1]);

	job = xmalloc(sizeof *job);
	job->state = JOB_RUNNING;

	job->cmd = xstrdup(cmd);
	job->pid = pid;
	job->status = 0;

	job->fd = out[0];
	
	return (job);
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

enum cmd_retval
cmd_if_shell_exec(struct cmd *self, struct cmd_q *cmdq)
{
	struct args			*args = self->args;
	struct cmd_if_shell_data	*cdata;
	char				*shellcmd, *cmd, *cause;
	struct cmd_list			*cmdlist;
	struct client			*c;
	struct session			*s = NULL;
	struct winlink			*wl = NULL;
	struct window_pane		*wp = NULL;
	struct format_tree		*ft;
	const char			*cwd;

	shellcmd = malloc(strlen(args->argv[0]));		/* allocation site */

	if (args_has(args, 'F')) {
		cmd = NULL;
		if (*shellcmd != '0' && *shellcmd != '\0')
			cmd = args->argv[1];
		else if (args->argc == 3)
			cmd = args->argv[2];
		if (cmd == NULL)
			return (CMD_RETURN_NORMAL);					/* memory leak */
		if (cause != NULL) {
			printf("%s", cause);
			free(cause);
			return (CMD_RETURN_ERROR);					/* memory leak */
		}
		
		return (CMD_RETURN_NORMAL);						/* memory leak */
	}

	cdata = xmalloc(sizeof *cdata);

	cdata->cmd_if = xstrdup(args->argv[1]);
	if (args->argc == 3)
		cdata->cmd_else = xstrdup(args->argv[2]);
	else
		cdata->cmd_else = NULL;

	cdata->cmdq = cmdq;

	cdata->references = 1;
	job_run(shellcmd, s, cwd, cdata);
	free(shellcmd);

	if (cdata->bflag)
		return (CMD_RETURN_NORMAL);
	return (CMD_RETURN_WAIT);
}

int main()
{
	struct cmd self;
	struct cmd_q cmdq;
	cmd_if_shell_exec(&self, &cmdq);
	return 0;
}


