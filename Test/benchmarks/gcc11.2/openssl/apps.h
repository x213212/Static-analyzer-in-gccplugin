/*
 * Copyright 1995-2017 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

/*
 * quick macro when you need to pass an unsigned char instead of a char.
 * this is true for some implementations of the is*() functions, for
 * example.
 */
#define _UC(c) ((unsigned char)(c))

/*
void app_RAND_load_conf(CONF *c, const char *section);
void app_RAND_write(void);

extern char *default_config_file;
extern BIO *bio_in;
extern BIO *bio_out;
extern BIO *bio_err;
extern const unsigned char tls13_aes128gcmsha256_id[];
extern const unsigned char tls13_aes256gcmsha384_id[];
BIO *dup_bio_in(int format);
BIO *dup_bio_out(int format);
BIO *dup_bio_err(int format);
BIO *bio_open_owner(const char *filename, int format, int private);
BIO *bio_open_default(const char *filename, char mode, int format);
BIO *bio_open_default_quiet(const char *filename, char mode, int format);
CONF *app_load_config(const char *filename);
CONF *app_load_config_quiet(const char *filename);
int app_load_modules(const CONF *config);
void unbuffer(FILE *fp);
void wait_for_async(SSL *s);

void corrupt_signature(const ASN1_STRING *signature);
int set_cert_times(X509 *x, const char *startdate, const char *enddate,
                   int days);
*/
/*
 * Common verification options.
 */
# define OPT_V_ENUM \
        OPT_V__FIRST=2000, \
        OPT_V_POLICY, OPT_V_PURPOSE, OPT_V_VERIFY_NAME, OPT_V_VERIFY_DEPTH, \
        OPT_V_ATTIME, OPT_V_VERIFY_HOSTNAME, OPT_V_VERIFY_EMAIL, \
        OPT_V_VERIFY_IP, OPT_V_IGNORE_CRITICAL, OPT_V_ISSUER_CHECKS, \
        OPT_V_CRL_CHECK, OPT_V_CRL_CHECK_ALL, OPT_V_POLICY_CHECK, \
        OPT_V_EXPLICIT_POLICY, OPT_V_INHIBIT_ANY, OPT_V_INHIBIT_MAP, \
        OPT_V_X509_STRICT, OPT_V_EXTENDED_CRL, OPT_V_USE_DELTAS, \
        OPT_V_POLICY_PRINT, OPT_V_CHECK_SS_SIG, OPT_V_TRUSTED_FIRST, \
        OPT_V_SUITEB_128_ONLY, OPT_V_SUITEB_128, OPT_V_SUITEB_192, \
        OPT_V_PARTIAL_CHAIN, OPT_V_NO_ALT_CHAINS, OPT_V_NO_CHECK_TIME, \
        OPT_V_VERIFY_AUTH_LEVEL, OPT_V_ALLOW_PROXY_CERTS, \
        OPT_V__LAST

