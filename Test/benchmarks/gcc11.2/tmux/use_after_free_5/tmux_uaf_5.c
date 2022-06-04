/*
	buggy parent : 53957dc
	commit id : 353f2a2
*/

#include <stdio.h>
#include <stdlib.h>
#define TTY
#define CLIENT 
#include "../stdio.h"
#include "../tmux.h"

#define POLLIN 0x1
#define POLLOUT 0x2

struct pollfd {
	int 	fd;
	int 	events;
};

struct imsgbuf {
	int			 fd;
	pid_t			 pid;
};

/* Termcap types. */
enum tty_code_type {
	TTYCODE_NONE = 0,
	TTYCODE_STRING,
	TTYCODE_NUMBER,
	TTYCODE_FLAG,
};

/* Termcap code. */
struct tty_code {
	enum tty_code_type	type;
	union {
		char 	       *string;
		int	 	number;
		int	 	flag;
	} value;
};

#define NTTYCODE 300
struct tty_term {
	char		*name;
	u_int		 references;

	struct tty_code	 codes[NTTYCODE];

#define TERM_256COLOURS 0x1
#define TERM_88COLOURS 0x2
#define TERM_EARLYWRAP 0x4
	int		 flags;

};

struct tty {
	char		*path;

    u_int            sx;
    u_int            sy;

	u_int		 cx;
	u_int		 cy;

	int		 mode;

	u_int		 rlower;
	u_int		 rupper;

	char		*termname;
	struct tty_term	*term;

	int		 fd;
	struct buffer	*in;
	struct buffer	*out;

	int		 log_fd;

	struct termios   tio;

	struct grid_cell cell;

	u_char		 acs[UCHAR_MAX + 1];

#define TTY_NOCURSOR 0x1
#define TTY_FREEZE 0x2
#define TTY_ESCAPE 0x4
#define TTY_UTF8 0x8
#define TTY_STARTED 0x10
#define TTY_OPENED 0x20
	int    		 flags;

	int		 term_flags;

	struct timeval	 key_timer;

	size_t		 ksize;	/* maximum key size */
	RB_HEAD(tty_keys, tty_key) ktree;
};

/* Client connection. */
struct client {
	struct imsgbuf	 ibuf;
	struct timeval	 tv;

	char		*title;
	char		*cwd;

	struct tty 	 tty;
	struct timeval	 repeat_timer;

	struct timeval	 status_timer;
	struct screen	 status;

#define CLIENT_TERMINAL 0x1
#define CLIENT_PREFIX 0x2
#define CLIENT_MOUSE 0x4
#define CLIENT_REDRAW 0x8
#define CLIENT_STATUS 0x10
#define CLIENT_REPEAT 0x20	/* allow command to repeat within repeat time */
#define CLIENT_SUSPENDED 0x40
#define CLIENT_BAD 0x80
#define CLIENT_IDENTIFY 0x100
#define CLIENT_DEAD 0x200
	int		 flags;

	struct timeval	 identify_timer;

	char		*message_string;
	struct timeval	 message_timer;

	char		*prompt_string;
	char		*prompt_buffer;
	size_t		 prompt_index;
	int		 (*prompt_callbackfn)(void *, const char *);
	void		 (*prompt_freefn)(void *);
	void		*prompt_data;

#define PROMPT_SINGLE 0x1
	int		 prompt_flags;

	u_int		 prompt_hindex;
	ARRAY_DECL(, char *) prompt_hdata;

	struct session	*session;

	int		 references;
};
ARRAY_DECL(clients, struct client *);


/* Client list. */
struct clients clients;
struct clients dead_clients;

int 	srv_fd;

/* Buffer macros. */
#define BUFFER_USED(b) ((b)->size)
#define BUFFER_FREE(b) ((b)->space - (b)->off - (b)->size)
#define BUFFER_IN(b) ((b)->base + (b)->off + (b)->size)
#define BUFFER_OUT(b) ((b)->base + (b)->off)

