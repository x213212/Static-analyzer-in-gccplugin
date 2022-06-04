/* $OpenBSD$ */

/*
 * Copyright (c) 2007 Nicholas Marriott <nicholas.marriott@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF MIND, USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef TMUX_H
#define TMUX_H

#include "tree.h"
#include "array.h"

/*
#define RB_ENTRY(type) struct {}
#define RB_HEAD(name, type) struct name {}
#define TAILQ_ENTRY(type) struct {}
#define TAILQ_HEAD(name, type) struct name {}
*/
#define LIST_ENTRY(type) struct {}
#define LIST_HEAD(name, type) struct name {}

#define __packed 
#define TTY_NAME_MAX 10
#define UCHAR_MAX 10

#define va_list char *
typedef char bitstr_t;

struct event {};
struct termios {};

#define PROTOCOL_VERSION 8

extern char   **environ;

struct key_binding;
struct key_bindings;
struct args;
struct client;
struct cmd_find_state;
struct cmdq_item;
struct cmdq_list;
struct environ;
struct format_job_tree;
struct input_ctx;
struct mode_tree_data;
struct mouse_event;
struct options;
struct options_entry;
struct session;
struct tmuxpeer;
struct tmuxproc;

/* Default global configuration file. */
#define TMUX_CONF "/etc/tmux.conf"

/*
 * Minimum layout cell size, NOT including separator line. The scroll region
 * cannot be one line in height so this must be at least two.
 */
#define PANE_MINIMUM 2

/* Automatic name refresh interval, in microseconds. Must be < 1 second. */
#define NAME_INTERVAL 500000

/* Maximum size of data to hold from a pane. */
#define READ_SIZE 4096

/* Attribute to make GCC check printf-like arguments. */
#define printflike(a, b) __attribute__ ((format (printf, a, b)))

/* Number of items in array. */
#ifndef nitems
#define nitems(_a) (sizeof((_a)) / sizeof((_a)[0]))
#endif

/* Alert option values. */
#define ALERT_NONE 0
#define ALERT_ANY 1
#define ALERT_CURRENT 2
#define ALERT_OTHER 3

/* Visual option values. */
#define VISUAL_OFF 0
#define VISUAL_ON 1
#define VISUAL_BOTH 2

/* Special key codes. */
#define KEYC_NONE 0xffff00000000ULL
#define KEYC_UNKNOWN 0xfffe00000000ULL
#define KEYC_BASE 0x000010000000ULL
#define KEYC_USER 0x000020000000ULL

/* Available user keys. */
#define KEYC_NUSER 1000

/* Key modifier bits. */
#define KEYC_ESCAPE 0x200000000000ULL
#define KEYC_CTRL 0x400000000000ULL
#define KEYC_SHIFT 0x800000000000ULL
#define KEYC_XTERM 0x1000000000000ULL

/* Mask to obtain key w/o modifiers. */
#define KEYC_MASK_MOD (KEYC_ESCAPE|KEYC_CTRL|KEYC_SHIFT|KEYC_XTERM)
#define KEYC_MASK_KEY (~KEYC_MASK_MOD)

/* Is this a mouse key? */
#define KEYC_IS_MOUSE(key) (((key) & KEYC_MASK_KEY) >= KEYC_MOUSE &&	\
    ((key) & KEYC_MASK_KEY) < KEYC_BSPACE)

/* Multiple click timeout. */
#define KEYC_CLICK_TIMEOUT 300

/* Mouse key codes. */
#define KEYC_MOUSE_KEY(name)				\
	KEYC_ ## name ## _PANE,				\
	KEYC_ ## name ## _STATUS,			\
	KEYC_ ## name ## _BORDER