# define OPT_V_OPTIONS \
        { "policy", OPT_V_POLICY, 's', "adds policy to the acceptable policy set"}, \
        { "purpose", OPT_V_PURPOSE, 's', \
            "certificate chain purpose"}, \
        { "verify_name", OPT_V_VERIFY_NAME, 's', "verification policy name"}, \
        { "verify_depth", OPT_V_VERIFY_DEPTH, 'n', \
            "chain depth limit" }, \
        { "auth_level", OPT_V_VERIFY_AUTH_LEVEL, 'n', \
            "chain authentication security level" }, \
        { "attime", OPT_V_ATTIME, 'M', "verification epoch time" }, \
        { "verify_hostname", OPT_V_VERIFY_HOSTNAME, 's', \
            "expected peer hostname" }, \
        { "verify_email", OPT_V_VERIFY_EMAIL, 's', \
            "expected peer email" }, \
        { "verify_ip", OPT_V_VERIFY_IP, 's', \
            "expected peer IP address" }, \
        { "ignore_critical", OPT_V_IGNORE_CRITICAL, '-', \
            "permit unhandled critical extensions"}, \
        { "issuer_checks", OPT_V_ISSUER_CHECKS, '-', "(deprecated)"}, \
        { "crl_check", OPT_V_CRL_CHECK, '-', "check leaf certificate revocation" }, \
        { "crl_check_all", OPT_V_CRL_CHECK_ALL, '-', "check full chain revocation" }, \
        { "policy_check", OPT_V_POLICY_CHECK, '-', "perform rfc5280 policy checks"}, \
        { "explicit_policy", OPT_V_EXPLICIT_POLICY, '-', \
            "set policy variable require-explicit-policy"}, \
        { "inhibit_any", OPT_V_INHIBIT_ANY, '-', \
            "set policy variable inhibit-any-policy"}, \
        { "inhibit_map", OPT_V_INHIBIT_MAP, '-', \
            "set policy variable inhibit-policy-mapping"}, \
        { "x509_strict", OPT_V_X509_STRICT, '-', \
            "disable certificate compatibility work-arounds"}, \
        { "extended_crl", OPT_V_EXTENDED_CRL, '-', \
            "enable extended CRL features"}, \
        { "use_deltas", OPT_V_USE_DELTAS, '-', \
            "use delta CRLs"}, \
        { "policy_print", OPT_V_POLICY_PRINT, '-', \
            "print policy processing diagnostics"}, \
        { "check_ss_sig", OPT_V_CHECK_SS_SIG, '-', \
            "check root CA self-signatures"}, \
        { "trusted_first", OPT_V_TRUSTED_FIRST, '-', \
            "search trust store first (default)" }, \
        { "suiteB_128_only", OPT_V_SUITEB_128_ONLY, '-', "Suite B 128-bit-only mode"}, \
        { "suiteB_128", OPT_V_SUITEB_128, '-', \
            "Suite B 128-bit mode allowing 192-bit algorithms"}, \
        { "suiteB_192", OPT_V_SUITEB_192, '-', "Suite B 192-bit-only mode" }, \
        { "partial_chain", OPT_V_PARTIAL_CHAIN, '-', \
            "accept chains anchored by intermediate trust-store CAs"}, \
        { "no_alt_chains", OPT_V_NO_ALT_CHAINS, '-', "(deprecated)" }, \
        { "no_check_time", OPT_V_NO_CHECK_TIME, '-', "ignore certificate validity time" }, \
        { "allow_proxy_certs", OPT_V_ALLOW_PROXY_CERTS, '-', "allow the use of proxy certificates" }

# define OPT_V_CASES \
        OPT_V__FIRST: case OPT_V__LAST: break; \
        case OPT_V_POLICY: \
        case OPT_V_PURPOSE: \
        case OPT_V_VERIFY_NAME: \
        case OPT_V_VERIFY_DEPTH: \
        case OPT_V_VERIFY_AUTH_LEVEL: \
        case OPT_V_ATTIME: \
        case OPT_V_VERIFY_HOSTNAME: \
        case OPT_V_VERIFY_EMAIL: \
        case OPT_V_VERIFY_IP: \
        case OPT_V_IGNORE_CRITICAL: \
        case OPT_V_ISSUER_CHECKS: \
        case OPT_V_CRL_CHECK: \
        case OPT_V_CRL_CHECK_ALL: \
        case OPT_V_POLICY_CHECK: \
        case OPT_V_EXPLICIT_POLICY: \
        case OPT_V_INHIBIT_ANY: \
        case OPT_V_INHIBIT_MAP: \
        case OPT_V_X509_STRICT: \
        case OPT_V_EXTENDED_CRL: \
        case OPT_V_USE_DELTAS: \
        case OPT_V_POLICY_PRINT: \
        case OPT_V_CHECK_SS_SIG: \
        case OPT_V_TRUSTED_FIRST: \
        case OPT_V_SUITEB_128_ONLY: \
        case OPT_V_SUITEB_128: \
        case OPT_V_SUITEB_192: \
        case OPT_V_PARTIAL_CHAIN: \
        case OPT_V_NO_ALT_CHAINS: \
        case OPT_V_NO_CHECK_TIME: \
        case OPT_V_ALLOW_PROXY_CERTS

/*
 * Common "extended validation" options.
 */
