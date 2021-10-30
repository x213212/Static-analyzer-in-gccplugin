/*
	buggy parent : 388d679	
	commit id : 62b0a0dea612e3683c6bd4bef359fceda00238e8
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../openssl.h"

/*
 * CT function codes.
 */
# define CT_F_CTLOG_NEW                                   117
# define CT_F_CTLOG_NEW_FROM_BASE64                       118
# define CT_F_CTLOG_NEW_FROM_CONF                         119
# define CT_F_CTLOG_STORE_LOAD_CTX_NEW                    122
# define CT_F_CTLOG_STORE_LOAD_FILE                       123
# define CT_F_CTLOG_STORE_LOAD_LOG                        130
# define CT_F_CTLOG_STORE_NEW                             131
# define CT_F_CT_BASE64_DECODE                            124
# define CT_F_CT_POLICY_EVAL_CTX_NEW                      133
# define CT_F_CT_V1_LOG_ID_FROM_PKEY                      125
# define CT_F_I2O_SCT                                     107
# define CT_F_I2O_SCT_LIST                                108
# define CT_F_I2O_SCT_SIGNATURE                           109
# define CT_F_O2I_SCT                                     110
# define CT_F_O2I_SCT_LIST                                111
# define CT_F_O2I_SCT_SIGNATURE                           112
# define CT_F_SCT_CTX_NEW                                 126
# define CT_F_SCT_CTX_VERIFY                              128
# define CT_F_SCT_NEW                                     100
# define CT_F_SCT_NEW_FROM_BASE64                         127
# define CT_F_SCT_SET0_LOG_ID                             101
# define CT_F_SCT_SET1_EXTENSIONS                         114
# define CT_F_SCT_SET1_LOG_ID                             115
# define CT_F_SCT_SET1_SIGNATURE                          116
# define CT_F_SCT_SET_LOG_ENTRY_TYPE                      102
# define CT_F_SCT_SET_SIGNATURE_NID                       103
# define CT_F_SCT_SET_VERSION                             104

/*
 * CT reason codes.
 */
# define CT_R_BASE64_DECODE_ERROR                         108
# define CT_R_INVALID_LOG_ID_LENGTH                       100
# define CT_R_LOG_CONF_INVALID                            109
# define CT_R_LOG_CONF_INVALID_KEY                        110
# define CT_R_LOG_CONF_MISSING_DESCRIPTION                111
# define CT_R_LOG_CONF_MISSING_KEY                        112
# define CT_R_LOG_KEY_INVALID                             113
# define CT_R_SCT_FUTURE_TIMESTAMP                        116
# define CT_R_SCT_INVALID                                 104
# define CT_R_SCT_INVALID_SIGNATURE                       107
# define CT_R_SCT_LIST_INVALID                            105
# define CT_R_SCT_LOG_ID_MISMATCH                         114
# define CT_R_SCT_NOT_SET                                 106
# define CT_R_SCT_UNSUPPORTED_VERSION                     115
# define CT_R_UNRECOGNIZED_SIGNATURE_NID                  101
# define CT_R_UNSUPPORTED_ENTRY_TYPE                      102
# define CT_R_UNSUPPORTED_VERSION                         103


#define BIN_PER_LINE    (64/4*3)
#define CHUNKS_PER_LINE (64/4)
#define CHAR_PER_LINE   (64+1)

