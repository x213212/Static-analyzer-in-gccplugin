/*
	buggy parent : a2c86dc
	commit id : 842bc2b
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WINDOW
#define SESSION
#define SESSION_GROUP
#include "../stdio.h"
#include "../tmux.h"

/* Window structure. */
struct window {
	char		*name;
	struct event	 name_timer;

	struct window_pane *active;
	struct window_panes panes;

	int		 lastlayout;
	struct layout_cell *layout_root;

	u_int		 sx;
	u_int		 sy;

	int		 flags;
#define WINDOW_BELL 0x1
#define WINDOW_HIDDEN 0x2
#define WINDOW_ACTIVITY 0x4
#define WINDOW_CONTENT 0x8
#define WINDOW_REDRAW 0x10

	struct options	 options;

	u_int		 references;
};
ARRAY_DECL(windows, struct window *);

struct session_group {
	TAILQ_HEAD(, session) sessions;

	TAILQ_ENTRY(session_group) entry;
};
TAILQ_HEAD(session_groups, session_group);

struct session {
	char		*name;

	u_int		 sx;
	u_int		 sy;

	struct winlink	*curw;
	struct winlink_stack lastw;
	struct winlinks	 windows;

	struct paste_stack buffers;

#define SESSION_UNATTACHED 0x1	/* not attached to any clients */
#define SESSION_DEAD 0x2
	int		 flags;

	struct termios	*tio;

	int		 references;

	TAILQ_ENTRY(session) gentry;
};
ARRAY_DECL(sessions, struct session *);


struct session_groups session_groups;
struct sessions sessions;
struct windows windows;


int
winlink_cmp(struct winlink *wl1, struct winlink *wl2)
{
	return (wl1->idx - wl2->idx);
}

RB_GENERATE(winlinks, winlink, entry, winlink_cmp);

#define SIZE_MAX (1 << 30)
void *
xrealloc(void *oldptr, size_t nmemb, size_t size)
{
	size_t	 newsize = nmemb * size;
	void	*newptr;

	if (newsize == 0)
		fatalx("zero size%s", "");
	if (SIZE_MAX / nmemb < size)
		fatalx("nmemb * size > SIZE_MAX%s", "");
	if ((newptr = realloc(oldptr, newsize)) == NULL)
		fatal("xrealloc failed%s", "");

	return (newptr);
}

struct winlink *
winlink_find_by_index(struct winlinks *wwl, int idx)
{
	struct winlink	wl;

	if (idx < 0)
		fatalx("bad index%s", "");

	wl.idx = idx;
	return (RB_FIND(winlinks, wwl, &wl));
}

u_int winlink_count(struct winlinks *wwl)
{
	struct winlink *wl;
	u_int n;
	
	n = 0;
	RB_FOREACH(wl, winlinks, wwl)
		n++;

	return (n);
}

int
winlink_next_index(struct winlinks *wwl, int idx)
{
	int	i;

	i = idx;
	do {
		if (winlink_find_by_index(wwl, i) == NULL)
			return (i);
		if (i == INT_MAX)
			i = 0;
		else
			i++;
	} while (i != idx);
	return (-1);
}


struct winlink *
winlink_find_by_window(struct winlinks *wwl, struct window *w)
{
	struct winlink	*wl;

	RB_FOREACH(wl, winlinks, wwl) {
		if (wl->window == w)
			return (wl);
	}

	return (NULL);
}

struct winlink *cmd_find_pane(int arg, struct session **sp)
{
	struct session *s;
	struct winlink *wl;
	u_int 		idx;
	
	if(ARRAY_EMPTY(&sessions)) {
		printf("can't establish current session\n");
		return NULL;
	}
	s = ARRAY_ITEM(&sessions, 1);
	if(sp != NULL)
		*sp = s;
	if(arg)
		return s->curw;
	return RB_NEXT(winlinks, &s->windows, s->curw);
}

/* Return if session has window. */
int
session_has(struct session *s, struct window *w)
{
	struct winlink	*wl;

	RB_FOREACH(wl, winlinks, &s->windows) {
		if (wl->window == w)
			return (1);
	}
	return (0);
}

/* Find the session group containing a session. */
struct session_group *
session_group_find(struct session *target)
{
	struct session_group	*sg;
	struct session		*s;

	TAILQ_FOREACH(sg, &session_groups, entry) {
		TAILQ_FOREACH(s, &sg->sessions, gentry) {
			if (s == target)
				return (sg);
		}
	}
	return (NULL);
}