# define OPT_X_ENUM \
        OPT_X__FIRST=1000, \
        OPT_X_KEY, OPT_X_CERT, OPT_X_CHAIN, OPT_X_CHAIN_BUILD, \
        OPT_X_CERTFORM, OPT_X_KEYFORM, \
        OPT_X__LAST

# define OPT_X_OPTIONS \
        { "xkey", OPT_X_KEY, '<', "key for Extended certificates"}, \
        { "xcert", OPT_X_CERT, '<', "cert for Extended certificates"}, \
        { "xchain", OPT_X_CHAIN, '<', "chain for Extended certificates"}, \
        { "xchain_build", OPT_X_CHAIN_BUILD, '-', \
            "build certificate chain for the extended certificates"}, \
        { "xcertform", OPT_X_CERTFORM, 'F', \
            "format of Extended certificate (PEM or DER) PEM default " }, \
        { "xkeyform", OPT_X_KEYFORM, 'F', \
            "format of Extended certificate's key (PEM or DER) PEM default"}

# define OPT_X_CASES \
        OPT_X__FIRST: case OPT_X__LAST: break; \
        case OPT_X_KEY: \
        case OPT_X_CERT: \
        case OPT_X_CHAIN: \
        case OPT_X_CHAIN_BUILD: \
        case OPT_X_CERTFORM: \
        case OPT_X_KEYFORM

/*
 * Common SSL options.
 * Any changes here must be coordinated with ../ssl/ssl_conf.c
 */
# define OPT_S_ENUM \
        OPT_S__FIRST=3000, \
        OPT_S_NOSSL3, OPT_S_NOTLS1, OPT_S_NOTLS1_1, OPT_S_NOTLS1_2, \
        OPT_S_NOTLS1_3, OPT_S_BUGS, OPT_S_NO_COMP, OPT_S_NOTICKET, \
        OPT_S_SERVERPREF, OPT_S_LEGACYRENEG, OPT_S_LEGACYCONN, \
        OPT_S_ONRESUMP, OPT_S_NOLEGACYCONN, OPT_S_ALLOW_NO_DHE_KEX, \
        OPT_S_STRICT, OPT_S_SIGALGS, OPT_S_CLIENTSIGALGS, OPT_S_GROUPS, \
        OPT_S_CURVES, OPT_S_NAMEDCURVE, OPT_S_CIPHER, \
        OPT_S_RECORD_PADDING, OPT_S_DEBUGBROKE, OPT_S_COMP, \
        OPT_S_NO_RENEGOTIATION, OPT_S__LAST

# define OPT_S_OPTIONS \
        {"no_ssl3", OPT_S_NOSSL3, '-',"Just disable SSLv3" }, \
        {"no_tls1", OPT_S_NOTLS1, '-', "Just disable TLSv1"}, \
        {"no_tls1_1", OPT_S_NOTLS1_1, '-', "Just disable TLSv1.1" }, \
        {"no_tls1_2", OPT_S_NOTLS1_2, '-', "Just disable TLSv1.2"}, \
        {"no_tls1_3", OPT_S_NOTLS1_3, '-', "Just disable TLSv1.3"}, \
        {"bugs", OPT_S_BUGS, '-', "Turn on SSL bug compatibility"}, \
        {"no_comp", OPT_S_NO_COMP, '-', "Disable SSL/TLS compression (default)" }, \
        {"comp", OPT_S_COMP, '-', "Use SSL/TLS-level compression" }, \
        {"no_ticket", OPT_S_NOTICKET, '-', \
            "Disable use of TLS session tickets"}, \
        {"serverpref", OPT_S_SERVERPREF, '-', "Use server's cipher preferences"}, \
        {"legacy_renegotiation", OPT_S_LEGACYRENEG, '-', \
            "Enable use of legacy renegotiation (dangerous)"}, \
        {"no_renegotiation", OPT_S_NO_RENEGOTIATION, '-', \
            "Disable all renegotiation."}, \
        {"legacy_server_connect", OPT_S_LEGACYCONN, '-', \
            "Allow initial connection to servers that don't support RI"}, \
        {"no_resumption_on_reneg", OPT_S_ONRESUMP, '-', \
            "Disallow session resumption on renegotiation"}, \
        {"no_legacy_server_connect", OPT_S_NOLEGACYCONN, '-', \
            "Disallow initial connection to servers that don't support RI"}, \
        {"allow_no_dhe_kex", OPT_S_ALLOW_NO_DHE_KEX, '-', \
            "In TLSv1.3 allow non-(ec)dhe based key exchange on resumption"}, \
        {"strict", OPT_S_STRICT, '-', \
            "Enforce strict certificate checks as per TLS standard"}, \
        {"sigalgs", OPT_S_SIGALGS, 's', \
            "Signature algorithms to support (colon-separated list)" }, \
        {"client_sigalgs", OPT_S_CLIENTSIGALGS, 's', \
            "Signature algorithms to support for client certificate" \
            " authentication (colon-separated list)" }, \
        {"groups", OPT_S_GROUPS, 's', \
            "Groups to advertise (colon-separated list)" }, \
        {"curves", OPT_S_CURVES, 's', \
            "Groups to advertise (colon-separated list)" }, \
        {"named_curve", OPT_S_NAMEDCURVE, 's', \
            "Elliptic curve used for ECDHE (server-side only)" }, \
        {"cipher", OPT_S_CIPHER, 's', "Specify cipher list to be used"}, \
        {"record_padding", OPT_S_RECORD_PADDING, 's', \
            "Block size to pad TLS 1.3 records to."}, \
        {"debug_broken_protocol", OPT_S_DEBUGBROKE, '-', \
            "Perform all sorts of protocol violations for testing purposes"}


