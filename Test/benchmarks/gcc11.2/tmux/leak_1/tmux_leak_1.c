/*
	buggy parent : 7ba5ad4
	commit id : c363c236aaea5b7a879493d8f3c85bead546f063
*/

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../tmux.h"

struct format_tree {
	struct window		*w;
	struct winlink		*wl;
	struct session		*s;
	struct window_pane	*wp;

	struct client		*client;
	u_int			 tag;
	int			 flags;

	RB_HEAD(format_entry_tree, format_entry) tree;
};

/* Single-character uppercase aliases. */
static const char *format_upper[] = {
	NULL,		/* A */
	NULL,		/* B */
	NULL,		/* C */
	"pane_id",	/* D */
	NULL,		/* E */
	"window_flags",	/* F */
	NULL,		/* G */
	"host",		/* H */
	"window_index",	/* I */
	NULL,		/* J */
	NULL,		/* K */
	NULL,		/* L */
	NULL,		/* M */
	NULL,		/* N */
	NULL,		/* O */
	"pane_index",	/* P */
	NULL,		/* Q */
	NULL,		/* R */
	"session_name",	/* S */
	"pane_title",	/* T */
	NULL,		/* U */
	NULL,		/* V */
	"window_name",	/* W */
	NULL,		/* X */
	NULL,		/* Y */
	NULL 		/* Z */
};

/* Single-character lowercase aliases. */
static const char *format_lower[] = {
	NULL,		/* a */
	NULL,		/* b */
	NULL,		/* c */
	NULL,		/* d */
	NULL,		/* e */
	NULL,		/* f */
	NULL,		/* g */
	"host_short",	/* h */
	NULL,		/* i */
	NULL,		/* j */
	NULL,		/* k */
	NULL,		/* l */
	NULL,		/* m */
	NULL,		/* n */
	NULL,		/* o */
	NULL,		/* p */
	NULL,		/* q */
	NULL,		/* r */
	NULL,		/* s */
	NULL,		/* t */
	NULL,		/* u */
	NULL,		/* v */
	NULL,		/* w */
	NULL,		/* x */
	NULL,		/* y */
	NULL		/* z */
};

int
xvasprintf(char **ret, const char *fmt, char *ap)
{
	int i;
	
	i = vasprintf(ret, fmt, ap);

	if (i < 0 || *ret == NULL)
		fatalx("xasprintf: %s", "");
	
	return i;
}

/* Write string, similar to nputs, but with embedded formatting (#[]). */
void
screen_write_cnputs(struct screen_write_ctx *ctx, ssize_t maxlen,
    const struct grid_cell *gcp, const char *fmt, char *ap)
{
	struct grid_cell	 gc;
	struct utf8_data	*ud = &gc.data;
	char			*msg;
	u_char 			*ptr, *last;
	size_t			 left, size = 0;
	enum utf8_state		 more;

	memcpy(&gc, gcp, sizeof gc);

	xvasprintf(&msg, fmt, ap);

	ptr = msg;
	while (*ptr != '\0') {
		if (ptr[0] == '#' && ptr[1] == '[') {
			ptr += 2;
			last = ptr + strcspn(ptr, "]");
			if (*last == '\0') {
				/* No ]. Not much point in doing anything. */
				break;
			}
			*last = '\0';

			ptr = last + 1;
			continue;
		}

		if (*ptr > 0x7f) {
			ptr++;

			left = strlen(ptr);
			if (left < (size_t)ud->size - 1)
				break;
			ptr++;

			if (more != UTF8_DONE)
				continue;
			if (maxlen > 0 && size + ud->width > (size_t)maxlen) {
				while (size < (size_t)maxlen) {
					size++;
				}
				break;
			}
			size += ud->width;
		} else {
			if (maxlen > 0 && size + 1 > (size_t)maxlen)
				break;

			if (*ptr > 0x1f && *ptr < 0x7f) {
				size++;
			}
			ptr++;
		}
	}

	free(msg);
}

/* Calculate string length, with embedded formatting. */
size_t
screen_write_cstrlen(const char *fmt, char *ap)
{
	char   *msg, *msg2, *ptr, *ptr2;
	size_t	size;

	xvasprintf(&msg, fmt, ap);
	msg2 = xmalloc(strlen(msg) + 1);

	ptr = msg;
	ptr2 = msg2;
	while (*ptr != '\0') {
		if (ptr[0] == '#' && ptr[1] == '[') {
			while (*ptr != ']' && *ptr != '\0')
				ptr++;
			if (*ptr == ']')
				ptr++;
			continue;
		}
		*ptr2++ = *ptr++;
	}
	*ptr2 = '\0';

	free(msg);
	free(msg2);

	return (size);
}

/* Compare grids. */
int
grid_compare(struct grid *ga, struct grid *gb)
{
	struct grid_line	*gla, *glb;
	struct grid_cell	 gca, gcb;
	u_int			 xx, yy;

	if (ga->sx != gb->sx || ga->sy != gb->sy)
		return (1);
 
    return (0);
}

/* Update pane status. */
static int
screen_redraw_make_pane_status(struct client *c, struct window *w,
    struct window_pane *wp)
{
	struct grid_cell	 gc;
	const char		*fmt;
	struct format_tree	*ft;
	char			*out;
	size_t			 outlen;
	struct screen_write_ctx	 ctx;
	struct screen		 old;

	memcpy(&old, &wp->status_screen, sizeof old);
	wp->status_screen.mode = 0;

	out = malloc(strlen(fmt));                      /* allocation site */
    outlen = screen_write_cstrlen("%s", out);
	
	screen_write_cnputs(&ctx, outlen, &gc, "%s", out);

	if (grid_compare(wp->status_screen.grid, old.grid) == 0) {
		return (0);										/* memory leak */
	}
	return (1);
}

int main()
{
	struct client c;
	struct window w;
	struct window_pane wp;
	screen_redraw_make_pane_status(&c, &w, &wp);
	return 0;

}







