/*
	buggy parent : 23fa978
	commit id : 85155346b3ca2dcdecf018dc8db9df94ceebeb0d
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../openssl.h"

#define PARAM_TIME 1474934400 /* Sep 27th, 2016 */

# define         X509_V_OK                                       0
# define         X509_V_ERR_UNSPECIFIED                          1
# define         X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT            2
# define         X509_V_ERR_UNABLE_TO_GET_CRL                    3
# define         X509_V_ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE     4
# define         X509_V_ERR_UNABLE_TO_DECRYPT_CRL_SIGNATURE      5
# define         X509_V_ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY   6
# define         X509_V_ERR_CERT_SIGNATURE_FAILURE               7
# define         X509_V_ERR_CRL_SIGNATURE_FAILURE                8
# define         X509_V_ERR_CERT_NOT_YET_VALID                   9
# define         X509_V_ERR_CERT_HAS_EXPIRED                     10
# define         X509_V_ERR_CRL_NOT_YET_VALID                    11
# define         X509_V_ERR_CRL_HAS_EXPIRED                      12
# define         X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD       13
# define         X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD        14
# define         X509_V_ERR_ERROR_IN_CRL_LAST_UPDATE_FIELD       15
# define         X509_V_ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD       16
# define         X509_V_ERR_OUT_OF_MEM                           17
# define         X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT          18
# define         X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN            19
# define         X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY    20
# define         X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE      21
# define         X509_V_ERR_CERT_CHAIN_TOO_LONG                  22
# define         X509_V_ERR_CERT_REVOKED                         23
# define         X509_V_ERR_INVALID_CA                           24
# define         X509_V_ERR_PATH_LENGTH_EXCEEDED                 25
# define         X509_V_ERR_INVALID_PURPOSE                      26
# define         X509_V_ERR_CERT_UNTRUSTED                       27
# define         X509_V_ERR_CERT_REJECTED                        28
/* These are 'informational' when looking for issuer cert */
# define         X509_V_ERR_SUBJECT_ISSUER_MISMATCH              29
# define         X509_V_ERR_AKID_SKID_MISMATCH                   30
# define         X509_V_ERR_AKID_ISSUER_SERIAL_MISMATCH          31
# define         X509_V_ERR_KEYUSAGE_NO_CERTSIGN                 32
# define         X509_V_ERR_UNABLE_TO_GET_CRL_ISSUER             33
# define         X509_V_ERR_UNHANDLED_CRITICAL_EXTENSION         34
# define         X509_V_ERR_KEYUSAGE_NO_CRL_SIGN                 35
# define         X509_V_ERR_UNHANDLED_CRITICAL_CRL_EXTENSION     36
# define         X509_V_ERR_INVALID_NON_CA                       37
# define         X509_V_ERR_PROXY_PATH_LENGTH_EXCEEDED           38
# define         X509_V_ERR_KEYUSAGE_NO_DIGITAL_SIGNATURE        39
# define         X509_V_ERR_PROXY_CERTIFICATES_NOT_ALLOWED       40
# define         X509_V_ERR_INVALID_EXTENSION                    41
# define         X509_V_ERR_INVALID_POLICY_EXTENSION             42
# define         X509_V_ERR_NO_EXPLICIT_POLICY                   43
# define         X509_V_ERR_DIFFERENT_CRL_SCOPE                  44
# define         X509_V_ERR_UNSUPPORTED_EXTENSION_FEATURE        45
# define         X509_V_ERR_UNNESTED_RESOURCE                    46
# define         X509_V_ERR_PERMITTED_VIOLATION                  47
# define         X509_V_ERR_EXCLUDED_VIOLATION                   48
# define         X509_V_ERR_SUBTREE_MINMAX                       49
/* The application is not happy */
# define         X509_V_ERR_APPLICATION_VERIFICATION             50
# define         X509_V_ERR_UNSUPPORTED_CONSTRAINT_TYPE          51
# define         X509_V_ERR_UNSUPPORTED_CONSTRAINT_SYNTAX        52
# define         X509_V_ERR_UNSUPPORTED_NAME_SYNTAX              53
# define         X509_V_ERR_CRL_PATH_VALIDATION_ERROR            54
/* Another issuer check debug option */
# define         X509_V_ERR_PATH_LOOP                            55
/* Suite B mode algorithm violation */
# define         X509_V_ERR_SUITE_B_INVALID_VERSION              56
# define         X509_V_ERR_SUITE_B_INVALID_ALGORITHM            57
# define         X509_V_ERR_SUITE_B_INVALID_CURVE                58
# define         X509_V_ERR_SUITE_B_INVALID_SIGNATURE_ALGORITHM  59
# define         X509_V_ERR_SUITE_B_LOS_NOT_ALLOWED              60
# define         X509_V_ERR_SUITE_B_CANNOT_SIGN_P_384_WITH_P_256 61
/* Host, email and IP check errors */
# define         X509_V_ERR_HOSTNAME_MISMATCH                    62
# define         X509_V_ERR_EMAIL_MISMATCH                       63
# define         X509_V_ERR_IP_ADDRESS_MISMATCH                  64
/* DANE TLSA errors */
# define         X509_V_ERR_DANE_NO_MATCH                        65
/* security level errors */
# define         X509_V_ERR_EE_KEY_TOO_SMALL                     66
# define         X509_V_ERR_CA_KEY_TOO_SMALL                     67
# define         X509_V_ERR_CA_MD_TOO_WEAK                       68
/* Caller error */
# define         X509_V_ERR_INVALID_CALL                         69
/* Issuer lookup error */
# define         X509_V_ERR_STORE_LOOKUP                         70
/* Certificate transparency */
# define         X509_V_ERR_NO_VALID_SCTS                        71

