/*
    commit id : 918a27facd3558444c69b1edbedb49478e82dff5
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../openssl.h"

BIO *bio_err;

# define EVP_PKEY_OP_UNDEFINED           0
# define EVP_PKEY_OP_PARAMGEN            (1<<1)
# define EVP_PKEY_OP_KEYGEN              (1<<2)
# define EVP_PKEY_OP_SIGN                (1<<3)
# define EVP_PKEY_OP_VERIFY              (1<<4)
# define EVP_PKEY_OP_VERIFYRECOVER       (1<<5)
# define EVP_PKEY_OP_SIGNCTX             (1<<6)
# define EVP_PKEY_OP_VERIFYCTX           (1<<7)
# define EVP_PKEY_OP_ENCRYPT             (1<<8)
# define EVP_PKEY_OP_DECRYPT             (1<<9)
# define EVP_PKEY_OP_DERIVE              (1<<10)


/*
 * EVP function codes.
 */
# define EVP_F_AESNI_INIT_KEY                             165
# define EVP_F_AES_INIT_KEY                               133
# define EVP_F_AES_OCB_CIPHER                             169
# define EVP_F_AES_T4_INIT_KEY                            178
# define EVP_F_AES_WRAP_CIPHER                            170
# define EVP_F_ALG_MODULE_INIT                            177
# define EVP_F_ARIA_CCM_INIT_KEY                          175
# define EVP_F_ARIA_GCM_INIT_KEY                          176
# define EVP_F_ARIA_INIT_KEY                              185
# define EVP_F_CAMELLIA_INIT_KEY                          159
# define EVP_F_CHACHA20_POLY1305_CTRL                     182
# define EVP_F_CMLL_T4_INIT_KEY                           179
# define EVP_F_DES_EDE3_WRAP_CIPHER                       171
# define EVP_F_DO_SIGVER_INIT                             161
# define EVP_F_EVP_CIPHERINIT_EX                          123
# define EVP_F_EVP_CIPHER_CTX_COPY                        163
# define EVP_F_EVP_CIPHER_CTX_CTRL                        124
# define EVP_F_EVP_CIPHER_CTX_SET_KEY_LENGTH              122
# define EVP_F_EVP_DECRYPTFINAL_EX                        101
# define EVP_F_EVP_DECRYPTUPDATE                          166
# define EVP_F_EVP_DIGESTFINALXOF                         174
# define EVP_F_EVP_DIGESTINIT_EX                          128
# define EVP_F_EVP_ENCRYPTFINAL_EX                        127
# define EVP_F_EVP_ENCRYPTUPDATE                          167
# define EVP_F_EVP_MD_CTX_COPY_EX                         110
# define EVP_F_EVP_MD_SIZE                                162
# define EVP_F_EVP_OPENINIT                               102
# define EVP_F_EVP_PBE_ALG_ADD                            115
# define EVP_F_EVP_PBE_ALG_ADD_TYPE                       160
# define EVP_F_EVP_PBE_CIPHERINIT                         116
# define EVP_F_EVP_PBE_SCRYPT                             181
# define EVP_F_EVP_PKCS82PKEY                             111
# define EVP_F_EVP_PKEY2PKCS8                             113
# define EVP_F_EVP_PKEY_CHECK                             186
# define EVP_F_EVP_PKEY_COPY_PARAMETERS                   103
# define EVP_F_EVP_PKEY_CTX_CTRL                          137
# define EVP_F_EVP_PKEY_CTX_CTRL_STR                      150
# define EVP_F_EVP_PKEY_CTX_DUP                           156
# define EVP_F_EVP_PKEY_CTX_MD                            168
# define EVP_F_EVP_PKEY_DECRYPT                           104
# define EVP_F_EVP_PKEY_DECRYPT_INIT                      138
# define EVP_F_EVP_PKEY_DECRYPT_OLD                       151
# define EVP_F_EVP_PKEY_DERIVE                            153
# define EVP_F_EVP_PKEY_DERIVE_INIT                       154
# define EVP_F_EVP_PKEY_DERIVE_SET_PEER                   155
# define EVP_F_EVP_PKEY_ENCRYPT                           105
# define EVP_F_EVP_PKEY_ENCRYPT_INIT                      139
# define EVP_F_EVP_PKEY_ENCRYPT_OLD                       152
# define EVP_F_EVP_PKEY_GET0_DH                           119
# define EVP_F_EVP_PKEY_GET0_DSA                          120
# define EVP_F_EVP_PKEY_GET0_EC_KEY                       131
# define EVP_F_EVP_PKEY_GET0_HMAC                         183
# define EVP_F_EVP_PKEY_GET0_POLY1305                     184
# define EVP_F_EVP_PKEY_GET0_RSA                          121
# define EVP_F_EVP_PKEY_GET0_SIPHASH                      172
# define EVP_F_EVP_PKEY_KEYGEN                            146
# define EVP_F_EVP_PKEY_KEYGEN_INIT                       147
# define EVP_F_EVP_PKEY_NEW                               106
# define EVP_F_EVP_PKEY_PARAMGEN                          148
# define EVP_F_EVP_PKEY_PARAMGEN_INIT                     149
# define EVP_F_EVP_PKEY_SIGN                              140
# define EVP_F_EVP_PKEY_SIGN_INIT                         141
# define EVP_F_EVP_PKEY_VERIFY                            142
# define EVP_F_EVP_PKEY_VERIFY_INIT                       143
# define EVP_F_EVP_PKEY_VERIFY_RECOVER                    144
# define EVP_F_EVP_PKEY_VERIFY_RECOVER_INIT               145
# define EVP_F_EVP_SIGNFINAL                              107
# define EVP_F_EVP_VERIFYFINAL                            108
# define EVP_F_INT_CTX_NEW                                157
# define EVP_F_PKCS5_PBE_KEYIVGEN                         117
# define EVP_F_PKCS5_V2_PBE_KEYIVGEN                      118
# define EVP_F_PKCS5_V2_PBKDF2_KEYIVGEN                   164
# define EVP_F_PKCS5_V2_SCRYPT_KEYIVGEN                   180
# define EVP_F_PKEY_SET_TYPE                              158
# define EVP_F_RC2_MAGIC_TO_METH                          109
# define EVP_F_RC5_CTRL                                   125
# define EVP_F_UPDATE                                     173

