/*
	buggy parent : 48291f0
	commit id : 51ac2a3202d55c439976ecce49229e35865c7ebd
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "./stdio.h"
#include "./tmux.h"

#define SIZE_MAX (1 << 30)

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

#define CMD_RETURN_YIELD CMD_RETURN_WAIT
#define CMD_RETURN_ATTACH CMD_RETURN_STOP

ARRAY_DECL(causelist, char *);

char			*cfg_cause;
int			 cfg_finished;
int	 		 cfg_references;
struct causelist	 cfg_causes;

void *
xrealloc(void *oldptr, size_t nmemb, size_t size)
{
	size_t	 newsize = nmemb * size;
	void	*newptr;

	if (newsize == 0)
		fatalx("zero size%s", "");
	if (1 << 30/ nmemb < size)
		fatalx("nmemb * size > SIZE_MAX%s", "");
	if ((newptr = realloc(oldptr, newsize)) == NULL)
		fatal("xrealloc failed%s", "");

	return (newptr);
}

int
cmd_string_getc(const char *s, size_t *p)
{
	const u_char	*ucs = s;

	if (ucs[*p] == '\0')
		return (EOF);
	return (ucs[(*p)++]);
}

void
cmd_string_ungetc(size_t *p)
{
	(*p)--;
}

char *
cmd_string_variable(const char *s, size_t *p)
{
	int			ch, fch;
	char		       *buf, *t;
	size_t			len;
	struct environ_entry   *envent;

#define cmd_string_first(ch) ((ch) == '_' || \
	((ch) >= 'a' && (ch) <= 'z') || ((ch) >= 'A' && (ch) <= 'Z'))
#define cmd_string_other(ch) ((ch) == '_' || \
	((ch) >= 'a' && (ch) <= 'z') || ((ch) >= 'A' && (ch) <= 'Z') || \
	((ch) >= '0' && (ch) <= '9'))

	buf = NULL;
	len = 0;

	fch = EOF;
	switch (ch = cmd_string_getc(s, p)) {
	case EOF:
		goto error;
	case '{':
		fch = '{';

		ch = cmd_string_getc(s, p);
		if (!cmd_string_first(ch))
			goto error;
		/* FALLTHROUGH */
	default:
		if (!cmd_string_first(ch)) {
			return (t);
		}

		buf = xrealloc(buf, 1, len + 1);
		buf[len++] = ch;

		for (;;) {
			ch = cmd_string_getc(s, p);
			if (ch == EOF || !cmd_string_other(ch))
				break;
			else {
				if (len >= SIZE_MAX - 3)
					goto error;
				buf = xrealloc(buf, 1, len + 1);
				buf[len++] = ch;
			}
		}
	}

	if (fch == '{' && ch != '}')
		goto error;
	if (ch != EOF && fch != '{')
		cmd_string_ungetc(p); /* ch */

	buf = xrealloc(buf, 1, len + 1);
	buf[len] = '\0';

	free(buf);
	if (envent == NULL)
		return (xstrdup(""));
	return (xstrdup(envent->value));

error:
	free(buf);
	return (NULL);
}

char *
cmd_string_string(const char *s, size_t *p, char endch, int esc)
{
	int	ch;
	char   *buf, *t;
	size_t	len;

	buf = NULL;
	len = 0;

	while ((ch = cmd_string_getc(s, p)) != endch) {
		switch (ch) {
		case EOF:
			goto error;
		case '\\':
			if (!esc)
				break;
			switch (ch = cmd_string_getc(s, p)) {
			case EOF:
				goto error;
			case 'e':
				ch = '\033';
				break;
			case 'r':
				ch = '\r';
				break;
			case 'n':
				ch = '\n';
				break;
			case 't':
				ch = '\t';
				break;
			}
			break;
		case '$':
			if (!esc)
				break;
			if ((t = cmd_string_variable(s, p)) == NULL)
				goto error;
			continue;
		}

		if (len >= SIZE_MAX - 2)
			goto error;
		buf = xrealloc(buf, 1, len + 1);
		buf[len++] = ch;
	}

	buf = xrealloc(buf, 1, len + 1);
	buf[len] = '\0';
	return (buf);

error:
	free(buf);
	return (NULL);
}

char *
cmd_string_expand_tilde(const char *s, size_t *p)
{
	struct passwd		*pw;
	struct environ_entry	*envent;
	char			*home, *path, *username;

	home = NULL;
	if (cmd_string_getc(s, p) == '/') {
		if (envent != NULL && *envent->value != '\0')
			home = envent->value;
		else if ((pw = getpwuid(getuid())) != NULL)
			home = pw->pw_dir;
	} else {
		cmd_string_ungetc(p);
		if ((username = cmd_string_string(s, p, '/', 0)) == NULL)
			return (NULL);
		if ((pw = getpwnam(username)) != NULL)
			home = pw->pw_dir;
		free(username);
	}
	if (home == NULL)
		return (NULL);

	return (strdup(home));
}
/*
 * Parse command string. Returns -1 on error. If returning -1, cause is error
 * string, or NULL for empty command.
 */