/*
 * Add a session to the session group containing target, creating it if
 * necessary.
 */
void
session_group_add(struct session *target, struct session *s)
{
	struct session_group	*sg;

	if ((sg = session_group_find(target)) == NULL) {
		sg = xmalloc(sizeof *sg);
		TAILQ_INSERT_TAIL(&session_groups, sg, entry);
		TAILQ_INIT(&sg->sessions);
		TAILQ_INSERT_TAIL(&sg->sessions, target, gentry);
	}
	TAILQ_INSERT_TAIL(&sg->sessions, s, gentry);
}

struct winlink *
winlink_add(struct winlinks *wwl, struct window *w, int idx)
{
	struct winlink	*wl;

	if (idx < 0) {
		if ((idx = winlink_next_index(wwl, -idx - 1)) == -1)
			return (NULL);
	} else if (winlink_find_by_index(wwl, idx) != NULL)
		return (NULL);

	wl = xcalloc(1, sizeof *wl);	/* allocation site */
	wl->idx = idx;
	wl->window = w;
	RB_INSERT(winlinks, wwl, wl);

	w->references++;

	return (wl);
}

int
window_index(struct window *s, u_int *i)
{
	for (*i = 0; *i < ARRAY_LENGTH(&windows); (*i)++) {
		if (s == ARRAY_ITEM(&windows, *i))
			return (0);
	}
	return (-1);
}

/* Remove a session from its group and destroy the group if empty. */
void
session_group_remove(struct session *s)
{
	struct session_group	*sg;

	if ((sg = session_group_find(s)) == NULL)
		return;
	TAILQ_REMOVE(&sg->sessions, s, gentry);
	if (TAILQ_NEXT(TAILQ_FIRST(&sg->sessions), gentry) == NULL)
		TAILQ_REMOVE(&sg->sessions, TAILQ_FIRST(&sg->sessions), gentry);
	if (TAILQ_EMPTY(&sg->sessions)) {
		TAILQ_REMOVE(&session_groups, sg, entry);
		xfree(sg);
	}
}

/* Find session by name. */
struct session *
session_find(const char *name)
{
	struct session	*s;
	u_int		 i;

	for (i = 0; i < ARRAY_LENGTH(&sessions); i++) {
		s = ARRAY_ITEM(&sessions, i);
		if (s != NULL && strcmp(s->name, name) == 0)
			return (s);
	}

	return (NULL);
}


/* Find session index. */
int
session_index(struct session *s, u_int *i)
{
	for (*i = 0; *i < ARRAY_LENGTH(&sessions); (*i)++) {
		if (s == ARRAY_ITEM(&sessions, *i))
			return (0);
	}
	return (-1);
}

void
winlink_stack_remove(struct winlink_stack *stack, struct winlink *wl)
{
	struct winlink	*wl2;

	if (wl == NULL)
		return;

	TAILQ_FOREACH(wl2, stack, sentry) {
		if (wl2 == wl) {
			TAILQ_REMOVE(stack, wl, sentry);
			return;
		}
	}
}

void
winlink_stack_push(struct winlink_stack *stack, struct winlink *wl)
{
	if (wl == NULL)
		return;

	winlink_stack_remove(stack, wl);
	TAILQ_INSERT_HEAD(stack, wl, sentry);
}

void
window_destroy(struct window *w)
{
	u_int	i;

	if (window_index(w, &i) != 0)
		fatalx("index not found%s\n", "");
	ARRAY_SET(&windows, i, NULL);
	while (!ARRAY_EMPTY(&windows) && ARRAY_LAST(&windows) == NULL)
		ARRAY_TRUNC(&windows, 1);

	if (w->name != NULL)
		xfree(w->name);
	xfree(w);
}

void
winlink_remove(struct winlinks *wwl, struct winlink *wl)
{
	struct window	*w = wl->window;

	RB_REMOVE(winlinks, wwl, wl);
	if (wl->status_text != NULL)
		xfree(wl->status_text);
	xfree(wl);

	if (w->references == 0)
		fatal("bad reference count%s\n", "");
	w->references--;
	if (w->references == 0)
		window_destroy(w);
}

/*
 * Synchronize a session with a target session. This means destroying all
 * winlinks then recreating them, then updating the current window, last window
 * stack and alerts.
 */