# define         X509_V_ERR_PROXY_SUBJECT_NAME_VIOLATION         72
/* OCSP status errors */
# define         X509_V_ERR_OCSP_VERIFY_NEEDED                   73  /* Need OCSP verification */
# define         X509_V_ERR_OCSP_VERIFY_FAILED                   74  /* Couldn't verify cert through OCSP */
# define         X509_V_ERR_OCSP_CERT_UNKNOWN                    75  /* Certificate wasn't recognized by the OCSP responder */


/* Use check time instead of current time */
# define X509_V_FLAG_USE_CHECK_TIME              0x2
/* Lookup CRLs */
# define X509_V_FLAG_CRL_CHECK                   0x4
/* Lookup CRLs for whole chain */
# define X509_V_FLAG_CRL_CHECK_ALL               0x8
/* Ignore unhandled critical extensions */
# define X509_V_FLAG_IGNORE_CRITICAL             0x10
/* Disable workarounds for broken certificates */
# define X509_V_FLAG_X509_STRICT                 0x20
/* Enable proxy certificate validation */
# define X509_V_FLAG_ALLOW_PROXY_CERTS           0x40
/* Enable policy checking */
# define X509_V_FLAG_POLICY_CHECK                0x80
/* Policy variable require-explicit-policy */
# define X509_V_FLAG_EXPLICIT_POLICY             0x100
/* Policy variable inhibit-any-policy */
# define X509_V_FLAG_INHIBIT_ANY                 0x200
/* Policy variable inhibit-policy-mapping */
# define X509_V_FLAG_INHIBIT_MAP                 0x400
/* Notify callback that policy is OK */
# define X509_V_FLAG_NOTIFY_POLICY               0x800
/* Extended CRL features such as indirect CRLs, alternate CRL signing keys */
# define X509_V_FLAG_EXTENDED_CRL_SUPPORT        0x1000
/* Delta CRL support */
# define X509_V_FLAG_USE_DELTAS                  0x2000
/* Check self-signed CA signature */
# define X509_V_FLAG_CHECK_SS_SIGNATURE          0x4000
/* Use trusted store first */
# define X509_V_FLAG_TRUSTED_FIRST               0x8000
/* Suite B 128 bit only mode: not normally used */
# define X509_V_FLAG_SUITEB_128_LOS_ONLY         0x10000
/* Suite B 192 bit only mode */
# define X509_V_FLAG_SUITEB_192_LOS              0x20000
/* Suite B 128 bit mode allowing 192 bit algorithms */
# define X509_V_FLAG_SUITEB_128_LOS              0x30000
/* Allow partial chains if at least one certificate is in trusted store */
# define X509_V_FLAG_PARTIAL_CHAIN               0x80000
/*
 * If the initial chain is not trusted, do not attempt to build an alternative
 * chain. Alternate chain checking was introduced in 1.1.0. Setting this flag
 * will force the behaviour to match that of previous versions.
 */
# define X509_V_FLAG_NO_ALT_CHAINS               0x100000
/* Do not check certificate/CRL validity against current time */
# define X509_V_FLAG_NO_CHECK_TIME               0x200000

