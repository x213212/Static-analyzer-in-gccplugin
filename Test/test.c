/*
    buggy parent : 0488c0b
    commit id : b6306d8049b04dca7fa738a86c892c43ba6a5fc4
*/

#include <stdio.h>
#include <stdlib.h>

#include "stdio.h"
#include "openssl.h"

struct tm {
	unsigned int tm_sec;
	unsigned int tm_min;
	unsigned int tm_hour;
	unsigned int tm_mday;
	unsigned int tm_mon;
	unsigned int tm_year;
};

#  define OCSP_REVOKED_STATUS_NOSTATUS               -1
#  define OCSP_REVOKED_STATUS_UNSPECIFIED             0
#  define OCSP_REVOKED_STATUS_KEYCOMPROMISE           1
#  define OCSP_REVOKED_STATUS_CACOMPROMISE            2
#  define OCSP_REVOKED_STATUS_AFFILIATIONCHANGED      3
#  define OCSP_REVOKED_STATUS_SUPERSEDED              4
#  define OCSP_REVOKED_STATUS_CESSATIONOFOPERATION    5
#  define OCSP_REVOKED_STATUS_CERTIFICATEHOLD         6
#  define OCSP_REVOKED_STATUS_REMOVEFROMCRL           8

/* Various flags and values */

#  define OCSP_DEFAULT_NONCE_LENGTH       16

#  define OCSP_NOCERTS                    0x1
#  define OCSP_NOINTERN                   0x2
#  define OCSP_NOSIGS                     0x4
#  define OCSP_NOCHAIN                    0x8
#  define OCSP_NOVERIFY                   0x10
#  define OCSP_NOEXPLICIT                 0x20
#  define OCSP_NOCASIGN                   0x40
#  define OCSP_NODELEGATED                0x80
#  define OCSP_NOCHECKS                   0x100
#  define OCSP_TRUSTOTHER                 0x200
#  define OCSP_RESPID_KEY                 0x400
#  define OCSP_NOTIME                     0x800
#  define OCSP_RESPONSE_STATUS_SUCCESSFUL           0
#  define OCSP_RESPONSE_STATUS_MALFORMEDREQUEST     1
#  define OCSP_RESPONSE_STATUS_INTERNALERROR        2
#  define OCSP_RESPONSE_STATUS_TRYLATER             3
#  define OCSP_RESPONSE_STATUS_SIGREQUIRED          5
#  define OCSP_RESPONSE_STATUS_UNAUTHORIZED         6
#  define V_OCSP_RESPID_NAME 0
#  define V_OCSP_RESPID_KEY  1
#  define V_OCSP_CERTSTATUS_GOOD    0
#  define V_OCSP_CERTSTATUS_REVOKED 1
#  define V_OCSP_CERTSTATUS_UNKNOWN 2


typedef struct ocsp_cert_id_st OCSP_CERTID;
typedef struct ocsp_one_request_st OCSP_ONEREQ;
typedef struct ocsp_req_info_st OCSP_REQINFO;
typedef struct ocsp_signature_st OCSP_SIGNATURE;
typedef struct ocsp_request_st OCSP_REQUEST;
typedef struct ocsp_resp_bytes_st OCSP_RESPBYTES;
typedef struct ocsp_revoked_info_st OCSP_REVOKEDINFO;
typedef struct ocsp_cert_status_st OCSP_CERTSTATUS;
typedef struct ocsp_single_response_st OCSP_SINGLERESP;
typedef struct ocsp_response_data_st OCSP_RESPDATA;
typedef struct ocsp_basic_response_st OCSP_BASICRESP;
typedef struct ocsp_crl_id_st OCSP_CRLID;
typedef struct ocsp_service_locator_st OCSP_SERVICELOC;
typedef struct ocsp_response_data_st OCSP_RESPDATA;
typedef struct ocsp_basic_response_st OCSP_BASICRESP;
typedef struct ocsp_responder_id_st OCSP_RESPID;

struct ocsp_responder_id_st {
    int type;
    union {
        X509_NAME *byName;
        ASN1_OCTET_STRING *byKey;
    } value;
};

struct ocsp_response_data_st {
    ASN1_INTEGER *version;
    OCSP_RESPID responderId;
    ASN1_GENERALIZEDTIME *producedAt;
    OCSP_SINGLERESP *responses;
};

struct ocsp_basic_response_st {
    OCSP_RESPDATA tbsResponseData;
    X509_ALGOR signatureAlgorithm;
    ASN1_BIT_STRING *signature;
};