/*
 * EVP reason codes.
 */
# define EVP_R_AES_KEY_SETUP_FAILED                       143
# define EVP_R_ARIA_KEY_SETUP_FAILED                      176
# define EVP_R_BAD_DECRYPT                                100
# define EVP_R_BUFFER_TOO_SMALL                           155
# define EVP_R_CAMELLIA_KEY_SETUP_FAILED                  157
# define EVP_R_CIPHER_PARAMETER_ERROR                     122
# define EVP_R_COMMAND_NOT_SUPPORTED                      147
# define EVP_R_COPY_ERROR                                 173
# define EVP_R_CTRL_NOT_IMPLEMENTED                       132
# define EVP_R_CTRL_OPERATION_NOT_IMPLEMENTED             133
# define EVP_R_DATA_NOT_MULTIPLE_OF_BLOCK_LENGTH          138
# define EVP_R_DECODE_ERROR                               114
# define EVP_R_DIFFERENT_KEY_TYPES                        101
# define EVP_R_DIFFERENT_PARAMETERS                       153
# define EVP_R_ERROR_LOADING_SECTION                      165
# define EVP_R_ERROR_SETTING_FIPS_MODE                    166
# define EVP_R_EXPECTING_AN_HMAC_KEY                      174
# define EVP_R_EXPECTING_AN_RSA_KEY                       127
# define EVP_R_EXPECTING_A_DH_KEY                         128
# define EVP_R_EXPECTING_A_DSA_KEY                        129
# define EVP_R_EXPECTING_A_EC_KEY                         142
# define EVP_R_EXPECTING_A_POLY1305_KEY                   164
# define EVP_R_EXPECTING_A_SIPHASH_KEY                    175
# define EVP_R_FIPS_MODE_NOT_SUPPORTED                    167
# define EVP_R_ILLEGAL_SCRYPT_PARAMETERS                  171
# define EVP_R_INITIALIZATION_ERROR                       134
# define EVP_R_INPUT_NOT_INITIALIZED                      111
# define EVP_R_INVALID_DIGEST                             152
# define EVP_R_INVALID_FIPS_MODE                          168
# define EVP_R_INVALID_KEY                                163
# define EVP_R_INVALID_KEY_LENGTH                         130
# define EVP_R_INVALID_OPERATION                          148
# define EVP_R_KEYGEN_FAILURE                             120
# define EVP_R_MEMORY_LIMIT_EXCEEDED                      172
# define EVP_R_MESSAGE_DIGEST_IS_NULL                     159
# define EVP_R_METHOD_NOT_SUPPORTED                       144
# define EVP_R_MISSING_PARAMETERS                         103
# define EVP_R_NOT_XOF_OR_INVALID_LENGTH                  178
# define EVP_R_NO_CIPHER_SET                              131
# define EVP_R_NO_DEFAULT_DIGEST                          158
# define EVP_R_NO_DIGEST_SET                              139
# define EVP_R_NO_KEY_SET                                 154
# define EVP_R_NO_OPERATION_SET                           149
# define EVP_R_ONLY_ONESHOT_SUPPORTED                     177
# define EVP_R_OPERATION_NOT_SUPPORTED_FOR_THIS_KEYTYPE   150
# define EVP_R_OPERATON_NOT_INITIALIZED                   151
# define EVP_R_PARTIALLY_OVERLAPPING                      162
# define EVP_R_PRIVATE_KEY_DECODE_ERROR                   145
# define EVP_R_PRIVATE_KEY_ENCODE_ERROR                   146
# define EVP_R_PUBLIC_KEY_NOT_RSA                         106
# define EVP_R_UNKNOWN_CIPHER                             160
# define EVP_R_UNKNOWN_DIGEST                             161
# define EVP_R_UNKNOWN_OPTION                             169
# define EVP_R_UNKNOWN_PBE_ALGORITHM                      121
# define EVP_R_UNSUPPORTED_ALGORITHM                      156
# define EVP_R_UNSUPPORTED_CIPHER                         107
# define EVP_R_UNSUPPORTED_KEYLENGTH                      123
# define EVP_R_UNSUPPORTED_KEY_DERIVATION_FUNCTION        124
# define EVP_R_UNSUPPORTED_KEY_SIZE                       108
# define EVP_R_UNSUPPORTED_NUMBER_OF_ROUNDS               135
# define EVP_R_UNSUPPORTED_PRF                            125
# define EVP_R_UNSUPPORTED_PRIVATE_KEY_ALGORITHM          118
# define EVP_R_UNSUPPORTED_SALT_TYPE                      126
# define EVP_R_WRAP_MODE_NOT_ALLOWED                      170
# define EVP_R_WRONG_FINAL_BLOCK_LENGTH                   109


