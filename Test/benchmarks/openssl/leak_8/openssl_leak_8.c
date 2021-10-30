/*
	commit id : 9561e2a169f499f8346ffdd7541bc4e3d81d6711
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../openssl.h"

void SSL_SESSION_free(SSL_SESSION *ss)
{
	OPENSSL_free(ss);
}

typedef int (*SSL_psk_use_session_cb_func)(SSL *ssl, const EVP_MD *md,
                                           const unsigned char **id,
                                           size_t *idlen,
                                           SSL_SESSION **sess);

struct ssl_ctx_st {
    SSL_psk_use_session_cb_func psk_use_session_cb;
};
struct ssl_session_st {
    const SSL_CIPHER *cipher;
};
static SSL_SESSION *psksess = NULL;
static char *psk_identity = "Client_identity";
static int psk_key;
const unsigned char tls13_aes128gcmsha256_id[] = { 0x13, 0x01 };
const unsigned char tls13_aes256gcmsha384_id[] = { 0x13, 0x02 };

static int psk_use_session_cb(SSL *s, const EVP_MD *md,
                              const unsigned char **id, size_t *idlen,
                              SSL_SESSION **sess)
{
    SSL_SESSION *usesess = NULL;
    const SSL_CIPHER *cipher = NULL;

    if (psksess != NULL) {
        SSL_SESSION_up_ref(psksess);
        usesess = psksess;
    } else {
        long key_len;
        unsigned char *key = OPENSSL_hexstr2buf(psk_key, &key_len);

        if (key == NULL) {
            return 0;
        }

        if (key_len == EVP_MD_size(EVP_sha256()))
            cipher = SSL_CIPHER_find(s, tls13_aes128gcmsha256_id);
        else if(key_len == EVP_MD_size(EVP_sha384()))
            cipher = SSL_CIPHER_find(s, tls13_aes256gcmsha384_id);

        if (cipher == NULL) {
            /* Doesn't look like a suitable TLSv1.3 key. Ignore it */
            OPENSSL_free(key);
            *id = NULL;
            *idlen = 0;
            *sess = NULL;
            return 0;
        }
        usesess = OPENSSL_malloc(sizeof(*usesess));
        if (usesess == NULL
                || !SSL_SESSION_set1_master_key(usesess, key, key_len)
                || !SSL_SESSION_set_cipher(usesess, cipher))
            goto err;
        }

    cipher = SSL_SESSION_get0_cipher(usesess);
    if (cipher == NULL)
        goto err;

    if (md != NULL && SSL_CIPHER_get_handshake_digest(cipher) != md) {
        /* PSK not usable, ignore it */
        *id = NULL;
        *idlen = 0;
        *sess = NULL;
        SSL_SESSION_free(usesess);
    } else {
        *sess = usesess;
        *id = (unsigned char *)psk_identity;
        *idlen = strlen(psk_identity);
    }

    return 1;

 err:
    SSL_SESSION_free(usesess);
    return 0;
}

void SSL_CTX_set_psk_use_session_callback(SSL_CTX *ctx,
                                           SSL_psk_use_session_cb_func cb)
{
    ctx->psk_use_session_cb = cb;
}

int main(int argc, char **argv)
{
	char *psksessf;
	SSL_CTX ctx;
	SSL_SESSION *sess;
	if (psksessf != NULL)
	{
		psksess = malloc(sizeof(SSL_SESSION)); /* allocation site */
		if(psksess == NULL)
			goto end;
	}


    if (psksess != NULL)
        SSL_CTX_set_psk_use_session_callback(&ctx, psk_use_session_cb);
	ctx.psk_use_session_cb(NULL, NULL, NULL, NULL, &sess);
	__USE(sess);
end:
	/* memory leak */
	return 0;
}
