/*
    buggy parent : a3839d8
    commit id : 5b8da1f53854c0923ec6e927e86709e4d72737b6
*/

#define OPENSSH 

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../openssh.h"

# define SSHBUF_SIZE_INIT	256		/* Initial allocation */
# define SSHBUF_SIZE_INC	256		/* Preferred increment length */
# define SSHBUF_PACK_MIN	8192		/* Minimim packable offset */

#define SSHBUF_SIZE_MAX		0x8000000	/* Hard maximum size */
#define SSHBUF_REFS_MAX		0x100000	/* Max child buffers */
#define SSHBUF_MAX_BIGNUM	(16384 / 8)	/* Max bignum *bytes* */
#define SSHBUF_MAX_ECPOINT	((528 * 2 / 8) + 1) /* Max EC point *bytes* */



#define Buffer              struct sshbuf
#define buffer_init(b)		sshbuf_init(b)
#define buffer_clear(b)		sshbuf_reset(b)
#define buffer_free(b)		sshbuf_free(b)
#define buffer_dump(b)		sshbuf_dump(b, stderr)

/* XXX cast is safe: sshbuf never stores more than len 2^31 */
#define buffer_len(b)		((u_int) sshbuf_len(b))
#define	buffer_ptr(b)		sshbuf_mutable_ptr(b)

/* Flag indicating that stdin should be redirected from /dev/null. */
extern int stdin_null_flag;

/* Flag indicating that no shell has been requested */
extern int no_shell_flag;

/* Flag indicating that ssh should daemonise after authentication is complete */
extern int fork_after_authentication_flag;

/* Control socket */
extern int muxserver_sock; /* XXX use mux_client_cleanup() instead */

/*
 * Name of the host we are connecting to.  This is the name given on the
 * command line, or the HostName specified for the user-supplied name in a
 * configuration file.
 */
extern char *host;

/*
 * Flag to indicate that we have received a window change signal which has
 * not yet been processed.  This will cause a message indicating the new
 * window size to be sent to the server a little later.  This is volatile
 * because this is updated in a signal handler.
 */
static volatile sig_atomic_t received_window_change_signal = 0;
static volatile sig_atomic_t received_signal = 0;

/* Flag indicating whether the user's terminal is in non-blocking mode. */
static int in_non_blocking_mode = 0;

/* Time when backgrounded control master using ControlPersist should exit */
static time_t control_persist_exit_time = 0;

/* Common data for the client loop code. */
volatile sig_atomic_t quit_pending; /* Set non-zero to quit the loop. */
static int last_was_cr;		/* Last character was a newline. */
static int exit_status;		/* Used to store the command exit status. */
static Buffer stderr_buffer;	/* Used for final exit message. */
static int connection_in;	/* Connection to server (input). */
static int connection_out;	/* Connection to server (output). */
static int need_rekeying;	/* Set to non-zero if rekeying is requested. */
static int session_closed;	/* In SSH2: login session closed. */
static u_int x11_refuse_time;	/* If >0, refuse x11 opens after this time. */

static void client_init_dispatch(void);
int	session_ident = -1;

/* Track escape per proto2 channel */
struct escape_filter_ctx {
	int escape_pending;
	int escape_char;
};

enum confirm_action { CONFIRM_WARN = 0, CONFIRM_CLOSE, CONFIRM_TTY };
/* Context for channel confirmation replies */
struct channel_reply_ctx {
	const char *request_type;
	int id;
	enum confirm_action action;
};
/* Definitions for channel types. */
#define SSH_CHANNEL_X11_LISTENER	1	/* Listening for inet X11 conn. */
#define SSH_CHANNEL_PORT_LISTENER	2	/* Listening on a port. */
#define SSH_CHANNEL_OPENING		3	/* waiting for confirmation */
#define SSH_CHANNEL_OPEN		4	/* normal open two-way channel */
#define SSH_CHANNEL_CLOSED		5	/* waiting for close confirmation */
#define SSH_CHANNEL_AUTH_SOCKET		6	/* authentication socket */
#define SSH_CHANNEL_X11_OPEN		7	/* reading first X11 packet */
#define SSH_CHANNEL_LARVAL		10	/* larval session */
#define SSH_CHANNEL_RPORT_LISTENER	11	/* Listening to a R-style port  */
#define SSH_CHANNEL_CONNECTING		12
#define SSH_CHANNEL_DYNAMIC		13
#define SSH_CHANNEL_ZOMBIE		14	/* Almost dead. */
#define SSH_CHANNEL_MUX_LISTENER	15	/* Listener for mux conn. */
#define SSH_CHANNEL_MUX_CLIENT		16	/* Conn. to mux slave */
#define SSH_CHANNEL_ABANDONED		17	/* Abandoned session, eg mux */
#define SSH_CHANNEL_UNIX_LISTENER	18	/* Listening on a domain socket. */
#define SSH_CHANNEL_RUNIX_LISTENER	19	/* Listening to a R-style domain socket. */
#define SSH_CHANNEL_MUX_PROXY		20	/* proxy channel for mux-slave */
#define SSH_CHANNEL_MAX_TYPE		21

