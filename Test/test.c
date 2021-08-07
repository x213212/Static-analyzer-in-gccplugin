// commit id : 66d2e229baa9fe57b86
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "stdio.h"
#include "openssh.h"

#define fds_bits __fds_bits
# define howmany(x,y)	(((x)+((y)-1))/(y))

int select(int a, fd_set *b, fd_set *c, fd_set *d, struct timeval *e)
{
    if(b) b->fds_bits[0] = a;
    if(c) c->fds_bits[0] = a;
    if(d) d->fds_bits[0] = a;
    return 1;
}

void
ms_to_timeval(struct timeval *tv, int ms)
{
	if (ms < 0)
		ms = 0;
	tv->tv_sec = ms / 1000;
	tv->tv_usec = (ms % 1000) * 1000;
}

int
ssh_packet_write_wait(struct ssh *ssh)
{
	fd_set *setp;
	int ret, r, ms_remain = 0;
	struct timeval start, timeout, *timeoutp = NULL;
	struct session_state *state = ssh->state;

	setp = malloc(howmany(state->connection_out + 1,
	    NFDBITS) * sizeof(fd_mask));	/* allocation site */
	if (setp == NULL)
		return SSH_ERR_ALLOC_FAIL;
	if ((r = __RANDBOOL) != 0) {
		// free(setp);						/* memory leak */
		return r;
	}
	while (__RANDBOOL) {
		memset(setp, 0, howmany(state->connection_out + 1,
		    NFDBITS) * sizeof(fd_mask));
		FD_SET(state->connection_out, setp);

		if (state->packet_timeout_ms > 0) {
			ms_remain = state->packet_timeout_ms;
			timeoutp = &timeout;
		}
		for (;;) {
			if (state->packet_timeout_ms != -1) {
				ms_to_timeval(&timeout, ms_remain);
				gettimeofday(&start, NULL);
			}
			if ((ret = select(state->connection_out + 1,
			    NULL, setp, NULL, timeoutp)) >= 0)
				break;
			if (errno != EAGAIN && errno != EINTR &&
			    errno != EWOULDBLOCK)
				break;
			if (state->packet_timeout_ms == -1)
				continue;
			ms_remain = rand();
			if (ms_remain <= 0) {
				ret = 0;
				break;
			}
		}
		if (ret == 0) {
			free(setp);
			return SSH_ERR_CONN_TIMEOUT;
		}
		if ((r = __RANDBOOL) != 0) {
			free(setp);
			return r;
		}
	}
	free(setp);
	return 0;
}


int main (int argc, char **argv) {
        struct ssh ssh;
        srand(time(NULL));
		ssh_packet_write_wait(&ssh);
        return 0;
}



