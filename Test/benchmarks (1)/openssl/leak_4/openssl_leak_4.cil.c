/* Generated by CIL v. 1.7.3 */
/* print_CIL_Input is true */

#line 216 "/usr/lib/gcc/x86_64-linux-gnu/5/include/stddef.h"
typedef unsigned long size_t;
#line 72 "../stdio.h"
typedef int int32_t___0;
#line 282 "../apps.h"
struct options_st {
   char const   *name ;
   int retval ;
   int valtype ;
   char const   *helpstr ;
};
#line 282 "../apps.h"
typedef struct options_st OPTIONS;
#line 274 "../openssl.h"
struct ASN1_TEMPLATE_st;
#line 274 "../openssl.h"
typedef struct ASN1_TEMPLATE_st ASN1_TEMPLATE;
#line 276
struct ASN1_VALUE_st;
#line 276 "../openssl.h"
typedef struct ASN1_VALUE_st ASN1_VALUE;
#line 278
struct asn1_string_st;
#line 278 "../openssl.h"
typedef struct asn1_string_st ASN1_INTEGER;
#line 279 "../openssl.h"
typedef struct asn1_string_st ASN1_ENUMERATED;
#line 280 "../openssl.h"
typedef struct asn1_string_st ASN1_BIT_STRING;
#line 281 "../openssl.h"
typedef struct asn1_string_st ASN1_OCTET_STRING;
#line 282 "../openssl.h"
typedef struct asn1_string_st ASN1_PRINTABLESTRING;
#line 283 "../openssl.h"
typedef struct asn1_string_st ASN1_T61STRING;
#line 284 "../openssl.h"
typedef struct asn1_string_st ASN1_IA5STRING;
#line 285 "../openssl.h"
typedef struct asn1_string_st ASN1_GENERALSTRING;
#line 286 "../openssl.h"
typedef struct asn1_string_st ASN1_UNIVERSALSTRING;
#line 287 "../openssl.h"
typedef struct asn1_string_st ASN1_BMPSTRING;
#line 288 "../openssl.h"
typedef struct asn1_string_st ASN1_UTCTIME;
#line 290 "../openssl.h"
typedef struct asn1_string_st ASN1_GENERALIZEDTIME;
#line 291 "../openssl.h"
typedef struct asn1_string_st ASN1_VISIBLESTRING;
#line 292 "../openssl.h"
typedef struct asn1_string_st ASN1_UTF8STRING;
#line 293 "../openssl.h"
typedef struct asn1_string_st ASN1_STRING;
#line 294 "../openssl.h"
typedef int ASN1_BOOLEAN;
#line 297
struct asn1_object_st;
#line 297 "../openssl.h"
typedef struct asn1_object_st ASN1_OBJECT;
#line 299
struct ASN1_ITEM_st;
#line 299 "../openssl.h"
typedef struct ASN1_ITEM_st ASN1_ITEM;
#line 300
struct asn1_pctx_st;
#line 300 "../openssl.h"
typedef struct asn1_pctx_st ASN1_PCTX;
#line 303
struct bio_st;
#line 303 "../openssl.h"
typedef struct bio_st BIO;
#line 304
struct bignum_st;
#line 304 "../openssl.h"
typedef struct bignum_st BIGNUM;
#line 305
struct bignum_ctx;
#line 305 "../openssl.h"
typedef struct bignum_ctx BN_CTX;
#line 307
struct bn_mont_ctx_st;
#line 307 "../openssl.h"
typedef struct bn_mont_ctx_st BN_MONT_CTX;
#line 309
struct bn_gencb_st;
#line 309 "../openssl.h"
typedef struct bn_gencb_st BN_GENCB;
#line 313
struct evp_cipher_st;
#line 313 "../openssl.h"
typedef struct evp_cipher_st EVP_CIPHER;
#line 314
struct evp_cipher_ctx_st;
#line 314 "../openssl.h"
typedef struct evp_cipher_ctx_st EVP_CIPHER_CTX;
#line 316
struct evp_md_ctx_st;
#line 316 "../openssl.h"
typedef struct evp_md_ctx_st EVP_MD_CTX;
#line 317
struct evp_pkey_st;
#line 317 "../openssl.h"
typedef struct evp_pkey_st EVP_PKEY;
#line 319
struct evp_pkey_asn1_method_st;
#line 319 "../openssl.h"
typedef struct evp_pkey_asn1_method_st EVP_PKEY_ASN1_METHOD;
#line 321
struct evp_pkey_method_st;
#line 321 "../openssl.h"
typedef struct evp_pkey_method_st EVP_PKEY_METHOD;
#line 322
struct evp_pkey_ctx_st;
#line 322 "../openssl.h"
typedef struct evp_pkey_ctx_st EVP_PKEY_CTX;
#line 328
struct dh_st;
#line 328 "../openssl.h"
typedef struct dh_st DH;
#line 329
struct dh_method;
#line 329 "../openssl.h"
typedef struct dh_method DH_METHOD;
#line 331
struct dsa_st;
#line 334
struct rsa_st;
#line 337
struct ec_key_st;
#line 346
struct X509_algor_st;
#line 346 "../openssl.h"
typedef struct X509_algor_st X509_ALGOR;
#line 351
struct X509_pubkey_st;
#line 351 "../openssl.h"
typedef struct X509_pubkey_st X509_PUBKEY;
#line 360
struct x509_sig_info_st;
#line 360 "../openssl.h"
typedef struct x509_sig_info_st X509_SIG_INFO;
#line 362
struct pkcs8_priv_key_info_st;
#line 362 "../openssl.h"
typedef struct pkcs8_priv_key_info_st PKCS8_PRIV_KEY_INFO;
#line 371
struct engine_st;
#line 371 "../openssl.h"
typedef struct engine_st ENGINE;
#line 388
struct crypto_ex_data_st;
#line 414 "../openssl.h"
typedef int CRYPTO_REF_COUNT;
#line 415 "../openssl.h"
typedef void CRYPTO_RWLOCK;
#line 416 "../openssl.h"
typedef struct crypto_ex_data_st CRYPTO_EX_DATA___0;
#line 419 "../openssl.h"
typedef ASN1_ITEM const   *ASN1_ITEM_EXP(void);
#line 423 "../openssl.h"
struct asn1_string_st {
   int length ;
   int type ;
   unsigned char *data ;
   long flags ;
};
#line 444 "../openssl.h"
struct ASN1_ITEM_st {
   char itype ;
   long utype ;
   ASN1_TEMPLATE const   *templates ;
   long tcount ;
   void const   *funcs ;
   long size ;
   char const   *sname ;
};
#line 456 "../openssl.h"
struct ASN1_TEMPLATE_st {
   unsigned long flags ;
   long tag ;
   unsigned long offset ;
   char const   *field_name ;
   ASN1_ITEM_EXP *item ;
};
#line 464 "../openssl.h"
struct asn1_object_st {
   char const   *sn ;
   char const   *ln ;
   int nid ;
   int length ;
   unsigned char const   *data ;
   int flags ;
};
#line 472 "../openssl.h"
union __anonunion_value_25 {
   char *ptr ;
   ASN1_BOOLEAN boolean ;
   ASN1_STRING *asn1_string ;
   ASN1_OBJECT *object ;
   ASN1_INTEGER *integer ;
   ASN1_ENUMERATED *enumerated ;
   ASN1_BIT_STRING *bit_string ;
   ASN1_OCTET_STRING *octet_string ;
   ASN1_PRINTABLESTRING *printablestring ;
   ASN1_T61STRING *t61string ;
   ASN1_IA5STRING *ia5string ;
   ASN1_GENERALSTRING *generalstring ;
   ASN1_BMPSTRING *bmpstring ;
   ASN1_UNIVERSALSTRING *universalstring ;
   ASN1_UTCTIME *utctime ;
   ASN1_GENERALIZEDTIME *generalizedtime ;
   ASN1_VISIBLESTRING *visiblestring ;
   ASN1_UTF8STRING *utf8string ;
   ASN1_STRING *set ;
   ASN1_STRING *sequence ;
   ASN1_VALUE *asn1_value ;
};
#line 472 "../openssl.h"
struct asn1_type_st {
   int type ;
   union __anonunion_value_25 value ;
};
#line 472 "../openssl.h"
typedef struct asn1_type_st ASN1_TYPE;
#line 509 "../openssl.h"
struct X509_algor_st {
   ASN1_OBJECT *algorithm ;
   ASN1_TYPE *parameter ;
};
#line 644 "../openssl.h"
struct bignum_st {
   unsigned long *d ;
   int top ;
   int dmax ;
   int neg ;
   int flags ;
};
#line 654 "../openssl.h"
struct crypto_ex_data_st {
   void *sk ;
};
#line 658 "../openssl.h"
struct dh_st {
   int pad ;
   int version ;
   BIGNUM *p ;
   BIGNUM *g ;
   int32_t___0 length ;
   BIGNUM *pub_key ;
   BIGNUM *priv_key ;
   int flags ;
   BN_MONT_CTX *method_mont_p ;
   BIGNUM *q ;
   BIGNUM *j ;
   unsigned char *seed ;
   int seedlen ;
   BIGNUM *counter ;
   CRYPTO_REF_COUNT references ;
   CRYPTO_EX_DATA___0 ex_data ;
   DH_METHOD const   *meth ;
   ENGINE *engine ;
   CRYPTO_RWLOCK *lock ;
};
#line 685 "../openssl.h"
struct dh_method {
   char *name ;
   int (*generate_key)(DH *dh ) ;
   int (*compute_key)(unsigned char *key , BIGNUM const   *pub_key , DH *dh ) ;
   int (*bn_mod_exp)(DH const   *dh , BIGNUM *r , BIGNUM const   *a , BIGNUM const   *p ,
                     BIGNUM const   *m , BN_CTX *ctx , BN_MONT_CTX *m_ctx ) ;
   int (*init)(DH *dh ) ;
   int (*finish)(DH *dh ) ;
   int flags ;
   char *app_data ;
   int (*generate_params)(DH *dh , int prime_len , int generator , BN_GENCB *cb ) ;
};
#line 177 "openssl_leak_4.c"
enum OPTION_choice {
    OPT_ERR = -1,
    OPT_EOF = 0,
    OPT_HELP = 1,
    OPT_ENGINE = 2,
    OPT_OUTFORM = 3,
    OPT_OUT = 4,
    OPT_PASS = 5,
    OPT_PARAMFILE = 6,
    OPT_ALGORITHM = 7,
    OPT_PKEYOPT = 8,
    OPT_GENPARAM = 9,
    OPT_TEXT = 10,
    OPT_CIPHER = 11
} ;
#line 177 "openssl_leak_4.c"
typedef enum OPTION_choice OPTION_CHOICE;
#line 204 "openssl_leak_4.c"
typedef int EVP_PKEY_gen_cb(EVP_PKEY_CTX *ctx );
#line 205 "openssl_leak_4.c"
typedef int CRYPTO_REF_COUNT___0;
#line 206 "openssl_leak_4.c"
typedef void CRYPTO_RWLOCK___0;
#line 208 "openssl_leak_4.c"
struct evp_pkey_asn1_method_st {
   int pkey_id ;
   int pkey_base_id ;
   unsigned long pkey_flags ;
   char *pem_str ;
   char *info ;
   int (*pub_decode)(EVP_PKEY *pk , X509_PUBKEY *pub ) ;
   int (*pub_encode)(X509_PUBKEY *pub , EVP_PKEY const   *pk ) ;
   int (*pub_cmp)(EVP_PKEY const   *a , EVP_PKEY const   *b ) ;
   int (*pub_print)(BIO *out , EVP_PKEY const   *pkey , int indent , ASN1_PCTX *pctx ) ;
   int (*priv_decode)(EVP_PKEY *pk , PKCS8_PRIV_KEY_INFO const   *p8inf ) ;
   int (*priv_encode)(PKCS8_PRIV_KEY_INFO *p8 , EVP_PKEY const   *pk ) ;
   int (*priv_print)(BIO *out , EVP_PKEY const   *pkey , int indent , ASN1_PCTX *pctx ) ;
   int (*pkey_size)(EVP_PKEY const   *pk ) ;
   int (*pkey_bits)(EVP_PKEY const   *pk ) ;
   int (*pkey_security_bits)(EVP_PKEY const   *pk ) ;
   int (*param_decode)(EVP_PKEY *pkey , unsigned char const   **pder , int derlen ) ;
   int (*param_encode)(EVP_PKEY const   *pkey , unsigned char **pder ) ;
   int (*param_missing)(EVP_PKEY const   *pk ) ;
   int (*param_copy)(EVP_PKEY *to , EVP_PKEY const   *from ) ;
   int (*param_cmp)(EVP_PKEY const   *a , EVP_PKEY const   *b ) ;
   int (*param_print)(BIO *out , EVP_PKEY const   *pkey , int indent , ASN1_PCTX *pctx ) ;
   int (*sig_print)(BIO *out , X509_ALGOR const   *sigalg , ASN1_STRING const   *sig ,
                    int indent , ASN1_PCTX *pctx ) ;
   void (*pkey_free)(EVP_PKEY *pkey ) ;
   int (*pkey_ctrl)(EVP_PKEY *pkey , int op , long arg1 , void *arg2 ) ;
   int (*old_priv_decode)(EVP_PKEY *pkey , unsigned char const   **pder , int derlen ) ;
   int (*old_priv_encode)(EVP_PKEY const   *pkey , unsigned char **pder ) ;
   int (*item_verify)(EVP_MD_CTX *ctx , ASN1_ITEM const   *it , void *asn , X509_ALGOR *a ,
                      ASN1_BIT_STRING *sig , EVP_PKEY *pkey ) ;
   int (*item_sign)(EVP_MD_CTX *ctx , ASN1_ITEM const   *it , void *asn , X509_ALGOR *alg1 ,
                    X509_ALGOR *alg2 , ASN1_BIT_STRING *sig ) ;
   int (*siginf_set)(X509_SIG_INFO *siginf , X509_ALGOR const   *alg , ASN1_STRING const   *sig ) ;
   int (*pkey_check)(EVP_PKEY const   *pk ) ;
};
#line 256 "openssl_leak_4.c"
union __anonunion_pkey_27 {
   void *ptr ;
   struct rsa_st *rsa ;
   struct dsa_st *dsa ;
   struct dh_st *dh ;
   struct ec_key_st *ec ;
};
#line 256 "openssl_leak_4.c"
struct evp_pkey_st {
   int type ;
   int save_type ;
   CRYPTO_REF_COUNT___0 references ;
   EVP_PKEY_ASN1_METHOD const   *ameth ;
   ENGINE *engine ;
   union __anonunion_pkey_27 pkey ;
   int save_parameters ;
   void *attributes ;
   CRYPTO_RWLOCK___0 *lock ;
};
#line 283 "openssl_leak_4.c"
struct evp_pkey_ctx_st {
   EVP_PKEY_METHOD const   *pmeth ;
   ENGINE *engine ;
   EVP_PKEY *pkey ;
   EVP_PKEY *peerkey ;
   int operation ;
   void *data ;
   void *app_data ;
   EVP_PKEY_gen_cb *pkey_gencb ;
   int *keygen_info ;
   int keygen_info_count ;
};
#line 307 "openssl_leak_4.c"
struct evp_pkey_method_st {
   int pkey_id ;
   int flags ;
   int (*init)(EVP_PKEY_CTX *ctx ) ;
   int (*copy)(EVP_PKEY_CTX *dst , EVP_PKEY_CTX *src ) ;
   void (*cleanup)(EVP_PKEY_CTX *ctx ) ;
   int (*paramgen_init)(EVP_PKEY_CTX *ctx ) ;
   int (*paramgen)(EVP_PKEY_CTX *ctx , EVP_PKEY *pkey ) ;
   int (*keygen_init)(EVP_PKEY_CTX *ctx ) ;
   int (*keygen)(EVP_PKEY_CTX *ctx , EVP_PKEY *pkey ) ;
   int (*sign_init)(EVP_PKEY_CTX *ctx ) ;
   int (*sign)(EVP_PKEY_CTX *ctx , unsigned char *sig , size_t *siglen , unsigned char const   *tbs ,
               size_t tbslen ) ;
   int (*verify_init)(EVP_PKEY_CTX *ctx ) ;
   int (*verify)(EVP_PKEY_CTX *ctx , unsigned char const   *sig , size_t siglen ,
                 unsigned char const   *tbs , size_t tbslen ) ;
   int (*verify_recover_init)(EVP_PKEY_CTX *ctx ) ;
   int (*verify_recover)(EVP_PKEY_CTX *ctx , unsigned char *rout , size_t *routlen ,
                         unsigned char const   *sig , size_t siglen ) ;
   int (*signctx_init)(EVP_PKEY_CTX *ctx , EVP_MD_CTX *mctx ) ;
   int (*signctx)(EVP_PKEY_CTX *ctx , unsigned char *sig , size_t *siglen , EVP_MD_CTX *mctx ) ;
   int (*verifyctx_init)(EVP_PKEY_CTX *ctx , EVP_MD_CTX *mctx ) ;
   int (*verifyctx)(EVP_PKEY_CTX *ctx , unsigned char const   *sig , int siglen ,
                    EVP_MD_CTX *mctx ) ;
   int (*encrypt_init)(EVP_PKEY_CTX *ctx ) ;
   int (*encrypt)(EVP_PKEY_CTX *ctx , unsigned char *out , size_t *outlen , unsigned char const   *in ,
                  size_t inlen ) ;
   int (*decrypt_init)(EVP_PKEY_CTX *ctx ) ;
   int (*decrypt)(EVP_PKEY_CTX *ctx , unsigned char *out , size_t *outlen , unsigned char const   *in ,
                  size_t inlen ) ;
   int (*derive_init)(EVP_PKEY_CTX *ctx ) ;
   int (*derive)(EVP_PKEY_CTX *ctx , unsigned char *key , size_t *keylen ) ;
   int (*ctrl)(EVP_PKEY_CTX *ctx , int type , int p1 , void *p2 ) ;
   int (*ctrl_str)(EVP_PKEY_CTX *ctx , char const   *type , char const   *value ) ;
   int (*digestsign)(EVP_MD_CTX *ctx , unsigned char *sig , size_t *siglen , unsigned char const   *tbs ,
                     size_t tbslen ) ;
   int (*digestverify)(EVP_MD_CTX *ctx , unsigned char const   *sig , size_t siglen ,
                       unsigned char const   *tbs , size_t tbslen ) ;
   int (*check)(EVP_PKEY *pkey ) ;
};
#line 383 "openssl_leak_4.c"
struct evp_cipher_st {
   int nid ;
   int block_size ;
   int key_len ;
   int iv_len ;
   unsigned long flags ;
   int (*init)(EVP_CIPHER_CTX *ctx , unsigned char const   *key , unsigned char const   *iv ,
               int enc ) ;
   int (*do_cipher)(EVP_CIPHER_CTX *ctx , unsigned char *out , unsigned char const   *in ,
                    size_t inl ) ;
   int (*cleanup)(EVP_CIPHER_CTX * ) ;
   int ctx_size ;
   int (*set_asn1_parameters)(EVP_CIPHER_CTX * , ASN1_TYPE * ) ;
   int (*get_asn1_parameters)(EVP_CIPHER_CTX * , ASN1_TYPE * ) ;
   int (*ctrl)(EVP_CIPHER_CTX * , int type , int arg , void *ptr ) ;
   void *app_data ;
};
#line 280 "../apps.h"
extern char const   OPT_HELP_STR[] ;
#line 325
extern char *opt_init(int ac , char **av , OPTIONS const   *o ) ;
#line 326
extern int opt_next(void) ;
#line 327
extern int opt_format(char const   *s , unsigned long flags , int *result ) ;
#line 11 "openssl_leak_4.c"
BIO *bio_err  ;
#line 183 "openssl_leak_4.c"
OPTIONS const   genpkey_options[13]  = 
#line 183
  {      {"help", 1, '-', "Display this summary"}, 
        {"out", 4, '>', "Output file"}, 
        {"outform", 3, 'F', "output format (DER or PEM)"}, 
        {"pass", 5, 's', "Output file pass phrase source"}, 
        {"paramfile", 6, '<', "Parameters file"}, 
        {"algorithm", 7, 's', "The public key algorithm"}, 
        {"pkeyopt", 8, 's', "Set the public key algorithm option as opt:value"}, 
        {"genparam", 9, '-', "Generate parameters, not key"}, 
        {"text", 10, '-', "Print the in text"}, 
        {"", 11, '-', "Cipher to use to encrypt the key"}, 
        {"engine", 2, 's', "Use engine, possibly a hardware device"}, 
        {OPT_HELP_STR, 1, 1, "Order of options may be important!  See the documentation.\n"}, 
        {(char const   *)((void *)0),
      0, 0, (char const   *)0}};
