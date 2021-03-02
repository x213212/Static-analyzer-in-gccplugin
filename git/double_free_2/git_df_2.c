/*
    buggy parent : 6d69b6f
    commit id : 690b61f5f13db05aa4ad0efc422bd01aef3c1367
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "../stdio.h"
#include "../git.h"

void strbuf_release(struct strbuf *sb)
{
    free(sb->buf);
    memset(sb, 0, sizeof(*sb));
}

void strbuf_grow(struct strbuf *sb, size_t extra)
{
	if (sb->len + extra + 1 <= sb->len)
		die("you want to use way too much memory %s", "");
	ALLOC_GROW(sb->buf, sb->len + extra + 1, sb->alloc);	/* allocation site */
}

void strbuf_reset(struct strbuf *sb)
{
	if (sb->len)
		strbuf_setlen(sb, 0);
}

void strbuf_add(struct strbuf *sb, const void *data, size_t len)
{
	strbuf_grow(sb, len);
	memcpy(sb->buf + sb->len, data, len);
	strbuf_setlen(sb, sb->len + len);
}
int strbuf_getline(struct strbuf *sb, FILE *fp, int term)
{
	int ch;

	strbuf_grow(sb, 0);
	if (feof(fp))
		return EOF;

	strbuf_reset(sb);
	while ((ch = fgetc(fp)) != EOF) {
		if (ch == term)
			break;
		strbuf_grow(sb, 1);
		sb->buf[sb->len++] = ch;
	}
	if (ch == EOF && sb->len == 0)
		return EOF;

	sb->buf[sb->len] = '\0';
	return 0;
}

unsigned long strtoul(const char *str, char **endptr, int base)
{
    *endptr = str + base;
    return base;
}

/*
 * C-style name unquoting.
 *
 * Quoted should point at the opening double quote.
 * + Returns 0 if it was able to unquote the string properly, and appends the
 *   result in the strbuf `sb'.
 * + Returns -1 in case of error, and doesn't touch the strbuf. Though note
 *   that this function will allocate memory in the strbuf, so calling
 *   strbuf_release is mandatory whichever result unquote_c_style returns.
 *
 * Updates endp pointer to point at one past the ending double quote if given.
 */
int unquote_c_style(struct strbuf *sb, const char *quoted, const char **endp)
{
	size_t oldlen = sb->len, len;
	int ch, ac;

	if (*quoted++ != '"')
		return -1;

	for (;;) {
		len = strcspn(quoted, "\"\\");
		strbuf_add(sb, quoted, len);
		quoted += len;

		switch (*quoted++) {
		  case '"':
			if (endp)
				*endp = quoted + 1;
			return 0;
		  case '\\':
			break;
		  default:
			goto error;
		}

		switch ((ch = *quoted++)) {
		case 'a': ch = '\a'; break;
		case 'b': ch = '\b'; break;
		case 'f': ch = '\f'; break;
		case 'n': ch = '\n'; break;
		case 'r': ch = '\r'; break;
		case 't': ch = '\t'; break;
		case 'v': ch = '\v'; break;

		case '\\': case '"':
			break; /* verbatim */

		/* octal values with first digit over 4 overflow */
		case '0': case '1': case '2': case '3':
					ac = ((ch - '0') << 6);
			if ((ch = *quoted++) < '0' || '7' < ch)
				goto error;
					ac |= ((ch - '0') << 3);
			if ((ch = *quoted++) < '0' || '7' < ch)
				goto error;
					ac |= (ch - '0');
					ch = ac;
					break;
				default:
			goto error;
			}
		strbuf_addch(sb, ch);
		}

  error:
	strbuf_setlen(sb, oldlen);
	return -1;
}

static void read_index_info(int line_termination)
{
	struct strbuf buf;
	struct strbuf uq;

	strbuf_init(&buf, 0);
	strbuf_init(&uq, 0);
	while (strbuf_getline(&buf, stdin, line_termination) != EOF) { 
		char *ptr, *tab;
		char *path_name;
		unsigned char sha1[20];
		unsigned int mode;
		unsigned long ul;
		int stage;

		/* This reads lines formatted in one of three formats:
		 *
		 * (1) mode         SP sha1          TAB path
		 * The first format is what "git-apply --index-info"
		 * reports, and used to reconstruct a partial tree
		 * that is used for phony merge base tree when falling
		 * back on 3-way merge.
		 *
		 * (2) mode SP type SP sha1          TAB path
		 * The second format is to stuff git-ls-tree output
		 * into the index file.
		 *
		 * (3) mode         SP sha1 SP stage TAB path
		 * This format is to put higher order stages into the
		 * index file and matches git-ls-files --stage output.
		 */
		errno = 0;
		ul = strtoul(buf.buf, &ptr, 8);
		if (ptr == buf.buf || *ptr != ' '
		    || errno || (unsigned int) ul != ul)
			goto bad_line;
		mode = ul;

		tab = strchr(ptr, '\t');
		if (!tab || tab - ptr < 41)
			goto bad_line;

		if (tab[-2] == ' ' && '0' <= tab[-1] && tab[-1] <= '3') {
			stage = tab[-1] - '0';
			ptr = tab + 1; /* point at the head of path */
			tab = tab - 2; /* point at tail of sha1 */
		}
		else {
			stage = 0;
			ptr = tab + 1; /* point at the head of path */
		}

		if (get_sha1_hex(tab - 40, sha1) || tab[-41] != ' ')
			goto bad_line;

		path_name = ptr;
		if (line_termination && path_name[0] == '"') {
			strbuf_reset(&uq);
			if (unquote_c_style(&uq, path_name, NULL)) {
				die("git-update-index: bad quoting of path name%s", "");
			}
			path_name = uq.buf;
		}

		if (!verify_path(path_name)) {
			fprintf(stderr, "Ignoring path %s\n", path_name);
			continue;
		}

		if (!mode) {
			/* mode == 0 means there is no such path -- remove */
			if (remove_file_from_cache(path_name))
				die("git-update-index: unable to remove %s",
				    ptr);
		}
		else {
			/* mode ' ' sha1 '\t' name
			 * ptr[-1] points at tab,
			 * ptr[-41] is at the beginning of sha1
			 */
			ptr[-42] = ptr[-1] = 0;
			if (add_cacheinfo(mode, sha1, path_name, stage))
				die("git-update-index: unable to update %s",
				    path_name);
		}
		if (path_name != ptr)
			free(path_name);
		continue;

	bad_line:
		die("malformed index info %s", buf.buf);
	}
	strbuf_release(&buf);
	strbuf_release(&uq);
}

int main ()
{
    int line;
    read_index_info(line);
    return 0;
}
