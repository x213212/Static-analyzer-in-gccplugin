/*
    buggy parent : 0179ca7
    commit id : 38ae8784074852c8e7b651f4f6e44e07466da7e1
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "../stdio.h"
#include "../git.h"

enum object_type {
	OBJ_BAD = -1,
	OBJ_NONE = 0,
	OBJ_COMMIT = 1,
	OBJ_TREE = 2,
	OBJ_BLOB = 3,
	OBJ_TAG = 4,
	/* 5 for future expansion */
	OBJ_OFS_DELTA = 6,
	OBJ_REF_DELTA = 7,
	OBJ_ANY,
	OBJ_MAX
};

static inline enum object_type object_type(unsigned int mode)
{
	return S_ISDIR(mode) ? OBJ_TREE :
		S_ISGITLINK(mode) ? OBJ_COMMIT :
		OBJ_BLOB;
}

/* Double-check local_repo_env below if you add to this list. */
#define GIT_DIR_ENVIRONMENT "GIT_DIR"
#define GIT_COMMON_DIR_ENVIRONMENT "GIT_COMMON_DIR"
#define GIT_NAMESPACE_ENVIRONMENT "GIT_NAMESPACE"
#define GIT_WORK_TREE_ENVIRONMENT "GIT_WORK_TREE"
#define GIT_PREFIX_ENVIRONMENT "GIT_PREFIX"
#define DEFAULT_GIT_DIR_ENVIRONMENT ".git"
#define DB_ENVIRONMENT "GIT_OBJECT_DIRECTORY"
#define INDEX_ENVIRONMENT "GIT_INDEX_FILE"
#define GRAFT_ENVIRONMENT "GIT_GRAFT_FILE"
#define GIT_SHALLOW_FILE_ENVIRONMENT "GIT_SHALLOW_FILE"
#define TEMPLATE_DIR_ENVIRONMENT "GIT_TEMPLATE_DIR"
#define CONFIG_ENVIRONMENT "GIT_CONFIG"
#define CONFIG_DATA_ENVIRONMENT "GIT_CONFIG_PARAMETERS"
#define EXEC_PATH_ENVIRONMENT "GIT_EXEC_PATH"
#define CEILING_DIRECTORIES_ENVIRONMENT "GIT_CEILING_DIRECTORIES"
#define NO_REPLACE_OBJECTS_ENVIRONMENT "GIT_NO_REPLACE_OBJECTS"
#define GITATTRIBUTES_FILE ".gitattributes"
#define INFOATTRIBUTES_FILE "info/attributes"
#define ATTRIBUTE_MACRO_PREFIX "[attr]"
#define GIT_NOTES_REF_ENVIRONMENT "GIT_NOTES_REF"
#define GIT_NOTES_DEFAULT_REF "refs/notes/commits"
#define GIT_NOTES_DISPLAY_REF_ENVIRONMENT "GIT_NOTES_DISPLAY_REF"
#define GIT_NOTES_REWRITE_REF_ENVIRONMENT "GIT_NOTES_REWRITE_REF"
#define GIT_NOTES_REWRITE_MODE_ENVIRONMENT "GIT_NOTES_REWRITE_MODE"
#define GIT_LITERAL_PATHSPECS_ENVIRONMENT "GIT_LITERAL_PATHSPECS"
#define GIT_GLOB_PATHSPECS_ENVIRONMENT "GIT_GLOB_PATHSPECS"
#define GIT_NOGLOB_PATHSPECS_ENVIRONMENT "GIT_NOGLOB_PATHSPECS"
#define GIT_ICASE_PATHSPECS_ENVIRONMENT "GIT_ICASE_PATHSPECS"

/*
 * This environment variable is expected to contain a boolean indicating
 * whether we should or should not treat:
 *
 *   GIT_DIR=foo.git git ...
 *
 * as if GIT_WORK_TREE=. was given. It's not expected that users will make use
 * of this, but we use it internally to communicate to sub-processes that we
 * are in a bare repo. If not set, defaults to true.
 */
#define GIT_IMPLICIT_WORK_TREE_ENVIRONMENT "GIT_IMPLICIT_WORK_TREE"

/*
 * Repository-local GIT_* environment variables; these will be cleared
 * when git spawns a sub-process that runs inside another repository.
 * The array is NULL-terminated, which makes it easy to pass in the "env"
 * parameter of a run-command invocation, or to do a simple walk.
 */
extern const char * const local_repo_env[];

extern int is_bare_repository_cfg;
extern int is_bare_repository(void);
extern int is_inside_git_dir(void);
extern char *git_work_tree_cfg;
extern int is_inside_work_tree(void);
extern const char *get_git_dir(void);
extern const char *get_git_common_dir(void);
extern int is_git_directory(const char *path);
extern char *get_object_directory(void);
extern char *get_index_file(void);
extern char *get_graft_file(void);
extern int set_git_dir(const char *path);
extern int get_common_dir(struct strbuf *sb, const char *gitdir);
extern const char *get_git_namespace(void);
extern const char *strip_namespace(const char *namespaced_ref);
extern const char *get_git_work_tree(void);