typedef enum OPTION_choice {
    OPT_ERR = -1, OPT_EOF = 0, OPT_HELP,
    OPT_ENGINE, OPT_OUTFORM, OPT_OUT, OPT_PASS, OPT_PARAMFILE,
    OPT_ALGORITHM, OPT_PKEYOPT, OPT_GENPARAM, OPT_TEXT, OPT_CIPHER
} OPTION_CHOICE;

const OPTIONS genpkey_options[] = {
    {"help", OPT_HELP, '-', "Display this summary"},
    {"out", OPT_OUT, '>', "Output file"},
    {"outform", OPT_OUTFORM, 'F', "output format (DER or PEM)"},
    {"pass", OPT_PASS, 's', "Output file pass phrase source"},
    {"paramfile", OPT_PARAMFILE, '<', "Parameters file"},
    {"algorithm", OPT_ALGORITHM, 's', "The public key algorithm"},
    {"pkeyopt", OPT_PKEYOPT, 's',
     "Set the public key algorithm option as opt:value"},
    {"genparam", OPT_GENPARAM, '-', "Generate parameters, not key"},
    {"text", OPT_TEXT, '-', "Print the in text"},
    {"", OPT_CIPHER, '-', "Cipher to use to encrypt the key"},
#ifndef OPENSSL_NO_ENGINE
    {"engine", OPT_ENGINE, 's', "Use engine, possibly a hardware device"},
#endif
    /* This is deliberately last. */
    {OPT_HELP_STR, 1, 1,
     "Order of options may be important!  See the documentation.\n"},
    {NULL}
};

typedef int EVP_PKEY_gen_cb(EVP_PKEY_CTX *ctx);
typedef int CRYPTO_REF_COUNT;
typedef void CRYPTO_RWLOCK;