#define CHANNEL_CANCEL_PORT_STATIC	-1


#define CHAN_EXTENDED_IGNORE		0
#define CHAN_EXTENDED_READ		1
#define CHAN_EXTENDED_WRITE		2

/* default window/packet sizes for tcp/x11-fwd-channel */
#define CHAN_SES_PACKET_DEFAULT	(32*1024)
#define CHAN_SES_WINDOW_DEFAULT	(64*CHAN_SES_PACKET_DEFAULT)
#define CHAN_TCP_PACKET_DEFAULT	(32*1024)
#define CHAN_TCP_WINDOW_DEFAULT	(64*CHAN_TCP_PACKET_DEFAULT)
#define CHAN_X11_PACKET_DEFAULT	(16*1024)
#define CHAN_X11_WINDOW_DEFAULT	(4*CHAN_X11_PACKET_DEFAULT)

/* possible input states */
#define CHAN_INPUT_OPEN			0
#define CHAN_INPUT_WAIT_DRAIN		1
#define CHAN_INPUT_WAIT_OCLOSE		2
#define CHAN_INPUT_CLOSED		3

/* possible output states */
#define CHAN_OUTPUT_OPEN		0
#define CHAN_OUTPUT_WAIT_DRAIN		1
#define CHAN_OUTPUT_WAIT_IEOF		2
#define CHAN_OUTPUT_CLOSED		3

#define CHAN_CLOSE_SENT			0x01
#define CHAN_CLOSE_RCVD			0x02
#define CHAN_EOF_SENT			0x04
#define CHAN_EOF_RCVD			0x08
#define CHAN_LOCAL			0x10

/* Read buffer size */
#define CHAN_RBUF	(16*1024)

/* Hard limit on number of channels */
#define CHANNELS_MAX_CHANNELS	(16*1024)


struct channel_confirms {};
struct channel_connect {};

typedef void channel_open_fn(struct ssh *, int, int, void *);
typedef void channel_callback_fn(struct ssh *, int, void *);
typedef int channel_infilter_fn(struct ssh *, struct Channel *, char *, int);
typedef void channel_filter_cleanup_fn(struct ssh *, int, void *);
typedef u_char *channel_outfilter_fn(struct ssh *, struct Channel *,
    u_char **, size_t *);
typedef int mux_callback_fn(struct ssh *, struct Channel *);

struct Channel {
	int     type;		/* channel type/state */
	int     self;		/* my own channel identifier */
	int	have_remote_id;	/* non-zero if remote_id is valid */

	u_int   istate;		/* input from channel (state of receive half) */
	u_int   ostate;		/* output to channel  (state of transmit half) */
	int     flags;		/* close sent/rcvd */
	int     rfd;		/* read fd */
	int     wfd;		/* write fd */
	int     efd;		/* extended fd */
	int     sock;		/* sock fd */
	int     ctl_chan;	/* control channel (multiplexed connections) */
	int     isatty;		/* rfd is a tty */
#ifdef _AIX
	int     wfd_isatty;	/* wfd is a tty */
#endif
	int	client_tty;	/* (client) TTY has been requested */
	int     force_drain;	/* force close on iEOF */
	time_t	notbefore;	/* Pause IO until deadline (time_t) */
	int     delayed;	/* post-select handlers for newly created
				 * channels are delayed until the first call
				 * to a matching pre-select handler.
				 * this way post-select handlers are not
				 * accidentally called if a FD gets reused */
	struct sshbuf *input;	/* data read from socket, to be sent over
				 * encrypted connection */
	struct sshbuf *output;	/* data received over encrypted connection for
				 * send on socket */
	struct sshbuf *extended;

	char    *path;
		/* path for unix domain sockets, or host name for forwards */
	int     listening_port;	/* port being listened for forwards */
	char   *listening_addr;	/* addr being listened for forwards */
	int     host_port;	/* remote port to connect for forwards */
	char   *remote_name;	/* remote hostname */