# define X509_VP_FLAG_DEFAULT                    0x1
# define X509_VP_FLAG_OVERWRITE                  0x2
# define X509_VP_FLAG_RESET_FLAGS                0x4
# define X509_VP_FLAG_LOCKED                     0x8
# define X509_VP_FLAG_ONCE                       0x10

/* Internal use: mask of policy related options */
# define X509_V_FLAG_POLICY_MASK (X509_V_FLAG_POLICY_CHECK \
                                | X509_V_FLAG_EXPLICIT_POLICY \
                                | X509_V_FLAG_INHIBIT_ANY \
                                | X509_V_FLAG_INHIBIT_MAP)


# define TEST_ptr(a)          test_ptr
# define TEST_long_eq(a, b)   test_long_eq(__FILE__, __LINE__, #a, #b, a, b)

struct X509_VERIFY_PARAM_st {
    char *name;
    time_t check_time;          /* Time to use */
    unsigned long flags;        /* Various verify flags */
    int purpose;                /* purpose to check untrusted certificates */
    int trust;                  /* trust setting to check */
    int depth;                  /* Verify depth */
    int auth_level;             /* Security level for chain verification */
    STACK_OF(ASN1_OBJECT) *policies; /* Permissible policies */
    /* Peer identity details */
    STACK_OF(OPENSSL_STRING) *hosts; /* Set of acceptable names */
    unsigned int hostflags;     /* Flags to control matching features */
    char *peername;             /* Matching hostname in peer certificate */
    char *email;                /* If not NULL email address to match */
    size_t emaillen;
    unsigned char *ip;          /* If not NULL IP address to match */
    size_t iplen;               /* Length of IP address */
};

static X509 *test_root = NULL;
static X509 *test_leaf = NULL;

void X509_free(X509 *a);

void X509_STORE_CTX_cleanup(X509_STORE_CTX *ctx)
{
    if (ctx->param != NULL) {
        printf("%x\n", ctx->param);
		if (ctx->parent == NULL)
			X509_VERIFY_PARAM_free(ctx->param);
        ctx->param = NULL;
    }
    X509_policy_tree_free(ctx->tree);
    ctx->tree = NULL;
    sk_X509_pop_free(ctx->chain, X509_free);
    ctx->chain = NULL;
}


void X509_STORE_CTX_free(X509_STORE_CTX *ctx)
{
    if (ctx == NULL)
        return;

    X509_STORE_CTX_cleanup(ctx);
    OPENSSL_free(ctx);
}

int test_ptr(const char *file, int line, const char *s, const void *p)
{
    if (p != NULL)
        return 1;
    test_fail_message(NULL, file, line, "ptr", s, "NULL", "!=", "%p", p);
    return 0;
}
void X509_VERIFY_PARAM_set_depth(X509_VERIFY_PARAM *param, int depth)
{
    param->depth = depth;
}

void X509_VERIFY_PARAM_set_time(X509_VERIFY_PARAM *param, time_t t)
{
    param->check_time = t;
    param->flags |= X509_V_FLAG_USE_CHECK_TIME;
}

time_t X509_VERIFY_PARAM_get_time(const X509_VERIFY_PARAM *param)
{
    return param->check_time;
}

int X509_VERIFY_PARAM_set_flags(X509_VERIFY_PARAM *param, unsigned long flags)
{
    param->flags |= flags;
    if (flags & X509_V_FLAG_POLICY_MASK)
        param->flags |= X509_V_FLAG_POLICY_CHECK;
    return 1;
}

void X509_STORE_CTX_set0_param(X509_STORE_CTX *ctx, X509_VERIFY_PARAM *param)
{
    X509_VERIFY_PARAM_free(ctx->param);
    ctx->param = param;
}

static const int minbits_table[] = { 80, 112, 128, 192, 256 };

/*
 * Check whether the public key of ``cert`` meets the security level of
 * ``ctx``.
 *
 * Returns 1 on success, 0 otherwise.
 */
static int check_key_level(X509_STORE_CTX *ctx, X509 *cert)
{
    EVP_PKEY *pkey = X509_get0_pubkey(cert);
    int level = ctx->param->auth_level;

    /* Unsupported or malformed keys are not secure */
    if (pkey == NULL)
        return 0;

    if (level <= 0)
        return 1;

    return EVP_PKEY_security_bits(pkey) >= minbits_table[level - 1];
}