struct evp_pkey_asn1_method_st {
    int pkey_id;
    int pkey_base_id;
    unsigned long pkey_flags;
    char *pem_str;
    char *info;
    int (*pub_decode) (EVP_PKEY *pk, X509_PUBKEY *pub);
    int (*pub_encode) (X509_PUBKEY *pub, const EVP_PKEY *pk);
    int (*pub_cmp) (const EVP_PKEY *a, const EVP_PKEY *b);
    int (*pub_print) (BIO *out, const EVP_PKEY *pkey, int indent,
                      ASN1_PCTX *pctx);
    int (*priv_decode) (EVP_PKEY *pk, const PKCS8_PRIV_KEY_INFO *p8inf);
    int (*priv_encode) (PKCS8_PRIV_KEY_INFO *p8, const EVP_PKEY *pk);
    int (*priv_print) (BIO *out, const EVP_PKEY *pkey, int indent,
                       ASN1_PCTX *pctx);
    int (*pkey_size) (const EVP_PKEY *pk);
    int (*pkey_bits) (const EVP_PKEY *pk);
    int (*pkey_security_bits) (const EVP_PKEY *pk);
    int (*param_decode) (EVP_PKEY *pkey,
                         const unsigned char **pder, int derlen);
    int (*param_encode) (const EVP_PKEY *pkey, unsigned char **pder);
    int (*param_missing) (const EVP_PKEY *pk);
    int (*param_copy) (EVP_PKEY *to, const EVP_PKEY *from);
    int (*param_cmp) (const EVP_PKEY *a, const EVP_PKEY *b);
    int (*param_print) (BIO *out, const EVP_PKEY *pkey, int indent,
                        ASN1_PCTX *pctx);
    int (*sig_print) (BIO *out,
                      const X509_ALGOR *sigalg, const ASN1_STRING *sig,
                      int indent, ASN1_PCTX *pctx);
    void (*pkey_free) (EVP_PKEY *pkey);
    int (*pkey_ctrl) (EVP_PKEY *pkey, int op, long arg1, void *arg2);
    /* Legacy functions for old PEM */
    int (*old_priv_decode) (EVP_PKEY *pkey,
                            const unsigned char **pder, int derlen);
    int (*old_priv_encode) (const EVP_PKEY *pkey, unsigned char **pder);
    /* Custom ASN1 signature verification */
    int (*item_verify) (EVP_MD_CTX *ctx, const ASN1_ITEM *it, void *asn,
                        X509_ALGOR *a, ASN1_BIT_STRING *sig, EVP_PKEY *pkey);
    int (*item_sign) (EVP_MD_CTX *ctx, const ASN1_ITEM *it, void *asn,
                      X509_ALGOR *alg1, X509_ALGOR *alg2,
                      ASN1_BIT_STRING *sig);
    int (*siginf_set) (X509_SIG_INFO *siginf, const X509_ALGOR *alg,
                       const ASN1_STRING *sig);
    /* Check */
    int (*pkey_check) (const EVP_PKEY *pk);
} /* EVP_PKEY_ASN1_METHOD */ ;


struct evp_pkey_st {
    int type;
    int save_type;
    CRYPTO_REF_COUNT references;
    const EVP_PKEY_ASN1_METHOD *ameth;
    ENGINE *engine;
    union {
        void *ptr;
# ifndef OPENSSL_NO_RSA
        struct rsa_st *rsa;     /* RSA */
# endif
# ifndef OPENSSL_NO_DSA
        struct dsa_st *dsa;     /* DSA */
# endif
# ifndef OPENSSL_NO_DH
        struct dh_st *dh;       /* DH */
# endif
# ifndef OPENSSL_NO_EC
        struct ec_key_st *ec;   /* ECC */
# endif
    } pkey;
    int save_parameters;
    STACK_OF(X509_ATTRIBUTE) *attributes; /* [ 0 ] */
    CRYPTO_RWLOCK *lock;
} /* EVP_PKEY */ ;


struct evp_pkey_ctx_st {
    /* Method associated with this operation */
    const EVP_PKEY_METHOD *pmeth;
    /* Engine that implements this method or NULL if builtin */
    ENGINE *engine;
    /* Key: may be NULL */
    EVP_PKEY *pkey;
    /* Peer key for key agreement, may be NULL */
    EVP_PKEY *peerkey;
    /* Actual operation */
    int operation;
    /* Algorithm specific data */
    void *data;
    /* Application specific data */
    void *app_data;
    /* Keygen callback */
    EVP_PKEY_gen_cb *pkey_gencb;
    /* implementation specific keygen data */
    int *keygen_info;
    int keygen_info_count;
} /* EVP_PKEY_CTX */ ;