struct ocsp_single_response_st {
    OCSP_CERTID *certId;
    OCSP_CERTSTATUS *certStatus;
    ASN1_GENERALIZEDTIME *thisUpdate;
    ASN1_GENERALIZEDTIME *nextUpdate;
};

# define TEST_true(a)         test_true(__FILE__, __LINE__, #a, (a) != 0)
# define TEST_ptr(a)          test_ptr(__FILE__, __LINE__, #a, a)
int test_ptr(const char *file, int line, const char *s, const void *p)
{
    if (p != NULL)
        return 1;
    test_fail_message(NULL, file, line, "ptr", s, "NULL", "!=", "%p", p);
    return 0;
}
int test_true(const char *file, int line, const char *s, int b)
{
    if (b)
        return 1;
    test_fail_message(NULL, file, line, "bool", s, "true", "==", "false");
    return 0;
}



int ASN1_STRING_set(ASN1_STRING *str, const void *_data, int len)
{
    unsigned char *c;
    const char *data = _data;

    if (len < 0) {
        if (data == NULL)
            return 0;
        else
            len = strlen(data);
    }
    if ((str->length <= len) || (str->data == NULL)) {
        c = str->data;
        str->data = OPENSSL_realloc(c, len + 1);
        if (str->data == NULL) {
            str->data = c;
            return 0;
        }
    }
    str->length = len;
    if (data != NULL) {
        memcpy(str->data, data, len);
        /* an allowance for strings :-) */
        str->data[len] = '\0';
    }
    return 1;
}

static int asn1_string_set_uint64(ASN1_STRING *a, uint64_t r, int itype)
{
    unsigned char tbuf[sizeof(r)];
    size_t off;

    a->type = itype;
    off = asn1_put_uint64(tbuf, r);
    return ASN1_STRING_set(a, tbuf + off, sizeof(tbuf) - off);
}

int ASN1_INTEGER_set_uint64(ASN1_INTEGER *a, uint64_t r)
{
    return asn1_string_set_uint64(a, r, V_ASN1_INTEGER);
}

int ASN1_BIT_STRING_set(ASN1_BIT_STRING *x, unsigned char *d, int len)
{
    return ASN1_STRING_set(x, d, len);
}

ASN1_TIME *asn1_time_from_tm(ASN1_TIME *s, struct tm *ts, int type)
{
    char* p;
    ASN1_TIME *tmps = NULL;
    const size_t len = 20;

    if (type == V_ASN1_UNDEF) {
        if (is_utc(ts->tm_year))
            type = V_ASN1_UTCTIME;
        else
            type = V_ASN1_GENERALIZEDTIME;
    } else if (type == V_ASN1_UTCTIME) {
        if (!is_utc(ts->tm_year))
            goto err;
    } else if (type != V_ASN1_GENERALIZEDTIME) {
        goto err;
    }

    if (s == NULL)
        tmps = ASN1_STRING_new();	/* allocation site */
    else
        tmps = s;
    if (tmps == NULL)
        return NULL;

    if (!ASN1_STRING_set(tmps, NULL, len))
        goto err;

    tmps->type = type;
    p = (char*)tmps->data;

    if (type == V_ASN1_GENERALIZEDTIME)
        tmps->length = BIO_snprintf(p, len, "%04d%02d%02d%02d%02d%02dZ",
                                    ts->tm_year + 1900, ts->tm_mon + 1,
                                    ts->tm_mday, ts->tm_hour, ts->tm_min,
                                    ts->tm_sec);
    else
        tmps->length = BIO_snprintf(p, len, "%02d%02d%02d%02d%02d%02dZ",
                                    ts->tm_year % 100, ts->tm_mon + 1,
                                    ts->tm_mday, ts->tm_hour, ts->tm_min,
                                    ts->tm_sec);

    return tmps;
 err:
    if (tmps != s)
        ASN1_STRING_free(tmps);
    return NULL;
}

ASN1_TIME *ASN1_TIME_adj(ASN1_TIME *s, time_t t,
                         int offset_day, long offset_sec)
{
    struct tm *ts;
    struct tm data;

    ts = OPENSSL_gmtime(&t, &data);
    if (ts == NULL) {
        return NULL;
    }
    if (offset_day || offset_sec) {
        if (!OPENSSL_gmtime_adj(ts, offset_day, offset_sec))
            return NULL;
    }
    return asn1_time_from_tm(s, ts, V_ASN1_UNDEF);
}

ASN1_TIME *ASN1_TIME_set(ASN1_TIME *s, time_t t)
{
    return ASN1_TIME_adj(s, t, 0, 0);
}

