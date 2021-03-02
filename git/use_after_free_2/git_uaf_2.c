/*
    buggy parent : 62104ba
    commit id :  2a73b3dad09ef162eb5917e9e0d01d7c306f6b35
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "../stdio.h"
#include "../git.h"

/* fake a poll(2) environment */
#define POLLIN      0x0001      /* any readable data available   */
#define POLLPRI     0x0002      /* OOB/Urgent readable data      */
#define POLLOUT     0x0004      /* file descriptor is writeable  */
#define POLLERR     0x0008      /* some poll error occurred      */
#define POLLHUP     0x0010      /* file descriptor was "hung up" */
#define POLLNVAL    0x0020      /* requested events "invalid"    */
#define POLLRDNORM  0x0040
#define POLLRDBAND  0x0080
#define POLLWRNORM  0x0100
#define POLLWRBAND  0x0200

#define CHILD_PROCESS_INIT { NULL, ARGV_ARRAY_INIT, ARGV_ARRAY_INIT }
#define ARGV_ARRAY_INIT { empty_argv, 0, 0 }
extern const char *empty_argv[];

enum child_state {
	GIT_CP_FREE,
	GIT_CP_WORKING,
	GIT_CP_WAIT_CLEANUP,
};

struct pollfd
{
  int fd;                       /* which file descriptor to poll */
  short events;                 /* events we are interested in   */
  short revents;                /* events found on return        */
};

struct argv_array {
	const char **argv;
	int argc;
	int alloc;
};

struct child_process {
	const char **argv;
	struct argv_array args;
	struct argv_array env_array;
	pid_t pid;
	/*
	 * Using .in, .out, .err:
	 * - Specify 0 for no redirections (child inherits stdin, stdout,
	 *   stderr from parent).
	 * - Specify -1 to have a pipe allocated as follows:
	 *     .in: returns the writable pipe end; parent writes to it,
	 *          the readable pipe end becomes child's stdin
	 *     .out, .err: returns the readable pipe end; parent reads from
	 *          it, the writable pipe end becomes child's stdout/stderr
	 *   The caller of start_command() must close the returned FDs
	 *   after it has completed reading from/writing to it!
	 * - Specify > 0 to set a channel to a particular FD as follows:
	 *     .in: a readable FD, becomes child's stdin
	 *     .out: a writable FD, becomes child's stdout/stderr
	 *     .err: a writable FD, becomes child's stderr
	 *   The specified FD is closed by start_command(), even in case
	 *   of errors!
	 */
	int in;
	int out;
	int err;
	const char *dir;
	const char *const *env;
	unsigned no_stdin:1;
	unsigned no_stdout:1;
	unsigned no_stderr:1;
	unsigned git_cmd:1; /* if this is to be git sub-command */
	unsigned silent_exec_failure:1;
	unsigned stdout_to_stderr:1;
	unsigned use_shell:1;
	unsigned clean_on_exit:1;
};

typedef int (*get_next_task_fn)(struct child_process *cp,
				struct strbuf *err,
				void *pp_cb,
				void **pp_task_cb);

typedef int (*start_failure_fn)(struct child_process *cp,
				struct strbuf *err,
				void *pp_cb,
				void *pp_task_cb);

typedef int (*task_finished_fn)(int result,
				struct child_process *cp,
				struct strbuf *err,
				void *pp_cb,
				void *pp_task_cb);

struct parallel_processes {
	void *data;

	int max_processes;
	int nr_processes;

	get_next_task_fn get_next_task;
	start_failure_fn start_failure;
	task_finished_fn task_finished;

	struct {
		enum child_state state;
		struct child_process process;
		struct strbuf err;
		void *data;
	} *children;
	/*
	 * The struct pollfd is logically part of *children,
	 * but the system call expects it as its own array.
	 */
	struct pollfd *pfd;

	unsigned shutdown : 1;

	int output_owner;
	struct strbuf buffered_output; /* of finished children */
};


static int default_start_failure(struct child_process *cp,
				 struct strbuf *err,
				 void *pp_cb,
				 void *pp_task_cb)
{
	int i;

	strbuf_addstr(err, "Starting a child failed:");
	for (i = 0; cp->argv[i]; i++)
		strbuf_addf(err, " %s", cp->argv[i]);

	return 0;
}