	u_int	remote_window;
	u_int	remote_maxpacket;
	u_int	local_window;
	u_int	local_window_max;
	u_int	local_consumed;
	u_int	local_maxpacket;
	int     extended_usage;
	int	single_connection;

	char   *ctype;		/* type */

	/* callback */
	channel_open_fn		*open_confirm;
	void			*open_confirm_ctx;
	channel_callback_fn	*detach_user;
	int			detach_close;
	struct channel_confirms	status_confirms;

	/* filter */
	channel_infilter_fn	*input_filter;
	channel_outfilter_fn	*output_filter;
	void			*filter_ctx;
	channel_filter_cleanup_fn *filter_cleanup;

	/* keep boundaries */
	int     		datagram;

	/* non-blocking connect */
	/* XXX make this a pointer so the structure can be opaque */
	struct channel_connect	connect_ctx;

	/* multiplexing protocol hook, called for each packet received */
	mux_callback_fn		*mux_rcb;
	void			*mux_ctx;
	int			mux_pause;
	int     		mux_downstream_id;
};

typedef struct Channel Channel;

typedef struct {
	char *host_to_connect;		/* Connect to 'host'. */
	int port_to_connect;		/* Connect to 'port'. */
	char *listen_host;		/* Remote side should listen address. */
	char *listen_path;		/* Remote side should listen path. */
	int listen_port;		/* Remote side should listen port. */
	Channel *downstream;		/* Downstream mux*/
} ForwardPermission;

typedef void chan_fn(struct ssh *, Channel *c,
    fd_set *readset, fd_set *writeset);

/* Master structure for channels state */
struct ssh_channels {
	/*
	 * Pointer to an array containing all allocated channels.  The array
	 * is dynamically extended as needed.
	 */
	Channel **channels;

	/*
	 * Size of the channel array.  All slots of the array must always be
	 * initialized (at least the type field); unused slots set to NULL
	 */
	u_int channels_alloc;

	/*
	 * Maximum file descriptor value used in any of the channels.  This is
	 * updated in channel_new.
	 */
	int channel_max_fd;

	/*
	 * 'channel_pre*' are called just before select() to add any bits
	 * relevant to channels in the select bitmasks.
	 *
	 * 'channel_post*': perform any appropriate operations for
	 * channels which have events pending.
	 */
	chan_fn **channel_pre;
	chan_fn **channel_post;

	/* -- tcp forwarding */

	/* List of all permitted host/port pairs to connect by the user. */
	ForwardPermission *permitted_opens;

	/* List of all permitted host/port pairs to connect by the admin. */
	ForwardPermission *permitted_adm_opens;

	/*
	 * Number of permitted host/port pairs in the array permitted by
	 * the user.
	 */
	u_int num_permitted_opens;

	/*
	 * Number of permitted host/port pair in the array permitted by
	 * the admin.
	 */
	u_int num_adm_permitted_opens;

	/*
	 * If this is true, all opens are permitted.  This is the case on
	 * the server on which we have to trust the client anyway, and the
	 * user could do anything after logging in anyway.
	 */
	int all_opens_permitted;

	/* -- X11 forwarding */

	/* Saved X11 local (client) display. */
	char *x11_saved_display;

	/* Saved X11 authentication protocol name. */
	char *x11_saved_proto;

	/* Saved X11 authentication data.  This is the real data. */
	char *x11_saved_data;
	u_int x11_saved_data_len;

	/* Deadline after which all X11 connections are refused */
	u_int x11_refuse_time;

	/*
	 * Fake X11 authentication data.  This is what the server will be
	 * sending us; we should replace any occurrences of this by the
	 * real data.
	 */
	u_char *x11_fake_data;
	u_int x11_fake_data_len;

	/* AF_UNSPEC or AF_INET or AF_INET6 */
	int IPv4or6;
};

ssize_t *vwrite(int, void *, size_t);

#define DISPATCH_MAX	255

enum {
	DISPATCH_BLOCK,
	DISPATCH_NONBLOCK
};


typedef int dispatch_fn(int, u_int32_t, struct ssh *);

struct ssh {
	/* Session state */
	struct session_state *state;

	/* Key exchange */
	struct kex *kex;

	/* cached local and remote ip addresses and ports */
	char *remote_ipaddr;
	int remote_port;
	char *local_ipaddr;
	int local_port;

	/* Optional preamble for log messages (e.g. username) */
	char *log_preamble;

	/* Dispatcher table */
	dispatch_fn *dispatch[DISPATCH_MAX];
	/* number of packets to ignore in the dispatcher */
	int dispatch_skip_packets;

	/* datafellows */
	int compat;

