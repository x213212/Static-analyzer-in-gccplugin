/*
    commit id : febb0afaef47ed74b2bdbde0b4278263390f4185
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../openssl.h"

typedef struct {
    /* Pointer to where we are currently reading from */
    const unsigned char *curr;
    /* Number of bytes remaining */
    size_t remaining;
} PACKET;

typedef void WPACKET;

struct ssl_st {
    struct {
        unsigned char *tls13_cookie;
        size_t tls13_cookie_len;
    } ext;
};

void SSL_free(SSL *s)
{
    if (s == NULL)
        return;

    OPENSSL_free(s->ext.tls13_cookie);	/* double-free */
    OPENSSL_free(s);
}

/*
 * Copy |pkt| bytes to a newly allocated buffer and store a pointer to the
 * result in |*data|, and the length in |len|.
 * If |*data| is not NULL, the old data is OPENSSL_free'd.
 * If the packet is empty, or malloc fails, |*data| will be set to NULL.
 * Returns 1 if the malloc succeeds and 0 otherwise.
 * Does not forward PACKET position (because it is typically the last thing
 * done with a given PACKET).
 */
int PACKET_memdup(const PACKET *pkt, unsigned char **data, size_t *len)
{
    size_t length;

    OPENSSL_free(*data);
    *data = NULL;
    *len = 0;

    length = PACKET_remaining(pkt);

    if (length == 0)
        return 1;

    *data = OPENSSL_memdup(pkt->curr, length);	/* allocation site */
    if (*data == NULL)
        return 0;

    *len = length;
    return 1;
}

int tls_parse_stoc_cookie(SSL *s, PACKET *pkt, unsigned int context, X509 *x,
                       size_t chainidx, int *al)
{
    PACKET cookie;

    if (!PACKET_as_length_prefixed_2(pkt, &cookie)
            || !PACKET_memdup(&cookie, &s->ext.tls13_cookie,
                              &s->ext.tls13_cookie_len)) {
        return 0;
    }

    return 1;
}

int tls_construct_ctos_cookie(SSL *s, WPACKET *pkt, unsigned int context,
                              X509 *x, size_t chainidx, int *al)
{
    int ret = 0;

    /* Should only be set if we've had an HRR */
    if (s->ext.tls13_cookie_len == 0)
        return 1;

    if (!WPACKET_put_bytes_u16(pkt, 0)
               /* Extension data sub-packet */
            || !WPACKET_start_sub_packet_u16(pkt)
            || !WPACKET_sub_memcpy_u16(pkt, s->ext.tls13_cookie,
                                       s->ext.tls13_cookie_len)
            || !WPACKET_close(pkt)) {
        goto end;
    }

    ret = 1;
 end:
    OPENSSL_free(s->ext.tls13_cookie);	/* freed */
    s->ext.tls13_cookie_len = 0;

    return ret;
}

int main()
{
    PACKET pkt;
    SSL *s = malloc(sizeof(SSL));
    s->ext.tls13_cookie = NULL;
    s->ext.tls13_cookie_len = 0;
    
    pkt.curr = "PACKET";

    tls_parse_stoc_cookie(s, &pkt, 0, NULL, 0, NULL); 
    tls_construct_ctos_cookie(s, NULL, 0, NULL, 0, NULL);
    
    SSL_free(s);
    return 0;
}

