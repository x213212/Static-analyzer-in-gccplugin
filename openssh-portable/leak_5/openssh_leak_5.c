/*
    commit id: 64a89ec07660abba4d0da7c0095b7371c98bab62
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "../stdio.h"
#include "../openssh.h"

/* Commands for interactive mode */
enum sftp_command {
	I_CHDIR = 1,
	I_CHGRP,
	I_CHMOD,
	I_CHOWN,
	I_DF,
	I_GET,
	I_HELP,
	I_LCHDIR,
	I_LINK,
	I_LLS,
	I_LMKDIR,
	I_LPWD,
	I_LS,
	I_LUMASK,
	I_MKDIR,
	I_PUT,
	I_PWD,
	I_QUIT,
	I_REGET,
	I_RENAME,
	I_REPUT,
	I_RM,
	I_RMDIR,
	I_SHELL,
	I_SYMLINK,
	I_VERSION,
	I_PROGRESS,
};

/* Minimum amount of data to read at a time */
#define MIN_READ_SIZE	512

/* Maximum depth to descend in directory trees */
#define MAX_DIR_DEPTH 64

/* Maximum packet that we are willing to send/accept */
#define SFTP_MAX_MSG_LENGTH	(256 * 1024)

/* version */
#define	SSH2_FILEXFER_VERSION		3

/* client to server */
#define SSH2_FXP_INIT			1
#define SSH2_FXP_OPEN			3
#define SSH2_FXP_CLOSE			4
#define SSH2_FXP_READ			5
#define SSH2_FXP_WRITE			6
#define SSH2_FXP_LSTAT			7
#define SSH2_FXP_STAT_VERSION_0		7
#define SSH2_FXP_FSTAT			8
#define SSH2_FXP_SETSTAT		9
#define SSH2_FXP_FSETSTAT		10
#define SSH2_FXP_OPENDIR		11
#define SSH2_FXP_READDIR		12
#define SSH2_FXP_REMOVE			13
#define SSH2_FXP_MKDIR			14
#define SSH2_FXP_RMDIR			15
#define SSH2_FXP_REALPATH		16
#define SSH2_FXP_STAT			17
#define SSH2_FXP_RENAME			18
#define SSH2_FXP_READLINK		19
#define SSH2_FXP_SYMLINK		20

/* server to client */
#define SSH2_FXP_VERSION		2
#define SSH2_FXP_STATUS			101
#define SSH2_FXP_HANDLE			102
#define SSH2_FXP_DATA			103
#define SSH2_FXP_NAME			104
#define SSH2_FXP_ATTRS			105

#define SSH2_FXP_EXTENDED		200
#define SSH2_FXP_EXTENDED_REPLY		201

/* attributes */
#define SSH2_FILEXFER_ATTR_SIZE		0x00000001
#define SSH2_FILEXFER_ATTR_UIDGID	0x00000002
#define SSH2_FILEXFER_ATTR_PERMISSIONS	0x00000004
#define SSH2_FILEXFER_ATTR_ACMODTIME	0x00000008
#define SSH2_FILEXFER_ATTR_EXTENDED	0x80000000

/* portable open modes */
#define SSH2_FXF_READ			0x00000001
#define SSH2_FXF_WRITE			0x00000002
#define SSH2_FXF_APPEND			0x00000004
#define SSH2_FXF_CREAT			0x00000008
#define SSH2_FXF_TRUNC			0x00000010
#define SSH2_FXF_EXCL			0x00000020

/* statvfs@openssh.com f_flag flags */
#define SSH2_FXE_STATVFS_ST_RDONLY	0x00000001
#define SSH2_FXE_STATVFS_ST_NOSUID	0x00000002