	/* Lists for private and public keys */
	TAILQ_HEAD(, key_entry) private_keys;
	TAILQ_HEAD(, key_entry) public_keys;

	/* Client/Server authentication context */
	void *authctxt;

	/* Channels context */
	struct ssh_channels *chanctxt;

	/* APP data */
	void *app_data;
};

void
sshbuf_init(struct sshbuf *ret)
{
	explicit_bzero(ret, sizeof(*ret));
	ret->alloc = SSHBUF_SIZE_INIT;
	ret->max_size = SSHBUF_SIZE_MAX;
	ret->readonly = 0;
	ret->dont_free = 1;
	ret->refcount = 1;
	if ((ret->cd = ret->d = calloc(1, ret->alloc)) == NULL)
		ret->alloc = 0;
}

void
sshbuf_free(struct sshbuf *buf)
{
	int dont_free = 0;

	if (buf == NULL)
		return;
	/*
	 * The following will leak on insane buffers, but this is the safest
	 * course of action - an invalid pointer or already-freed pointer may
	 * have been passed to us and continuing to scribble over memory would
	 * be bad.
	 */
	if (sshbuf_check_sanity(buf) != 0)
		return;
	/*
	 * If we are a child, the free our parent to decrement its reference
	 * count and possibly free it.
	 */
	sshbuf_free(buf->parent);
	buf->parent = NULL;
	/*
	 * If we are a parent with still-extant children, then don't free just
	 * yet. The last child's call to sshbuf_free should decrement our
	 * refcount to 0 and trigger the actual free.
	 */
	buf->refcount--;
	if (buf->refcount > 0)
		return;
	dont_free = buf->dont_free;
	if (!buf->readonly) {
		explicit_bzero(buf->d, buf->alloc);
		free(buf->d);
	}
	explicit_bzero(buf, sizeof(*buf));
	if (!dont_free)
		free(buf);
}

void
sshbuf_reset(struct sshbuf *buf)
{
	u_char *d;

	if (buf->readonly || buf->refcount > 1) {
		/* Nonsensical. Just make buffer appear empty */
		buf->off = buf->size;
		return;
	}
	(void) sshbuf_check_sanity(buf);
	buf->off = buf->size = 0;
	if (buf->alloc != SSHBUF_SIZE_INIT) {
		if ((d = recallocarray(buf->d, buf->alloc, SSHBUF_SIZE_INIT,
		    1)) != NULL) {
			buf->cd = buf->d = d;
			buf->alloc = SSHBUF_SIZE_INIT;
		}
	}
	explicit_bzero(buf->d, SSHBUF_SIZE_INIT);
}

static void
client_suspend_self(Buffer *bin, Buffer *bout, Buffer *berr)
{
	/* Flush stdout and stderr buffers. */
	if (buffer_len(bout) > 0)
		atomicio(vwrite, fileno(stdout), buffer_ptr(bout),
		    buffer_len(bout));
	if (buffer_len(berr) > 0)
		atomicio(vwrite, fileno(stderr), buffer_ptr(berr),
		    buffer_len(berr));

    /*
     * Free (and clear) the buffer to reduce the amount of data that gets
     * written to swap
     */

    buffer_free(bin);
    buffer_free(bout);
    buffer_free(berr);


	/* Reset window sizes in case they have changed */
	received_window_change_signal = 1;

    /* OK, we have been continued by the user. Reinitialize buffers */

    buffer_init(bin);   /* use-after-free */
    buffer_init(bout);  /* use-after-free */
    buffer_init(berr);  /* use-after-free */
}

/*
 * Allocate a new channel object and set its type and socket. This will cause
 * remote_name to be freed.
 */
