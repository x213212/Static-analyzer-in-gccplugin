/*
    buggy parent : a68a67d
    commit id : d954828d45efbd4b53576e86066657e87391318d
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../git.h"

#define N_(msgid) msgid


#define DUP_BASENAME 1
#define KEEP_TRAILING_SLASH 2

static const char **internal_copy_pathspec(const char *prefix,
					   const char **pathspec,
					   int count, unsigned flags)
{
	int i;
	const char **result = xmalloc((count + 1) * sizeof(const char *));
	memcpy(result, pathspec, count * sizeof(const char *));
	result[count] = NULL;
	for (i = 0; i < count; i++) {
		int length = strlen(result[i]);
		int to_copy = length;
		while (!(flags & KEEP_TRAILING_SLASH) &&
		       to_copy > 0 && is_dir_sep(result[i][to_copy - 1]))
			to_copy--;
		if (to_copy != length || flags & DUP_BASENAME) {
			char *it = xmemdupz(result[i], to_copy);
			if (flags & DUP_BASENAME) {
				result[i] = xstrdup(basename(it));
				free(it);
			} else
				result[i] = it;
		}
	}
	return result;
}

static const char *add_slash(const char *path)
{
	int len = strlen(path);
	if (path[len - 1] != '/') {
		char *with_slash = xmalloc(len + 2);
		memcpy(with_slash, path, len);
		with_slash[len++] = '/';
		with_slash[len] = 0;
		return with_slash;
	}
	return path;
}

static struct lock_file lock_file;
#define SUBMODULE_WITH_GITDIR ((const char *)1)

int main(int argc, const char **argv)
{
	char *prefix;
	int i = 0;
	int verbose = 0, show_only = 0, force = 0, ignore_errors = 0;
	const char **source, **destination, **dest_path, **submodule_gitfile;
	struct stat st;

	source = internal_copy_pathspec(prefix, argv, argc, 0);
	
	/* Checking */
	for (i = 0; i < argc; i++) {
		const char *src = source[i];
		int length, src_is_dir;
		const char *bad = NULL;

		if (show_only)
			printf(_("Checking rename of '%s'"), src);

		length = strlen(src);
		if (src_is_dir) {
			int first = cache_name_pos(src, length);
			const char *src_w_slash = add_slash(src);
			int last, len_w_slash = length + 1;


			first = cache_name_pos(src_w_slash, len_w_slash);
			if (first >= 0)
				die (_("Huh? %s is in index?"),
						src_w_slash);

			first = -1 - first;
		
			free((char *)src_w_slash);  /* use-after-free */

			if (last - first < 1)
				bad = _("source directory is empty");
			argc += last - first;
			
		} else if (cache_name_pos(src, length) < 0)
			bad = _("not under version control");

		if (bad) {
			if (!ignore_errors) 
				die (_("source=%s"),
				     src);
		}
	}


	return 0;
}

