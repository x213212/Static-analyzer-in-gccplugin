/*
	commit id : 5acee1c04ed38afd6a32da4a66e6855ccdc52af3
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/sysctl.h>
#include <errno.h>
#include <sys/stat.h>

#include "../stdio.h"
#include "../tmux.h"


#ifndef nitems
#define nitems(_a) (sizeof((_a)) / sizeof((_a)[0]))
#endif

#define SRUN 0
#define SIDL 1
#define SONPROC 2
#define SSTOP 3
#define SZOMB 4
#define SDEAD 5
#define P_SINTR 6


#define CTL_KERN 0
#define KERN_PROC 0
#define KERN_PROC_PGRP 0

#define is_runnable(p) \
	((p)->p_stat == SRUN || (p)->p_stat == SIDL || (p)->p_stat == SONPROC)
#define is_stopped(p) \
	((p)->p_stat == SSTOP || (p)->p_stat == SZOMB || (p)->p_stat == SDEAD)

struct kinfo_proc {
	int p_tdev;	
	int p_stat;
	int p_estcpu;
	int p_slptime;
	int p_flag;
	char *p_comm;
	int p_pid;
};

struct kinfo_proc	*cmp_procs(struct kinfo_proc *, struct kinfo_proc *);
char			*get_proc_name(int, char *);

struct kinfo_proc *
cmp_procs(struct kinfo_proc *p1, struct kinfo_proc *p2)
{
	if (is_runnable(p1) && !is_runnable(p2))
		return (p1);
	if (!is_runnable(p1) && is_runnable(p2))
		return (p2);

	if (is_stopped(p1) && !is_stopped(p2))
		return (p1);
	if (!is_stopped(p1) && is_stopped(p2))
		return (p2);

	if (p1->p_estcpu > p2->p_estcpu)
		return (p1);
	if (p1->p_estcpu < p2->p_estcpu)
		return (p2);

	if (p1->p_slptime < p2->p_slptime)
		return (p1);
	if (p1->p_slptime > p2->p_slptime)
		return (p2);

	if ((p1->p_flag & P_SINTR) && !(p2->p_flag & P_SINTR))
		return (p1);
	if (!(p1->p_flag & P_SINTR) && (p2->p_flag & P_SINTR))
		return (p2);

	if (strcmp(p1->p_comm, p2->p_comm) < 0)
		return (p1);
	if (strcmp(p1->p_comm, p2->p_comm) > 0)
		return (p2);

	if (p1->p_pid > p2->p_pid)
		return (p1);
	return (p2);
}


char *
get_proc_name(int fd, char *tty)
{
	int		 mib[6] = { CTL_KERN, KERN_PROC, KERN_PROC_PGRP, 0,
				    sizeof(struct kinfo_proc), 0 };
	struct stat	 sb;
	size_t		 len;
	struct kinfo_proc *buf, *newbuf, *bestp;
	u_int		 i;
	char		*name;

	buf = NULL;

	if (stat(tty, &sb) == -1)
		return (NULL);
	if ((mib[3] = tcgetpgrp(fd)) == -1)
		return (NULL);

retry:
	if (sysctl(mib, nitems(mib), NULL, &len, NULL, 0) == -1)
		return (NULL);										/* memory leak */
	len = (len * 5) / 4;

	if ((newbuf = realloc(buf, len)) == NULL)				/* allocation site */
		goto error;
	buf = newbuf;

	mib[5] = (int)(len / sizeof(struct kinfo_proc));
	if (sysctl(mib, nitems(mib), buf, &len, NULL, 0) == -1) {
		if (errno == ENOMEM)
			goto retry;
		goto error;
	}

	bestp = NULL;
	for (i = 0; i < len / sizeof (struct kinfo_proc); i++) {
		if ((dev_t)buf[i].p_tdev != sb.st_rdev)
			continue;
		if (bestp == NULL)
			bestp = &buf[i];
		else
			bestp = cmp_procs(&buf[i], bestp);
	}

	name = NULL;
	if (bestp != NULL)
		name = strdup(bestp->p_comm);

	free(buf);
	return (name);

error:
	free(buf);
	return (NULL);
}

int main ()
{
	int fd;
	char *name;
	name = get_proc_name(fd, "tty");
	*name = 0;
	free(name);
	return 0;
}