void
session_group_synchronize1(struct session *target, struct session *s)
{
	struct winlinks		 old_windows, *ww;
	struct winlink_stack	 old_lastw;
	struct winlink		*wl, *wl2;
	struct session_alert	*sa;

	/* Don't do anything if the session is empty (it'll be destroyed). */
	ww = &target->windows;
	if (RB_EMPTY(ww))
		return;

	/* Save the old pointer and reset it. */
	memcpy(&old_windows, &s->windows, sizeof old_windows);
	RB_INIT(&s->windows);

	/* Link all the windows from the target. */
	RB_FOREACH(wl, winlinks, ww)
		winlink_add(&s->windows, wl->window, wl->idx);

	/* Fix up the last window stack. */
	memcpy(&old_lastw, &s->lastw, sizeof old_lastw);
	TAILQ_INIT(&s->lastw);
	TAILQ_FOREACH(wl, &old_lastw, sentry) {
		wl2 = winlink_find_by_index(&s->windows, wl->idx);
		if (wl2 != NULL)
			TAILQ_INSERT_TAIL(&s->lastw, wl2, sentry);
	}
	s->curw = TAILQ_FIRST(&s->lastw);

	/* Then free the old winlinks list. */
	while (!RB_EMPTY(&old_windows)) {
		wl = RB_ROOT(&old_windows);
		winlink_remove(&old_windows, wl);
	}
}

/* Synchronize a session to its session group. */
void
session_group_synchronize_to(struct session *s)
{
	struct session_group	*sg;
	struct session		*target;

	if ((sg = session_group_find(s)) == NULL)
		return;

	target = NULL;
	TAILQ_FOREACH(target, &sg->sessions, gentry) {
		if (target != s)
			break;
	}
	session_group_synchronize1(target, s);
}

/* Synchronize a session group to a session. */
void
session_group_synchronize_from(struct session *target)
{
	struct session_group	*sg;
	struct session		*s;

	if ((sg = session_group_find(target)) == NULL)
		return;

	TAILQ_FOREACH(s, &sg->sessions, gentry) {
		if (s != target)
			session_group_synchronize1(target, s);
	}
}

/* Destroy a session. */
void
session_destroy(struct session *s)
{
	u_int	i;

	printf("session %s destroyed\n", s->name);

	if (session_index(s, &i) != 0)
		fatalx("session not found%s\n", "");
	ARRAY_SET(&sessions, i, NULL);
	while (!ARRAY_EMPTY(&sessions) && ARRAY_LAST(&sessions) == NULL)
		ARRAY_TRUNC(&sessions, 1);

	session_group_remove(s);

	while (!TAILQ_EMPTY(&s->lastw))
		winlink_stack_remove(&s->lastw, TAILQ_FIRST(&s->lastw));
	while (!RB_EMPTY(&s->windows))
		winlink_remove(&s->windows, RB_ROOT(&s->windows));

	xfree(s->name);
	xfree(s);
}

/* Detach a window from a session. */
int
session_detach(struct session *s, struct winlink *wl)
{
	winlink_stack_remove(&s->lastw, wl);
	winlink_remove(&s->windows, wl);
	session_group_synchronize_from(s);
	if (RB_EMPTY(&s->windows)) {
		session_destroy(s);
		return (1);
	}
	return (0);
}

void
server_destroy_session_group(struct session *s)
{
	struct session_group	*sg;

	if ((sg = session_group_find(s)) == NULL)
		return;
	else {
		TAILQ_REMOVE(&session_groups, sg, entry);
		xfree(sg);
	}
}

void
server_kill_window(struct window *w)
{
	struct session	*s;
	struct winlink	*wl;
	u_int		 i;

	for (i = 0; i < ARRAY_LENGTH(&sessions); i++) {
		s = ARRAY_ITEM(&sessions, i);
		if (s == NULL || !session_has(s, w))
			continue;
		while ((wl = winlink_find_by_window(&s->windows, w)) != NULL) {
			if (session_detach(s, wl)) {
				server_destroy_session_group(s);
				break;
			} 
		}
	}
}

/* Move session to specific window. */
int
session_select(struct session *s, int idx)
{
	struct winlink	*wl;

	wl = winlink_find_by_index(&s->windows, idx);
	if (wl == NULL)
		return (-1);
	if (wl == s->curw)
		return (1);
	winlink_stack_remove(&s->lastw, wl);
	winlink_stack_push(&s->lastw, s->curw);
	s->curw = wl;
	return (0);
}