static int default_task_finished(int result,
				 struct child_process *cp,
				 struct strbuf *err,
				 void *pp_cb,
				 void *pp_task_cb)
{
	int i;

	if (!result)
		return 0;

	strbuf_addf(err, "A child failed with return code %d:", result);
	for (i = 0; cp->argv[i]; i++)
		strbuf_addf(err, " %s", cp->argv[i]);

	return 0;
}

static struct parallel_processes *pp_for_signal;
static void pp_init(struct parallel_processes *pp,
		    int n,
		    get_next_task_fn get_next_task,
		    start_failure_fn start_failure,
		    task_finished_fn task_finished,
		    void *data)
{
	int i;

	if (n < 1)
		n = online_cpus();

	pp->max_processes = n;

	trace_printf("run_processes_parallel: preparing to run up to %d tasks", n);

	pp->data = data;
	if (!get_next_task)
		die("BUG: you need to specify a get_next_task function%s", "");
	pp->get_next_task = get_next_task;

	pp->start_failure = start_failure ? start_failure : default_start_failure;
	pp->task_finished = task_finished ? task_finished : default_task_finished;

	pp->nr_processes = 0;
	pp->output_owner = 0;
	pp->shutdown = 0;
	pp->children = xcalloc(n, sizeof(*pp->children));
	pp->pfd = xcalloc(n, sizeof(*pp->pfd));
	strbuf_init(&pp->buffered_output, 0);

	for (i = 0; i < n; i++) {
		strbuf_init(&pp->children[i].err, 0);
		child_process_init(&pp->children[i].process);
		pp->pfd[i].events = POLLIN | POLLHUP;
		pp->pfd[i].fd = -1;
	}

	pp_for_signal = pp;
}

/* returns
 *  0 if a new task was started.
 *  1 if no new jobs was started (get_next_task ran out of work, non critical
 *    problem with starting a new command)
 * <0 no new job was started, user wishes to shutdown early. Use negative code
 *    to signal the children.
 */
static int pp_start_one(struct parallel_processes *pp)
{
	int i, code;

	for (i = 0; i < pp->max_processes; i++)
		if (pp->children[i].state == GIT_CP_FREE)
			break;
	if (i == pp->max_processes)
		die("BUG: bookkeeping is hard%s", "");

	code = pp->get_next_task(&pp->children[i].process,
				 &pp->children[i].err,
				 pp->data,
				 &pp->children[i].data);
	if (!code) {
		strbuf_addbuf(&pp->buffered_output, &pp->children[i].err);
		strbuf_reset(&pp->children[i].err);
		return 1;
	}
	pp->children[i].process.err = -1;
	pp->children[i].process.stdout_to_stderr = 1;
	pp->children[i].process.no_stdin = 1;

	if (start_command(&pp->children[i].process)) {
		code = pp->start_failure(&pp->children[i].process,
					 &pp->children[i].err,
					 pp->data,
					 &pp->children[i].data);
		strbuf_addbuf(&pp->buffered_output, &pp->children[i].err);
		strbuf_reset(&pp->children[i].err);
		if (code)
			pp->shutdown = 1;
		return code;
	}

	pp->nr_processes++;
	pp->children[i].state = GIT_CP_WORKING;
	pp->pfd[i].fd = pp->children[i].process.err;
	return 0;
}

static void kill_children(struct parallel_processes *pp, int signo)
{
	int i, n = pp->max_processes;

	for (i = 0; i < n; i++)
		if (pp->children[i].state == GIT_CP_WORKING)
			kill(pp->children[i].process.pid, signo);
}