/* Buffer structure. */
struct buffer {
	u_char	*base;		/* buffer start */
	size_t	 space;		/* total size of buffer */

	size_t	 size;		/* size of data in buffer */
	size_t	 off;		/* offset of data in buffer */
};

/* Mapping of a pollfd to an fd independent of its position in the array. */
struct poll_item {
	int	 fd;
	int	 events;

	void	 (*fn)(int, int, void *);
	void	*data;

	RB_ENTRY(poll_item) entry;
};
RB_HEAD(poll_items, poll_item) poll_items;

int		 server_poll_cmp(struct poll_item *, struct poll_item *);
struct poll_item*server_poll_lookup(int);
void		 server_poll_add(int, int, void (*)(int, int, void *), void *);
struct pollfd	*server_poll_flatten(int *);
void		 server_poll_dispatch(struct pollfd *, int);
void		 server_poll_reset(void);

RB_PROTOTYPE(poll_items, poll_item, entry, server_poll_cmp);
RB_GENERATE(poll_items, poll_item, entry, server_poll_cmp);

#define SIZE_MAX (1 << 30)
void *
xrealloc(void *oldptr, size_t nmemb, size_t size)
{
	size_t	 newsize = nmemb * size;
	void	*newptr;

	if (newsize == 0)
		fatalx("zero size%s\n", "");
	if (SIZE_MAX / nmemb < size)
		fatalx("nmemb * size > SIZE_MAX%s\n", "");
	if ((newptr = realloc(oldptr, newsize)) == NULL)
		fatal("xrealloc failed%s\n", "");

	return (newptr);
}

/* Create a buffer. */
struct buffer *
buffer_create(size_t size)
{
	struct buffer	*b;

	if (size == 0)
		fatalx("zero size%s\n", "");

	b = xmalloc(sizeof *b);		/* allocation site */

	b->space = size;

	return (b);
}

/* Destroy a buffer. */
void
buffer_destroy(struct buffer *b)
{
	xfree(b);
}

/* Adjust buffer after data appended. */
void
buffer_add(struct buffer *b, size_t size)
{
	if (size == 0)
		fatalx("zero size%s\n", "");
	if (size > b->space - b->size)
		fatalx("overflow%s\n", "");

	b->size += size;
}

/* Ensure free space for size in buffer. */
void
buffer_ensure(struct buffer *b, size_t size)
{
	if (size == 0)
		fatalx("zero size%s\n", "");

	if (SIZE_MAX - b->size < size)
		fatalx("size too big%s\n", "");
}


/* Copy data into a buffer. */
void
buffer_write(struct buffer *b, const void *data, size_t size)
{
	buffer_ensure(b, size);
	buffer_add(b, size);
}


/* Fill buffers from socket based on poll results. */
int
buffer_poll(int fd, int events, struct buffer *in, struct buffer *out)
{
	ssize_t	n = rand()%2 + 1;

	if (in != NULL) {
		buffer_ensure(in, BUFSIZ);
		buffer_add(in, n);
	}
	
	if(__RANDBOOL) 
		return -1;

	return (0);
}
int
server_poll_cmp(struct poll_item *pitem1, struct poll_item *pitem2)
{
	return (pitem1->fd - pitem2->fd);
}

void
tty_close(struct tty *tty)
{
	if (tty->flags & TTY_OPENED) {
		buffer_destroy(tty->in);
		buffer_destroy(tty->out);
		tty->flags &= ~TTY_OPENED;
	}
}


void
tty_free(struct tty *tty)
{
	tty_close(tty);
}


/* Check if the server should be shutting down (no more clients or windows). */
int
server_should_shutdown(void)
{
	u_int	i;

	for (i = 0; i < ARRAY_LENGTH(&clients); i++) {
		if (ARRAY_ITEM(&clients, i) != NULL)
			return (0);
	}
	return (1);
}