int join_pane_exec(void)
{
	struct session			*dst_s;
	struct winlink			*src_wl, *dst_wl;
	struct window			*src_w, *dst_w;

	if ((dst_wl = cmd_find_pane(0, &dst_s)) == NULL)
		return (-1);
	dst_w = dst_wl->window;

	if ((src_wl = cmd_find_pane(1, NULL)) == NULL)
		return (-1);
	src_w = src_wl->window;

	if (src_w == dst_w) {
		printf("can't join a pane to its own window\n");
		return (-1);
	}
	server_kill_window(src_w);	/* dst_wl can be freed */
	
	session_select(dst_s, dst_wl->idx);	/* use-after-free*/

	return (0);
}

struct window *
window_create1(u_int sx, u_int sy)
{
	struct window	*w;
	u_int		 i;

	w = xmalloc(sizeof *w);
	w->name = NULL;
	w->flags = 0;

	w->active = NULL;

	w->lastlayout = -1;
	w->layout_root = NULL;

	w->sx = sx;
	w->sy = sy;

	for (i = 0; i < ARRAY_LENGTH(&windows); i++) {
		if (ARRAY_ITEM(&windows, i) == NULL) {
			ARRAY_SET(&windows, i, w);
			break;
		}
	}
	if (i == ARRAY_LENGTH(&windows))
		ARRAY_ADD(&windows, w);
	w->references = 0;

	return (w);
}

/* Attach a window to a session. */
struct winlink *
session_attach(struct session *s, struct window *w, int idx, char **cause)
{
	struct winlink	*wl;

	if ((wl = winlink_add(&s->windows, w, idx)) == NULL)
		printf("index in use: %d\n", idx);
	session_group_synchronize_from(s);
	return (wl);
}

/* Create a new window on a session. */
struct winlink *
session_new(struct session *s,
    const char *name, const char *cmd, const char *cwd, int idx, char **cause)
{
	struct window	*w;
	const char	*shell;
	
	u_int		 hlimit;

	w = window_create1(s->sx, s->sy);
	if (w == NULL) {
		return (NULL);
	}

	return (session_attach(s, w, idx, cause));
}

/* Create a new session. */
struct session *
session_create(const char *name, const char *cmd, const char *cwd,
    struct environ *env, struct termios *tio, int idx, u_int sx, u_int sy,
    char **cause)
{
	struct session	*s;
	u_int		 i;

	s = xmalloc(sizeof *s);
	s->references = 0;
	s->flags = 0;

	s->curw = NULL;
	TAILQ_INIT(&s->lastw);
	RB_INIT(&s->windows);

	s->tio = NULL;
	s->sx = sx;
	s->sy = sy;

	for (i = 0; i < ARRAY_LENGTH(&sessions); i++) {
		if (ARRAY_ITEM(&sessions, i) == NULL) {
			ARRAY_SET(&sessions, i, s);
			break;
		}
	}
	if (i == ARRAY_LENGTH(&sessions))
		ARRAY_ADD(&sessions, s);
	if(name == NULL)
		fatal("no name%s\n", "");
	s->name = xstrdup(name);

	if (cmd != NULL) {
		if (session_new(s, NULL, cmd, cwd, idx, cause) == NULL) {
			session_destroy(s);
			return (NULL);
		}
		session_select(s, RB_ROOT(&s->windows)->idx);
	}
	printf("session %s created\n", s->name);

	return (s);
}

int main(int argc, char **argv)
{
	struct session *s, *groupwith;
	struct window *w;
	char *target, *cmd, *name;
	char *cause;
	int idx = 0, sx, sy;

	ARRAY_INIT(&windows);
	ARRAY_INIT(&sessions);
	TAILQ_INIT(&session_groups);
	
	for(int i = 0; i < argc; i++)
	{
		if(argv[i][0] == 't' && i > 0)
			target = strdup(argv[i - 1]);
		else
			target = NULL;
		
		groupwith = session_find(target);
		
		if(target == NULL)
			cmd = strdup("cmd");
		else
			cmd = NULL;
		
		free(target);
		
		name = strdup(argv[i]);
		s = session_create(name, cmd, "cwd", NULL, NULL, idx++, sx, sy, &cause);
		w = window_create1(sx, sy);
		session_attach(s, w, idx++, &cause);
		free(cmd);
		if(groupwith != NULL) {
			session_group_add(groupwith, s);
			session_group_synchronize_to(s);
			session_select(s, RB_ROOT(&s->windows)->idx);
		}
	}

	join_pane_exec();
	while(!ARRAY_EMPTY(&sessions))
		session_destroy(ARRAY_ITEM(&sessions, 0));	

	return 0;
}