int
cmd_string_parse(const char *s, struct cmd_list **cmdlist, char **cause)
{
	size_t		p;
	int		ch, i, argc, rval;
	char	      **argv, *buf, *t;
	const char     *whitespace, *equals;
	size_t		len;

	argv = NULL;
	argc = 0;

	buf = NULL;
	len = 0;

	*cause = NULL;

	*cmdlist = NULL;
	rval = -1;

	p = 0;
	for (;;) {
		ch = cmd_string_getc(s, &p);
		switch (ch) {
		case '\'':
			if ((t = cmd_string_string(s, &p, '\'', 0)) == NULL)
				goto error;
			break;
		case '"':
			if ((t = cmd_string_string(s, &p, '"', 1)) == NULL)
				goto error;
			break;
		case '$':
			if ((t = cmd_string_variable(s, &p)) == NULL)
				goto error;
			break;
		case '#':
			/* Comment: discard rest of line. */
			while ((ch = cmd_string_getc(s, &p)) != EOF)
				;
			/* FALLTHROUGH */
		case EOF:
		case ' ':
		case '\t':
			if (buf != NULL) {
				buf = xrealloc(buf, 1, len + 1);
				buf[len] = '\0';

				argv = xrealloc(argv, argc + 1, sizeof *argv);
				argv[argc++] = buf;

				buf = NULL;
				len = 0;
			}

			if (ch != EOF)
				break;

			while (argc != 0) {
				equals = strchr(argv[0], '=');
				whitespace = argv[0] + strcspn(argv[0], " \t");
				if (equals == NULL || equals > whitespace)
					break;
				argc--;
				memmove(argv, argv + 1, argc * (sizeof *argv));
			}
			if (argc == 0)
				goto out;

			if (*cmdlist == NULL)
				goto out;

			rval = 0;
			goto out;
		case '~':
			if (buf == NULL) {
				t = cmd_string_expand_tilde(s, &p);
				if (t == NULL)
					goto error;
				break;
			}
			/* FALLTHROUGH */
		default:
			if (len >= SIZE_MAX - 2)
				goto error;

			buf = xrealloc(buf, 1, len + 1);
			buf[len++] = ch;
			break;
		}
	}

error:
out:
	free(buf);

	if (argv != NULL) {
		for (i = 0; i < argc; i++)
			free(argv[i]);
		free(argv);
	}

	return (rval);
}


/*
 * Load configuration file. Returns -1 for an error with a list of messages in
 * causes. Note that causes must be initialised by the caller!
 */
enum cmd_retval
load_cfg(const char *path, struct cmd_ctx *ctxin, struct causelist *causes)
{
	FILE		*f;
	u_int		 n;
	char		*buf, *copy, *line, *cause;
	size_t		 len, oldlen;
	struct cmd_list	*cmdlist;
	struct cmd_ctx	 ctx;
	enum cmd_retval	 retval;

	if ((f = fopen(path, "rb")) == NULL) {
		return (CMD_RETURN_ERROR);
	}

	cfg_references++;

	n = 0;
	line = NULL;
	retval = CMD_RETURN_NORMAL;
	while ((buf = fgets(buf, len, f))) {
		/* Trim \n. */
		if (buf[len - 1] == '\n')
			len--;
		printf ("%s: %s", path, buf);

		/* Current line is the continuation of the previous one. */
		if (line != NULL) {
			oldlen = strlen(line);
			line = xrealloc(line, 1, oldlen + len + 1);	/* allocation site */
		} else {
			oldlen = 0;
			line = xmalloc(len + 1);					/* allocation site */
		}

		/* Append current line to the previous. */
		memcpy(line + oldlen, buf, len);
		line[oldlen + len] = '\0';
		n++;

		/* Continuation: get next line? */
		len = strlen(line);
		if (len > 0 && line[len - 1] == '\\') {
			line[len - 1] = '\0';

			/* Ignore escaped backslash at EOL. */
			if (len > 1 && line[len - 2] != '\\')
				continue;
		}
		copy = line;									
		line = NULL;

		/* Skip empty lines. */
		buf = copy;
		while (isspace((u_char)*buf))
			buf++;
		if (*buf == '\0') {
			continue;						/* memory leak */
		}

		if (cmd_string_parse(buf, &cmdlist, &cause) != 0) {
			free(copy);
			if (cause == NULL)
				continue;
			free(cause);
			continue;
		}
		free(copy);
		if (cmdlist == NULL)
			continue;

		if (ctxin == NULL) {
			ctx.msgdata = NULL;
			ctx.curclient = NULL;
			ctx.cmdclient = NULL;
		} else {
			ctx.msgdata = ctxin->msgdata;
			ctx.curclient = ctxin->curclient;
			ctx.cmdclient = ctxin->cmdclient;
		}


		cfg_cause = NULL;
	}
	if (line != NULL) {
		free(line);
	}
	fclose(f);

	cfg_references--;

	return (retval);											
}

int main()
{
	struct cmd_ctx ctxin;
	struct causelist causes;
	causes.list = malloc(1);
	ARRAY_INIT(&causes);
	load_cfg("path", &ctxin, &causes);
	return 0;
}

















































































