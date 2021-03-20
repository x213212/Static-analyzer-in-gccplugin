/*
    buggy parent : 9d1d2b7
    commit id : 2e1175d43d05e83fe836e1c8c8e7c25b7ee659ae
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "../stdio.h"
#include "../git.h"

#define die_errno die

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
#define GIT_IMPLICIT_WORK_TREE_ENVIRONMENT "GIT_IMPLICIT_WORK_TREE"

#define RUN_COMMAND_NO_STDIN 1
#define RUN_GIT_CMD	     2	/*If this is to be git sub-command */
#define RUN_COMMAND_STDOUT_TO_STDERR 4
#define RUN_SILENT_EXEC_FAILURE 8
#define RUN_USING_SHELL 16
#define RUN_CLEAN_ON_EXIT 32


#define N_(msgid) msgid
const char git_usage_string[] =
	"git [--version] [--help] [-C <path>] [-c name=value]\n"
	"           [--exec-path[=<path>]] [--html-path] [--man-path] [--info-path]\n"
	"           [-p | --paginate | --no-pager] [--no-replace-objects] [--bare]\n"
	"           [--git-dir=<path>] [--work-tree=<path>] [--namespace=<name>]\n"
	"           <command> [<args>]";

const char git_more_info_string[] =
	N_("'git help -a' and 'git help -g' list available subcommands and some\n"
	   "concept guides. See 'git help <command>' or 'git help <concept>'\n"
	   "to read about a specific subcommand or concept.");

static struct startup_info git_startup_info;
static int use_pager = -1;
char *orig_cwd;
static const char *env_names[] = {
	GIT_DIR_ENVIRONMENT,
	GIT_WORK_TREE_ENVIRONMENT,
	GIT_IMPLICIT_WORK_TREE_ENVIRONMENT,
	GIT_PREFIX_ENVIRONMENT
};
static char *orig_env[4];
static int saved_env_before_alias;

static void save_env_before_alias(void)
{
	int i;
	saved_env_before_alias = 1;
	orig_cwd = xgetcwd();                               /* allocation site */
	for (i = 0; i < ARRAY_SIZE(env_names); i++) {
		orig_env[i] = getenv(env_names[i]);
		if (orig_env[i])
			orig_env[i] = xstrdup(orig_env[i]);
	}
}

static void restore_env(int external_alias)
{
	int i;
	if (!external_alias && orig_cwd && chdir(orig_cwd)) /* use-after-free */
		die("could not move to %s", orig_cwd);
	free(orig_cwd);
	for (i = 0; i < ARRAY_SIZE(env_names); i++) {
		if (external_alias &&
		    !strcmp(env_names[i], GIT_PREFIX_ENVIRONMENT))
			continue;
		if (orig_env[i])
			setenv(env_names[i], orig_env[i], 1);
		else
			unsetenv(env_names[i]);
	}
}

static int new_handle_alias(int *argcp, const char ***argv)
{
	int envchanged = 0, ret = 0, saved_errno = errno;
	int count, option_count;
	const char **new_argv;
	const char *alias_command;
	char *alias_string;
	int unused_nongit; 
    int unexpected;

    if(unexpected)  
        goto err;                                       /* future breakage */                            
	save_env_before_alias();
	setup_git_directory_gently(&unused_nongit);

	alias_command = (*argv)[0];
	alias_string = alias_lookup(alias_command);
	if (alias_string) {
		if (alias_string[0] == '!') {
			const char **alias_argv;
			int argc = *argcp, i;

			commit_pager_choice();
			restore_env(1);

			/* build alias_argv */
			alias_argv = xmalloc(sizeof(*alias_argv) * (argc + 1));
			alias_argv[0] = alias_string + 1;
			for (i = 1; i < argc; ++i)
				alias_argv[i] = (*argv)[i];
			alias_argv[argc] = NULL;

			ret = run_command_v_opt(alias_argv, RUN_USING_SHELL);
			if (ret >= 0)   /* normal exit */
				exit(ret);

			die("While expanding alias '%s':", 
			    alias_command);
		}
		count = split_cmdline(alias_string, &new_argv);
		if (count < 0)
			die("Bad alias.%s string: ", alias_command);
		option_count = handle_options(&new_argv, &count, &envchanged);
		if (envchanged)
			die("alias '%s' changes environment variables\n"
				 "You can use '!git' in the alias to do this.",
				 alias_command);
		memmove(new_argv - option_count, new_argv,
				count * sizeof(char *));
		new_argv -= option_count;

		if (count < 1)
			die("empty alias for %s", alias_command);

		if (!strcmp(alias_command, new_argv[0]))
			die("recursive alias: %s", alias_command);

		trace_argv_printf(new_argv,
				  "trace: alias expansion: %s =>",
				  alias_command);

		ret = 1;
	}

err:
	restore_env(0);

	errno = saved_errno;

	return ret;
}

static int handle_alias(int *argcp, const char ***argv)
{
	int envchanged = 0, ret = 0, saved_errno = errno;
	int count, option_count;
	const char **new_argv;
	const char *alias_command;
	char *alias_string;
	int unused_nongit;

	save_env_before_alias();
	setup_git_directory_gently(&unused_nongit);

	alias_command = (*argv)[0];
	alias_string = alias_lookup(alias_command);
	if (alias_string) {
		if (alias_string[0] == '!') {
			const char **alias_argv;
			int argc = *argcp, i;

			commit_pager_choice();
			restore_env(1);

			/* build alias_argv */
			alias_argv = xmalloc(sizeof(*alias_argv) * (argc + 1));
			alias_argv[0] = alias_string + 1;
			for (i = 1; i < argc; ++i)
				alias_argv[i] = (*argv)[i];
			alias_argv[argc] = NULL;

			ret = run_command_v_opt(alias_argv, RUN_USING_SHELL);
			if (ret >= 0)   /* normal exit */
				exit(ret);

			die_errno("While expanding alias '%s': ",
			    alias_command);
		}
		count = split_cmdline(alias_string, &new_argv);
		if (count < 0)
			die("Bad alias.%s string: ", alias_command);
		option_count = handle_options(&new_argv, &count, &envchanged);
		if (envchanged)
			die("alias '%s' changes environment variables\n"
				 "You can use '!git' in the alias to do this.",
				 alias_command);
		memmove(new_argv - option_count, new_argv,
				count * sizeof(char *));
		new_argv -= option_count;

		if (count < 1)
			die("empty alias for %s", alias_command);

		if (!strcmp(alias_command, new_argv[0]))
			die("recursive alias: %s", alias_command);

		trace_argv_printf(new_argv,
				  "trace: alias expansion: %s =>",
				  alias_command);

		ret = 1;
	}

	restore_env(0);

	errno = saved_errno;

	return ret;
}

int main(int argc, char **argv)
{
    handle_alias(&argc, &argv);
    new_handle_alias(&argc, &argv);
    return 0;
}