#define EVP_PKEY_FLAG_DYNAMIC   1

struct evp_pkey_method_st {
    int pkey_id;
    int flags;
    int (*init) (EVP_PKEY_CTX *ctx);
    int (*copy) (EVP_PKEY_CTX *dst, EVP_PKEY_CTX *src);
    void (*cleanup) (EVP_PKEY_CTX *ctx);
    int (*paramgen_init) (EVP_PKEY_CTX *ctx);
    int (*paramgen) (EVP_PKEY_CTX *ctx, EVP_PKEY *pkey);
    int (*keygen_init) (EVP_PKEY_CTX *ctx);
    int (*keygen) (EVP_PKEY_CTX *ctx, EVP_PKEY *pkey);
    int (*sign_init) (EVP_PKEY_CTX *ctx);
    int (*sign) (EVP_PKEY_CTX *ctx, unsigned char *sig, size_t *siglen,
                 const unsigned char *tbs, size_t tbslen);
    int (*verify_init) (EVP_PKEY_CTX *ctx);
    int (*verify) (EVP_PKEY_CTX *ctx,
                   const unsigned char *sig, size_t siglen,
                   const unsigned char *tbs, size_t tbslen);
    int (*verify_recover_init) (EVP_PKEY_CTX *ctx);
    int (*verify_recover) (EVP_PKEY_CTX *ctx,
                           unsigned char *rout, size_t *routlen,
                           const unsigned char *sig, size_t siglen);
    int (*signctx_init) (EVP_PKEY_CTX *ctx, EVP_MD_CTX *mctx);
    int (*signctx) (EVP_PKEY_CTX *ctx, unsigned char *sig, size_t *siglen,
                    EVP_MD_CTX *mctx);
    int (*verifyctx_init) (EVP_PKEY_CTX *ctx, EVP_MD_CTX *mctx);
    int (*verifyctx) (EVP_PKEY_CTX *ctx, const unsigned char *sig, int siglen,
                      EVP_MD_CTX *mctx);
    int (*encrypt_init) (EVP_PKEY_CTX *ctx);
    int (*encrypt) (EVP_PKEY_CTX *ctx, unsigned char *out, size_t *outlen,
                    const unsigned char *in, size_t inlen);
    int (*decrypt_init) (EVP_PKEY_CTX *ctx);
    int (*decrypt) (EVP_PKEY_CTX *ctx, unsigned char *out, size_t *outlen,
                    const unsigned char *in, size_t inlen);
    int (*derive_init) (EVP_PKEY_CTX *ctx);
    int (*derive) (EVP_PKEY_CTX *ctx, unsigned char *key, size_t *keylen);
    int (*ctrl) (EVP_PKEY_CTX *ctx, int type, int p1, void *p2);
    int (*ctrl_str) (EVP_PKEY_CTX *ctx, const char *type, const char *value);
    int (*digestsign) (EVP_MD_CTX *ctx, unsigned char *sig, size_t *siglen,
                       const unsigned char *tbs, size_t tbslen);
    int (*digestverify) (EVP_MD_CTX *ctx, const unsigned char *sig,
                         size_t siglen, const unsigned char *tbs,
                         size_t tbslen);
    int (*check) (EVP_PKEY *pkey);
} /* EVP_PKEY_METHOD */ ;
extern const EVP_PKEY_METHOD cmac_pkey_meth;
extern const EVP_PKEY_METHOD dh_pkey_meth;
extern const EVP_PKEY_METHOD dhx_pkey_meth;
extern const EVP_PKEY_METHOD dsa_pkey_meth;
extern const EVP_PKEY_METHOD ec_pkey_meth;
extern const EVP_PKEY_METHOD ecx25519_pkey_meth;
extern const EVP_PKEY_METHOD ed25519_pkey_meth;
extern const EVP_PKEY_METHOD hmac_pkey_meth;
extern const EVP_PKEY_METHOD rsa_pkey_meth;
extern const EVP_PKEY_METHOD rsa_pss_pkey_meth;
extern const EVP_PKEY_METHOD scrypt_pkey_meth;
extern const EVP_PKEY_METHOD tls1_prf_pkey_meth;
extern const EVP_PKEY_METHOD hkdf_pkey_meth;
extern const EVP_PKEY_METHOD poly1305_pkey_meth;
extern const EVP_PKEY_METHOD siphash_pkey_meth;

