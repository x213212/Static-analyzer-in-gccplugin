/*
    ssh-add.c
    commit id : da5d9b12256275411e34824a880f8ff471acd0e2
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>

#include "../stdio.h"
#include "../openssh.h"

#ifndef ETCDIR
#define ETCDIR				"/etc"
#endif

#ifndef _PATH_SSH_PIDDIR
#define _PATH_SSH_PIDDIR		"/var/run"
#endif

/*
 * System-wide file containing host keys of known hosts.  This file should be
 * world-readable.
 */
#define _PATH_SSH_SYSTEM_HOSTFILE	ETCDIR "/ssh_known_hosts"
#define _PATH_SSH_SYSTEM_HOSTFILE2	ETCDIR "/ssh_known_hosts2"

/*
 * Of these, ssh_host_key must be readable only by root, whereas ssh_config
 * should be world-readable.
 */
#define _PATH_SERVER_CONFIG_FILE	ETCDIR "/sshd_config"
#define _PATH_HOST_CONFIG_FILE		ETCDIR "/ssh_config"
#define _PATH_HOST_KEY_FILE		ETCDIR "/ssh_host_key"
#define _PATH_HOST_DSA_KEY_FILE		ETCDIR "/ssh_host_dsa_key"
#define _PATH_HOST_RSA_KEY_FILE		ETCDIR "/ssh_host_rsa_key"
#define _PATH_DH_PRIMES			ETCDIR "/primes"

#ifndef _PATH_SSH_PROGRAM
#define _PATH_SSH_PROGRAM		"/usr/bin/ssh"
#endif

/*
 * The process id of the daemon listening for connections is saved here to
 * make it easier to kill the correct daemon when necessary.
 */
#define _PATH_SSH_DAEMON_PID_FILE	_PATH_SSH_PIDDIR "/sshd.pid"

/*
 * The directory in user\'s home directory in which the files reside. The
 * directory should be world-readable (though not all files are).
 */
#define _PATH_SSH_USER_DIR		".ssh"

/*
 * Per-user file containing host keys of known hosts.  This file need not be
 * readable by anyone except the user him/herself, though this does not
 * contain anything particularly secret.
 */
#define _PATH_SSH_USER_HOSTFILE		"~/.ssh/known_hosts"
#define _PATH_SSH_USER_HOSTFILE2	"~/.ssh/known_hosts2"

/*
 * Name of the default file containing client-side authentication key. This
 * file should only be readable by the user him/herself.
 */
#define _PATH_SSH_CLIENT_IDENTITY	".ssh/identity"
#define _PATH_SSH_CLIENT_ID_DSA		".ssh/id_dsa"
#define _PATH_SSH_CLIENT_ID_RSA		".ssh/id_rsa"

/*
 * Configuration file in user\'s home directory.  This file need not be
 * readable by anyone but the user him/herself, but does not contain anything
 * particularly secret.  If the user\'s home directory resides on an NFS
 * volume where root is mapped to nobody, this may need to be world-readable.
 */
#define _PATH_SSH_USER_CONFFILE		".ssh/config"

/*
 * File containing a list of those rsa keys that permit logging in as this
 * user.  This file need not be readable by anyone but the user him/herself,
 * but does not contain anything particularly secret.  If the user\'s home
 * directory resides on an NFS volume where root is mapped to nobody, this
 * may need to be world-readable.  (This file is read by the daemon which is
 * running as root.)
 */
#define _PATH_SSH_USER_PERMITTED_KEYS	".ssh/authorized_keys"
#define _PATH_SSH_USER_PERMITTED_KEYS2	".ssh/authorized_keys2"

/*
 * Per-user and system-wide ssh "rc" files.  These files are executed with
 * /bin/sh before starting the shell or command if they exist.  They will be
 * passed "proto cookie" as arguments if X11 forwarding with spoofing is in
 * use.  xauth will be run if neither of these exists.
 */
#define _PATH_SSH_USER_RC		".ssh/rc"
#define _PATH_SSH_SYSTEM_RC		ETCDIR "/sshrc"

/*
 * Ssh-only version of /etc/hosts.equiv.  Additionally, the daemon may use
 * ~/.rhosts and /etc/hosts.equiv if rhosts authentication is enabled.
 */
#define _PATH_SSH_HOSTS_EQUIV		ETCDIR "/shosts.equiv"
#define _PATH_RHOSTS_EQUIV		"/etc/hosts.equiv"

/*
 * Default location of askpass
 */
#ifndef _PATH_SSH_ASKPASS_DEFAULT
#define _PATH_SSH_ASKPASS_DEFAULT	"/usr/X11R6/bin/ssh-askpass"
#endif

/* for scp */
#ifndef _PATH_CP
#define _PATH_CP			"cp"
#endif

/* for sftp */
#ifndef _PATH_SFTP_SERVER
#define _PATH_SFTP_SERVER		"/usr/libexec/sftp-server"
#endif
#ifndef _PATH_LS
#define _PATH_LS			"ls"
#endif

/* path to login program */
#ifndef LOGIN_PROGRAM
# ifdef LOGIN_PROGRAM_FALLBACK
#  define LOGIN_PROGRAM         LOGIN_PROGRAM_FALLBACK
# else
#  define LOGIN_PROGRAM         "/usr/bin/login"
# endif
#endif /* LOGIN_PROGRAM */

/* Askpass program define */
#ifndef ASKPASS_PROGRAM
#define ASKPASS_PROGRAM         "/usr/lib/ssh/ssh-askpass"
#endif /* ASKPASS_PROGRAM */

/*
 * Relevant only when using builtin PRNG.
 */
