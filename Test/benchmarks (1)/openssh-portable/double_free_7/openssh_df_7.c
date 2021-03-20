/*
    commit id : 0d772d9d11ceedd9432383019a75859813149673
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../openssh.h"


/*
 * First, socket and INET6 related definitions 
 */
#ifndef HAVE_STRUCT_SOCKADDR_STORAGE
# define	_SS_MAXSIZE	128	/* Implementation specific max size */
# define       _SS_PADSIZE     (_SS_MAXSIZE - sizeof (struct sockaddr))
# define ss_family ss_sa.sa_family
#endif /* !HAVE_STRUCT_SOCKADDR_STORAGE */

#ifndef IN6_IS_ADDR_LOOPBACK
# define IN6_IS_ADDR_LOOPBACK(a) \
	(((u_int32_t *)(a))[0] == 0 && ((u_int32_t *)(a))[1] == 0 && \
	 ((u_int32_t *)(a))[2] == 0 && ((u_int32_t *)(a))[3] == htonl(1))
#endif /* !IN6_IS_ADDR_LOOPBACK */

#ifndef HAVE_STRUCT_IN6_ADDR
struct in6_addr {
	u_int8_t	s6_addr[16];
};
#endif /* !HAVE_STRUCT_IN6_ADDR */

#ifndef HAVE_STRUCT_SOCKADDR_IN6
struct sockaddr_in6 {
	unsigned short	sin6_family;
	u_int16_t	sin6_port;
	u_int32_t	sin6_flowinfo;
	struct in6_addr	sin6_addr;
	u_int32_t	sin6_scope_id;
};
#endif /* !HAVE_STRUCT_SOCKADDR_IN6 */

#ifndef AF_INET6
/* Define it to something that should never appear */
#define AF_INET6 AF_MAX
#endif

/*
 * Next, RFC2553 name / address resolution API
 */

#ifndef NI_NUMERICHOST
# define NI_NUMERICHOST    (1)
#endif
#ifndef NI_NAMEREQD
# define NI_NAMEREQD       (1<<1)
#endif
#ifndef NI_NUMERICSERV
# define NI_NUMERICSERV    (1<<2)
#endif

#ifndef AI_PASSIVE
# define AI_PASSIVE		(1)
#endif
#ifndef AI_CANONNAME
# define AI_CANONNAME		(1<<1)
#endif
#ifndef AI_NUMERICHOST
# define AI_NUMERICHOST		(1<<2)
#endif

#ifndef NI_MAXSERV
# define NI_MAXSERV 32
#endif /* !NI_MAXSERV */
#ifndef NI_MAXHOST
# define NI_MAXHOST 1025
#endif /* !NI_MAXHOST */

#ifndef EAI_NODATA
# define EAI_NODATA	(INT_MAX - 1)
#endif
#ifndef EAI_MEMORY
# define EAI_MEMORY	(INT_MAX - 2)
#endif
#ifndef EAI_NONAME
# define EAI_NONAME	(INT_MAX - 3)
#endif
#ifndef EAI_SYSTEM
# define EAI_SYSTEM	(INT_MAX - 4)
#endif
#ifndef EAI_FAMILY
# define EAI_FAMILY	(INT_MAX - 5)
#endif


/* Data structure for representing a forwarding request. */

typedef unsigned short u_short;
typedef struct {
	char	 *listen_host;		/* Host (address) to listen on. */
	u_short	  listen_port;		/* Port to forward. */
	char	 *connect_host;		/* Host to connect. */
	u_short	  connect_port;		/* Port to connect on connect_host. */
}       Forward;

int compat13 = 0;
int compat20 = 0;
int datafellows = 0;


/*
 * parse_forward
 * parses a string containing a port forwarding specification of the form:
 *   dynamicfwd == 0
 *	[listenhost:]listenport:connecthost:connectport
 *   dynamicfwd == 1
 *	[listenhost:]listenport
 * returns number of arguments parsed or zero on error
 */