struct evp_md_st {
    int type;
    int pkey_type;
    int md_size;
    unsigned long flags;
    int (*init) (EVP_MD_CTX *ctx);
    int (*update) (EVP_MD_CTX *ctx, const void *data, size_t count);
    int (*final) (EVP_MD_CTX *ctx, unsigned char *md);
    int (*copy) (EVP_MD_CTX *to, const EVP_MD_CTX *from);
    int (*cleanup) (EVP_MD_CTX *ctx);
    int block_size;
    int ctx_size;               /* how big does the ctx->md_data need to be */
    /* control function */
    int (*md_ctrl) (EVP_MD_CTX *ctx, int cmd, int p1, void *p2);
} /* EVP_MD */ ;

struct evp_cipher_st {
    int nid;
    int block_size;
    /* Default value for variable length ciphers */
    int key_len;
    int iv_len;
    /* Various flags */
    unsigned long flags;
    /* init key */
    int (*init) (EVP_CIPHER_CTX *ctx, const unsigned char *key,
                 const unsigned char *iv, int enc);
    /* encrypt/decrypt data */
    int (*do_cipher) (EVP_CIPHER_CTX *ctx, unsigned char *out,
                      const unsigned char *in, size_t inl);
    /* cleanup ctx */
    int (*cleanup) (EVP_CIPHER_CTX *);
    /* how big ctx->cipher_data needs to be */
    int ctx_size;
    /* Populate a ASN1_TYPE with parameters */
    int (*set_asn1_parameters) (EVP_CIPHER_CTX *, ASN1_TYPE *);
    /* Get parameters from a ASN1_TYPE */
    int (*get_asn1_parameters) (EVP_CIPHER_CTX *, ASN1_TYPE *);
    /* Miscellaneous operations */
    int (*ctrl) (EVP_CIPHER_CTX *, int type, int arg, void *ptr);
    /* Application data */
    void *app_data;
} /* EVP_CIPHER */ ;



void EVP_PKEY_CTX_free(EVP_PKEY_CTX *ctx)
{
    if (ctx == NULL)
        return;
    if (ctx->pmeth && ctx->pmeth->cleanup)
        ctx->pmeth->cleanup(ctx);
    EVP_PKEY_free(ctx->pkey);
    EVP_PKEY_free(ctx->peerkey);
#ifndef OPENSSL_NO_ENGINE
    ENGINE_finish(ctx->engine);
#endif
    OPENSSL_free(ctx);
}

static EVP_PKEY_CTX *int_ctx_new(EVP_PKEY *pkey, ENGINE *e, int id)
{
    EVP_PKEY_CTX *ret;
    const EVP_PKEY_METHOD *pmeth;
    if (id == -1) {
        if (!pkey || !pkey->ameth)
            return NULL;
        id = pkey->ameth->pkey_id;
    }
#ifndef OPENSSL_NO_ENGINE
    if (e == NULL && pkey != NULL)
        e = pkey->engine;
    /* Try to find an ENGINE which implements this method */
    if (e) {
        if (!ENGINE_init(e)) {
            return NULL;
        }
    } else {
        e = ENGINE_get_pkey_meth_engine(id); /* allocation site */
    }

    /*
     * If an ENGINE handled this method look it up. Otherwise use internal
     * tables.
     */

    if (e)
        pmeth = ENGINE_get_pkey_meth(e, id);
    else
#endif
        pmeth = EVP_PKEY_meth_find(id);

    if (pmeth == NULL) {
        /* memory leak */
		EVPerr(EVP_F_INT_CTX_NEW, EVP_R_UNSUPPORTED_ALGORITHM);
        return NULL;
    }

    ret = OPENSSL_zalloc(sizeof(*ret));
    if (ret == NULL) {
#ifndef OPENSSL_NO_ENGINE
        ENGINE_finish(e);
#endif
        EVPerr(EVP_F_INT_CTX_NEW, ERR_R_MALLOC_FAILURE);
        return NULL;
    }
    ret->engine = e;
    ret->pmeth = pmeth;
    ret->operation = EVP_PKEY_OP_UNDEFINED;
    ret->pkey = pkey;
    if (pkey)
        EVP_PKEY_up_ref(pkey);

    if (pmeth->init) {
        if (pmeth->init(ret) <= 0) {
            ret->pmeth = NULL;
            EVP_PKEY_CTX_free(ret);
            return NULL;
        }
    }

    return ret;
}