#line 419
extern int ( /* missing proto */  EVP_PKEY_free)() ;
#line 422
extern int ( /* missing proto */  ENGINE_finish)() ;
#line 424
extern int ( /* missing proto */  OPENSSL_free)() ;
#line 413 "openssl_leak_4.c"
void EVP_PKEY_CTX_free(EVP_PKEY_CTX *ctx ) 
{ 


  {
#line 415
  if ((unsigned long )ctx == (unsigned long )((void *)0)) {
#line 416
    return;
  }
#line 417
  if (ctx->pmeth) {
#line 417
    if ((ctx->pmeth)->cleanup) {
#line 418
      (*((ctx->pmeth)->cleanup))(ctx);
    }
  }
#line 419
  EVP_PKEY_free(ctx->pkey);
#line 420
  EVP_PKEY_free(ctx->peerkey);
#line 422
  ENGINE_finish(ctx->engine);
#line 424
  OPENSSL_free(ctx);
#line 425
  return;
}
}
#line 441
extern int ( /* missing proto */  ENGINE_init)() ;
#line 445
extern int ( /* missing proto */  ENGINE_get_pkey_meth_engine)() ;
#line 454
extern int ( /* missing proto */  ENGINE_get_pkey_meth)() ;
#line 457
extern int ( /* missing proto */  EVP_PKEY_meth_find)() ;
#line 463
extern int ( /* missing proto */  EVPerr)() ;
#line 467
extern int ( /* missing proto */  OPENSSL_zalloc)() ;
#line 480
extern int ( /* missing proto */  EVP_PKEY_up_ref)() ;
#line 427 "openssl_leak_4.c"
static EVP_PKEY_CTX *int_ctx_new(EVP_PKEY *pkey , ENGINE *e , int id ) 
{ 
  EVP_PKEY_CTX *ret ;
  EVP_PKEY_METHOD const   *pmeth ;
  int tmp ;
  int tmp___0 ;
  int tmp___1 ;
  int tmp___2 ;
  int tmp___3 ;
  int tmp___4 ;

  {
#line 431
  if (id == -1) {
#line 432
    if (! pkey) {
#line 433
      return ((EVP_PKEY_CTX *)((void *)0));
    } else
#line 432
    if (! pkey->ameth) {
#line 433
      return ((EVP_PKEY_CTX *)((void *)0));
    }
#line 434
    id = (int )(pkey->ameth)->pkey_id;
  }
#line 437
  if ((unsigned long )e == (unsigned long )((void *)0)) {
#line 437
    if ((unsigned long )pkey != (unsigned long )((void *)0)) {
#line 438
      e = pkey->engine;
    }
  }
#line 440
  if (e) {
#line 441
    tmp = ENGINE_init(e);
#line 441
    if (! tmp) {
#line 442
      return ((EVP_PKEY_CTX *)((void *)0));
    }
  } else {
#line 445
    tmp___0 = ENGINE_get_pkey_meth_engine(id);
#line 445
    e = (ENGINE *)tmp___0;
  }
#line 453
  if (e) {
#line 454
    tmp___1 = ENGINE_get_pkey_meth(e, id);
#line 454
    pmeth = (EVP_PKEY_METHOD const   *)tmp___1;
  } else {
#line 457
    tmp___2 = EVP_PKEY_meth_find(id);
#line 457
    pmeth = (EVP_PKEY_METHOD const   *)tmp___2;
  }
#line 459
  if ((unsigned long )pmeth == (unsigned long )((void *)0)) {
#line 461
    ENGINE_finish(e);
#line 463
    EVPerr(157, 156);
#line 464
    return ((EVP_PKEY_CTX *)((void *)0));
  }
#line 467
  tmp___3 = OPENSSL_zalloc(sizeof(*ret));
#line 467
  ret = (EVP_PKEY_CTX *)tmp___3;
#line 468
  if ((unsigned long )ret == (unsigned long )((void *)0)) {
#line 470
    ENGINE_finish(e);
#line 472
    EVPerr(157, 65);
#line 473
    return ((EVP_PKEY_CTX *)((void *)0));
  }
#line 475
  ret->engine = e;
#line 476
  ret->pmeth = pmeth;
#line 477
  ret->operation = 0;
#line 478
  ret->pkey = pkey;
#line 479
  if (pkey) {
#line 480
    EVP_PKEY_up_ref(pkey);
  }
#line 482
  if (pmeth->init) {
#line 483
    tmp___4 = (*(pmeth->init))(ret);
#line 483
    if (tmp___4 <= 0) {
#line 484
      ret->pmeth = (EVP_PKEY_METHOD const   *)((void *)0);
#line 485
      EVP_PKEY_CTX_free(ret);
#line 486
      return ((EVP_PKEY_CTX *)((void *)0));
    }
  }
#line 490
  return (ret);
}
}
#line 505
extern int ( /* missing proto */  EVP_PKEY_asn1_find_str)() ;
#line 509
extern int ( /* missing proto */  ENGINE_get_pkey_asn1_meth_str)() ;
#line 516
extern int ( /* missing proto */  ERR_clear_error)() ;
#line 518
extern int ( /* missing proto */  EVP_PKEY_asn1_get0_info)() ;
#line 522
EVP_PKEY_CTX *EVP_PKEY_CTX_new_id(int id , ENGINE *e ) ;
#line 527
extern int ( /* missing proto */  EVP_PKEY_paramgen_init)() ;
#line 530
extern int ( /* missing proto */  EVP_PKEY_keygen_init)() ;
#line 538
extern int ( /* missing proto */  BIO_printf)() ;
#line 539
extern int ( /* missing proto */  ERR_print_errors)() ;
#line 493 "openssl_leak_4.c"
int init_gen_str(EVP_PKEY_CTX **pctx , char const   *algname , ENGINE *e , int do_param ) 
{ 
  EVP_PKEY_CTX *ctx ;
  EVP_PKEY_ASN1_METHOD const   *ameth ;
  ENGINE *tmpeng ;
  int pkey_id ;
  int tmp ;
  int tmp___0 ;
  int tmp___1 ;
  int tmp___2 ;
  int tmp___3 ;

  {
#line 496
  ctx = (EVP_PKEY_CTX *)((void *)0);
#line 498
  tmpeng = (ENGINE *)((void *)0);
#line 501
  if (*pctx) {
#line 502
    return (0);
  }
#line 505
  tmp = EVP_PKEY_asn1_find_str(& tmpeng, algname, -1);
#line 505
  ameth = (EVP_PKEY_ASN1_METHOD const   *)tmp;
#line 508
  if (! ameth) {
#line 508
    if (e) {
#line 509
      tmp___0 = ENGINE_get_pkey_asn1_meth_str(e, algname, -1);
#line 509
      ameth = (EVP_PKEY_ASN1_METHOD const   *)tmp___0;
    }
  }
#line 512
  if (! ameth) {
#line 513
    return (0);
  }
#line 516
  ERR_clear_error();
#line 518
  EVP_PKEY_asn1_get0_info(& pkey_id, (void *)0, (void *)0, (void *)0, (void *)0, ameth);
#line 520
  ENGINE_finish(tmpeng);
#line 522
  tmp___1 = (int )EVP_PKEY_CTX_new_id(pkey_id, e);
#line 522
  ctx = (EVP_PKEY_CTX *)tmp___1;
#line 524
  if (! ctx) {
#line 525
    goto err;
  }
#line 526
  if (do_param) {
#line 527
    tmp___2 = EVP_PKEY_paramgen_init(ctx);
#line 527
    if (tmp___2 <= 0) {
#line 528
      goto err;
    }
  } else {
#line 530
    tmp___3 = EVP_PKEY_keygen_init(ctx);
#line 530
    if (tmp___3 <= 0) {
#line 531
      goto err;
    }
  }
#line 534
  *pctx = ctx;
#line 535
  return (1);
  err: 
#line 538
  BIO_printf(bio_err, "Error initializing %s context\n", algname);
#line 539
  ERR_print_errors(bio_err);
#line 540
  EVP_PKEY_CTX_free(ctx);
#line 541
  return (0);
}
}
#line 546 "openssl_leak_4.c"
EVP_PKEY_CTX *EVP_PKEY_CTX_new(EVP_PKEY *pkey , ENGINE *e ) 
{ 
  EVP_PKEY_CTX *tmp ;

  {
#line 548
  tmp = int_ctx_new(pkey, e, -1);
#line 548
  return (tmp);
}
}
#line 551 "openssl_leak_4.c"
EVP_PKEY_CTX *EVP_PKEY_CTX_new_id(int id , ENGINE *e ) 
{ 
  EVP_PKEY_CTX *tmp ;

  {
#line 553
  tmp = int_ctx_new((EVP_PKEY *)((void *)0), e, id);
#line 553
  return (tmp);
}
}
#line 563
extern int ( /* missing proto */  BIO_puts)() ;
#line 567
extern int ( /* missing proto */  BIO_new_file)() ;
#line 573
extern int ( /* missing proto */  PEM_read_bio_Parameters)() ;
#line 574
extern int ( /* missing proto */  BIO_free)() ;
#line 557 "openssl_leak_4.c"
static int init_keygen_file(EVP_PKEY_CTX **pctx , char const   *file , ENGINE *e ) 
{ 
  BIO *pbio ;
  EVP_PKEY *pkey ;
  EVP_PKEY_CTX *ctx ;
  int tmp ;
  int tmp___0 ;
  int tmp___1 ;

  {
#line 560
  pkey = (EVP_PKEY *)((void *)0);
#line 561
  ctx = (EVP_PKEY_CTX *)((void *)0);
#line 562
  if (*pctx) {
#line 563
    BIO_puts(bio_err, "Parameters already set!\n");
#line 564
    return (0);
  }
#line 567
  tmp = BIO_new_file(file, "r");
#line 567
  pbio = (BIO *)tmp;
#line 568
  if (! pbio) {
#line 569
    BIO_printf(bio_err, "Can\'t open parameter file %s\n", file);
#line 570
    return (0);
  }
#line 573
  tmp___0 = PEM_read_bio_Parameters(pbio, (void *)0);
#line 573
  pkey = (EVP_PKEY *)tmp___0;
#line 574
  BIO_free(pbio);
#line 576
  if (! pkey) {
#line 577
    BIO_printf(bio_err, "Error reading parameter file %s\n", file);
#line 578
    return (0);
  }
#line 581
  ctx = EVP_PKEY_CTX_new(pkey, e);
#line 582
  if ((unsigned long )ctx == (unsigned long )((void *)0)) {
#line 583
    goto err;
  }
#line 584
  tmp___1 = EVP_PKEY_keygen_init(ctx);
#line 584
  if (tmp___1 <= 0) {
#line 585
    goto err;
  }
#line 586
  EVP_PKEY_free(pkey);
#line 587
  *pctx = ctx;
#line 588
  return (1);
  err: 
#line 591
  BIO_puts(bio_err, "Error initializing context\n");
#line 592
  ERR_print_errors(bio_err);
#line 593
  EVP_PKEY_CTX_free(ctx);
#line 594
  EVP_PKEY_free(pkey);
#line 595
  return (0);
}
}
#line 602
extern int ( /* missing proto */  EVP_PKEY_CTX_get_app_data)() ;
#line 604
extern int ( /* missing proto */  EVP_PKEY_CTX_get_keygen_info)() ;
#line 613
extern int ( /* missing proto */  BIO_write)() ;
#line 614
extern int ( /* missing proto */  BIO_flush)() ;
#line 599 "openssl_leak_4.c"
static int genpkey_cb(EVP_PKEY_CTX *ctx ) 
{ 
  char c ;
  BIO *b ;
  int tmp ;
  int p ;

  {
#line 601
  c = (char )'*';
#line 602
  tmp = EVP_PKEY_CTX_get_app_data(ctx);
#line 602
  b = (BIO *)tmp;
#line 604
  p = EVP_PKEY_CTX_get_keygen_info(ctx, 0);
#line 605
  if (p == 0) {
#line 606
    c = (char )'.';
  }
#line 607
  if (p == 1) {
#line 608
    c = (char )'+';
  }
#line 609
  if (p == 2) {
#line 610
    c = (char )'*';
  }
#line 611
  if (p == 3) {
#line 612
    c = (char )'\n';
  }
#line 613
  BIO_write(b, & c, 1);
#line 614
  BIO_flush(b);
#line 615
  return (1);
}
}
#line 639
extern int ( /* missing proto */  opt_help)() ;
#line 642
extern int ( /* missing proto */  opt_arg)() ;
#line 652
extern int ( /* missing proto */  setup_engine)() ;
#line 669
extern int ( /* missing proto */  pkey_ctrl_string)() ;
#line 686
extern int ( /* missing proto */  opt_cipher)() ;
#line 686
extern int ( /* missing proto */  opt_unknown)() ;
#line 691
extern int ( /* missing proto */  opt_num_rest)() ;
#line 700
extern int ( /* missing proto */  app_passwd)() ;
#line 705
extern int ( /* missing proto */  bio_open_owner)() ;
#line 709
extern int ( /* missing proto */  EVP_PKEY_CTX_set_cb)() ;
#line 710
extern int ( /* missing proto */  EVP_PKEY_CTX_set_app_data)() ;
#line 713
extern int ( /* missing proto */  EVP_PKEY_paramgen)() ;
#line 719
extern int ( /* missing proto */  EVP_PKEY_keygen)() ;
#line 727
extern int ( /* missing proto */  PEM_write_bio_Parameters)() ;
#line 729
extern int ( /* missing proto */  assert)() ;
#line 730
extern int ( /* missing proto */  PEM_write_bio_PrivateKey)() ;
#line 733
extern int ( /* missing proto */  i2d_PrivateKey_bio)() ;
#line 746
extern int ( /* missing proto */  EVP_PKEY_print_params)() ;
#line 748
extern int ( /* missing proto */  EVP_PKEY_print_private)() ;
#line 761
extern int ( /* missing proto */  BIO_free_all)() ;
#line 763
extern int ( /* missing proto */  release_engine)() ;
#line 617 "openssl_leak_4.c"
int genpkey_main(int argc , char **argv ) 
{ 
  BIO *in ;
  BIO *out ;
  ENGINE *e ;
  EVP_PKEY *pkey ;
  EVP_PKEY_CTX *ctx ;
  char *outfile ;
  char *passarg ;
  char *pass ;
  char *prog ;
  EVP_CIPHER const   *cipher ;
  OPTION_CHOICE o ;
  int outformat ;
  int text ;
  int ret ;
  int rv ;
  int do_param ;
  int private ;
  int tmp ;
  int tmp___0 ;
  int tmp___1 ;
  int tmp___2 ;
  int tmp___3 ;
  int tmp___4 ;
  int tmp___5 ;
  int tmp___6 ;
  int tmp___7 ;
  int tmp___8 ;
  int tmp___9 ;
  int tmp___10 ;
  int tmp___11 ;
  int tmp___12 ;
  int tmp___13 ;
  int tmp___14 ;
  int tmp___15 ;
  int tmp___16 ;
  int tmp___17 ;
  int tmp___18 ;

  {
#line 619
  in = (BIO *)((void *)0);
#line 619
  out = (BIO *)((void *)0);
#line 620
  e = (ENGINE *)((void *)0);
#line 621
  pkey = (EVP_PKEY *)((void *)0);
#line 622
  ctx = (EVP_PKEY_CTX *)((void *)0);
#line 623
  outfile = (char *)((void *)0);
#line 623
  passarg = (char *)((void *)0);
#line 623
  pass = (char *)((void *)0);
#line 624
  cipher = (EVP_CIPHER const   *)((void *)0);
#line 626
  outformat = 32773;
#line 626
  text = 0;
#line 626
  ret = 1;
#line 626
  do_param = 0;
#line 627
  private = 0;
#line 629
  prog = opt_init(argc, argv, genpkey_options);
#line 630
  while (1) {
#line 630
    tmp___14 = opt_next();
#line 630
    o = (OPTION_CHOICE )tmp___14;
#line 630
    if (! ((int )o != 0)) {
#line 630
      break;
    }
#line 631
    switch ((int )o) {
    opthelp: 
    case -1: 
    case 0: 
#line 635
    BIO_printf(bio_err, "%s: Use -help for summary.\n", prog);
#line 636
    goto end;
    case 1: 
#line 638
    ret = 0;
#line 639
    opt_help(genpkey_options);
#line 640
    goto end;
    case 3: 
#line 642
    tmp = opt_arg();
#line 642
    tmp___0 = opt_format((char const   *)tmp, (unsigned long )(1L << 1), & outformat);
#line 642
    if (! tmp___0) {
#line 643
      goto opthelp;
    }
#line 644
    break;
    case 4: 
#line 646
    tmp___1 = opt_arg();
#line 646
    outfile = (char *)tmp___1;
#line 647
    break;
    case 5: 
#line 649
    tmp___2 = opt_arg();
#line 649
    passarg = (char *)tmp___2;
#line 650
    break;
    case 2: 
#line 652
    tmp___3 = opt_arg();
#line 652
    tmp___4 = setup_engine(tmp___3, 0);
#line 652
    e = (ENGINE *)tmp___4;
#line 653
    break;
    case 6: 
#line 655
    if (do_param == 1) {
#line 656
      goto opthelp;
    }
#line 657
    tmp___5 = opt_arg();
#line 657
    tmp___6 = init_keygen_file(& ctx, (char const   *)tmp___5, e);
#line 657
    if (! tmp___6) {
#line 658
      goto end;
    }
#line 659
    break;
    case 7: 
#line 661
    tmp___7 = opt_arg();
#line 661
    tmp___8 = init_gen_str(& ctx, (char const   *)tmp___7, e, do_param);
#line 661
    if (! tmp___8) {
#line 662
      goto end;
    }
#line 663
    break;
    case 8: 
#line 665
    if ((unsigned long )ctx == (unsigned long )((void *)0)) {
#line 666
      BIO_printf(bio_err, "%s: No keytype specified.\n", prog);
#line 667
      goto opthelp;
    }
#line 669
    tmp___10 = opt_arg();
#line 669
    tmp___11 = pkey_ctrl_string(ctx, tmp___10);
#line 669
    if (tmp___11 <= 0) {
#line 670
      tmp___9 = opt_arg();
#line 670
      BIO_printf(bio_err, "%s: Error setting %s parameter:\n", prog, tmp___9);
#line 673
      ERR_print_errors(bio_err);
#line 674
      goto end;
    }
#line 676
    break;
    case 9: 
#line 678
    if ((unsigned long )ctx != (unsigned long )((void *)0)) {
#line 679
      goto opthelp;
    }
#line 680
    do_param = 1;
#line 681
    break;
    case 10: 
#line 683
    text = 1;
#line 684
    break;
    case 11: 
#line 686
    tmp___12 = opt_unknown();
#line 686
    tmp___13 = opt_cipher(tmp___12, & cipher);
#line 686
    if (tmp___13) {
#line 686
      if (do_param == 1) {
#line 688
        goto opthelp;
      }
    } else {
#line 688
      goto opthelp;
    }
    }
  }
#line 691
  argc = opt_num_rest();
#line 692
  if (argc != 0) {
#line 693
    goto opthelp;
  }
#line 695
  if (do_param) {
#line 695
    private = 0;
  } else {
#line 695
    private = 1;
  }
#line 697
  if ((unsigned long )ctx == (unsigned long )((void *)0)) {
#line 698
    goto opthelp;
  }
#line 700
  tmp___15 = app_passwd(passarg, (void *)0, & pass, (void *)0);
#line 700
  if (! tmp___15) {
#line 701
    BIO_puts(bio_err, "Error getting password\n");
#line 702
    goto end;
  }
#line 705
  tmp___16 = bio_open_owner(outfile, outformat, private);
#line 705
  out = (BIO *)tmp___16;
#line 706
  if ((unsigned long )out == (unsigned long )((void *)0)) {
#line 707
    goto end;
  }
#line 709
  EVP_PKEY_CTX_set_cb(ctx, & genpkey_cb);
#line 710
  EVP_PKEY_CTX_set_app_data(ctx, bio_err);
#line 712
  if (do_param) {
#line 713
    tmp___17 = EVP_PKEY_paramgen(ctx, & pkey);
#line 713
    if (tmp___17 <= 0) {
#line 714
      BIO_puts(bio_err, "Error generating parameters\n");
#line 715
      ERR_print_errors(bio_err);
#line 716
      goto end;
    }
  } else {
#line 719
    tmp___18 = EVP_PKEY_keygen(ctx, & pkey);
#line 719
    if (tmp___18 <= 0) {
#line 720
      BIO_puts(bio_err, "Error generating key\n");
#line 721
      ERR_print_errors(bio_err);
#line 722
      goto end;
    }
  }
#line 726
  if (do_param) {
#line 727
    rv = PEM_write_bio_Parameters(out, pkey);
  } else
#line 728
  if (outformat == 32773) {
#line 729
    assert(private);
#line 730
    rv = PEM_write_bio_PrivateKey(out, pkey, cipher, (void *)0, 0, (void *)0, pass);
  } else
#line 731
  if (outformat == 4) {
#line 732
    assert(private);
#line 733
    rv = i2d_PrivateKey_bio(out, pkey);
  } else {
#line 735
    BIO_printf(bio_err, "Bad format specified for key\n");
#line 736
    goto end;
  }
#line 739
  if (rv <= 0) {
#line 740
    BIO_puts(bio_err, "Error writing key\n");
#line 741
    ERR_print_errors(bio_err);
  }
#line 744
  if (text) {
#line 745
    if (do_param) {
#line 746
      rv = EVP_PKEY_print_params(out, pkey, 0, (void *)0);
    } else {
#line 748
      rv = EVP_PKEY_print_private(out, pkey, 0, (void *)0);
    }
#line 750
    if (rv <= 0) {
#line 751
      BIO_puts(bio_err, "Error printing key\n");
#line 752
      ERR_print_errors(bio_err);
    }
  }
#line 756
  ret = 0;
  end: 
#line 759
  EVP_PKEY_free(pkey);
#line 760
  EVP_PKEY_CTX_free(ctx);
#line 761
  BIO_free_all(out);
#line 762
  BIO_free(in);
#line 763
  release_engine(e);
#line 764
  OPENSSL_free(pass);
#line 765
  return (ret);
}
}
#line 768 "openssl_leak_4.c"
int main(int argc , char **argv ) 
{ 


  {
#line 770
  genpkey_main(argc, argv);
#line 771
  return (0);
}
}