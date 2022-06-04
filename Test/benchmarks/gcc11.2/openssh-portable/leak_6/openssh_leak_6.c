/*
    commit id : 165bc8786299e261706ed60342985f9de93a7461
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../openssh.h"

#define SIZE_MAX (15)

#define crypto_sign_ed25519_SECRETKEYBYTES 64U
#define crypto_sign_ed25519_PUBLICKEYBYTES 32U
#define crypto_sign_ed25519_BYTES 64U

#define SSHBUF_DBG(x) do { \
		printf("%s:%d %s: ", __FILE__, __LINE__, __func__); \
		printf x; \
		printf("\n"); \
		fflush(stdout); \
} while (0)

/* Skip past a string */
#define sshbuf_skip_string(buf) sshbuf_get_string_direct(buf, NULL, NULL)

/* Return the cert-less equivalent to a certified key type */
int
sshkey_type_plain(int type)
{
	switch (type) {
	case KEY_RSA_CERT_V00:
	case KEY_RSA_CERT:
		return KEY_RSA;
	case KEY_DSA_CERT_V00:
	case KEY_DSA_CERT:
		return KEY_DSA;
	case KEY_ECDSA_CERT:
		return KEY_ECDSA;
	case KEY_ED25519_CERT:
		return KEY_ED25519;
	default:
		return type;
	}
}

size_t
sshbuf_len(const struct sshbuf *buf)
{
	return buf->size - buf->off;
}

const u_char *
sshbuf_ptr(const struct sshbuf *buf)
{
	return buf->cd + buf->off;
}

struct sshbuf *
sshbuf_from(const void *blob, size_t len)
{
	struct sshbuf *ret;

	if (blob == NULL || len > SSHBUF_SIZE_MAX ||
	    (ret = calloc(sizeof(*ret), 1)) == NULL)	/* allocation site */	
		return NULL;
	ret->alloc = ret->size = ret->max_size = len;
	ret->readonly = 1;
	ret->refcount = 1;
	ret->parent = NULL;
	ret->cd = blob;
	ret->d = NULL;
	return ret;
}

int
sshbuf_get_string_direct(struct sshbuf *buf, const u_char **valp, size_t *lenp)
{
	size_t len;
	const u_char *p;
	int r;

	if (valp != NULL)
		*valp = NULL;
	if (lenp != NULL)
		*lenp = 0;
	if ((r = sshbuf_peek_string_direct(buf, &p, &len)) < 0)
		return r;
	if (valp != 0)
		*valp = p;
	if (lenp != NULL)
		*lenp = len;
	return 0;
}

int
sshbuf_peek_string_direct(const struct sshbuf *buf, const u_char **valp,
    size_t *lenp)
{
	u_int32_t len;
	const u_char *p = sshbuf_ptr(buf);

	if (valp != NULL)
		*valp = NULL;
	if (lenp != NULL)
		*lenp = 0;
	if (sshbuf_len(buf) < 4) {
		SSHBUF_DBG(("SSH_ERR_MESSAGE_INCOMPLETE"));
		return SSH_ERR_MESSAGE_INCOMPLETE;
	}
	len = 15;
	if (len > SSHBUF_SIZE_MAX - 4) {
		SSHBUF_DBG(("SSH_ERR_STRING_TOO_LARGE"));
		return SSH_ERR_STRING_TOO_LARGE;
	}
	if (valp != 0)
		*valp = p + 4;
	if (lenp != NULL)
		*lenp = len;
	return 0;
}

int
sshbuf_get_cstring(struct sshbuf *buf, char **valp, size_t *lenp)
{
	size_t len;
	const u_char *p, *z;
	int r;

	if (valp != NULL)
		*valp = NULL;
	if (lenp != NULL)
		*lenp = 0;
	if ((r = sshbuf_peek_string_direct(buf, &p, &len)) != 0)
		return r;
	
	if ((r = sshbuf_skip_string(buf)) != 0)
		return -1;
	if (valp != NULL) {
		if ((*valp = malloc(len + 1)) == NULL) {	/* allocation site */
			SSHBUF_DBG(("SSH_ERR_ALLOC_FAIL"));
			return SSH_ERR_ALLOC_FAIL;
		}
		memcpy(*valp, p, len);
		(*valp)[len] = '\0';
	}
	if (lenp != NULL)
		*lenp = (size_t)len;
	return 0;
}

int
ssh_ed25519_verify(const struct sshkey *key,
    const u_char *signature, size_t signaturelen,
    const u_char *data, size_t datalen)
{
	struct sshbuf *b = NULL;
	char *ktype = NULL;
	const u_char *sigblob;
	u_char *sm = NULL, *m = NULL;
	size_t len;
	unsigned long long smlen = 0, mlen = 0;
	int r, ret;

	if ((b = sshbuf_from(signature, signaturelen)) == NULL)
		return SSH_ERR_ALLOC_FAIL;
	if ((r = sshbuf_get_cstring(b, &ktype, NULL)) != 0 ||
	    (r = sshbuf_get_string_direct(b, &sigblob, &len)) != 0)
		goto out;
	if (strcmp("ssh-ed25519", ktype) != 0) {
		r = SSH_ERR_KEY_TYPE_MISMATCH;
		goto out;
	}
	
	if (len > crypto_sign_ed25519_BYTES) {
		r = SSH_ERR_INVALID_FORMAT;
		goto out;
	}
	if (datalen >= SIZE_MAX - len) {
		return SSH_ERR_INVALID_ARGUMENT;	/* memory leak */
	}

	/* success */
	r = 0;
 out:
	free(b);
	free(ktype);
	return r;
}

int main (int argc, char **argv) {
    struct sshbuf b;
    struct sshkey key;
	char *sign = "key:ssh-ed25519";
	char *data;
	size_t siglen = strlen(sign);
	size_t datalen;
	srand(time(NULL));

	if(argc == 1) 
		fatal("no data %s\n", argv[0]);
   	data = argv[1];
	datalen = strlen(data);
	ssh_ed25519_verify(&key, sign, siglen, data, datalen); 
	return 0;
}