int init_gen_str(EVP_PKEY_CTX **pctx,
                 const char *algname, ENGINE *e, int do_param)
{
    EVP_PKEY_CTX *ctx = NULL;
    const EVP_PKEY_ASN1_METHOD *ameth;
    ENGINE *tmpeng = NULL;
    int pkey_id;

    if (*pctx) {
        return 0;
    }

    ameth = EVP_PKEY_asn1_find_str(&tmpeng, algname, -1);

#ifndef OPENSSL_NO_ENGINE
    if (!ameth && e)
        ameth = ENGINE_get_pkey_asn1_meth_str(e, algname, -1);
#endif

    if (!ameth) {
        return 0;
    }

    ERR_clear_error();

    EVP_PKEY_asn1_get0_info(&pkey_id, NULL, NULL, NULL, NULL, ameth);
#ifndef OPENSSL_NO_ENGINE
    ENGINE_finish(tmpeng);
#endif
    ctx = EVP_PKEY_CTX_new_id(pkey_id, e);

    if (!ctx)
        goto err;
    if (do_param) {
        if (EVP_PKEY_paramgen_init(ctx) <= 0)
            goto err;
    } else {
        if (EVP_PKEY_keygen_init(ctx) <= 0)
            goto err;
    }

    *pctx = ctx;
    return 1;

 err:
    BIO_printf(bio_err, "Error initializing %s context\n", algname);
    ERR_print_errors(bio_err);
    EVP_PKEY_CTX_free(ctx);
    return 0;

}


EVP_PKEY_CTX *EVP_PKEY_CTX_new(EVP_PKEY *pkey, ENGINE *e)
{
    return int_ctx_new(pkey, e, -1);
}

EVP_PKEY_CTX *EVP_PKEY_CTX_new_id(int id, ENGINE *e)
{
    return int_ctx_new(NULL, e, id);
}


static int init_keygen_file(EVP_PKEY_CTX **pctx, const char *file, ENGINE *e)
{
    BIO *pbio;
    EVP_PKEY *pkey = NULL;
    EVP_PKEY_CTX *ctx = NULL;
    if (*pctx) {
        BIO_puts(bio_err, "Parameters already set!\n");
        return 0;
    }

    pbio = BIO_new_file(file, "r");
    if (!pbio) {
        BIO_printf(bio_err, "Can't open parameter file %s\n", file);
        return 0;
    }

    pkey = PEM_read_bio_Parameters(pbio, NULL);
    BIO_free(pbio);

    if (!pkey) {
        BIO_printf(bio_err, "Error reading parameter file %s\n", file);
        return 0;
    }

    ctx = EVP_PKEY_CTX_new(pkey, e);
    if (ctx == NULL)
        goto err;
    if (EVP_PKEY_keygen_init(ctx) <= 0)
        goto err;
    EVP_PKEY_free(pkey);
    *pctx = ctx;
    return 1;

 err:
    BIO_puts(bio_err, "Error initializing context\n");
    ERR_print_errors(bio_err);
    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(pkey);
    return 0;

}

