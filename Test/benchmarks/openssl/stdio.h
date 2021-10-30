#define INT_MAX 10000 
#define ULONG_MAX 10000

#define	_PATH_PWD		"/etc"
#define	_PATH_PASSWD		"/etc/passwd"
#define	_PASSWD			"passwd"
#define	_PATH_MASTERPASSWD	"/etc/master.passwd"
#define	_PATH_MASTERPASSWD_LOCK	"/etc/ptmp"
#define	_MASTERPASSWD		"master.passwd"

#define	_PATH_MP_DB		"/etc/pwd.db"
#define	_MP_DB			"pwd.db"
#define	_PATH_SMP_DB		"/etc/spwd.db"
#define	_SMP_DB			"spwd.db"

#define	_PATH_PWD_MKDB		"/usr/sbin/pwd_mkdb"

#define	_PW_KEYBYNAME		'1'	/* stored by name */
#define	_PW_KEYBYNUM		'2'	/* stored by entry in the "file" */
#define	_PW_KEYBYUID		'3'	/* stored by uid */

#define	_PASSWORD_EFMT1		'_'	/* extended encryption format */

#define	_PASSWORD_LEN		128	/* max length, not counting NULL */

#define _PASSWORD_NOUID		0x01	/* flag for no specified uid. */
#define _PASSWORD_NOGID		0x02	/* flag for no specified gid. */
#define _PASSWORD_NOCHG		0x04	/* flag for no specified change. */
#define _PASSWORD_NOEXP		0x08	/* flag for no specified expire. */

#define _PASSWORD_WARNDAYS	14	/* days to warn about expiry */
#define _PASSWORD_CHGNOW	-1	/* special day to force password
					 * change at next login */

#define F_DUPFD		1
#define F_GETFD		2
#define F_GETFL		3
#define F_GETLK		4
#define F_SETFD		5
#define F_SETFL		6
#define F_SETLK		7
#define F_SETLKW	8
#define F_GETLK64	9
#define F_SETLK64	10
#define F_SETLKW64	11

#define F_UNLCK		0
#define F_RDLCK		1
#define F_WRLCK		2

#define O_RDONLY	0x0000
#define O_WRONLY	0x0001
#define O_RDWR		0x0002
#define O_ACCMODE	0x0003

#define O_BINARY	0x0004	/* must fit in char, reserved by dos */
#define O_TEXT		0x0008	/* must fit in char, reserved by dos */
#define O_NOINHERIT	0x0080	/* DOS-specific */

#define O_CREAT		0x0100	/* second byte, away from DOS bits */
#define O_EXCL		0x0200
#define O_NOCTTY	0x0400
#define O_TRUNC		0x0800
#define O_APPEND	0x1000
#define O_NONBLOCK	0x2000

typedef unsigned short u_short;
typedef unsigned int u_int;
typedef unsigned long u_long;
typedef signed char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef unsigned char u_int8_t;
typedef unsigned short int u_int16_t;
typedef unsigned int u_int32_t;
typedef unsigned long long uint64_t;


typedef unsigned char u_char;
# define ULLONG_MAX ((unsigned long long)-1)
#define SIZE_T_MAX ULONG_MAX

typedef long clock_t;
typedef int sa_family_t;
typedef int pid_t;
typedef int sig_atomic_t;
typedef __fd_mask fd_mask;


#define  STDIN_FILENO   0   /* standard input file descriptor */
#define STDOUT_FILENO   1   /* standard output file descriptor */
#define STDERR_FILENO   2   /* standard error file descriptor */

#define NI_MAXHOST 1025
#define NI_MAXSERV 32
#define PATH_MAX 4096
#define PRIuMAX "llu"
#define SCNuMAX PRIuMAX
#define PRIu32 "u"
#define PRIx32 "x"
#define PRIo32 "o"

#define PRItime PRIuMAX
#define parse_timestamp strtoumax
#define TIME_MAX UINTMAX_MAX
#define PATH_SEP ':'



extern FILE* stdin;
extern FILE* stderr;
extern FILE* stdout;




struct passwd {
	char	*pw_name;		/* user name */
	char	*pw_passwd;		/* encrypted password */
	int	    pw_uid;			/* user uid */
	int 	pw_gid;			/* user gid */
	int      pw_change;		/* password change time */
	char	*pw_class;		/* user access class */
	char	*pw_gecos;		/* Honeywell login info */
	char	*pw_dir;		/* home directory */
	char	*pw_shell;		/* default shell */
	int      pw_expire;		/* account expiration */
};


#define die_errno(a, b) die(a, b)
#define die(a, b) fatal(a, b)

#define fatalx(a, b) fatal(a, b)
#define fatal(a, b) { printf(a, b); exit(1); }

#define xmalloc(a) malloc(a)
#define xstrdup(a) strdup(a)
#define xcalloc(a, b) calloc(a, b)
#define xfree(a) free(a)

#define reallocarray(ptr, b, c) realloc(ptr, b * c)

#define __USE(exp) {printf("%d\n", exp);}
#define __NONDET(ret, X, Y) {ret = (rand() % 2 == 1 ? X : Y);}
#define __RANDBOOL ((rand() % 2 == 1) ? 1 : 0)

// #define xasprintf(a, b, c) do { *a = malloc(strlen(b) + strlen(c)); if(*a == NULL) fatal("xasprintf: %s", ""); } while(0) 


