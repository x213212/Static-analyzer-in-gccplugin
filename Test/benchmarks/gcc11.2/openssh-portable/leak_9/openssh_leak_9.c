/*
    commit id : 393920745fd328d3fe07f739a3cf7e1e6db45b60
    parent : 534b2cc
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../openssh.h"

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


/* Maximum packet that we are willing to send/accept */
#define SFTP_MAX_MSG_LENGTH	(256 * 1024)

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

typedef struct SFTP_DIRENT SFTP_DIRENT;

struct SFTP_DIRENT {
	char *filename;
	char *longname;
	Attrib a;
};

struct bwlimit {
	size_t buflen;
	u_int64_t rate, thresh, lamt;
	struct timeval bwstart, bwend;
};

/* Minimum amount of data to read at a time */
#define MIN_READ_SIZE	512

/* Maximum depth to descend in directory trees */
#define MAX_DIR_DEPTH 64

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

volatile sig_atomic_t interrupted = 0;

void *
xrealloc(void *ptr, size_t nmemb, size_t size)
{
	void *new_ptr;
	size_t new_size = nmemb * size;

	if (new_size == 0)
		fatal("xrealloc: zero size%s\n", "");
	if (SIZE_T_MAX / nmemb < size)
		fatal("xrealloc: nmemb * size > SIZE_T_MAX%s\n", "");
	if (ptr == NULL)
		new_ptr = malloc(new_size);
	else
		new_ptr = realloc(ptr, new_size);
	if (new_ptr == NULL)
		fatal("xrealloc: out of memory (new_size %lu bytes\n)",
		    (u_long) new_size);
	return new_ptr;
}

void free_sftp_dirents(SFTP_DIRENT **s)
{
	int i;

	for (i = 0; s[i]; i++) {
		free(s[i]->filename);
		free(s[i]->longname);
		free(s[i]);
	}
	free(s);
}

static int
do_lsreaddir(char *path, int print_flag,
    SFTP_DIRENT ***dir)
{
	Buffer msg;
	u_int count, type, id, handle_len, i, expected_id, ents = 0;
	char *handle;

	if (dir) {
		ents = 0;
		*dir = xcalloc(1, sizeof(**dir));	/* allocation site */
		(*dir)[0] = NULL;
	}

	for(; !interrupted;) {
		if(__RANDBOOL) 
			return -1;	

		count = rand() % 5;
		if(count == 0)
			break;
		for (i = 0; i < count; i++) {
			char *filename, *longname;

			filename = path;
			longname = "longname";

			if (print_flag)
				printf("%s\n", longname);

			/*
			 * Directory entries should never contain '/'
			 * These can be used to attack recursive ops
			 * (e.g. send '../../../../etc/passwd')
			 */
			if (strchr(filename, '/') != NULL) {
				printf("Server sent suspect path \"%s\" "
				    "during readdir of \"%s\"", filename, path);
				goto next;
			}

			if (dir) {
				*dir = xrealloc(*dir, ents + 2, sizeof(**dir));
				(*dir)[ents] = xcalloc(1, sizeof(***dir));
				(*dir)[ents]->filename = xstrdup(filename);
				(*dir)[ents]->longname = xstrdup(longname);
				(*dir)[++ents] = NULL;
			}
 next:
 		interrupted = __RANDBOOL;
		}
	}

	/* Don't return partial matches on interrupt */
	if (interrupted && dir != NULL && *dir != NULL) {
		free_sftp_dirents(*dir);
		*dir = xcalloc(1, sizeof(**dir));
		**dir = NULL;
	}

	return 0;
}

int
do_readdir(char *path, SFTP_DIRENT ***dir)
{
	return(do_lsreaddir(path, 0, dir));
}

int main(int argc, char **argv)
{
    char *src;
    char *dst;
    int i, ret = 0;
	SFTP_DIRENT **dir_entries;
	char *filename;
	srand(time(NULL));
	
	if (argc == 1)
		src = argv[0];
	else
		src = argv[1];

	if (do_readdir(src, &dir_entries) == -1) {
		printf("%s: Failed to get directory contents", src);
		/* memory-leak */
        return -1;
	}

	for (i = 0; dir_entries[i] != NULL; i++) {
		filename = dir_entries[i]->filename;

		if (strcmp(filename, ".") == 0 ||
			strcmp(filename, "..") == 0)
			continue;

	}

	free_sftp_dirents(dir_entries);

	return 0;
}