# define OPT_S_CASES \
        OPT_S__FIRST: case OPT_S__LAST: break; \
        case OPT_S_NOSSL3: \
        case OPT_S_NOTLS1: \
        case OPT_S_NOTLS1_1: \
        case OPT_S_NOTLS1_2: \
        case OPT_S_NOTLS1_3: \
        case OPT_S_BUGS: \
        case OPT_S_NO_COMP: \
        case OPT_S_COMP: \
        case OPT_S_NOTICKET: \
        case OPT_S_SERVERPREF: \
        case OPT_S_LEGACYRENEG: \
        case OPT_S_LEGACYCONN: \
        case OPT_S_ONRESUMP: \
        case OPT_S_NOLEGACYCONN: \
        case OPT_S_ALLOW_NO_DHE_KEX: \
        case OPT_S_STRICT: \
        case OPT_S_SIGALGS: \
        case OPT_S_CLIENTSIGALGS: \
        case OPT_S_GROUPS: \
        case OPT_S_CURVES: \
        case OPT_S_NAMEDCURVE: \
        case OPT_S_CIPHER: \
        case OPT_S_RECORD_PADDING: \
        case OPT_S_NO_RENEGOTIATION: \
        case OPT_S_DEBUGBROKE

#define IS_NO_PROT_FLAG(o) \
 (o == OPT_S_NOSSL3 || o == OPT_S_NOTLS1 || o == OPT_S_NOTLS1_1 \
  || o == OPT_S_NOTLS1_2 || o == OPT_S_NOTLS1_3)

/*
 * Random state options.
 */
# define OPT_R_ENUM \
        OPT_R__FIRST=1500, OPT_R_RAND, OPT_R_WRITERAND, OPT_R__LAST

# define OPT_R_OPTIONS \
    {"rand", OPT_R_RAND, 's', "Load the file(s) into the random number generator"}, \
    {"writerand", OPT_R_WRITERAND, '>', "Write random data to the specified file"}

# define OPT_R_CASES \
        OPT_R__FIRST: case OPT_R__LAST: break; \
        case OPT_R_RAND: case OPT_R_WRITERAND

/*
 * Option parsing.
 */
extern const char OPT_HELP_STR[];
extern const char OPT_MORE_STR[];
typedef struct options_st {
    const char *name;
    int retval;
    /*
     * value type: - no value (also the value zero), n number, p positive
     * number, u unsigned, l long, s string, < input file, > output file,
     * f any format, F der/pem format, E der/pem/engine format identifier.
     * l, n and u include zero; p does not.
     */
    int valtype;
    const char *helpstr;
} OPTIONS;

/*
 * A string/int pairing; widely use for option value lookup, hence the
 * name OPT_PAIR. But that name is misleading in s_cb.c, so we also use
 * the "generic" name STRINT_PAIR.
 */
