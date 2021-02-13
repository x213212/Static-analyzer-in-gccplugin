/*
    commit id : f1e44ea9d9a6d4c1a95a0024132e603bd1778c9c
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../openssh.h"

static int
to_blob(const Key *key, u_char **blobp, u_int *lenp, int force_plain)
{
	Buffer b;
	int len, type;

	if (key == NULL) {
		error("key_to_blob: key == NULL");
		return 0;
	}
	buffer_init(&b);
	type = force_plain ? key_type_plain(key->type) : key->type;
		len = buffer_len(&b);
	if (lenp != NULL)
		*lenp = len;
	
    if (blobp != NULL) {
		*blobp = xmalloc(len); /* allocation site */
		memcpy(*blobp, buffer_ptr(&b), len);
	}
	memset(buffer_ptr(&b), 0, len);
	buffer_free(&b);
	return len;
}



int
key_to_blob(const Key *key, u_char **blobp, u_int *lenp)
{
	return to_blob(key, blobp, lenp, 0);
}

void
buffer_append(Buffer *buffer, const void *data, u_int len)
{
	void *p;
	p = buffer_append_space(buffer, len);
	memcpy(p, data, len);
}

/*
 * Stores and arbitrary binary string in the buffer.
 */
void
buffer_put_string(Buffer *buffer, const void *buf, u_int len)
{
	buffer_put_int(buffer, len);
	buffer_append(buffer, buf, len);
}

static void
process_add(void)
{
	char *name, *pin;
	Key **keys;
	int i, nkeys;
	u_char *blob;
	u_int blen;
	Buffer msg;

	buffer_init(&msg);
	name = get_string(NULL);
	pin = get_string(NULL);
	if ((nkeys = pkcs11_add_provider(name, pin, &keys)) > 0) {
		buffer_put_char(&msg, SSH2_AGENT_IDENTITIES_ANSWER);
		buffer_put_int(&msg, nkeys);
		for (i = 0; i < nkeys; i++) {
			key_to_blob(keys[i], &blob, &blen); 
			buffer_put_string(&msg, blob, blen); /* use-after-free when key_to_blob failed */ 
			buffer_put_cstring(&msg, name);
            *blob = 1;
			free(blob);
			add_key(keys[i], name);
		}
		free(keys);
	} else {
		buffer_put_char(&msg, SSH_AGENT_FAILURE);
	}
	free(pin);
	free(name);
	send_msg(&msg);
	buffer_free(&msg);
}


int main () {
    process_add();
    return 0;
}


