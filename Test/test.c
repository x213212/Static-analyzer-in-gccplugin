/* 
	buggy parent : a715878
	commit id : 063a474fe055cadcb7f7f64e5061ed65d506c407
*/

#include <stdio.h>
#include <stdlib.h>

#include "stdio.h"
#include "tmux.h"

#define SIZE_MAX (1 << 30)

/*
 * Parse a command from a string.
 */

int	cmd_string_getc(const char *, size_t *);
void	cmd_string_ungetc(const char *, size_t *);
char   *cmd_string_string(const char *, size_t *, char, int);
char   *cmd_string_variable(const char *, size_t *);
char   *cmd_string_expand_tilde(const char *, size_t *);

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

int
cmd_string_getc(const char *s, size_t *p)
{
	if (s[*p] == '\0')
		return (EOF);
	return (s[(*p)++]);
}

void
cmd_string_ungetc(const char *s, size_t *p)
{
	(*p)--;
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
			buf = xrealloc(buf, 1, len + strlen(t) + 1);
			strcpy(buf + len, t);
			len += strlen(t);
			xfree(t);
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
	if (buf != NULL)
		xfree(buf);
	return (NULL);
}

char *
cmd_string_variable(const char *s, size_t *p)
{
	int	ch, fch;
	char   *buf, *t;
	size_t	len;

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
			t = strdup("$");
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
		cmd_string_ungetc(s, p); /* ch */

	buf = xrealloc(buf, 1, len + 1);
	buf[len] = '\0';

	if ((t = getenv(buf)) == NULL) {
		xfree(buf);
		return (xstrdup(""));
	}
	xfree(buf);
	return (xstrdup(t));

error:
	if (buf != NULL)
		xfree(buf);
	return (NULL);
}

char *
cmd_string_expand_tilde(const char *s, size_t *p)
{
	struct passwd	*pw;
	char		*home, *path, *username;

	home = NULL;
	if (cmd_string_getc(s, p) == '/') {
		if ((home = getenv("HOME")) == NULL) {
			if ((pw = getpwuid(getuid())) != NULL)
				home = pw->pw_dir;
		}
	} else {
		cmd_string_ungetc(s, p);
		if ((username = cmd_string_string(s, p, '/', 0)) == NULL)
			return (NULL);
		if ((pw = getpwnam(username)) != NULL)
			home = pw->pw_dir;
		if (username != NULL)
			xfree(username);
	}
	if (home == NULL)
		return (NULL);

	path = strdup(home);
	return (path);
}
/*
 * Parse command string. Returns -1 on error. If returning -1, cause is error
 * string, or NULL for empty command.
 */
int
cmd_string_parse(const char *s, struct cmd_list **cmdlist, char **cause)
{
	size_t		p;
	int		ch, i, argc, rval, have_arg;
	char	      **argv, *buf, *t;
	const char     *whitespace, *equals;
	size_t		len;

	argv = NULL;
	argc = 0;

	buf = NULL;
	len = 0;

	have_arg = 0;

	*cause = NULL;

	*cmdlist = NULL;
	rval = -1;

	p = 0;
	for (;;) {
		ch = cmd_string_getc(s, &p);
		switch (ch) {
		case '#':
			/* Comment: discard rest of line. */
			while ((ch = cmd_string_getc(s, &p)) != EOF)
				;
			/* FALLTHROUGH */
		case EOF:
		case ' ':
		case '\t':
 			if (have_arg) {
				buf = xrealloc(buf, 1, len + 1); /* allocation site */
				buf[len] = '\0';

				argv = xrealloc(argv, argc + 1, sizeof *argv);
				argv[argc++] = buf;

				buf = NULL;
				len = 0;

				have_arg = 0;
			}

			if (ch != EOF)
				break;

			while (argc != 0) {
				equals = strchr(argv[0], '=');
				whitespace = argv[0] + strcspn(argv[0], " \t");
				if (equals == NULL || equals > whitespace)
					break;
				argc--;
				memmove(argv, argv + 1, argc * (sizeof *argv));	/* memory leak */
			}
			if (argc == 0)
				goto out;

			do
				xfree(argv[argc - 1]);			
			while (--argc > 0);

			rval = 0;
			goto out;
		/* FALLTHROUGH */
		default:
			if (len >= SIZE_MAX - 2)
				goto error;

			buf = xrealloc(buf, 1, len + 1);
			buf[len++] = ch;

			have_arg = 1;
			break;
		}
	}

error:

out:
	if (buf != NULL)
		xfree(buf);

	if (argv != NULL) {
		for (i = 0; i < argc; i++)
			xfree(argv[argc]);					/* double-free */
		xfree(argv);
	}

	return (rval);
}

int main(int argc, char **argv)
{
	char *cause;
	struct cmd_list *cmdlist;

	if(argc > 1)
		cmd_string_parse(argv[1], &cmdlist, &cause);	

	return 0;
}