Channel *
channel_new(struct ssh *ssh, char *ctype, int type, int rfd, int wfd, int efd,
    u_int window, u_int maxpack, int extusage, char *remote_name, int nonblock)
{
	struct ssh_channels *sc = ssh->chanctxt;
	u_int i, found;
	Channel *c;

	/* Try to find a free slot where to put the new channel. */
	for (i = 0; i < sc->channels_alloc; i++) {
		if (sc->channels[i] == NULL) {
			/* Found a free slot. */
			found = i;
			break;
		}
	}
	if (i >= sc->channels_alloc) {
		/*
		 * There are no free slots. Take last+1 slot and expand
		 * the array.
		 */
		found = sc->channels_alloc;
		if (sc->channels_alloc > CHANNELS_MAX_CHANNELS)
			fatal("%s: internal error: channels_alloc too big",
			    __func__);
		sc->channels = xrecallocarray(sc->channels, sc->channels_alloc,
		    sc->channels_alloc + 10, sizeof(*sc->channels));
		sc->channels_alloc += 10;
		debug2("channel: expanding %d", sc->channels_alloc);
	}
	/* Initialize and return new channel. */
	c = sc->channels[found] = xcalloc(1, sizeof(Channel));
	if ((c->input = sshbuf_new()) == NULL || /* allocation site */
	    (c->output = sshbuf_new()) == NULL || /* allocation site */
	    (c->extended = sshbuf_new()) == NULL) /* allocation site */
		fatal("%s: sshbuf_new failed", __func__);
	c->ostate = CHAN_OUTPUT_OPEN;
	c->istate = CHAN_INPUT_OPEN;
	channel_register_fds(ssh, c, rfd, wfd, efd, extusage, nonblock, 0);
	c->self = found;
	c->type = type;
	c->ctype = ctype;
	c->local_window = window;
	c->local_window_max = window;
	c->local_maxpacket = maxpack;
	c->remote_name = xstrdup(remote_name);
	c->ctl_chan = -1;
	c->delayed = 1;		/* prevent call to channel_post handler */
	debug("channel %d: new [%s]", found, remote_name);
	return c;
}

void
channel_init_channels(struct ssh *ssh)
{
	struct ssh_channels *sc;

	if ((sc = calloc(1, sizeof(*sc))) == NULL ||
	    (sc->channel_pre = calloc(SSH_CHANNEL_MAX_TYPE,
	    sizeof(*sc->channel_pre))) == NULL ||
	    (sc->channel_post = calloc(SSH_CHANNEL_MAX_TYPE,
	    sizeof(*sc->channel_post))) == NULL)
		fatal("%s: allocation failed", __func__);
	sc->channels_alloc = 10;
	sc->channels = xcalloc(sc->channels_alloc, sizeof(*sc->channels));
	channel_handler_init(sc);

	ssh->chanctxt = sc;
}

/* Free the channel and close its fd/socket. */
void
channel_free(struct ssh *ssh, Channel *c)
{
	struct ssh_channels *sc = ssh->chanctxt;
	char *s;
	u_int i, n;
	Channel *other;
	struct channel_confirm *cc;

	for (n = 0, i = 0; i < sc->channels_alloc; i++) {
		if ((other = sc->channels[i]) == NULL)
			continue;
		n++;
		/* detach from mux client and prepare for closing */
		if (c->type == SSH_CHANNEL_MUX_CLIENT &&
		    other->type == SSH_CHANNEL_MUX_PROXY &&
		    other->mux_ctx == c) {
			other->mux_ctx = NULL;
			other->type = SSH_CHANNEL_OPEN;
			other->istate = CHAN_INPUT_CLOSED;
			other->ostate = CHAN_OUTPUT_CLOSED;
		}
	}
	debug("channel %d: free: %s, nchannels %u", c->self,
	    c->remote_name ? c->remote_name : "???", n);

	if (c->type == SSH_CHANNEL_MUX_CLIENT)
		mux_remove_remote_forwardings(ssh, c);

	s = channel_open_message(ssh);
	debug3("channel %d: status: %s", c->self, s);
	free(s);

	channel_close_fds(ssh, c);
	sshbuf_free(c->input);
	sshbuf_free(c->output);
	sshbuf_free(c->extended);
	c->input = c->output = c->extended = NULL;
	free(c->remote_name);
	c->remote_name = NULL;
	free(c->path);
	c->path = NULL;
	free(c->listening_addr);
	c->listening_addr = NULL;
	sc->channels[c->self] = NULL;
	bzero(c, sizeof(*c));
	free(c);
}

void
channel_free_all(struct ssh *ssh)
{
	u_int i;

	for (i = 0; i < ssh->chanctxt->channels_alloc; i++)
		if (ssh->chanctxt->channels[i] != NULL)
			channel_free(ssh, ssh->chanctxt->channels[i]);
}


int main (int argc, char **argv) {
    struct ssh ssh;
    Channel *c;
    int in, out;

    channel_init_channels(&ssh);
    for(int i = 1; i < argc; i++) {
  	    c = channel_new(&ssh, "stdio-forward", SSH_CHANNEL_OPENING, in, out,
	        -1, CHAN_TCP_WINDOW_DEFAULT, CHAN_TCP_PACKET_DEFAULT,
   	        0, "stdio-forward", /*nonblock*/0);
    }
    client_suspend_self(c->input, c->output, c->extended);
    channel_free_all(&ssh);
    return 0;
}