typedef struct string_int_pair_st {
    const char *name;
    int retval;
} OPT_PAIR, STRINT_PAIR;

/* Flags to pass into opt_format; see FORMAT_xxx, below. */
# define OPT_FMT_PEMDER          (1L <<  1)
# define OPT_FMT_PKCS12          (1L <<  2)
# define OPT_FMT_SMIME           (1L <<  3)
# define OPT_FMT_ENGINE          (1L <<  4)
# define OPT_FMT_MSBLOB          (1L <<  5)
# define OPT_FMT_NETSCAPE        (1L <<  6)
# define OPT_FMT_NSS             (1L <<  7)
# define OPT_FMT_TEXT            (1L <<  8)
# define OPT_FMT_HTTP            (1L <<  9)
# define OPT_FMT_PVK             (1L << 10)
# define OPT_FMT_PDE     (OPT_FMT_PEMDER | OPT_FMT_ENGINE)
# define OPT_FMT_PDS     (OPT_FMT_PEMDER | OPT_FMT_SMIME)
# define OPT_FMT_ANY     ( \
        OPT_FMT_PEMDER | OPT_FMT_PKCS12 | OPT_FMT_SMIME | \
        OPT_FMT_ENGINE | OPT_FMT_MSBLOB | OPT_FMT_NETSCAPE | \
        OPT_FMT_NSS | OPT_FMT_TEXT | OPT_FMT_HTTP | OPT_FMT_PVK)

char *opt_progname(const char *argv0);
char *opt_getprog(void);
char *opt_init(int ac, char **av, const OPTIONS * o);
int opt_next(void);
int opt_format(const char *s, unsigned long flags, int *result);
int opt_int(const char *arg, int *result);
int opt_ulong(const char *arg, unsigned long *result);
int opt_long(const char *arg, long *result);
/*
int opt_pair(const char *arg, const OPT_PAIR * pairs, int *result);
int opt_cipher(const char *name, const EVP_CIPHER **cipherp);
int opt_md(const char *name, const EVP_MD **mdp);
char *opt_arg(void);
char *opt_flag(void);
char *opt_unknown(void);
char **opt_rest(void);
int opt_num_rest(void);
int opt_verify(int i, X509_VERIFY_PARAM *vpm);
int opt_rand(int i);
void opt_help(const OPTIONS * list);
int opt_format_error(const char *s, unsigned long flags);
*/
typedef struct args_st {
    int size;
    int argc;
    char **argv;
} ARGS;

/*
 * VMS C only for now, implemented in vms_decc_init.c
 * If other C compilers forget to terminate argv with NULL, this function
 * can be re-used.
 */
char **copy_argv(int *argc, char *argv[]);
/*
 * Win32-specific argv initialization that splits OS-supplied UNICODE
 * command line string to array of UTF8-encoded strings.
 */
void win32_utf8argv(int *argc, char **argv[]);


# define PW_MIN_LENGTH 4
typedef struct pw_cb_data {
    const void *password;
    const char *prompt_info;
} PW_CB_DATA;

/*
int password_callback(char *buf, int bufsiz, int verify, PW_CB_DATA *cb_data);

int setup_ui_method(void);
void destroy_ui_method(void);
const UI_METHOD *get_ui_method(void);

int chopup_args(ARGS *arg, char *buf);
# ifdef HEADER_X509_H
int dump_cert_text(BIO *out, X509 *x);
void print_name(BIO *out, const char *title, X509_NAME *nm,
                unsigned long lflags);
# endif
void print_bignum_var(BIO *, const BIGNUM *, const char*,
                      int, unsigned char *);
void print_array(BIO *, const char *, int, const unsigned char *);
int set_nameopt(const char *arg);
unsigned long get_nameopt(void);
int set_cert_ex(unsigned long *flags, const char *arg);
int set_name_ex(unsigned long *flags, const char *arg);
int set_ext_copy(int *copy_type, const char *arg);
int copy_extensions(X509 *x, X509_REQ *req, int copy_type);
int app_passwd(const char *arg1, const char *arg2, char **pass1, char **pass2);
int add_oid_section(CONF *conf);
X509 *load_cert(const char *file, int format, const char *cert_descrip);
X509_CRL *load_crl(const char *infile, int format);
EVP_PKEY *load_key(const char *file, int format, int maybe_stdin,
                   const char *pass, ENGINE *e, const char *key_descrip);
EVP_PKEY *load_pubkey(const char *file, int format, int maybe_stdin,
                      const char *pass, ENGINE *e, const char *key_descrip);
int load_certs(const char *file, STACK_OF(X509) **certs, int format,
               const char *pass, const char *cert_descrip);
int load_crls(const char *file, STACK_OF(X509_CRL) **crls, int format,
              const char *pass, const char *cert_descrip);
X509_STORE *setup_verify(const char *CAfile, const char *CApath,
                         int noCAfile, int noCApath);
__owur int ctx_set_verify_locations(SSL_CTX *ctx, const char *CAfile,
                                    const char *CApath, int noCAfile,
                                    int noCApath);

*/