#define KEYC_MOUSE_STRING(name, s)			\
	{ #s "Pane", KEYC_ ## name ## _PANE },		\
	{ #s "Status", KEYC_ ## name ## _STATUS },	\
	{ #s "Border", KEYC_ ## name ## _BORDER }

/*
 * A single key. This can be ASCII or Unicode or one of the keys starting at
 * KEYC_BASE.
 */
typedef unsigned long long key_code;

/* Special key codes. */
enum {
	/* Focus events. */
	KEYC_FOCUS_IN = KEYC_BASE,
	KEYC_FOCUS_OUT,

	/* Paste brackets. */
	KEYC_PASTE_START,
	KEYC_PASTE_END,

	/* Mouse keys. */
	KEYC_MOUSE, /* unclassified mouse event */
	KEYC_DRAGGING, /* dragging in progress */
	KEYC_MOUSE_KEY(MOUSEMOVE),
	KEYC_MOUSE_KEY(MOUSEDOWN1),
	KEYC_MOUSE_KEY(MOUSEDOWN2),
	KEYC_MOUSE_KEY(MOUSEDOWN3),
	KEYC_MOUSE_KEY(MOUSEUP1),
	KEYC_MOUSE_KEY(MOUSEUP2),
	KEYC_MOUSE_KEY(MOUSEUP3),
	KEYC_MOUSE_KEY(MOUSEDRAG1),
	KEYC_MOUSE_KEY(MOUSEDRAG2),
	KEYC_MOUSE_KEY(MOUSEDRAG3),
	KEYC_MOUSE_KEY(MOUSEDRAGEND1),
	KEYC_MOUSE_KEY(MOUSEDRAGEND2),
	KEYC_MOUSE_KEY(MOUSEDRAGEND3),
	KEYC_MOUSE_KEY(WHEELUP),
	KEYC_MOUSE_KEY(WHEELDOWN),
	KEYC_MOUSE_KEY(DOUBLECLICK1),
	KEYC_MOUSE_KEY(DOUBLECLICK2),
	KEYC_MOUSE_KEY(DOUBLECLICK3),
	KEYC_MOUSE_KEY(TRIPLECLICK1),
	KEYC_MOUSE_KEY(TRIPLECLICK2),
	KEYC_MOUSE_KEY(TRIPLECLICK3),

	/* Backspace key. */
	KEYC_BSPACE,

	/* Function keys. */
	KEYC_F1,
	KEYC_F2,
	KEYC_F3,
	KEYC_F4,
	KEYC_F5,
	KEYC_F6,
	KEYC_F7,
	KEYC_F8,
	KEYC_F9,
	KEYC_F10,
	KEYC_F11,
	KEYC_F12,
	KEYC_IC,
	KEYC_DC,
	KEYC_HOME,
	KEYC_END,
	KEYC_NPAGE,
	KEYC_PPAGE,
	KEYC_BTAB,

	/* Arrow keys. */
	KEYC_UP,
	KEYC_DOWN,
	KEYC_LEFT,
	KEYC_RIGHT,

	/* Numeric keypad. */
	KEYC_KP_SLASH,
	KEYC_KP_STAR,
	KEYC_KP_MINUS,
	KEYC_KP_SEVEN,
	KEYC_KP_EIGHT,
	KEYC_KP_NINE,
	KEYC_KP_PLUS,
	KEYC_KP_FOUR,
	KEYC_KP_FIVE,
	KEYC_KP_SIX,
	KEYC_KP_ONE,
	KEYC_KP_TWO,
	KEYC_KP_THREE,
	KEYC_KP_ENTER,
	KEYC_KP_ZERO,
	KEYC_KP_PERIOD,
};

#ifndef TTY
/* Termcap codes. */
enum tty_code_code {
	TTYC_AX = 0,
	TTYC_ACSC,
	TTYC_BCE,
	TTYC_BEL,
	TTYC_BLINK,
	TTYC_BOLD,
	TTYC_CIVIS,
	TTYC_CLEAR,
	TTYC_CNORM,
	TTYC_COLORS,
	TTYC_CR,
	TTYC_CS,
	TTYC_CSR,
	TTYC_CUB,
	TTYC_CUB1,
	TTYC_CUD,
	TTYC_CUD1,
	TTYC_CUF,
	TTYC_CUF1,
	TTYC_CUP,
	TTYC_CUU,
	TTYC_CUU1,
	TTYC_CVVIS,
	TTYC_DCH,
	TTYC_DCH1,
	TTYC_DIM,
	TTYC_DL,
	TTYC_DL1,
	TTYC_E3,
	TTYC_ECH,
	TTYC_ED,
	TTYC_EL,
	TTYC_EL1,
	TTYC_ENACS,
	TTYC_FSL,
	TTYC_HOME,
	TTYC_HPA,
	TTYC_ICH,
	TTYC_ICH1,
	TTYC_IL,
	TTYC_IL1,
	TTYC_INDN,
	TTYC_INVIS,
	TTYC_KCBT,
	TTYC_KCUB1,
	TTYC_KCUD1,
	TTYC_KCUF1,
	TTYC_KCUU1,
	TTYC_KDC2,
	TTYC_KDC3,
	TTYC_KDC4,
	TTYC_KDC5,
	TTYC_KDC6,
	TTYC_KDC7,
	TTYC_KDCH1,
	TTYC_KDN2,
	TTYC_KDN3,
	TTYC_KDN4,
	TTYC_KDN5,
	TTYC_KDN6,
	TTYC_KDN7,
	TTYC_KEND,
	TTYC_KEND2,
	TTYC_KEND3,
	TTYC_KEND4,
	TTYC_KEND5,
	TTYC_KEND6,
	TTYC_KEND7,
	TTYC_KF1,
	TTYC_KF10,
	TTYC_KF11,
	TTYC_KF12,
	TTYC_KF13,
	TTYC_KF14,
	TTYC_KF15,
	TTYC_KF16,
	TTYC_KF17,
	TTYC_KF18,
	TTYC_KF19,
	TTYC_KF2,
	TTYC_KF20,
	TTYC_KF21,
	TTYC_KF22,
	TTYC_KF23,
	TTYC_KF24,
	TTYC_KF25,
	TTYC_KF26,
	TTYC_KF27,
	TTYC_KF28,
	TTYC_KF29,
	TTYC_KF3,
	TTYC_KF30,
	TTYC_KF31,
	TTYC_KF32,
	TTYC_KF33,
	TTYC_KF34,
	TTYC_KF35,
	TTYC_KF36,
	TTYC_KF37,
	TTYC_KF38,
	TTYC_KF39,
	TTYC_KF4,
	TTYC_KF40,
	TTYC_KF41,
	TTYC_KF42,
	TTYC_KF43,
	TTYC_KF44,
	TTYC_KF45,
	TTYC_KF46,
	TTYC_KF47,
	TTYC_KF48,
	TTYC_KF49,
	TTYC_KF5,
	TTYC_KF50,
	TTYC_KF51,
	TTYC_KF52,
	TTYC_KF53,
	TTYC_KF54,
	TTYC_KF55,
	TTYC_KF56,
	TTYC_KF57,
	TTYC_KF58,
	TTYC_KF59,
	TTYC_KF6,
	TTYC_KF60,
	TTYC_KF61,
	TTYC_KF62,
	TTYC_KF63,
	TTYC_KF7,
	TTYC_KF8,
	TTYC_KF9,
	TTYC_KHOM2,
	TTYC_KHOM3,
	TTYC_KHOM4,
	TTYC_KHOM5,
	TTYC_KHOM6,
	TTYC_KHOM7,
	TTYC_KHOME,
	TTYC_KIC2,
	TTYC_KIC3,
	TTYC_KIC4,
	TTYC_KIC5,
	TTYC_KIC6,
	TTYC_KIC7,
	TTYC_KICH1,
	TTYC_KIND,
	TTYC_KLFT2,
	TTYC_KLFT3,
	TTYC_KLFT4,
	TTYC_KLFT5,
	TTYC_KLFT6,
	TTYC_KLFT7,
	TTYC_KMOUS,
	TTYC_KNP,
	TTYC_KNXT2,
	TTYC_KNXT3,
	TTYC_KNXT4,
	TTYC_KNXT5,
	TTYC_KNXT6,
	TTYC_KNXT7,
	TTYC_KPP,
	TTYC_KPRV2,
	TTYC_KPRV3,
	TTYC_KPRV4,
	TTYC_KPRV5,
	TTYC_KPRV6,
	TTYC_KPRV7,
	TTYC_KRI,
	TTYC_KRIT2,
	TTYC_KRIT3,
	TTYC_KRIT4,
	TTYC_KRIT5,
	TTYC_KRIT6,
	TTYC_KRIT7,
	TTYC_KUP2,
	TTYC_KUP3,
	TTYC_KUP4,
	TTYC_KUP5,
	TTYC_KUP6,
	TTYC_KUP7,
	TTYC_MS,
	TTYC_OP,
	TTYC_REV,
	TTYC_RI,
	TTYC_RMACS,
	TTYC_RMCUP,
	TTYC_RMKX,
	TTYC_SE,
	TTYC_SETAB,
	TTYC_SETAF,
	TTYC_SETRGBB,
	TTYC_SETRGBF,
	TTYC_SGR0,
	TTYC_SITM,
	TTYC_SMACS,
	TTYC_SMCUP,
	TTYC_SMKX,
	TTYC_SMSO,
	TTYC_SMUL,
	TTYC_SMXX,
	TTYC_SS,
	TTYC_TC,
	TTYC_TSL,
	TTYC_U8,
	TTYC_VPA,
	TTYC_XENL,
	TTYC_XT,
};
#endif
/* Message codes. */
enum msgtype {
	MSG_VERSION = 12,

	MSG_IDENTIFY_FLAGS = 100,
	MSG_IDENTIFY_TERM,
	MSG_IDENTIFY_TTYNAME,
	MSG_IDENTIFY_OLDCWD, /* unused */
	MSG_IDENTIFY_STDIN,
	MSG_IDENTIFY_ENVIRON,
	MSG_IDENTIFY_DONE,
	MSG_IDENTIFY_CLIENTPID,
	MSG_IDENTIFY_CWD,

	MSG_COMMAND = 200,
	MSG_DATA,
	MSG_DETACH,
	MSG_DETACHKILL,
	MSG_EXIT,
	MSG_EXITED,
	MSG_EXITING,
	MSG_LOCK,
	MSG_READY,
	MSG_RESIZE,
	MSG_SHELL,
	MSG_SHUTDOWN,
	MSG_STDERR,
	MSG_STDIN,
	MSG_STDOUT,
	MSG_SUSPEND,
	MSG_UNLOCK,
	MSG_WAKEUP,
	MSG_EXEC,
};

struct options {
	RB_HEAD(options_tree, options_entry) tree;
	struct options	*parent;
};

/* Paste buffer. */
struct paste_buffer {
	char		*data;
	size_t		 size;
	
	char 		*name;
	int 		automatic;
	u_int 		order;
	
	RB_ENTRY(paste_buffer) name_entry;
	RB_ENTRY(paste_buffer) time_entry;
};
ARRAY_DECL(paste_stack, struct paste_buffer *);

/*
 * Message data.
 *
 * Don't forget to bump PROTOCOL_VERSION if any of these change!
 */
struct msg_command_data {
	int	argc;
}; /* followed by packed argv */

struct msg_stdin_data {
	ssize_t	size;
	char	data[BUFSIZ];
};

struct msg_stdout_data {
	ssize_t	size;
	char	data[BUFSIZ];
};

struct msg_stderr_data {
	ssize_t	size;
	char	data[BUFSIZ];
};

/* Mode keys. */
#define MODEKEY_EMACS 0
#define MODEKEY_VI 1

/* Modes. */
#define MODE_CURSOR 0x1
#define MODE_INSERT 0x2
#define MODE_KCURSOR 0x4
#define MODE_KKEYPAD 0x8	/* set = application, clear = number */
#define MODE_WRAP 0x10		/* whether lines wrap */
#define MODE_MOUSE_STANDARD 0x20
#define MODE_MOUSE_BUTTON 0x40
#define MODE_BLINKING 0x80
#define MODE_MOUSE_UTF8 0x100
#define MODE_MOUSE_SGR 0x200
#define MODE_BRACKETPASTE 0x400
#define MODE_FOCUSON 0x800
#define MODE_MOUSE_ALL 0x1000

#define ALL_MODES 0xffffff
#define ALL_MOUSE_MODES (MODE_MOUSE_STANDARD|MODE_MOUSE_BUTTON|MODE_MOUSE_ALL)

/*
 * A single UTF-8 character. UTF8_SIZE must be big enough to hold at least one
 * combining character as well.
*/
#define UTF8_SIZE 9
struct utf8_data {
	u_char	data[UTF8_SIZE];

	u_char	have;
	u_char	size;

	u_char	width;	/* 0xff if invalid */
} __packed;
enum utf8_state {
	UTF8_MORE,
	UTF8_DONE,
	UTF8_ERROR
};

/* Colour flags. */
#define COLOUR_FLAG_256 0x01000000
#define COLOUR_FLAG_RGB 0x02000000

/* Grid attributes. Anything above 0xff is stored in an extended cell. */
#define GRID_ATTR_BRIGHT 0x1
#define GRID_ATTR_DIM 0x2
#define GRID_ATTR_UNDERSCORE 0x4
#define GRID_ATTR_BLINK 0x8
#define GRID_ATTR_REVERSE 0x10
#define GRID_ATTR_HIDDEN 0x20
#define GRID_ATTR_ITALICS 0x40
#define GRID_ATTR_CHARSET 0x80	/* alternative character set */
#define GRID_ATTR_STRIKETHROUGH 0x100

/* Grid flags. */
#define GRID_FLAG_FG256 0x1
#define GRID_FLAG_BG256 0x2
#define GRID_FLAG_PADDING 0x4
#define GRID_FLAG_EXTENDED 0x8
#define GRID_FLAG_SELECTED 0x10
#define GRID_FLAG_NOPALETTE 0x20

/* Grid line flags. */
#define GRID_LINE_WRAPPED 0x1
#define GRID_LINE_EXTENDED 0x2

/* Grid cell data. */
struct grid_cell {
	u_char			flags;
	u_short			attr;
	int			fg;
	int			bg;
	struct utf8_data	data;
};
struct grid_cell_entry {
	u_char			flags;
	union {
		u_int		offset;
		struct {
			u_char	attr;
			u_char	fg;
			u_char	bg;
			u_char	data;
		} data;
	};
} __packed;

/* Grid line. */
struct grid_line {
	u_int			 cellused;
	u_int			 cellsize;
	struct grid_cell_entry	*celldata;

	u_int			 extdsize;
	struct grid_cell	*extddata;

	int			 flags;
} __packed;

/* Entire grid of cells. */
struct grid {
	int			 flags;
#define GRID_HISTORY 0x1 /* scroll lines into history */

	u_int			 sx;
	u_int			 sy;

	u_int			 hscrolled;
	u_int			 hsize;
	u_int			 hlimit;

	struct grid_line	*linedata;
};

/* Hook data structures. */
struct hook {
	const char	*name;

	struct cmd_list	*cmdlist;

	RB_ENTRY(hook)	 entry;
};

/* Scheduled job. */
struct job;
typedef void (*job_update_cb) (struct job *);
typedef void (*job_complete_cb) (struct job *);
typedef void (*job_free_cb) (void *);
struct job {
	enum {
		JOB_RUNNING,
		JOB_DEAD,
		JOB_CLOSED
	} state;

	char			*cmd;
	pid_t			 pid;
	int			 status;

	int			 fd;
	struct bufferevent	*event;

	job_update_cb		 updatecb;
	job_complete_cb		 completecb;
	job_free_cb		 freecb;
	void 			(*callbackfn) (struct job *);
	void			*data;

	LIST_ENTRY(job)		 entry;
};
LIST_HEAD(joblist, job);

/* Screen selection. */
struct screen_sel {
	int		 flag;
	int		 hidden;

	int		 rectflag;
	enum {
		LINE_SEL_NONE,
		LINE_SEL_LEFT_RIGHT,
		LINE_SEL_RIGHT_LEFT,
	} lineflag;

	int		 modekeys;

	u_int		 sx;
	u_int		 sy;

	u_int		 ex;
	u_int		 ey;
	struct grid_cell cell;
};

/* Virtual screen. */
struct screen_titles;
struct screen {
	char			*title;
	struct screen_titles	*titles;

	struct grid		*grid;		/* grid data */

	u_int			 cx;		/* cursor x */
	u_int			 cy;		/* cursor y */

	u_int			 cstyle;	/* cursor style */
	char			*ccolour;	/* cursor colour string */

	u_int			 rupper;	/* scroll region top */
	u_int			 rlower;	/* scroll region bottom */
	
	u_int			 hsize;

	int			 mode;

	bitstr_t		*tabs;

	struct screen_sel	 sel;
};

/* Screen write context. */
struct screen_write_collect_item;
struct screen_write_collect_line;
struct screen_write_ctx {
	struct window_pane	*wp;
	struct screen		*s;

	struct screen_write_collect_item *item;
	struct screen_write_collect_line *list;
	u_int			 scrolled;
	u_int			 bg;

	u_int			 cells;
	u_int			 written;
	u_int			 skipped;
};

/* Screen size. */
#define screen_size_x(s) ((s)->grid->sx)
#define screen_size_y(s) ((s)->grid->sy)
#define screen_hsize(s) ((s)->grid->hsize)
#define screen_hlimit(s) ((s)->grid->hlimit)

/*
 * Window mode. Windows can be in several modes and this is used to call the
 * right function to handle input and output.
 */
#ifndef WINDOW_MODE
struct window_mode {
	const char	*name;

	struct screen	*(*init)(struct window_pane *, struct cmd_find_state *,
			     struct args *);
	void		 (*free)(struct window_pane *);
	void		 (*resize)(struct window_pane *, u_int, u_int);
	void		 (*key)(struct window_pane *, struct client *,
			     struct session *, key_code, struct mouse_event *);

	const char	*(*key_table)(struct window_pane *);
	void		 (*command)(struct window_pane *, struct client *,
			     struct session *, struct args *,
			     struct mouse_event *);
};
#endif

#define WINDOW_MODE_TIMEOUT 180

/* Structures for choose mode. */
struct window_choose_data {
	struct client		*start_client;
	struct session		*start_session;
	struct session		*session;
	u_int			 idx;
	int			 type;
#define TREE_OTHER 0x0
#define TREE_WINDOW 0x1
#define TREE_SESSION 0x2

	struct session		*tree_session; /* session of items in tree */

	struct winlink		*wl;
	int			 pane_id;

	char			*ft_template;
	struct format_tree	*ft;

	char			*command;
};

/* Child window structure. */
struct window_pane {
	u_int		 id;
	u_int		 active_point;

	struct window	*window;

	struct layout_cell *layout_cell;
	struct layout_cell *saved_layout_cell;

	u_int		 sx;
	u_int		 sy;

	u_int		 osx;
	u_int		 osy;

	u_int		 xoff;
	u_int		 yoff;

	int		 flags;
#define PANE_REDRAW 0x1
#define PANE_DROP 0x2
#define PANE_FOCUSED 0x4
#define PANE_RESIZE 0x8
#define PANE_RESIZEFORCE 0x10
#define PANE_FOCUSPUSH 0x20
#define PANE_INPUTOFF 0x40
#define PANE_CHANGED 0x80
#define PANE_EXITED 0x100
#define PANE_STATUSREADY 0x200
#define PANE_STATUSDRAWN 0x400

	int		 argc;
	char	       **argv;
	char		*shell;
	const char	*cwd;

	pid_t		 pid;
	char		 tty[TTY_NAME_MAX];
	int		 status;

	int		 fd;
	struct bufferevent *event;

	struct event	 resize_timer;

	struct input_ctx *ictx;

	struct grid_cell colgc;

	int		*palette;

	int		 pipe_fd;
	struct bufferevent *pipe_event;
	size_t		 pipe_off;

	struct screen	*screen;
	struct screen	 base;

	struct screen	 status_screen;
	size_t		 status_size;

	/* Saved in alternative screen mode. */
	u_int		 saved_cx;
	u_int		 saved_cy;
	struct grid	*saved_grid;
	struct grid_cell saved_cell;

	const struct window_mode *mode;
	void		*modedata;
	struct event	 modetimer;
	time_t		 modelast;
	u_int		 modeprefix;
	char		*searchstr;

	TAILQ_ENTRY(window_pane) entry;
	RB_ENTRY(window_pane) tree_entry;
};
TAILQ_HEAD(window_panes, window_pane);
RB_HEAD(window_pane_tree, window_pane);

#ifndef WINDOW
/* Window structure. */
struct window {
	u_int		 id;

	char		*name;
	struct event	 name_event;
	struct timeval	 name_time;

	struct event	 alerts_timer;

	struct timeval	 activity_time;

	struct window_pane *active;
	struct window_pane *last;
	struct window_panes panes;

	int		 lastlayout;
	struct layout_cell *layout_root;
	struct layout_cell *saved_layout_root;
	char		*old_layout;

	u_int		 sx;
	u_int		 sy;

	int		 flags;
#define WINDOW_BELL 0x1
#define WINDOW_ACTIVITY 0x2
/* 0x4 unused */
#define WINDOW_SILENCE 0x8
#define WINDOW_ZOOMED 0x1000
#define WINDOW_FORCEWIDTH 0x2000
#define WINDOW_FORCEHEIGHT 0x4000
#define WINDOW_STYLECHANGED 0x8000
#define WINDOW_ALERTFLAGS (WINDOW_BELL|WINDOW_ACTIVITY|WINDOW_SILENCE)

	int		 alerts_queued;
	TAILQ_ENTRY(window) alerts_entry;

	struct options	*options;

	struct grid_cell style;
	struct grid_cell active_style;

	u_int		 references;
	TAILQ_HEAD(, winlink) winlinks;

	RB_ENTRY(window) entry;
};
RB_HEAD(windows, window);
#endif


/* Entry on local window list. */
struct winlink {
	int		 idx;
	struct session	*session;
	struct window	*window;

	size_t		 status_width;
	struct grid_cell status_cell;
	char		*status_text;

	int		 flags;
#define WINLINK_BELL 0x1
#define WINLINK_ACTIVITY 0x2
#define WINLINK_SILENCE 0x4
#define WINLINK_ALERTFLAGS (WINLINK_BELL|WINLINK_ACTIVITY|WINLINK_SILENCE)

	RB_ENTRY(winlink) entry;
	TAILQ_ENTRY(winlink) wentry;
	TAILQ_ENTRY(winlink) sentry;
};
RB_HEAD(winlinks, winlink);
TAILQ_HEAD(winlink_stack, winlink);

/* Layout direction. */
enum layout_type {
	LAYOUT_LEFTRIGHT,
	LAYOUT_TOPBOTTOM,
	LAYOUT_WINDOWPANE
};

/* Layout cells queue. */
TAILQ_HEAD(layout_cells, layout_cell);

/* Layout cell. */
struct layout_cell {
	enum layout_type type;

	struct layout_cell *parent;

	u_int		 sx;
	u_int		 sy;

	u_int		 xoff;
	u_int		 yoff;

	struct window_pane *wp;
	struct layout_cells cells;

	TAILQ_ENTRY(layout_cell) entry;
};

/* Environment variable. */
struct environ_entry {
	char		*name;
	char		*value;

	RB_ENTRY(environ_entry) entry;
};

#ifndef SESSION_GROUP
/* Client session. */
struct session_group {
	const char		*name;
	TAILQ_HEAD(, session)	 sessions;

	RB_ENTRY(session_group)	 entry;
};
RB_HEAD(session_groups, session_group);
#endif

#ifndef SESSION
struct session {
	u_int		 id;

	char		*name;
	const char	*cwd;

	struct timeval	 creation_time;
	struct timeval	 last_attached_time;
	struct timeval	 activity_time;
	struct timeval	 last_activity_time;

	struct event	 lock_timer;

	u_int		 sx;
	u_int		 sy;

	struct winlink	*curw;
	struct winlink_stack lastw;
	struct winlinks	 windows;

	int		 statusat;

	struct hooks	*hooks;
	struct options	*options;

#define SESSION_UNATTACHED 0x1	/* not attached to any clients */
#define SESSION_PASTING 0x2
#define SESSION_ALERTED 0x4
	int		 flags;

	u_int		 attached;

	struct termios	*tio;

	struct environ	*environ;

	int		 references;

	TAILQ_ENTRY(session) gentry;
	RB_ENTRY(session)    entry;
};
RB_HEAD(sessions, session);
#endif

/* Mouse button masks. */
#define MOUSE_MASK_BUTTONS 3
#define MOUSE_MASK_SHIFT 4
#define MOUSE_MASK_META 8
#define MOUSE_MASK_CTRL 16
#define MOUSE_MASK_DRAG 32
#define MOUSE_MASK_WHEEL 64

/* Mouse wheel states. */
#define MOUSE_WHEEL_UP 0
#define MOUSE_WHEEL_DOWN 64

/* Mouse helpers. */
#define MOUSE_BUTTONS(b) ((b) & MOUSE_MASK_BUTTONS)
#define MOUSE_WHEEL(b) ((b) & MOUSE_MASK_WHEEL)
#define MOUSE_DRAG(b) ((b) & MOUSE_MASK_DRAG)
#define MOUSE_RELEASE(b) (((b) & MOUSE_MASK_BUTTONS) == 3)

/* Mouse input. */
struct mouse_event {
	int		valid;

	key_code	key;
	int		statusat;

	u_int		x;
	u_int		y;
	u_int		b;

	u_int		lx;
	u_int		ly;
	u_int		lb;

	int		s;
	int		w;
	int		wp;

	u_int		sgr_type;
	u_int		sgr_b;
};

/* TTY information. */
struct tty_key {
	char		 ch;
	key_code	 key;

	struct tty_key	*left;
	struct tty_key	*right;

	struct tty_key	*next;
};

struct tty_code;
#ifndef TTY
struct tty_term {
	char		*name;
	u_int		 references;

	char		 acs[UCHAR_MAX + 1][2];

	struct tty_code	*codes;

#define TERM_256COLOURS 0x1
#define TERM_EARLYWRAP 0x2
	int		 flags;

	LIST_ENTRY(tty_term) entry;
};
LIST_HEAD(tty_terms, tty_term);

struct tty {
	struct client	*client;

	u_int		 sx;
	u_int		 sy;

	u_int		 cx;
	u_int		 cy;
	u_int		 cstyle;
	char		*ccolour;

	int		 mode;

	u_int		 rlower;
	u_int		 rupper;

	u_int		 rleft;
	u_int		 rright;

	int		 fd;
	struct event	 event_in;
	struct evbuffer	*in;
	struct event	 event_out;
	struct evbuffer	*out;
	struct event	 timer;
	size_t		 discarded;

	struct termios	 tio;

	struct grid_cell cell;

	int		 last_wp;
	struct grid_cell last_cell;

#define TTY_NOCURSOR 0x1
#define TTY_FREEZE 0x2
#define TTY_TIMER 0x4
#define TTY_UTF8 0x8
#define TTY_STARTED 0x10
#define TTY_OPENED 0x20
#define TTY_FOCUS 0x40
#define TTY_BLOCK 0x80
	int		 flags;

	struct tty_term	*term;
	char		*term_name;
	int		 term_flags;
	enum {
		TTY_VT100,
		TTY_VT101,
		TTY_VT102,
		TTY_VT220,
		TTY_VT320,
		TTY_VT420,
		TTY_UNKNOWN
	} term_type;

	struct mouse_event mouse;
	int		 mouse_drag_flag;
	void		(*mouse_drag_update)(struct client *,
			    struct mouse_event *);
	void		(*mouse_drag_release)(struct client *,
			    struct mouse_event *);

	struct event	 key_timer;
	struct tty_key	*key_tree;
};
#endif
#define TTY_TYPES \
	{ "VT100", "VT101", "VT102", "VT220", "VT320", "VT420", "Unknown" }

/* TTY command context. */
struct tty_ctx {
	struct window_pane	*wp;

	const struct grid_cell	*cell;
	int			 wrapped;

	u_int		 num;
	void		*ptr;

	/*
	 * Cursor and region position before the screen was updated - this is
	 * where the command should be applied; the values in the screen have
	 * already been updated.
	 */
	u_int		 ocx;
	u_int		 ocy;

	u_int		 orupper;
	u_int		 orlower;

	u_int		 xoff;
	u_int		 yoff;

	/* The background colour used for clearing (erasing). */
	u_int		 bg;
};

/* Saved message entry. */
struct message_entry {
	char	*msg;
	u_int	 msg_num;
	time_t	 msg_time;
	TAILQ_ENTRY(message_entry) entry;
};


/* Parsed arguments structures. */
struct args_entry {
    u_char           flag;
    char            *value;
    RB_ENTRY(args_entry)     entry;
};

RB_HEAD(args_tree, args_entry);
struct args {
	struct args_tree	  tree;
	int			  argc;
	char			**argv;
};

/* Command find structures. */
enum cmd_find_type {
	CMD_FIND_PANE,
	CMD_FIND_WINDOW,
	CMD_FIND_SESSION,
};
struct cmd_find_state {
	int			 flags;
	struct cmd_find_state	*current;

	struct session		*s;
	struct winlink		*wl;
	struct window		*w;
	struct window_pane	*wp;
	int			 idx;
};

/* Command find flags. */
#define CMD_FIND_PREFER_UNATTACHED 0x1
#define CMD_FIND_QUIET 0x2
#define CMD_FIND_WINDOW_INDEX 0x4
#define CMD_FIND_DEFAULT_MARKED 0x8
#define CMD_FIND_EXACT_SESSION 0x10
#define CMD_FIND_EXACT_WINDOW 0x20
#define CMD_FIND_CANFAIL 0x40

#ifndef CMD
/* Command and list of commands. */
struct cmd {
	const struct cmd_entry	*entry;
	struct args		*args;

	char			*file;
	u_int			 line;

#define CMD_CONTROL 0x1
	int			 flags;
	void 			*data;
	TAILQ_ENTRY(cmd)	 qentry;
};

struct cmd_list {
	int			 references;
	TAILQ_HEAD(, cmd)	 list;
};
#endif

/* Command return values. */
enum cmd_retval {
	CMD_RETURN_ERROR = -1,
	CMD_RETURN_NORMAL = 0,
	CMD_RETURN_WAIT,
	CMD_RETURN_STOP
};

/* Command queue item type. */
enum cmdq_type {
	CMDQ_COMMAND,
	CMDQ_CALLBACK,
};

/* Command queue item shared state. */
struct cmdq_shared {
	int			 references;

	int			 flags;
#define CMDQ_SHARED_REPEAT 0x1

	struct format_tree	*formats;

	struct mouse_event	 mouse;
	struct cmd_find_state	 current;
};

/* Command queue item. */
typedef enum cmd_retval (*cmdq_cb) (struct cmdq_item *, void *);

struct cmdq_item; 
TAILQ_HEAD(cmdq_list, cmdq_item);

/* Command definition flag. */
struct cmd_entry_flag {
	char			 flag;
	enum cmd_find_type	 type;
	int			 flags;
};

/* Command definition. */
struct cmd_entry;

#ifndef CLIENT
/* Client connection. */
typedef int (*prompt_input_cb)(struct client *, void *, const char *, int);
typedef void (*prompt_free_cb)(void *);
struct client {
	const char	*name;
	struct tmuxpeer	*peer;
	struct cmdq_list queue;

	pid_t		 pid;
	int		 fd;
	struct event	 event;
	int		 retval;

	struct timeval	 creation_time;
	struct timeval	 activity_time;

	struct environ	*environ;
	struct format_job_tree	*jobs;

	char		*title;
	const char	*cwd;

	char		*term;
	char		*ttyname;
	struct tty	 tty;

	size_t		 written;
	size_t		 discarded;
	size_t		 redraw;

	void		(*stdin_callback)(struct client *, int, void *);
	void		*stdin_callback_data;
	struct evbuffer	*stdin_data;
	int		 stdin_closed;
	struct evbuffer	*stdout_data;
	struct evbuffer	*stderr_data;

	struct event	 repeat_timer;

	struct event	 click_timer;
	u_int		 click_button;

	struct event	 status_timer;
	struct screen	 status;

	struct screen	*old_status;

#define CLIENT_TERMINAL 0x1
#define CLIENT_LOGIN 0x2
#define CLIENT_EXIT 0x4
#define CLIENT_REDRAW 0x8
#define CLIENT_STATUS 0x10
#define CLIENT_REPEAT 0x20
#define CLIENT_SUSPENDED 0x40
#define CLIENT_ATTACHED 0x80
#define CLIENT_IDENTIFY 0x100
#define CLIENT_DEAD 0x200
#define CLIENT_BORDERS 0x400
#define CLIENT_READONLY 0x800
#define CLIENT_DETACHING 0x1000
#define CLIENT_CONTROL 0x2000
#define CLIENT_CONTROLCONTROL 0x4000
#define CLIENT_FOCUSED 0x8000
#define CLIENT_UTF8 0x10000
#define CLIENT_256COLOURS 0x20000
#define CLIENT_IDENTIFIED 0x40000
#define CLIENT_STATUSFORCE 0x80000
#define CLIENT_DOUBLECLICK 0x100000
#define CLIENT_TRIPLECLICK 0x200000
#define CLIENT_SIZECHANGED 0x400000
#define CLIENT_STATUSOFF 0x800000
	int		 flags;
	struct key_table *keytable;

	struct event	 identify_timer;
	void		(*identify_callback)(struct client *,
			     struct window_pane *);
	void		*identify_callback_data;

	char		*message_string;
	struct event	 message_timer;
	u_int		 message_next;
	TAILQ_HEAD(, message_entry) message_log;

	char		*prompt_string;
	struct utf8_data *prompt_buffer;
	size_t		 prompt_index;
	prompt_input_cb	 prompt_inputcb;
	prompt_free_cb	 prompt_freecb;
	void		*prompt_data;
	u_int		 prompt_hindex;
	enum { PROMPT_ENTRY, PROMPT_COMMAND } prompt_mode;

#define PROMPT_SINGLE 0x1
#define PROMPT_NUMERIC 0x2
#define PROMPT_INCREMENTAL 0x4
#define PROMPT_NOFORMAT 0x8
	int		 prompt_flags;

	struct session	*session;
	struct session	*last_session;

	int		 wlmouse;

	int		 references;

	struct cmd_q *cmdq;

	TAILQ_ENTRY(client) entry;
};
TAILQ_HEAD(clients, client);
#endif

#ifndef KEY_BINDINGS
/* Key binding and key table. */
struct key_binding {
	key_code		 key;
	struct cmd_list		*cmdlist;

	int			 flags;
#define KEY_BINDING_REPEAT 0x1

	RB_ENTRY(key_binding)	 entry;
};
RB_HEAD(key_bindings, key_binding);

struct key_table {
	const char		 *name;
	struct key_bindings	 key_bindings;

	u_int			 references;

	RB_ENTRY(key_table)	 entry;
};
RB_HEAD(key_tables, key_table);
#endif

/* Option table entries. */
enum options_table_type {
	OPTIONS_TABLE_STRING,
	OPTIONS_TABLE_NUMBER,
	OPTIONS_TABLE_KEY,
	OPTIONS_TABLE_COLOUR,
	OPTIONS_TABLE_ATTRIBUTES,
	OPTIONS_TABLE_FLAG,
	OPTIONS_TABLE_CHOICE,
	OPTIONS_TABLE_STYLE,
	OPTIONS_TABLE_ARRAY,
};
enum options_table_scope {
	OPTIONS_TABLE_NONE,
	OPTIONS_TABLE_SERVER,
	OPTIONS_TABLE_SESSION,
	OPTIONS_TABLE_WINDOW,
};

struct options_table_entry {
	const char		 *name;
	enum options_table_type	  type;
	enum options_table_scope  scope;

	u_int			  minimum;
	u_int			  maximum;
	const char		**choices;

	const char		 *default_str;
	long long		  default_num;

	const char		 *separator;
	const char		 *style;
};

/* Common command usages. */
#define CMD_TARGET_PANE_USAGE "[-t target-pane]"
#define CMD_TARGET_WINDOW_USAGE "[-t target-window]"
#define CMD_TARGET_SESSION_USAGE "[-t target-session]"
#define CMD_TARGET_CLIENT_USAGE "[-t target-client]"
#define CMD_SRCDST_PANE_USAGE "[-s src-pane] [-t dst-pane]"
#define CMD_SRCDST_WINDOW_USAGE "[-s src-window] [-t dst-window]"
#define CMD_SRCDST_SESSION_USAGE "[-s src-session] [-t dst-session]"
#define CMD_SRCDST_CLIENT_USAGE "[-s src-client] [-t dst-client]"
#define CMD_BUFFER_USAGE "[-b buffer-name]"

/* proc.c */
struct imsg;

/* paste.c */

struct paste_buffer;
/* format.c */
#define FORMAT_STATUS 0x1
#define FORMAT_FORCE 0x2
#define FORMAT_NOJOBS 0x4
#define FORMAT_NONE 0
#define FORMAT_PANE 0x80000000U
#define FORMAT_WINDOW 0x40000000U
struct format_tree;

#endif /* TMUX_H */