/* status messages */
#define SSH2_FX_OK			0
#define SSH2_FX_EOF			1
#define SSH2_FX_NO_SUCH_FILE		2
#define SSH2_FX_PERMISSION_DENIED	3
#define SSH2_FX_FAILURE			4
#define SSH2_FX_BAD_MESSAGE		5
#define SSH2_FX_NO_CONNECTION		6
#define SSH2_FX_CONNECTION_LOST		7
#define SSH2_FX_OP_UNSUPPORTED		8
#define SSH2_FX_MAX			8

# define STDIN_FILENO    0

struct bwlimit {
	size_t buflen;
	u_int64_t rate, thresh, lamt;
	struct timeval bwstart, bwend;
};

struct sftp_conn {
	int fd_in;
	int fd_out;
	u_int transfer_buflen;
	u_int num_requests;
	u_int version;
	u_int msg_id;
#define SFTP_EXT_POSIX_RENAME	0x00000001
#define SFTP_EXT_STATVFS	0x00000002
#define SFTP_EXT_FSTATVFS	0x00000004
#define SFTP_EXT_HARDLINK	0x00000008
#define SFTP_EXT_FSYNC		0x00000010
	u_int exts;
	u_int64_t limit_kbps;
	struct bwlimit bwlimit_in, bwlimit_out;
};

struct complete_ctx {
    struct sftp_conn *conn;
    char **remote_pathp;
};


struct iovec 
{
     void  *iov_base;    /* Starting address */
     size_t iov_len;     /* Number of bytes to transfer */
};

typedef struct Attrib Attrib;

/* File attributes */
struct Attrib {
	u_int32_t	flags;
	u_int64_t	size;
	u_int32_t	uid;
	u_int32_t	gid;
	u_int32_t	perm;
	u_int32_t	atime;
	u_int32_t	mtime;
};


typedef void EditLine;
typedef void History;
typedef int HistEvent;


static int sftpio(void *, size_t );
int writev(int, struct iovec *, int);
ssize_t *read(int, void *, size_t);

int batchmode = 0;
FILE *infile;
/* Suppress diagnositic messages */
int quiet = 0;

struct sftp_conn *
do_init(int fd_in, int fd_out, u_int transfer_buflen, u_int num_requests,
    u_int64_t limit_kbps)
{
	u_char type;
	struct sftp_conn *ret;
	int r;

	ret = xcalloc(1, sizeof(*ret)); /* allocation site */
	ret->msg_id = 1;
	ret->fd_in = fd_in;
	ret->fd_out = fd_out;
	ret->transfer_buflen = transfer_buflen;
	ret->num_requests = num_requests;
	ret->exts = 0;
	ret->limit_kbps = 0;

	if (type != SSH2_FXP_VERSION) {
		printf("Invalid packet back from SSH2_FXP_INIT (type %u)\n",
		    type);
		// memory-leak
		return(NULL);
	}

	printf("Remote version: %u", ret->version);

	/* Some filexfer v.0 servers don't support large packets */
	if (ret->version == 0)
		ret->transfer_buflen = ret->transfer_buflen < 20480? ret->transfer_buflen : 20480;

	ret->limit_kbps = limit_kbps;

	return ret;
}

char *
do_realpath(struct sftp_conn *conn, const char *path)
{
	u_int expected_id, count, id;
	char *filename, *longname;
	Attrib a;
	u_char type;
	int r;

	expected_id = id = conn->msg_id++;

	if (type == SSH2_FXP_STATUS) {
		u_int status;
		return NULL;
	}

	return(strdup(path));
}



