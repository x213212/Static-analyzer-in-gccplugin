/*
    commit id : ec6eda16ebab771aa3dfc90629b41953b999cb1e
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../openssh.h"

#define crypto_sign_ed25519_SECRETKEYBYTES 64U
#define crypto_sign_ed25519_PUBLICKEYBYTES 32U
#define crypto_sign_ed25519_BYTES 64U

typedef void EVP_MD;
typedef void BN_GENCB;
struct sshkey *
sshkey_new(int type)
{
	struct sshkey *k;
	DSA *dsa;

	if ((k = calloc(1, sizeof(*k))) == NULL)
		return NULL;
	k->type = type;
	k->ecdsa = NULL;
	k->ecdsa_nid = -1;
	k->dsa = NULL;
	k->rsa = NULL;
	k->cert = NULL;
	k->ed25519_sk = NULL;
	k->ed25519_pk = NULL;
	switch (k->type) {
	case KEY_DSA:
	case KEY_DSA_CERT:
		if ((dsa = malloc(sizeof(DSA))) == NULL)
			return NULL;
		k->dsa = dsa;
		break;
	case KEY_UNSPEC:
		break;
	default:
		free(k);
		return NULL;
		break;
	}
	return k;
}

void
sshkey_free(struct sshkey *k)
{
	if (k == NULL)
		return;
	switch (k->type) {
    case KEY_DSA:
	case KEY_DSA_CERT:
		if (k->dsa != NULL)
			free(k->dsa);
		k->dsa = NULL;
		break;
	default:
		break;
	}
	free(k);
}

int dsa_builtin_paramgen(DSA *ret)
{
    int ok = 0;

	__USE(ret); 
    if (ret->p == NULL || ret->q == NULL || ret->g == NULL) {
		ok = __RANDBOOL;
	}
	return ok;
}

int DSA_generate_parameters_ex(DSA *ret)
{
    const EVP_MD *evpmd;
    size_t qbits = 8;

    return dsa_builtin_paramgen(ret);
    
}

static int
dsa_generate_private_key(u_int bits, DSA **dsap)
{
	DSA *private;
	int ret = SSH_ERR_INTERNAL_ERROR;

	if (dsap == NULL)
		return SSH_ERR_INVALID_ARGUMENT;
	if ((private = malloc(sizeof(DSA))) == NULL) { /* allocation site */
		ret = SSH_ERR_ALLOC_FAIL;
		goto out;
	}
	*dsap = NULL;
	if (!DSA_generate_parameters_ex(private)) {
        free(private); /* double-free */
        ret = SSH_ERR_LIBCRYPTO_ERROR;
		goto out;
	}
	*dsap = private;
	private = NULL;
	ret = 0;
 out:
	if (private != NULL)
		free(private); /* double-free */
	return ret;
}

int
sshkey_generate(int type, u_int bits, struct sshkey **keyp)
{
	struct sshkey *k;
	int ret = SSH_ERR_INTERNAL_ERROR;

	if (keyp == NULL)
		return SSH_ERR_INVALID_ARGUMENT;
	*keyp = NULL;
	if ((k = sshkey_new(KEY_UNSPEC)) == NULL)
		return SSH_ERR_ALLOC_FAIL;
	switch (type) {
	case KEY_DSA:
		ret = dsa_generate_private_key(bits, &k->dsa);
		break;
	default:
		ret = SSH_ERR_INVALID_ARGUMENT;
	}
	if (ret == 0) {
		k->type = type;
		*keyp = k;
	} else
		sshkey_free(k);
	return ret;
}

int main() {
    int type;
    u_int bits;
    struct sshkey *private, public;
	srand(time(NULL));
    if(sshkey_generate(KEY_DSA, bits, &private))
		fatal("sshkey_generate error%s\n", "");
	private->dsa->flags = 0;
	sshkey_free(private);
    return 0;
}