int
parse_forward(Forward *fwd, const char *fwdspec, int dynamicfwd)
{
	int i;
	char *p, *cp, *fwdarg[4];

	memset(fwd, '\0', sizeof(*fwd));

	cp = p = xstrdup(fwdspec);

	/* skip leading spaces */
	while (isspace(*cp))
		cp++;

	for (i = 0; i < 4; ++i)
		if ((fwdarg[i] = hpdelim(&cp)) == NULL)
			break;

	/* Check for trailing garbage */
	if (cp != NULL)
		i = 0;	/* failure */
	switch (i) {
	case 1:
		fwd->listen_host = NULL;
		fwd->listen_port = a2port(fwdarg[0]);
		fwd->connect_host = xstrdup("socks");
		break;

	case 2:
		fwd->listen_host = xstrdup(cleanhostname(fwdarg[0]));
		fwd->listen_port = a2port(fwdarg[1]);
		fwd->connect_host = xstrdup("socks");
		break;

	case 3:
		fwd->listen_host = NULL;
		fwd->listen_port = a2port(fwdarg[0]);
		fwd->connect_host = xstrdup(cleanhostname(fwdarg[1]));
		fwd->connect_port = a2port(fwdarg[2]);
		break;

	case 4:
		fwd->listen_host = xstrdup(cleanhostname(fwdarg[0]));
		fwd->listen_port = a2port(fwdarg[1]);
		fwd->connect_host = xstrdup(cleanhostname(fwdarg[2]));
		fwd->connect_port = a2port(fwdarg[3]);
		break;
	default:
		i = 0; /* failure */
	}

	xfree(p);

	if (dynamicfwd) {
		if (!(i == 1 || i == 2))
			goto fail_free;
	} else {
		if (!(i == 3 || i == 4))
			goto fail_free;
		if (fwd->connect_port == 0)
			goto fail_free;
	}

	if (fwd->listen_port == 0)
		goto fail_free;

	if (fwd->connect_host != NULL &&
	    strlen(fwd->connect_host) >= NI_MAXHOST)
		goto fail_free;

	return (i);

 fail_free:
	if (fwd->connect_host != NULL) {
		xfree(fwd->connect_host); /* double-free */
    }
	if (fwd->listen_host != NULL) {
		xfree(fwd->listen_host); /* double-free */
	
    }
	return (0);
}


static void
process_cmdline(void)
{
	void (*handler)(int);
	char *s, *cmd, *cancel_host;
	int delete = 0;
	int local = 0, remote = 0, dynamic = 0;
	u_short cancel_port;
	Forward fwd;

	bzero(&fwd, sizeof(fwd));
	fwd.listen_host = fwd.connect_host = NULL;

	leave_raw_mode();
	cmd = s = read_passphrase("\r\nssh> ", RP_ECHO);
	if (s == NULL)
		goto out;
	while (isspace(*s))
		s++;
	if (*s == '-')
		s++;	/* Skip cmdline '-', if any */
	if (*s == '\0')
		goto out;

	if (*s == 'h' || *s == 'H' || *s == '?') {
		logit("Commands:");
		logit("      -L[bind_address:]port:host:hostport    "
		    "Request local forward");
		logit("      -R[bind_address:]port:host:hostport    "
		    "Request remote forward");
		logit("      -D[bind_address:]port                  "
		    "Request dynamic forward");
		logit("      -KR[bind_address:]port                 "
		    "Cancel remote forward");
		logit("      !args                                  "
		    "Execute local command");
		goto out;
	}

	if (*s == '!') {
		s++;
		ssh_local_cmd(s);
		goto out;
	}

	if (*s == 'K') {
		delete = 1;
		s++;
	}
	if (*s == 'L')
		local = 1;
	else if (*s == 'R')
		remote = 1;
	else if (*s == 'D')
		dynamic = 1;
	else {
		logit("Invalid command.");
		goto out;
	}

	if ((local || dynamic) && delete) {
		logit("Not supported.");
		goto out;
	}
	if (remote && delete && !compat20) {
		logit("Not supported for SSH protocol version 1.");
		goto out;
	}

	while (isspace(*++s))
		;

	if (delete) {
		cancel_port = 0;
		cancel_host = hpdelim(&s);	/* may be NULL */
		if (s != NULL) {
			cancel_port = a2port(s);
			cancel_host = cleanhostname(cancel_host);
		} else {
			cancel_port = a2port(cancel_host);
			cancel_host = NULL;
		}
		if (cancel_port == 0) {
			logit("Bad forwarding close port");
			goto out;
		}
		channel_request_rforward_cancel(cancel_host, cancel_port);
	} else {
		if (!parse_forward(&fwd, s, dynamic ? 1 : 0)) {
			logit("Bad forwarding specification.");
			goto out;
		}
		if (local || dynamic) {
			if (channel_setup_local_fwd_listener(fwd.listen_host,
			    fwd.listen_port, fwd.connect_host,
			    fwd.connect_port) < 0) {
				logit("Port forwarding failed.");
				goto out;
			}
		} else {
			if (channel_request_remote_forwarding(fwd.listen_host,
			    fwd.listen_port, fwd.connect_host,
			    fwd.connect_port) < 0) {
				logit("Port forwarding failed.");
				goto out;
			}
		}

		logit("Forwarding port.");
	}

out:
	enter_raw_mode();
	if (cmd)
		xfree(cmd);
	if (fwd.listen_host != NULL)
		xfree(fwd.listen_host); /* double-free */
	if (fwd.connect_host != NULL)
		xfree(fwd.connect_host); /* double-free */
}


int main() {
    process_cmdline();
    return 0;
}