/* Free dead, unreferenced clients and sessions. */
void
server_clean_dead(void)
{
	struct client	*c;
	u_int		 i;
	
	for (i = 0; i < ARRAY_LENGTH(&dead_clients); i++) {
		c = ARRAY_ITEM(&dead_clients, i);
		if (c == NULL || c->references != 0)
			continue;
		ARRAY_SET(&dead_clients, i, NULL);
		xfree(c);
	}
}

/* Create a new client. */
void
server_client_create(int fd)
{
	struct client	*c;
	int		 mode;
	u_int		 i;
	c = xmalloc(sizeof *c);
	c->references = 0;

	c->ibuf.fd = fd;
	c->tty.fd = __RANDBOOL? 0: -1;
	c->title = NULL;

	c->session = NULL;
	c->tty.sx = 80;
	c->tty.sy = 24;
	c->tty.in = buffer_create(BUFSIZ);
	c->tty.out = buffer_create(BUFSIZ);
	c->tty.flags = TTY_OPENED;
	c->flags |= CLIENT_TERMINAL;

	c->message_string = NULL;

	c->prompt_string = NULL;
	c->prompt_buffer = NULL;
	c->prompt_index = 0;

	printf("new client %d\n", fd);
	for (i = 0; i < ARRAY_LENGTH(&clients); i++) {
		if (ARRAY_ITEM(&clients, i) == NULL) {
			ARRAY_SET(&clients, i, c);
			return;
		}
	}
	ARRAY_ADD(&clients, c);
}


/* Lost a client. */
void
server_client_lost(struct client *c)
{
	u_int	i;

	for (i = 0; i < ARRAY_LENGTH(&clients); i++) {
		if (ARRAY_ITEM(&clients, i) == c)
			ARRAY_SET(&clients, i, NULL);
	}

	/*
	 * If CLIENT_TERMINAL hasn't been set, then tty_init hasn't been called
	 * and tty_free might close an unrelated fd.
	 */
	if (c->flags & CLIENT_TERMINAL)
		tty_free(&c->tty);

	for (i = 0; i < ARRAY_LENGTH(&dead_clients); i++) {
		if (ARRAY_ITEM(&dead_clients, i) == NULL) {
			ARRAY_SET(&dead_clients, i, c);
			break;
		}
	}
	if (i == ARRAY_LENGTH(&dead_clients))
		ARRAY_ADD(&dead_clients, c);
	c->flags |= CLIENT_DEAD;
}
void
server_poll_reset(void)
{
	struct poll_item	*pitem;

	while (!RB_EMPTY(&poll_items)) {
		pitem = RB_ROOT(&poll_items);
		RB_REMOVE(poll_items, &poll_items, pitem);
		xfree(pitem);
	}
}

struct poll_item *
server_poll_lookup(int fd)
{
	struct poll_item	pitem;

	pitem.fd = fd;
	return (RB_FIND(poll_items, &poll_items, &pitem));
}

struct pollfd *
server_poll_flatten(int *nfds)
{
	struct poll_item	*pitem;
	struct pollfd		*pfds;

	pfds = NULL;
	*nfds = 0;
	RB_FOREACH(pitem, poll_items, &poll_items) {
		pfds = xrealloc(pfds, (*nfds) + 1, sizeof *pfds);
		pfds[*nfds].fd = pitem->fd;
		pfds[*nfds].events = pitem->events;
		(*nfds)++;
	}
	return (pfds);
}

/* Callback for server socket. */
void
server_callback(int fd, int events, void *data)
{
	server_client_create(++srv_fd);
}

void
server_poll_dispatch(struct pollfd *pfds, int nfds)
{
	struct poll_item	*pitem;
	struct pollfd		*pfd;

	while (nfds > 0) {
		pfd = &pfds[--nfds];
		pitem = server_poll_lookup(pfd->fd);
		pitem->fn(pitem->fd, pfd->events, pitem->data);
	}
	xfree(pfds);
}