static int genpkey_cb(EVP_PKEY_CTX *ctx)
{
    char c = '*';
    BIO *b = EVP_PKEY_CTX_get_app_data(ctx);
    int p;
    p = EVP_PKEY_CTX_get_keygen_info(ctx, 0);
    if (p == 0)
        c = '.';
    if (p == 1)
        c = '+';
    if (p == 2)
        c = '*';
    if (p == 3)
        c = '\n';
    BIO_write(b, &c, 1);
    (void)BIO_flush(b);
    return 1;
}
int genpkey_main(int argc, char **argv)
{
    BIO *in = NULL, *out = NULL;
    ENGINE *e = NULL;
    EVP_PKEY *pkey = NULL;
    EVP_PKEY_CTX *ctx = NULL;
    char *outfile = NULL, *passarg = NULL, *pass = NULL, *prog;
    const EVP_CIPHER *cipher = NULL;
    OPTION_CHOICE o;
    int outformat = FORMAT_PEM, text = 0, ret = 1, rv, do_param = 0;
    int private = 0;

    prog = opt_init(argc, argv, genpkey_options);
    while ((o = opt_next()) != OPT_EOF) {
        switch (o) {
        case OPT_EOF:
        case OPT_ERR:
 opthelp:
            BIO_printf(bio_err, "%s: Use -help for summary.\n", prog);
            goto end;
        case OPT_HELP:
            ret = 0;
            opt_help(genpkey_options);
            goto end;
        case OPT_OUTFORM:
            if (!opt_format(opt_arg(), OPT_FMT_PEMDER, &outformat))
                goto opthelp;
            break;
        case OPT_OUT:
            outfile = opt_arg();
            break;
        case OPT_PASS:
            passarg = opt_arg();
            break;
        case OPT_ENGINE:
            e = setup_engine(opt_arg());
            break;
        case OPT_PARAMFILE:
            if (do_param == 1)
                goto opthelp;
            if (!init_keygen_file(&ctx, opt_arg(), e))
                goto end;
            break;
        case OPT_ALGORITHM:
            if (!init_gen_str(&ctx, opt_arg(), e, do_param))
                goto end;
            break;
        case OPT_PKEYOPT:
            if (ctx == NULL) {
                BIO_printf(bio_err, "%s: No keytype specified.\n", prog);
                goto opthelp;
            }
            if (pkey_ctrl_string(ctx, opt_arg()) <= 0) {
                BIO_printf(bio_err,
                           "%s: Error setting %s parameter:\n",
                           prog, opt_arg());
                ERR_print_errors(bio_err);
                goto end;
            }
            break;
        case OPT_GENPARAM:
            if (ctx != NULL)
                goto opthelp;
            do_param = 1;
            break;
        case OPT_TEXT:
            text = 1;
            break;
        case OPT_CIPHER:
            if (!opt_cipher(opt_unknown(), &cipher)
                || do_param == 1)
                goto opthelp;
        }
    }
    argc = opt_num_rest();
    if (argc != 0)
        goto opthelp;

    private = do_param ? 0 : 1;

    if (ctx == NULL)
        goto opthelp;

    if (!app_passwd(passarg, NULL, &pass, NULL)) {
        BIO_puts(bio_err, "Error getting password\n");
        goto end;
    }

    out = bio_open_owner(outfile, outformat, private);
    if (out == NULL)
        goto end;

    EVP_PKEY_CTX_set_cb(ctx, genpkey_cb);
    EVP_PKEY_CTX_set_app_data(ctx, bio_err);

    if (do_param) {
        if (EVP_PKEY_paramgen(ctx, &pkey) <= 0) {
            BIO_puts(bio_err, "Error generating parameters\n");
            ERR_print_errors(bio_err);
            goto end;
        }
    } else {
        if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
            BIO_puts(bio_err, "Error generating key\n");
            ERR_print_errors(bio_err);
            goto end;
        }
    }

    if (do_param) {
        rv = PEM_write_bio_Parameters(out, pkey);
    } else if (outformat == FORMAT_PEM) {
        assert(private);
        rv = PEM_write_bio_PrivateKey(out, pkey, cipher, NULL, 0, NULL, pass);
    } else if (outformat == FORMAT_ASN1) {
        assert(private);
        rv = i2d_PrivateKey_bio(out, pkey);
    } else {
        BIO_printf(bio_err, "Bad format specified for key\n");
        goto end;
    }

    if (rv <= 0) {
        BIO_puts(bio_err, "Error writing key\n");
        ERR_print_errors(bio_err);
    }

    if (text) {
        if (do_param)
            rv = EVP_PKEY_print_params(out, pkey, 0, NULL);
        else
            rv = EVP_PKEY_print_private(out, pkey, 0, NULL);

        if (rv <= 0) {
            BIO_puts(bio_err, "Error printing key\n");
            ERR_print_errors(bio_err);
        }
    }

    ret = 0;

 end:
    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(ctx);
    BIO_free_all(out);
    BIO_free(in);
    release_engine(e);
    OPENSSL_free(pass);
    return ret;
}

int main (int argc, char **argv)
{
    genpkey_main(argc, argv);
    return 0;
}