int X509_verify_cert(X509_STORE_CTX *ctx)
{
    SSL_DANE *dane = ctx->dane;
    int ret;

    if (ctx->cert == NULL) {
        ctx->error = X509_V_ERR_INVALID_CALL;
        return -1;
    }

    if (ctx->chain != NULL) {
        /*
         * This X509_STORE_CTX has already been used to verify a cert. We
         * cannot do another one.
         */
        ctx->error = X509_V_ERR_INVALID_CALL;
        return -1;
    }

    /*
     * first we make sure the chain we are going to build is present and that
     * the first entry is in place
     */
    if (((ctx->chain = sk_X509_new_null()) == NULL) ||
        (!sk_X509_push(ctx->chain, ctx->cert))) {
        ctx->error = X509_V_ERR_OUT_OF_MEM;
        return -1;
    }
    X509_up_ref(ctx->cert);
    ctx->num_untrusted = 1;

    /* If the peer's public key is too weak, we can stop early. */
    if (!check_key_level(ctx, ctx->cert) &&
        !verify_cb_cert(ctx, ctx->cert, 0, X509_V_ERR_EE_KEY_TOO_SMALL))
        return 0;

    if (DANETLS_ENABLED(dane))
        ret = dane_verify(ctx);
    else
        ret = verify_chain(ctx);

    /*
     * Safety-net.  If we are returning an error, we must also set ctx->error,
     * so that the chain is not considered verified should the error be ignored
     * (e.g. TLS with SSL_VERIFY_NONE).
     */
    if (ret <= 0 && ctx->error == X509_V_OK)
        ctx->error = X509_V_ERR_UNSPECIFIED;
    return ret;
}
/*
 * Verify |leaf| certificate (chained up to |root|).  |crls| if
 * not NULL, is a list of CRLs to include in the verification. It is
 * also free'd before returning, which is kinda yucky but convenient.
 * Returns a value from X509_V_ERR_xxx or X509_V_OK.
 */
static int verify(X509 *leaf, X509 *root, STACK_OF(X509_CRL) *crls,
                  unsigned long flags)
{
    X509_STORE_CTX *ctx = X509_STORE_CTX_new();
    X509_STORE *store = X509_STORE_new();
    X509_VERIFY_PARAM *param = X509_VERIFY_PARAM_new();		/* allocation site */
    STACK_OF(X509) *roots = sk_X509_new_null();
    int status = X509_V_ERR_UNSPECIFIED;

    if (!TEST_ptr(ctx)
        || !TEST_ptr(store)
        || !TEST_ptr(param)
        || !TEST_ptr(roots))
        goto err; 	/* memory leak */

    /* Create a stack; upref the cert because we free it below. */
    X509_up_ref(root);
    if (!TEST_true(sk_X509_push(roots, root))
        || !TEST_true(X509_STORE_CTX_init(ctx, store, leaf, NULL)))
        goto err;	/* memory leak */
    X509_STORE_CTX_set0_trusted_stack(ctx, roots);
    X509_STORE_CTX_set0_crls(ctx, crls);
    X509_VERIFY_PARAM_set_time(param, PARAM_TIME);
    if (!TEST_long_eq(X509_VERIFY_PARAM_get_time(param), PARAM_TIME))
        goto err;	/* memory leak */
    X509_VERIFY_PARAM_set_depth(param, 16);
    if (flags)
        X509_VERIFY_PARAM_set_flags(param, flags);
    X509_STORE_CTX_set0_param(ctx, param);

    ERR_clear_error();
    status = X509_verify_cert(ctx) == 1 ? X509_V_OK
                                        : X509_STORE_CTX_get_error(ctx);
err:
    sk_X509_pop_free(roots, X509_free);
    X509_STORE_CTX_free(ctx);
    X509_STORE_free(store);
    return status;
}

static int test_no_crl(void)
{
    return TEST_int_eq(verify(test_leaf, test_root, NULL,
                              X509_V_FLAG_CRL_CHECK),
                       X509_V_ERR_UNABLE_TO_GET_CRL);
}

int main(void)
{
    if (!TEST_ptr(test_root = X509_from_strings(kCRLTestRoot))
        || !TEST_ptr(test_leaf = X509_from_strings(kCRLTestLeaf)))
        return 0;
	test_no_crl();		
    X509_free(test_root);
    X509_free(test_leaf);
}