static const unsigned char data_bin2ascii[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ\
abcdefghijklmnopqrstuvwxyz0123456789+/";

/*-
 * 0xF0 is a EOLN
 * 0xF1 is ignore but next needs to be 0xF0 (for \r\n processing).
 * 0xF2 is EOF
 * 0xE0 is ignore at start of line.
 * 0xFF is error
 */

#define B64_EOLN                0xF0
#define B64_CR                  0xF1
#define B64_EOF                 0xF2
#define B64_WS                  0xE0
#define B64_ERROR               0xFF
#define B64_NOT_BASE64(a)       (((a)|0x13) == 0xF3)
#define B64_BASE64(a)           (!B64_NOT_BASE64(a))


int EVP_DecodeBlock(unsigned char *t, const unsigned char *f, int n)
{
    int i, ret = 0, a, b, c, d;
    unsigned long l;

    /* trim white space from the start of the line. */
    while ((conv_ascii2bin(*f) == B64_WS) && (n > 0)) {
        f++;
        n--;
    }

    /*
     * strip off stuff at the end of the line ascii2bin values B64_WS,
     * B64_EOLN, B64_EOLN and B64_EOF
     */
    while ((n > 3) && (B64_NOT_BASE64(conv_ascii2bin(f[n - 1]))))
        n--;

    if (n % 4 != 0)
        return (-1);

    for (i = 0; i < n; i += 4) {
        a = conv_ascii2bin(*(f++));
        b = conv_ascii2bin(*(f++));
        c = conv_ascii2bin(*(f++));
        d = conv_ascii2bin(*(f++));
        if ((a & 0x80) || (b & 0x80) || (c & 0x80) || (d & 0x80))
            return (-1);
        l = ((((unsigned long)a) << 18L) |
             (((unsigned long)b) << 12L) |
             (((unsigned long)c) << 6L) | (((unsigned long)d)));
        *(t++) = (unsigned char)(l >> 16L) & 0xff;
        *(t++) = (unsigned char)(l >> 8L) & 0xff;
        *(t++) = (unsigned char)(l) & 0xff;
        ret += 3;
    }
    return (ret);
}

/*
 * Decodes the base64 string |in| into |out|.
 * A new string will be malloc'd and assigned to |out|. This will be owned by
 * the caller. Do not provide a pre-allocated string in |out|.
 */
static int ct_base64_decode(const char *in, unsigned char **out)
{
    size_t inlen = strlen(in);
    int outlen;
    unsigned char *outbuf = NULL;

    if (inlen == 0) {
        *out = NULL;
        return 0;
    }

    outlen = (inlen / 4) * 3;
    outbuf = OPENSSL_malloc(outlen);				/* allocation site */
    if (outbuf == NULL) {
        CTerr(CT_F_CT_BASE64_DECODE, ERR_R_MALLOC_FAILURE);
        goto err;
    }

    outlen = EVP_DecodeBlock(outbuf, (unsigned char *)in, inlen);
    if (outlen < 0) {
        CTerr(CT_F_CT_BASE64_DECODE, CT_R_BASE64_DECODE_ERROR);
        goto err;
    }

    /* Subtract padding bytes from |outlen| */
    while (in[--inlen] == '=') {
        --outlen;
    }

    *out = outbuf;
    return outlen;
err:
    OPENSSL_free(outbuf);
    return -1;
}

/*
 * Allocate, build and returns a new |ct_log| from input |pkey_base64|
 * It returns 1 on success,
 * 0 on decoding failure, or invalid parameter if any
 * -1 on internal (malloc) failure
 */
int CTLOG_new_from_base64(CTLOG **ct_log, const char *pkey_base64, const char *name)
{
    unsigned char *pkey_der = NULL;
    int pkey_der_len = ct_base64_decode(pkey_base64, &pkey_der);
    const unsigned char *p;
    EVP_PKEY *pkey = NULL;

    if (ct_log == NULL) {
        CTerr(CT_F_CTLOG_NEW_FROM_BASE64, ERR_R_PASSED_INVALID_ARGUMENT);
        return 0;
    }

    if (pkey_der_len <= 0) {
        CTerr(CT_F_CTLOG_NEW_FROM_BASE64, CT_R_LOG_CONF_INVALID_KEY);
        return 0;
    }

    p = pkey_der;
    pkey = d2i_PUBKEY(NULL, &p, pkey_der_len);
    OPENSSL_free(pkey_der);
    if (pkey == NULL) {
        CTerr(CT_F_CTLOG_NEW_FROM_BASE64, CT_R_LOG_CONF_INVALID_KEY);
        return 0;
    }

    *ct_log = CTLOG_new(pkey, name);
    if (*ct_log == NULL) {
        EVP_PKEY_free(pkey);
        return 0;
    }

    return 1;
}

int main()
{
	CTLOG *ct_log;
	CTLOG_new_from_base64(&ct_log, "pkey_base64", "name");
	return 0;
}