int X509_NAME_add_entry_by_NID(X509_NAME *name, int nid, int type,
                               const unsigned char *bytes, int len, int loc,
                               int set)
{
    X509_NAME_ENTRY *ne;
    int ret;
    ne = X509_NAME_ENTRY_create_by_NID(NULL, nid, type, bytes, len);
    if (!ne)
        return 0;
    ret = X509_NAME_add_entry(name, ne, loc, set);
    X509_NAME_ENTRY_free(ne);
    return ret;
}

OCSP_SINGLERESP *OCSP_basic_add1_status(OCSP_BASICRESP *rsp,
                                        OCSP_CERTID *cid,
                                        int status, int reason,
                                        ASN1_TIME *revtime,
                                        ASN1_TIME *thisupd,
                                        ASN1_TIME *nextupd)
{
    OCSP_SINGLERESP *single = NULL;
    OCSP_CERTSTATUS *cs;
    OCSP_REVOKEDINFO *ri;

    if (rsp->tbsResponseData.responses == NULL
        && (rsp->tbsResponseData.responses
                = sk_OCSP_SINGLERESP_new_null()) == NULL)
        goto err;

    if ((single->certId = OCSP_CERTID_dup(cid)) == NULL)
        goto err;

        if (!(sk_OCSP_SINGLERESP_push(rsp->tbsResponseData.responses, single)))
        goto err;
    return single;
 err:
    OCSP_SINGLERESP_free(single);
    return NULL;
}

static OCSP_BASICRESP *make_dummy_resp(void)
{
    const unsigned char namestr[] = "openssl.example.com";
    unsigned char keybytes[128] = {7};
    OCSP_BASICRESP *bs = OCSP_BASICRESP_new();					/* allocation site */
    OCSP_CERTID *cid;
    ASN1_TIME *thisupd = ASN1_TIME_set(NULL, time(NULL));       
    ASN1_TIME *nextupd = ASN1_TIME_set(NULL, time(NULL) + 200); 
    X509_NAME *name = X509_NAME_new();                          /* allocation site */
    ASN1_BIT_STRING *key = ASN1_BIT_STRING_new();               /* allocation site */
    ASN1_INTEGER *serial = ASN1_INTEGER_new();                  /* allocation site */

    if (!X509_NAME_add_entry_by_NID(name, NID_commonName, MBSTRING_ASC,
                                   namestr, -1, -1, 1)
        || !ASN1_BIT_STRING_set(key, keybytes, sizeof(keybytes)
        || !ASN1_INTEGER_set_uint64(serial, (uint64_t)1)))
        return NULL;                                            /* memory leak */
    cid = OCSP_cert_id_new(EVP_sha256(), name, key, serial);    /* allocation site */
    if (!TEST_ptr(bs)
        || !TEST_ptr(thisupd)
        || !TEST_ptr(nextupd)
        || !TEST_ptr(cid)
        || !TEST_true(OCSP_basic_add1_status(bs, cid,
                                             V_OCSP_CERTSTATUS_UNKNOWN,
                                             0, NULL, thisupd, nextupd)))
        return NULL;                                            /* memory leak */
    ASN1_TIME_free(thisupd);
    ASN1_TIME_free(nextupd);
    ASN1_BIT_STRING_free(key);
    ASN1_INTEGER_free(serial);
    OCSP_CERTID_free(cid);
    X509_NAME_free(name);
    return bs;
}

int main(void)
{
    OCSP_BASICRESP *bs;
    X509 *signer = NULL, *tmp;
    EVP_PKEY *key = NULL;
    /*
     * Test a response with no certs at all; get the signer from the
     * extra certs given to OCSP_resp_get0_signer().
     */
    bs = make_dummy_resp();
    if (!TEST_ptr(bs)
        || !TEST_true(get_cert_and_key(&signer, &key))
        || !TEST_true(OCSP_basic_sign(bs, signer, key, EVP_sha1(),
                                      NULL, OCSP_NOCERTS)))
        return 0;
    OCSP_BASICRESP_free(bs);

    /* Do it again but include the signer cert */
    bs = make_dummy_resp();
    tmp = NULL;
    if (!TEST_ptr(bs)
        || !TEST_true(OCSP_basic_sign(bs, signer, key, EVP_sha1(),
                                      NULL, 0)))
        return 0;
    if (!TEST_true(OCSP_resp_get0_signer(bs, &tmp, NULL))
        || !TEST_int_eq(X509_cmp(tmp, signer), 0))
        return 0;
    OCSP_BASICRESP_free(bs);
    X509_free(signer);
    EVP_PKEY_free(key);
    return 1;
}




