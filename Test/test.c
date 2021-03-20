/*
    commit id : 0d6771b4648889ae5bc4235f9e3fc6cd82b710bd
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "./stdio.h"
#include "./openssh.h"

#  define MAXPATHLEN 64

int quiet = 0;

typedef unsigned int uid_t;
// char * tilde_expand_filename(const char *filename, uid_t uid) __attribute__((noinline));
size_t
strlcat(char * restrict dst, const char * restrict src, size_t maxlen) {
    const size_t srclen = strlen(src);
    const size_t dstlen = strnlen(dst, maxlen);
    if (dstlen == maxlen) return maxlen+srclen;
    if (srclen < maxlen-dstlen) {
        memcpy(dst+dstlen, src, srclen+1);
    } else {
        memcpy(dst+dstlen, src, maxlen-1);
        dst[dstlen+maxlen-1] = '\0';
    }
    return dstlen + srclen;
}

/*
 * Expands tildes in the file name.  Returns data allocated by xmalloc.
 * Warning: this calls getpw*.
 */
char *
tilde_expand_filename(const char *filename, uid_t uid)
{
	const char *path;
	char user[128], ret[MAXPATHLEN];
	struct passwd *pw;
	u_int len, slash;

	if (*filename != '~')
		return (xstrdup(filename));		/* allocation site */
	filename++;

	path = strchr(filename, '/');
	if (path != NULL && path > filename) {		/* ~user/path */
		slash = path - filename;
		if (slash > sizeof(user) - 1)
			fatal("tilde_expand_filename: ~username too long%s\n", "");
		memcpy(user, filename, slash);
		user[slash] = '\0';
	}

	/* Skip leading '/' from specified path */
	if (path != NULL)
		filename = path + 1;
	if (strlcat(ret, filename, sizeof(ret)) >= sizeof(ret))
		fatal("tilde_expand_filename: Path too long%s\n", "");

	return (xstrdup(ret));				/* allocation site */
}

int
read_keyfile_line(FILE *f, const char *filename, char *buf, size_t bufsz,
   u_long *lineno)
{
	while (__RANDBOOL) {
		(*lineno)++;
		printf("%s: %s line %lu exceeds size limit\n", __func__,
			    filename, *lineno);
		if(__RANDBOOL)
			return 0;
	}
	return -1;
}


static void
update_krl_from_file(struct passwd *pw, const char *file)
{
	Key *key = NULL;
	u_long lnum = 0;
	char *path, *cp, *ep, line[SSH_MAX_PUBKEY_BYTES];
	unsigned long long serial, serial2;
	int i, was_explicit_key, was_sha1, r;
	FILE *krl_spec;

	path = tilde_expand_filename(file, pw->pw_uid);
	if (strcmp(path, "-") == 0) {
		krl_spec = stdin;
		free(path);
		path = xstrdup("(standard input)");
	} else if ((krl_spec = fopen(path, "r")) == NULL)
		fatal("fopen %s: \n", path);

	if (!quiet)
		printf("Revoking from %s\n", path);
	while (read_keyfile_line(krl_spec, path, line, sizeof(line),
	    &lnum) == 0) {
		if(__RANDBOOL)
			fatal("%s: invalid\n", path);
	}
	if (strcmp(path, "-") != 0)
		fclose(krl_spec);
    /* memory leak */
}

int main(int argc, char **argv) {
    struct passwd pw;
    int updating;
	int i;
	srand(time(NULL));
	for (i = 0; i < argc; i++)
		update_krl_from_file(&pw, argv[i]);
    return 0;
}
