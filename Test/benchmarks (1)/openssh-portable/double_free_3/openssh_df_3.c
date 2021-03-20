/*
    commit id : 2ec0342ed4f1fcf4d7b140f9d91bc97c8025221a
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../openssh.h"

/* Context for session open confirmation callback */
struct mux_session_confirm_ctx {
	char *cmd;
	char *term;
	char **env;
	u_int rid;
};

/*
 * Returns an arbitrary binary string from the buffer.  The string cannot
 * be longer than 256k.  The returned value points to memory allocated
 * with xmalloc; it is the responsibility of the calling function to free
 * the data.  If length_ptr is non-NULL, the length of the returned data
 * will be stored there.  A null character will be automatically appended
 * to the returned string, and is not counted in length.
 */
void *
buffer_get_string_ret(char *buffer, u_int *length_ptr)
{
	u_char *value;
	u_int len;

	len = strlen(buffer);
	/* Allocate space for the string.  Add one byte for a null character. */
	printf("len : %d\n", len);
	printf("buf : %s\n", buffer);
	value = xmalloc(len + 1);	/* allocation site */
	/* Get the string. */
	if (len > 10) {
		printf("buffer_get_string_ret: buffer_get failed\n");
		xfree(value);
		return (NULL);
	}
	/* Append a null character to make processing easier. */
	value[len] = '\0';
	/* Optionally return the length of the string. */
	if (length_ptr)
		*length_ptr = len;
	return (value);
}

static int
process_mux_new_session(int ac, char **av)
{
	struct mux_session_confirm_ctx *cctx;
	char *reserved, *cmd, *cp;
	u_int i, j, len, env_len, escape_char, window, packetmax;
	int new_fd[3];
	i = 0;

	/* Reply for SSHMUX_COMMAND_OPEN */
	cctx = xcalloc(1, sizeof(*cctx));
	
	cmd = reserved = NULL;
	if ((reserved = buffer_get_string_ret(av[i++], NULL)) == NULL ||
		(cmd = buffer_get_string_ret(av[i++], &len)) == NULL) {	
 malf:
		xfree(cctx);
		
		if (cmd != NULL)
			xfree(cmd);										/* double-free */
		if (reserved != NULL)
			xfree(reserved);
		printf("%s: malformed message\n", __func__);
		return -1;
	}
	xfree(reserved);
	reserved = NULL;

	env_len = 0;
	while (i < ac) {
#define MUX_MAX_ENV_VARS	4096
		if ((cp = buffer_get_string_ret(av[i++], &len)) == NULL) {
            xfree(cmd);										/* double-free */
            goto malf;
        }

		xfree(cp);
	}

	printf("%s channel request cmd \"%s\"\n", __func__, cmd);

	cctx->cmd = strdup(cmd);
	xfree(cmd);
	cmd = NULL;

	xfree(cctx->cmd);
	xfree(cctx);
	/* reply is deferred, sent by mux_session_confirm */
	return 0;
}

int main(int argc, char **argv) {
	if(argc == 1)
		fatal("no argument%s\n", "");
	process_mux_new_session(argc, argv);
    return 0;
}




