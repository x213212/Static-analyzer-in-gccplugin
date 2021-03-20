/*
    buggy parent: cb2a6eb 
    commit id: 26fb4b033c664df7e220db1960f1cdad73bbba06
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../openssl.h"

# define SSL_SESSION_ASN1_VERSION 0x0001
# define SSL_MAX_SSL_SESSION_ID_LENGTH           32
# define SSL_MAX_SID_CTX_LENGTH                  32
# define SSL_MIN_RSA_MODULUS_LENGTH_IN_BYTES     (512/8)
# define SSL_MAX_KEY_ARG_LENGTH                  8
# define SSL_MAX_MASTER_KEY_LENGTH               48
# define TLS13_MAX_RESUMPTION_MASTER_LENGTH      64


typedef void WPACKET;

struct ssl_st {
    struct ssl3_state_st *s3;   /* SSLv3 variables */
    int client_version;
};

typedef struct ssl3_state_st {
	struct {
        unsigned char *pms;
        size_t pmslen;
	} tmp;
} SSL3_STATE;


int ssl3_new(SSL *s)
{
    SSL3_STATE *s3;

    if ((s3 = OPENSSL_zalloc(sizeof(*s3))) == NULL)
        goto err;
    s->s3 = s3;
    return (1);
 err:
    return (0);
}

void ssl3_free(SSL *s)
{
    if (s == NULL || s->s3 == NULL)
        return;

    OPENSSL_clear_free(s->s3->tmp.pms, s->s3->tmp.pmslen);

    OPENSSL_clear_free(s->s3, sizeof(*s->s3));
    s->s3 = NULL;
}

static int tls_construct_cke_rsa(SSL *s, WPACKET *pkt, int *al)
{
    unsigned char *encdata = NULL;
    size_t enclen;
    unsigned char *pms = NULL;
    size_t pmslen = 0;

    pmslen = SSL_MAX_MASTER_KEY_LENGTH;
    pms = OPENSSL_malloc(pmslen);       /* allocation site */
    if (pms == NULL) {
        return 0;
    }

    pms[0] = s->client_version >> 8;
    pms[1] = s->client_version & 0xff;
    /* TODO(size_t): Convert this function */
    if (RAND_bytes(pms + 2, (int)(pmslen - 2)) <= 0) {
        goto err;
    }

    s->s3->tmp.pms = pms;
    s->s3->tmp.pmslen = pmslen;

    /* Log the premaster secret, if logging is enabled. */
    if (!ssl_log_rsa_client_key_exchange(s, encdata, enclen, pms, pmslen))
        goto err;

    return 1;
 err:
    OPENSSL_clear_free(s->s3->tmp.pms, s->s3->tmp.pmslen);  /* freed */

    return 0;
}

int tls_construct_client_key_exchange(SSL *s, WPACKET *pkt)
{
    unsigned long alg_k;
    int al = -1;

    if (!tls_construct_cke_rsa(s, pkt, &al))
        goto err;

    return 1;
 err:
    OPENSSL_clear_free(s->s3->tmp.pms, s->s3->tmp.pmslen);  /* double-free */
    s->s3->tmp.pms = NULL;
    return 0;
}

int main()
{
    SSL s;
    if(!ssl3_new(&s))
        return 1;
    
    if(!tls_construct_client_key_exchange(&s, NULL))
        goto end;
    __USE(s.s3->tmp.pms);
end:
    ssl3_free(&s);
    return 0;
}