# define DB_type         0
# define DB_exp_date     1
# define DB_rev_date     2
# define DB_serial       3      /* index - unique */
# define DB_file         4
# define DB_name         5      /* index - unique when active and not
                                 * disabled */
# define DB_NUMBER       6

# define DB_TYPE_REV     'R'    /* Revoked  */
# define DB_TYPE_EXP     'E'    /* Expired  */
# define DB_TYPE_VAL     'V'    /* Valid ; inserted with: ca ... -valid */
# define DB_TYPE_SUSP    'S'    /* Suspended  */


typedef struct db_attr_st {
    int unique_subject;
} DB_ATTR;
#define TXT_DB void
typedef struct ca_db_st {
    DB_ATTR attributes;
    TXT_DB *db;
} CA_DB;

void* app_malloc(int sz, const char *what);
/* See OPT_FMT_xxx, above. */
/* On some platforms, it's important to distinguish between text and binary
 * files.  On some, there might even be specific file formats for different
 * contents.  The FORMAT_xxx macros are meant to express an intent with the
 * file being read or created.
 */
# define B_FORMAT_TEXT   0x8000
# define FORMAT_UNDEF    0
# define FORMAT_TEXT    (1 | B_FORMAT_TEXT)     /* Generic text */
# define FORMAT_BINARY   2                      /* Generic binary */
# define FORMAT_BASE64  (3 | B_FORMAT_TEXT)     /* Base64 */
# define FORMAT_ASN1     4                      /* ASN.1/DER */
# define FORMAT_PEM     (5 | B_FORMAT_TEXT)
# define FORMAT_PKCS12   6
# define FORMAT_SMIME   (7 | B_FORMAT_TEXT)
# define FORMAT_ENGINE   8                      /* Not really a file format */
# define FORMAT_PEMRSA  (9 | B_FORMAT_TEXT)     /* PEM RSAPubicKey format */
# define FORMAT_ASN1RSA  10                     /* DER RSAPubicKey format */
# define FORMAT_MSBLOB   11                     /* MS Key blob format */
# define FORMAT_PVK      12                     /* MS PVK file format */
# define FORMAT_HTTP     13                     /* Download using HTTP */
# define FORMAT_NSS      14                     /* NSS keylog format */

# define EXT_COPY_NONE   0
# define EXT_COPY_ADD    1
# define EXT_COPY_ALL    2

# define NETSCAPE_CERT_HDR       "certificate"

# define APP_PASS_LEN    1024

/*
 * IETF RFC 5280 says serial number must be <= 20 bytes. Use 159 bits
 * so that the first bit will never be one, so that the DER encoding
 * rules won't force a leading octet.
 */
# define SERIAL_RAND_BITS        159

int app_isdir(const char *);
int app_access(const char *, int flag);
int fileno_stdin(void);
int fileno_stdout(void);
int raw_read_stdin(void *, int);
int raw_write_stdout(const void *, int);

# define TM_START        0
# define TM_STOP         1
double app_tminterval(int stop, int usertime);

void make_uppercase(char *string);

typedef struct verify_options_st {
    int depth;
    int quiet;
    int error;
    int return_error;
} VERIFY_CB_ARGS;

extern VERIFY_CB_ARGS verify_args;