void
server_poll_add(int fd, int events, void (*fn)(int, int, void *), void *data)
{
	struct poll_item	*pitem;

	pitem = xmalloc(sizeof *pitem);
	pitem->fd = fd;
	pitem->events = events;

	pitem->fn = fn;
	pitem->data = data;

	RB_INSERT(poll_items, &poll_items, pitem);
}

/* Process a single client event. */
void
server_client_callback(int fd, int events, void *data)
{
	struct client	*c = data;

	/* developer fix */
//	if(c->flags & CLIENT_DEAD)
//		return;

	if (c->flags & CLIENT_BAD) {
		goto client_lost;
	}

	if (c->flags & CLIENT_SUSPENDED)
		return;

	printf("client : %d is used\n", c->ibuf.fd);
	if (buffer_poll(fd, events, c->tty.in, c->tty.out) != 0)	/* use-after-free */
		goto client_lost;

	return;

client_lost:
	printf("client : %d is lost\n", c->ibuf.fd);
	server_client_lost(c);
}

/* Fill client pollfds. */
void
server_fill_clients(void)
{
	struct client	*c;
	u_int		 i;
	int		 events;

	for (i = 0; i < ARRAY_LENGTH(&clients); i++) {
		c = ARRAY_ITEM(&clients, i);

		if (c != NULL) {
			events = 0;
			if (!(c->flags & CLIENT_BAD))
				events |= POLLIN;
			if (c->ibuf.fd < 0)
				events |= POLLOUT;
			server_poll_add(
			    c->ibuf.fd, events, server_client_callback, c);
		}

		if (c != NULL && !(c->flags & CLIENT_SUSPENDED) &&
		    c->tty.fd != -1) {
			events = POLLIN;
			if (BUFFER_USED(c->tty.out) > 0)
				events |= POLLOUT;
			server_poll_add(
			    c->tty.fd, events, server_client_callback, c);
		}
	}
}

/* Handle data input or output from client. */
void
server_client_handle_data(struct client *c)
{
	const char *s = "handling data";
	buffer_write(c->tty.out, s, strlen(s));
	if(__RANDBOOL)
		c->flags |= CLIENT_BAD;
}

/* Client functions that need to happen every loop. */
void
server_client_loop(void)
{
	struct client		*c;
	u_int		 	 i;

	for (i = 0; i < ARRAY_LENGTH(&clients); i++) {
		c = ARRAY_ITEM(&clients, i);
		if (c == NULL)
			continue;
		server_client_handle_data(c);
	}
}


int main(int argc, char **argv)
{
	struct client *c;
	struct pollfd *pfds;
	int 	pair[2], srv_fd, nfds;
	u_int 	i;
	char 	*cause;
	
	srand(time(NULL));
	srv_fd = 0;

	ARRAY_INIT(&clients);
	ARRAY_INIT(&dead_clients);

	server_client_create(srv_fd);
	
	pfds = NULL;
	for (;;) {
		if (server_should_shutdown())
			break;

		srv_fd++;

		/* Initialise pollfd array and add server socket. */
		server_poll_reset();
		server_poll_add(srv_fd, POLLIN, server_callback, NULL);

		server_fill_clients();

		/* Do the poll. */
		pfds = server_poll_flatten(&nfds);
		
		server_poll_dispatch(pfds, nfds);

		server_client_loop();

		/* Collect dead clients and sessions. */
		server_clean_dead();

		if(__RANDBOOL) break;
	}
	server_poll_reset();

	for (i = 0; i < ARRAY_LENGTH(&clients); i++) {
		if (ARRAY_ITEM(&clients, i) != NULL)
			server_client_lost(ARRAY_ITEM(&clients, i));
	}
	ARRAY_FREE(&clients);

	server_clean_dead();
}