static int
parse_dispatch_command(struct sftp_conn *conn, const char *cmd, char **pwd,
    int err_abort)
{
	char *path1, *path2, *tmp;
	int ignore_errors = 0, aflag = 0, fflag = 0, hflag = 0, 
	iflag = 0;
	int lflag = 0, pflag = 0, rflag = 0, sflag = 0;
	int cmdnum, i;
	unsigned long n_arg = 0;
	Attrib a, *aa;
	char path_buf[PATH_MAX];
	int err = 0;

	path1 = path2 = NULL;
	if (ignore_errors != 0)
		err_abort = 0;

	/* Perform command */
	switch (cmdnum) {
	case 0:
		/* Blank line */
		break;
	case -1:
		/* Unrecognized command */
		err = -1;
		break;
	case I_REGET:
		aflag = 1;
		/* FALLTHROUGH */
	case I_CHDIR:
		if ((tmp = do_realpath(conn, path1)) == NULL) {
			err = 1;
			break;
		}
	
		if (!(aa->flags & SSH2_FILEXFER_ATTR_PERMISSIONS)) {
			error("Can't change directory: Can't check target");
			free(tmp);
			err = 1;
			break;
		}
		free(*pwd);
		*pwd = tmp;
		break;
	default:
		printf("%d is not implemented", cmdnum);
	}

	/* If an unignored error occurs in batch mode we should abort. */
	if (err_abort && err != 0)
		return (-1);
	else if (cmdnum == I_QUIT)
		return (1);

	return (0);
}


int
interactive_loop(struct sftp_conn *conn, char *file1, char *file2)
{
	char *remote_path;
	char *dir = NULL;
	char cmd[2048];
	int err, interactive;
	EditLine *el = NULL;

	remote_path = do_realpath(conn, ".");
	if (remote_path == NULL)
		fatal("Need cwd%s", "");

	if (file1 != NULL) {
		dir = xstrdup(file1);

		if (file2 == NULL) {
			if (!quiet)
				printf("Changing to: %s\n", dir);
			snprintf(cmd, sizeof cmd, "cd \"%s\"", dir);
			if (parse_dispatch_command(conn, cmd,
			    &remote_path, 1) != 0) {
				free(dir);
				free(remote_path);
				free(conn);
				return (-1);
			}
		} else {
			/* XXX this is wrong wrt quoting */
			snprintf(cmd, sizeof cmd, "get %s%s%s",
			    dir,
			    file2 == NULL ? "" : " ",
			    file2 == NULL ? "" : file2);
			err = parse_dispatch_command(conn, cmd,
			    &remote_path, 1);
			free(dir);
			free(remote_path);
			free(conn);
			return (err);
		}
		free(dir);
	}

	err = 0;
	for (;;) {
		char *cp;

		if (el == NULL) {
			if (fgets(cmd, sizeof(cmd), infile) == NULL) {
				break;
			}
		} 
		
        err = parse_dispatch_command(conn, cmd, &remote_path,
		    batchmode);
		if (err != 0)
			break;
	}
	free(remote_path);
	free(conn);

	/* err == 1 signifies normal "quit" exit */
	return (err >= 0 ? 0 : -1);
}


#define _PATH_SSH_PROGRAM		"/usr/bin/ssh"
#define DEFAULT_COPY_BUFLEN	32768	/* Size of buffer for up/download */
#define DEFAULT_NUM_REQUESTS	64	/* # concurrent outstanding requests */


int
main(int argc, char **argv)
{
	int in, out, ch, err;
	char *host = NULL, *userhost, *cp, *file2 = NULL;
	int debug_level = 0, sshver = 2;
	char *file1 = NULL, *sftp_server = NULL;
	char *ssh_program = _PATH_SSH_PROGRAM, *sftp_direct = NULL;
	const char *errstr;
	LogLevel ll = SYSLOG_LEVEL_INFO;
	extern int optind;
	extern char *optarg;
	struct sftp_conn *conn;
	size_t copy_buffer_len = DEFAULT_COPY_BUFLEN;
	size_t num_requests = DEFAULT_NUM_REQUESTS;
	long long limit_kbps = 0;

	conn = do_init(in, out, copy_buffer_len, num_requests, limit_kbps);
	if (conn == NULL) {
		printf("Couldn't initialise connection to server\n");
		return 1;
	}

	err = interactive_loop(conn, file1, file2);
    return 0;
}