#define READ_GITFILE_ERR_STAT_FAILED 1
#define READ_GITFILE_ERR_NOT_A_FILE 2
#define READ_GITFILE_ERR_OPEN_FAILED 3
#define READ_GITFILE_ERR_READ_FAILED 4
#define READ_GITFILE_ERR_INVALID_FORMAT 5
#define READ_GITFILE_ERR_NO_PATH 6
#define READ_GITFILE_ERR_NOT_A_REPO 7
#define READ_GITFILE_ERR_TOO_LARGE 8
extern const char *read_gitfile_gently(const char *path, int *return_error_code);
#define read_gitfile(path) read_gitfile_gently((path), NULL)
extern const char *resolve_gitdir(const char *suspect);
extern void set_git_work_tree(const char *tree);

#define ALTERNATE_DB_ENVIRONMENT "GIT_ALTERNATE_OBJECT_DIRECTORIES"


/*
 * Do not use this for inspecting *tracked* content.  When path is a
 * symlink to a directory, we do not want to say it is a directory when
 * dealing with tracked content in the working tree.
 */
int is_directory(const char *path)
{
	struct stat st;
	return (!stat(path, &st) && S_ISDIR(st.st_mode));
}

/* We allow "recursive" symbolic links. Only within reason, though. */
#define MAXDEPTH 5

/*
 * Return the real path (i.e., absolute path, with symlinks resolved
 * and extra slashes removed) equivalent to the specified path.  (If
 * you want an absolute path but don't mind links, use
 * absolute_path().)  The return value is a pointer to a static
 * buffer.
 *
 * The input and all intermediate paths must be shorter than MAX_PATH.
 * The directory part of path (i.e., everything up to the last
 * dir_sep) must denote a valid, existing directory, but the last
 * component need not exist.  If die_on_error is set, then die with an
 * informative error message if there is a problem.  Otherwise, return
 * NULL on errors (without generating any output).
 *
 * If path is our buffer, then return path, as it's already what the
 * user wants.
 */
 
const char *real_path(const char *path)
{
	return real_path_internal(path, 1);
}


/*
 * Try to read the location of the git directory from the .git file,
 * return path to git directory if found.
 *
 * On failure, if return_error_code is not NULL, return_error_code
 * will be set to an error code and NULL will be returned. If
 * return_error_code is NULL the function will die instead (for most
 * cases).
 */
const char *read_gitfile_gently(const char *path, int *return_error_code)
{
	const int max_file_size = 1 << 20;  /* 1MB */
	int error_code = 0;
	char *buf = NULL;
	char *dir = NULL;
	const char *slash;
	struct stat st;
	int fd;
	ssize_t len;

	if (stat(path, &st)) {
		error_code = READ_GITFILE_ERR_STAT_FAILED;
		goto cleanup_return;
	}
	if (!S_ISREG(st.st_mode)) {
		error_code = READ_GITFILE_ERR_NOT_A_FILE;
		goto cleanup_return;
	}
	if (st.st_size > max_file_size) {
		error_code = READ_GITFILE_ERR_TOO_LARGE;
		goto cleanup_return;
	}
	fd = open(path, O_RDONLY);
	if (fd < 0) {
		error_code = READ_GITFILE_ERR_OPEN_FAILED;
		goto cleanup_return;
	}
	buf = xmalloc(st.st_size + 1);                          /* allocation site */
	len = read_in_full(fd, buf, st.st_size);
	close(fd);
	if (len != st.st_size) {
		error_code = READ_GITFILE_ERR_READ_FAILED;
		goto cleanup_return;
	}
	buf[len] = '\0';
	if (!starts_with(buf, "gitdir: ")) {
		error_code = READ_GITFILE_ERR_INVALID_FORMAT;
		goto cleanup_return;
	}
	while (buf[len - 1] == '\n' || buf[len - 1] == '\r')
		len--;
	if (len < 9) {
		error_code = READ_GITFILE_ERR_NO_PATH;
		goto cleanup_return;
	}
	buf[len] = '\0';
	dir = buf + 8;

	if (!is_absolute_path(dir) && (slash = strrchr(path, '/'))) {
		size_t pathlen = slash+1 - path;
		size_t dirlen = pathlen + len - 8;
		dir = xmalloc(dirlen + 1);                          /* allocation site */
		strncpy(dir, path, pathlen);
		strncpy(dir + pathlen, buf + 8, len - 8);
		dir[dirlen] = '\0';
		free(buf);
		buf = dir;
	}
	if (!is_git_directory(dir)) {
		error_code = READ_GITFILE_ERR_NOT_A_REPO;
		goto cleanup_return;
	}
	update_linked_gitdir(path, dir);
	path = real_path(dir);

cleanup_return:
	free(buf);

	if (return_error_code)
		*return_error_code = error_code;

	if (error_code) {
		if (return_error_code)
			return NULL;

		switch (error_code) {
		case READ_GITFILE_ERR_STAT_FAILED:
		case READ_GITFILE_ERR_NOT_A_FILE:
			return NULL;
		case READ_GITFILE_ERR_OPEN_FAILED:
			die_errno("Error opening '%s'", path);
		case READ_GITFILE_ERR_TOO_LARGE:
			die("Too large to be a .git file: '%s'", path);
		case READ_GITFILE_ERR_READ_FAILED:
			die("Error reading %s", path);
		case READ_GITFILE_ERR_INVALID_FORMAT:
			die("Invalid gitfile format: %s", path);
		case READ_GITFILE_ERR_NO_PATH:
			die("No path in gitfile: %s", path);
		case READ_GITFILE_ERR_NOT_A_REPO:
			die("Not a git repository: %s", dir);           /* use-after-free */
		default:
			assert(0);
		}
	}

	return path;
}

int main()
{
    int error_code;
    read_gitfile_gently("path", &error_code);
    return 0;
}