static int pp_collect_finished(struct parallel_processes *pp)
{
	int i, code;
	int n = pp->max_processes;
	int result = 0;

	while (pp->nr_processes > 0) {
		for (i = 0; i < pp->max_processes; i++)
			if (pp->children[i].state == GIT_CP_WAIT_CLEANUP)
				break;
		if (i == pp->max_processes)
			break;

		code = finish_command(&pp->children[i].process);

		code = pp->task_finished(code, &pp->children[i].process,
					 &pp->children[i].err, pp->data,
					 &pp->children[i].data);

		if (code)
			result = code;
		if (code < 0)
			break;

		pp->nr_processes--;
		pp->children[i].state = GIT_CP_FREE;
		pp->pfd[i].fd = -1;
		child_process_init(&pp->children[i].process);

		if (i != pp->output_owner) {
			strbuf_addbuf(&pp->buffered_output, &pp->children[i].err);
			strbuf_reset(&pp->children[i].err);
		} else {
			fputs(pp->children[i].err.buf, stderr);
			strbuf_reset(&pp->children[i].err);

			/* Output all other finished child processes */
			fputs(pp->buffered_output.buf, stderr);
			strbuf_reset(&pp->buffered_output);

			/*
			 * Pick next process to output live.
			 * NEEDSWORK:
			 * For now we pick it randomly by doing a round
			 * robin. Later we may want to pick the one with
			 * the most output or the longest or shortest
			 * running process time.
			 */
			for (i = 0; i < n; i++)
				if (pp->children[(pp->output_owner + i) % n].state == GIT_CP_WORKING)
					break;
			pp->output_owner = (pp->output_owner + i) % n;
		}
	}
	return result;
}

int finish_command(struct child_process *cmd)
{
    int ret = wait_or_whine(cmd->pid, cmd->argv[0], 0);
    child_process_clear(cmd);
    return ret;
}

void child_process_clear(struct child_process *child)
{
	argv_array_clear(&child->args);
	argv_array_clear(&child->env_array);
}


int run_processes_parallel(int n,
			   get_next_task_fn get_next_task,
			   start_failure_fn start_failure,
			   task_finished_fn task_finished,
			   void *pp_cb)
{
	int i, code;
	int output_timeout = 100;
	int spawn_cap = 4;
	struct parallel_processes pp;

	pp_init(&pp, n, get_next_task, start_failure, task_finished, pp_cb);
	while (1) {
		for (i = 0;
		    i < spawn_cap && !pp.shutdown &&
		    pp.nr_processes < pp.max_processes;
		    i++) {
			code = pp_start_one(&pp);
			if (!code)
				continue;
			if (code < 0) {
				pp.shutdown = 1;
				kill_children(&pp, -code);
			}
			break;
		}
		if (!pp.nr_processes)
			break;
		pp_buffer_stderr(&pp, output_timeout);
		pp_output(&pp);
		code = pp_collect_finished(&pp);
		if (code) {
			pp.shutdown = 1;
			if (code < 0)
				kill_children(&pp, -code);
		}
	}

	pp_cleanup(&pp);
	return 0;
}

static int number_callbacks;
static int parallel_next(struct child_process *cp,
			 struct strbuf *err,
			 void *cb,
			 void **task_cb)
{
	struct child_process *d = cb;
	if (number_callbacks >= 4)
		return 0;

	argv_array_pushv(&cp->args, d->argv);
	strbuf_addf(err, "preloaded output of a child\n");
	number_callbacks++;
	return 1;
}

static int no_job(struct child_process *cp,
		  struct strbuf *err,
		  void *cb,
		  void **task_cb)
{
	strbuf_addf(err, "no further jobs available\n");
	return 0;
}

static int task_finished(int result,
			 struct child_process *cp,
			 struct strbuf *err,
			 void *pp_cb,
			 void *pp_task_cb)
{
	strbuf_addf(err, "asking for a quick stop\n");
	return 1;
}

int main(int argc, char **argv)
{
	struct child_process proc = CHILD_PROCESS_INIT;
	int jobs;

	if (argc < 3)
		return 1;
	proc.argv = (const char **)argv + 2;

	if (!strcmp(argv[1], "start-command-ENOENT")) {
		if (start_command(&proc) < 0 && errno == ENOENT)
			return 0;
		fprintf(stderr, "FAIL %s\n", argv[1]);
		return 1;
	}
	if (!strcmp(argv[1], "run-command"))
		exit(run_command(&proc));

	jobs = atoi(argv[2]);
	proc.argv = (const char **)argv + 3;

	if (!strcmp(argv[1], "run-command-parallel"))
		exit(run_processes_parallel(jobs, parallel_next,
					    NULL, NULL, &proc));

	if (!strcmp(argv[1], "run-command-abort"))
		exit(run_processes_parallel(jobs, parallel_next,
					    NULL, task_finished, &proc));

	if (!strcmp(argv[1], "run-command-no-jobs"))
		exit(run_processes_parallel(jobs, no_job,
					    NULL, task_finished, &proc));

	fprintf(stderr, "check usage\n");
	return 1;
}

