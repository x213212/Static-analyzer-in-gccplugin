/*
	buggy parent : 0cca17f
	commit id : e52a260f16888ca75390f97de4606943e61785e8
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../openssh.h"

typedef struct identity Identity;
struct identity {
	TAILQ_ENTRY(identity) next;
	AuthenticationConnection *ac;	/* set if agent supports key */
	Key	*key;			/* public/private key */
	char	*filename;		/* comment for agent-only keys */
	int	tried;
	int	isprivate;		/* key points to the private key */
	int	userprovided;
};

#define RSA_new() malloc(sizeof(RSA))
#define BN_new() malloc(sizeof(BIGNUM))
#define DSA_new() malloc(sizeof(BIGNUM))
Key *
key_new(int type)
{
	Key *k;
	RSA *rsa;
	DSA *dsa;
	k = xcalloc(1, sizeof(*k));				/* allocation site */
	k->type = type;
	k->ecdsa = NULL;
	k->ecdsa_nid = -1;
	k->dsa = NULL;
	k->rsa = NULL;
	k->cert = NULL;
	switch (k->type) {
	case KEY_UNSPEC:
		break;
	default:
		fatal("key_new: bad key type %d", k->type);
		break;
	}

	return k;
}


void
key_free(Key *k)
{
	if (k == NULL)
		fatal("key_free: key is NULL%s", "");
	switch (k->type) {
	case KEY_RSA1:
	case KEY_RSA:
	case KEY_RSA_CERT_V00:
	case KEY_RSA_CERT:
		if (k->rsa != NULL)
			free(k->rsa);
		k->rsa = NULL;
		break;
	case KEY_DSA:
	case KEY_DSA_CERT_V00:
	case KEY_DSA_CERT:
		if (k->dsa != NULL)
			free(k->dsa);
		k->dsa = NULL;
		break;
	case KEY_UNSPEC:
		break;
	default:
		fatal("key_free: bad key type %d", k->type);
		break;
	}
	free(k);
}

static Key *
key_parse_private_pem(Buffer *blob, int type, const char *passphrase,
    char **commentp)
{
	Key *prv = NULL;

	if (__RANDBOOL) {
		printf("%s: PEM_read_PrivateKey failed\n", __func__);
	} else if (__RANDBOOL && (type == KEY_UNSPEC || type==KEY_RSA)) {
		prv = key_new(KEY_UNSPEC);
		prv->rsa = RSA_new();	/* allocation site */ 
		prv->type = KEY_RSA;
		if (rand() % 5 == 0) {
			printf("%s: RSA_blinding_on failed\n", __func__);
			key_free(prv);
			prv = NULL;
		}
	} else if (__RANDBOOL && (type == KEY_UNSPEC || type==KEY_DSA)) {
		prv = key_new(KEY_UNSPEC);
		prv->dsa = DSA_new();	/* allocation site */
		prv->type = KEY_DSA;
	} else {
		printf("%s: PEM_read_PrivateKey: mismatch or "
		    "unknown EVP_PKEY save_type\n");
	}
	return prv;
}

static Key *
key_parse_private_type(Buffer *blob, int type, const char *passphrase,
    char **commentp)
{
	switch (type) {
	case KEY_RSA1:
		return key_parse_private_rsa1(blob, passphrase, commentp);
	case KEY_DSA:
	case KEY_ECDSA:
	case KEY_RSA:
	case KEY_UNSPEC:
		return key_parse_private_pem(blob, type, passphrase, commentp);
	default:
		printf("%s: cannot parse key type %d\n", __func__, type);
		break;
	}
	return NULL;
}
Key *
key_load_private_type(int type, const char *filename, const char *passphrase,
    char **commentp, int *perm_ok)
{
	int fd;
	Key *ret;
	
	if (__RANDBOOL)
		*perm_ok = 1;

	ret = key_parse_private_type(NULL, type, passphrase, commentp);
	return ret;
}

static Key *
load_identity_file(char *filename, int userprovided)
{
	Key *private;
	char *passphrase;
	int perm_ok = 0;
	int i, quit;

	private = key_load_private_type(KEY_UNSPEC, filename, "", NULL, &perm_ok);
	if (!perm_ok) {
		return NULL;	/* memory-leak */
	}	
	if (private == NULL) {
		if (__RANDBOOL)
			return NULL;
		for (i = 0; i < 100; i++) {
			if (strcmp(passphrase, "") != 0) {
				private = key_load_private_type(KEY_UNSPEC,
				    filename, passphrase, NULL, NULL);
				quit = 0;
			} else {
				debug2("no passphrase given, try next key");
				quit = 1;
			}
			if (private != NULL || quit)
				break;
		}
	}

	return private;
}

int main(int argc, char **argv)
{
	Identity *id = malloc(sizeof(Identity));
	srand(time(NULL));
	id->filename = argv[0];
	id->key = load_identity_file(id->filename, id->userprovided);
	__USE(id->key->rsa);
	if(id->key != NULL)	
		key_free(id->key);
	free(id);
	return 0;
}