#ifndef SSH_PRNG_SEED_FILE
# define SSH_PRNG_SEED_FILE      _PATH_SSH_USER_DIR"/prng_seed"
#endif /* SSH_PRNG_SEED_FILE */
#ifndef SSH_PRNG_COMMAND_FILE
# define SSH_PRNG_COMMAND_FILE   ETCDIR "/ssh_prng_cmds"
#endif /* SSH_PRNG_COMMAND_FILE */

#include "../stdio.h"
#include "../openssh.h"


#ifdef HAVE___PROGNAME
extern char *__progname;
#else
char *__progname;
#endif


enum fp_type {
	SSH_FP_SHA1,
	SSH_FP_MD5
};

/* we keep a cache of one passphrases */
static char *pass = NULL;
void
clear_pass(void)
{
	if (pass) {
		memset(pass, 0, strlen(pass));
		xfree(pass);
		pass = NULL;
	}
}

char *
ssh_askpass(char *askpass, char *msg)
{
	pid_t pid;
	size_t len;
	char *nl, *pass;
	int p[2], status;
	char buf[1024];

	if (fflush(stdout) != 0)
		error("ssh_askpass: fflush: %s", strerror(errno));
	if (askpass == NULL)
		fatal("internal error: askpass undefined%s", "");
	if (pipe(p) < 0)
		fatal("ssh_askpass: pipe: %s", strerror(errno));
	if ((pid = fork()) < 0)
		fatal("ssh_askpass: fork: %s", strerror(errno));
	if (pid == 0) {
		close(p[0]);
		if (dup2(p[1], STDOUT_FILENO) < 0)
			fatal("ssh_askpass: dup2: %s", strerror(errno));
		execlp(askpass, askpass, msg, (char *) 0);
		fatal("ssh_askpass: exec(%s)", askpass);
	}
	close(p[1]);
	len = read(p[0], buf, sizeof buf);
	close(p[0]);
	while (waitpid(pid, &status, 0) < 0)
		if (errno != EINTR)
			break;
	if (len <= 1)
		return xstrdup("");
	nl = strchr(buf, '\n');
	if (nl)
		*nl = '\0';
	pass = xstrdup(buf); /* allocation site */
	memset(buf, 0, sizeof(buf));
	return pass;
}

void
add_file(AuthenticationConnection *ac, const char *filename)
{
	struct stat st;
	Key *private;
	char *comment = NULL, *askpass = NULL;
	char buf[1024], msg[1024];
	int interactive = isatty(STDIN_FILENO);

	if (stat(filename, &st) < 0) {
		perror(filename);
		exit(1);
	}
	if (!interactive && getenv("DISPLAY")) {
		if (getenv(SSH_ASKPASS_ENV))
			askpass = getenv(SSH_ASKPASS_ENV);
		else
			askpass = _PATH_SSH_ASKPASS_DEFAULT;
	}

	/* At first, try empty passphrase */
	private = key_load_private(filename, "", &comment);
	if (comment == NULL)
		comment = xstrdup(filename);
	/* try last */
	if (private == NULL && pass != NULL)
		private = key_load_private(filename, pass, NULL);
	if (private == NULL) {
		/* clear passphrase since it did not work */
		clear_pass();
		printf("Need passphrase for %.200s\n", filename);
		if (!interactive && askpass == NULL) {
			xfree(comment);
			return;
		}
		snprintf(msg, sizeof msg, "Enter passphrase for %.200s", comment);
		for (;;) {
			if (interactive) {
				snprintf(buf, sizeof buf, "%s: ", msg);
				pass = read_passphrase(buf, 1);
			} else {
				pass = ssh_askpass(askpass, msg);
			}
			if (strcmp(pass, "") == 0) {
				xfree(pass); /* double-free */
				xfree(comment);
				return;
			}
			private = key_load_private(filename, pass, &comment);
			if (private != NULL)
				break;
			clear_pass();
			strlcpy(msg, "Bad passphrase, try again", sizeof msg);
		}
	}
	if (ssh_add_identity(ac, private, comment))
		fprintf(stderr, "Identity added: %s (%s)\n", filename, comment);
	else
		fprintf(stderr, "Could not add identity: %s\n", filename);
	xfree(comment);
	key_free(private);
}

int
main(int argc, char **argv)
{
	AuthenticationConnection *ac = NULL;
	struct passwd *pw;
	char buf[1024];
	int no_files = 1;
	int i;
	int deleting = 0;

	__progname = get_progname(argv[0]);
	init_rng();

	SSLeay_add_all_algorithms();

	/* At first, get a connection to the authentication agent. */
	ac = ssh_get_authentication_connection();
	if (ac == NULL) {
		fprintf(stderr, "Could not open a connection to your authentication agent.\n");
		exit(1);
	}
	for (i = 1; i < argc; i++) {
		if ((strcmp(argv[i], "-l") == 0) ||
		    (strcmp(argv[i], "-L") == 0)) {
			list_identities(ac, argv[i][1] == 'l' ? 1 : 0);
			/* Don't default-add/delete if -l. */
			no_files = 0;
			continue;
		}
		if (strcmp(argv[i], "-d") == 0) {
			deleting = 1;
			continue;
		}
		if (strcmp(argv[i], "-D") == 0) {
			delete_all(ac);
			no_files = 0;
			continue;
		}
		no_files = 0;
		if (deleting)
			delete_file(ac, argv[i]);
		else
			add_file(ac, argv[i]);
	}
	if (no_files) {
		pw = getpwuid(getuid());
		if (!pw) {
			fprintf(stderr, "No user found with uid %u\n",
			    (u_int)getuid());
			ssh_close_authentication_connection(ac);
			exit(1);
		}
		if (deleting)
			delete_file(ac, buf);
		else
			add_file(ac, buf);
	}
	clear_pass();
	ssh_close_authentication_connection(ac);
	return 0;
}

