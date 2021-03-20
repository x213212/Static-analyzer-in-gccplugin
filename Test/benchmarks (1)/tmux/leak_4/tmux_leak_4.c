/*
	buggy parent : 54bcaab
	commit id : d566c780e54010112d499707cd80a594144d1a89
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>

#include "../stdio.h"
#include "../tmux.h"

#define _PATH_TMP "/tmp"
#define S_IRWXU 0
#define S_IRWXO 0

size_t
strlcpy(char *dst, const char *src, size_t siz)
{
        char *d = dst;
        const char *s = src;
        size_t n = siz;

        /* Copy as many bytes as will fit */
        if (n != 0 && --n != 0) {
                do {
                        if ((*d++ = *s++) == 0)
                                break;
                } while (--n != 0);
        }

        /* Not enough room in dst, add NUL and traverse rest of src */
        if (n == 0) {
                if (siz != 0)
                        *d = '\0';                /* NUL-terminate dst */
                while (*s++)
                        ;
        }

        return(s - src - 1);        /* count does not include NUL */
}

int vasprintf(char **ret, const char *fmt, char *ap)
{
	*ret = malloc(strlen(fmt) + strlen(ap));		/* allocation site */
	if(*ret == NULL) return -1;
	return 0;
}

int
xasprintf(char **ret, const char *fmt, char *ap)
{
	int i;

	i = xvasprintf(ret, fmt, ap);

	return i;
}

int
xvasprintf(char **ret, const char *fmt, va_list ap)
{
	int i;

	i = vasprintf(ret, fmt, ap);

	if (i < 0 || *ret == NULL)
		fatalx("xasprintf: %s", strerror(errno));

	return i;
}

static char *
make_label(const char *label)
{
	char		*base, resolved[PATH_MAX], *path, *s;
	struct stat	 sb;
	uid_t		 uid;
	int		 saved_errno;

	if (label == NULL)
		label = "default";
	uid = getuid();

	if ((s = getenv("TMUX_TMPDIR")) != NULL && *s != '\0')
		xasprintf(&base, "%s/tmux-", s);
	else
		xasprintf(&base, "%s/tmux-", _PATH_TMP);

	if (mkdir(base, S_IRWXU) != 0 && errno != EEXIST)
		goto fail;

	if (lstat(base, &sb) != 0)
		goto fail;
	if (!S_ISDIR(sb.st_mode)) {
		errno = ENOTDIR;
		goto fail;
	}
	if (sb.st_uid != uid || (sb.st_mode & S_IRWXO) != 0) {
		errno = EACCES;
		goto fail;
	}

	if (realpath(base, resolved) == NULL)
		strlcpy(resolved, base, sizeof resolved);

	/* memory leak */	
	return (path);

fail:
	saved_errno = errno;
	free(base);
	errno = saved_errno;
	return (NULL);
}

int main ()
{
	make_label("label");

	return 0;
}
