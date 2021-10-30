/*
    buggy parent : b6705d4 
    commit id : 44f19af7434cdb996f1ce11789150baa07db27e6
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../openssl.h"

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

# define SSL_ST_CONNECT                  0x1000
# define SSL_ST_ACCEPT                   0x2000

# define SSL_ST_MASK                     0x0FFF

# define SSL_CB_LOOP                     0x01
# define SSL_CB_EXIT                     0x02
# define SSL_CB_READ                     0x04
# define SSL_CB_WRITE                    0x08
# define SSL_CB_ALERT                    0x4000/* used in callback */
# define SSL_CB_READ_ALERT               (SSL_CB_ALERT|SSL_CB_READ)
# define SSL_CB_WRITE_ALERT              (SSL_CB_ALERT|SSL_CB_WRITE)
# define SSL_CB_ACCEPT_LOOP              (SSL_ST_ACCEPT|SSL_CB_LOOP)
# define SSL_CB_ACCEPT_EXIT              (SSL_ST_ACCEPT|SSL_CB_EXIT)
# define SSL_CB_CONNECT_LOOP             (SSL_ST_CONNECT|SSL_CB_LOOP)
# define SSL_CB_CONNECT_EXIT             (SSL_ST_CONNECT|SSL_CB_EXIT)
# define SSL_CB_HANDSHAKE_START          0x10
# define SSL_CB_HANDSHAKE_DONE           0x20
/*
 * use either SSL_VERIFY_NONE or SSL_VERIFY_PEER, the last 2 options are
 * 'ored' with SSL_VERIFY_PEER if they are desired
 */
# define SSL_VERIFY_NONE                 0x00
# define SSL_VERIFY_PEER                 0x01
# define SSL_VERIFY_FAIL_IF_NO_PEER_CERT 0x02
# define SSL_VERIFY_CLIENT_ONCE          0x04



# define NETSCAPE_CERT_HDR       "certificate"

# define APP_PASS_LEN    1024

typedef enum {
    TLS_ST_BEFORE,
    TLS_ST_OK,
    DTLS_ST_CR_HELLO_VERIFY_REQUEST,
    TLS_ST_CR_SRVR_HELLO,
    TLS_ST_CR_CERT,
    TLS_ST_CR_CERT_STATUS,
    TLS_ST_CR_KEY_EXCH,
    TLS_ST_CR_CERT_REQ,
    TLS_ST_CR_SRVR_DONE,
    TLS_ST_CR_SESSION_TICKET,
    TLS_ST_CR_CHANGE,
    TLS_ST_CR_FINISHED,
    TLS_ST_CW_CLNT_HELLO,
    TLS_ST_CW_CERT,
    TLS_ST_CW_KEY_EXCH,
    TLS_ST_CW_CERT_VRFY,
    TLS_ST_CW_CHANGE,
    TLS_ST_CW_NEXT_PROTO,
    TLS_ST_CW_FINISHED,
    TLS_ST_SW_HELLO_REQ,
    TLS_ST_SR_CLNT_HELLO,
    DTLS_ST_SW_HELLO_VERIFY_REQUEST,
    TLS_ST_SW_SRVR_HELLO,
    TLS_ST_SW_CERT,
    TLS_ST_SW_KEY_EXCH,
    TLS_ST_SW_CERT_REQ,
    TLS_ST_SW_SRVR_DONE,
    TLS_ST_SR_CERT,
    TLS_ST_SR_KEY_EXCH,
    TLS_ST_SR_CERT_VRFY,
    TLS_ST_SR_NEXT_PROTO,
    TLS_ST_SR_CHANGE,
    TLS_ST_SR_FINISHED,
    TLS_ST_SW_SESSION_TICKET,
    TLS_ST_SW_CERT_STATUS,
    TLS_ST_SW_CHANGE,
    TLS_ST_SW_FINISHED,
    TLS_ST_SW_ENCRYPTED_EXTENSIONS,
    TLS_ST_CR_ENCRYPTED_EXTENSIONS,
    TLS_ST_CR_CERT_VRFY,
    TLS_ST_SW_CERT_VRFY,
    TLS_ST_CR_HELLO_REQ,
    TLS_ST_SW_HELLO_RETRY_REQUEST,
    TLS_ST_CR_HELLO_RETRY_REQUEST,
    TLS_ST_SW_KEY_UPDATE,
    TLS_ST_CW_KEY_UPDATE,
    TLS_ST_SR_KEY_UPDATE,
    TLS_ST_CR_KEY_UPDATE,
    TLS_ST_EARLY_DATA,
    TLS_ST_PENDING_EARLY_DATA_END,
    TLS_ST_CW_END_OF_EARLY_DATA,
    TLS_ST_SR_END_OF_EARLY_DATA
} OSSL_HANDSHAKE_STATE;

/*
 * Most of the following state values are no longer used and are defined to be
 * the closest equivalent value in the current state machine code. Not all
 * defines have an equivalent and are set to a dummy value (-1). SSL_ST_CONNECT
 * and SSL_ST_ACCEPT are still in use in the definition of SSL_CB_ACCEPT_LOOP,
 * SSL_CB_ACCEPT_EXIT, SSL_CB_CONNECT_LOOP and SSL_CB_CONNECT_EXIT.
 */

# define SSL_ST_CONNECT                  0x1000
# define SSL_ST_ACCEPT                   0x2000

# define SSL_ST_MASK                     0x0FFF

# define SSL_CB_LOOP                     0x01
# define SSL_CB_EXIT                     0x02
# define SSL_CB_READ                     0x04
# define SSL_CB_WRITE                    0x08
# define SSL_CB_ALERT                    0x4000/* used in callback */
# define SSL_CB_READ_ALERT               (SSL_CB_ALERT|SSL_CB_READ)
# define SSL_CB_WRITE_ALERT              (SSL_CB_ALERT|SSL_CB_WRITE)
# define SSL_CB_ACCEPT_LOOP              (SSL_ST_ACCEPT|SSL_CB_LOOP)
# define SSL_CB_ACCEPT_EXIT              (SSL_ST_ACCEPT|SSL_CB_EXIT)
# define SSL_CB_CONNECT_LOOP             (SSL_ST_CONNECT|SSL_CB_LOOP)
# define SSL_CB_CONNECT_EXIT             (SSL_ST_CONNECT|SSL_CB_EXIT)
# define SSL_CB_HANDSHAKE_START          0x10
# define SSL_CB_HANDSHAKE_DONE           0x20



#define PORT            "4433"
#define PROTOCOL        "tcp"


typedef enum OPTION_choice {
    OPT_ERR = -1, OPT_EOF = 0, OPT_HELP,
    OPT_4, OPT_6, OPT_HOST, OPT_PORT, OPT_CONNECT, OPT_UNIX,
    OPT_XMPPHOST, OPT_VERIFY, OPT_NAMEOPT,
    OPT_CERT, OPT_CRL, OPT_CRL_DOWNLOAD, OPT_SESS_OUT, OPT_SESS_IN,
    OPT_CERTFORM, OPT_CRLFORM, OPT_VERIFY_RET_ERROR, OPT_VERIFY_QUIET,
    OPT_BRIEF, OPT_PREXIT, OPT_CRLF, OPT_QUIET, OPT_NBIO,
    OPT_SSL_CLIENT_ENGINE, OPT_IGN_EOF, OPT_NO_IGN_EOF,
    OPT_DEBUG, OPT_TLSEXTDEBUG, OPT_STATUS, OPT_WDEBUG,
    OPT_MSG, OPT_MSGFILE, OPT_ENGINE, OPT_TRACE, OPT_SECURITY_DEBUG,
    OPT_SECURITY_DEBUG_VERBOSE, OPT_SHOWCERTS, OPT_NBIO_TEST, OPT_STATE,
    OPT_PSK_IDENTITY, OPT_PSK, OPT_PSK_SESS,
#ifndef OPENSSL_NO_SRP
    OPT_SRPUSER, OPT_SRPPASS, OPT_SRP_STRENGTH, OPT_SRP_LATEUSER,
    OPT_SRP_MOREGROUPS,
#endif
    OPT_SSL3, OPT_SSL_CONFIG,
    OPT_TLS1_3, OPT_TLS1_2, OPT_TLS1_1, OPT_TLS1, OPT_DTLS, OPT_DTLS1,
    OPT_DTLS1_2, OPT_SCTP, OPT_TIMEOUT, OPT_MTU, OPT_KEYFORM, OPT_PASS,
    OPT_CERT_CHAIN, OPT_CAPATH, OPT_NOCAPATH, OPT_CHAINCAPATH, OPT_VERIFYCAPATH,
    OPT_KEY, OPT_RECONNECT, OPT_BUILD_CHAIN, OPT_CAFILE, OPT_NOCAFILE,
    OPT_CHAINCAFILE, OPT_VERIFYCAFILE, OPT_NEXTPROTONEG, OPT_ALPN,
    OPT_SERVERINFO, OPT_STARTTLS, OPT_SERVERNAME, OPT_NOSERVERNAME, OPT_ASYNC,
    OPT_USE_SRTP, OPT_KEYMATEXPORT, OPT_KEYMATEXPORTLEN, OPT_PROTOHOST,
    OPT_MAXFRAGLEN, OPT_MAX_SEND_FRAG, OPT_SPLIT_SEND_FRAG, OPT_MAX_PIPELINES,
    OPT_READ_BUF, OPT_KEYLOG_FILE, OPT_EARLY_DATA, OPT_REQCAFILE,
    OPT_V_ENUM,
    OPT_X_ENUM,
    OPT_S_ENUM,
    OPT_FALLBACKSCSV, OPT_NOCMDS, OPT_PROXY, OPT_DANE_TLSA_DOMAIN,
#ifndef OPENSSL_NO_CT
    OPT_CT, OPT_NOCT, OPT_CTLOG_FILE,
#endif
    OPT_DANE_TLSA_RRDATA, OPT_DANE_EE_NO_NAME,
    OPT_R_ENUM
} OPTION_CHOICE;

typedef enum PROTOCOL_choice {
    PROTO_OFF,
    PROTO_SMTP,
    PROTO_POP3,
    PROTO_IMAP,
    PROTO_FTP,
    PROTO_TELNET,
    PROTO_XMPP,
    PROTO_XMPP_SERVER,
    PROTO_CONNECT,
    PROTO_IRC,
    PROTO_MYSQL,
    PROTO_POSTGRES,
    PROTO_LMTP,
    PROTO_NNTP,
    PROTO_SIEVE,
    PROTO_LDAP
} PROTOCOL_CHOICE;


static char *prog;
static int c_debug = 0;
static int c_showcerts = 0;
static char *keymatexportlabel = NULL;
static int keymatexportlen = 20;
static BIO *bio_c_out = NULL;
static int c_quiet = 0;
static char *sess_out = NULL;
static SSL_SESSION *psksess = NULL;

BIO *bio_in = NULL;
BIO *bio_out = NULL;
BIO *bio_err = NULL;



/* Free |*dest| and optionally set it to a copy of |source|. */
static void freeandcopy(char **dest, const char *source)
{
    OPENSSL_free(*dest);
    *dest = NULL;
    if (source != NULL)
        *dest = OPENSSL_strdup(source);
}



int main(int argc, char **argv)
{
    SSL *con = NULL;
    SSL_CONF_CTX *cctx = NULL;
    char *dane_tlsa_domain = NULL;
    int dane_ee_no_name = 0;
    const char *CApath = NULL, *CAfile = NULL;
    char *cbuf = NULL, *sbuf = NULL;
    char *mbuf = NULL, *proxystr = NULL, *connectstr = NULL;
    char *cert_file = NULL, *key_file = NULL, *chain_file = NULL;
    char *chCApath = NULL, *chCAfile = NULL, *host = NULL;
    char *port = OPENSSL_strdup(PORT);
    char *passarg = NULL, *pass = NULL, *vfyCApath = NULL, *vfyCAfile = NULL;
    char *ReqCAfile = NULL;
    char *sess_in = NULL, *crl_file = NULL, *p;
    const char *protohost = NULL;
    struct timeval timeout, *timeoutp;
    fd_set readfds, writefds;
    int noCApath = 0, noCAfile = 0;
    int build_chain = 0, cbuf_len, cbuf_off, cert_format = FORMAT_PEM;
    int key_format = FORMAT_PEM, crlf = 0, full_log = 1, mbuf_len = 0;
    int prexit = 0;
    int sdebug = 0;
    int reconnect = 0, verify = SSL_VERIFY_NONE, vpmtouched = 0;
    int ret = 1, in_init = 1, i, nbio_test = 0, s = -1, k, width, state = 0;
    int sbuf_len, sbuf_off, cmdletters = 1;
    int starttls_proto = PROTO_OFF, crl_format = FORMAT_PEM, crl_download = 0;
    int write_tty, read_tty, write_ssl, read_ssl, tty_on, ssl_pending;
#if !defined(OPENSSL_SYS_WINDOWS) && !defined(OPENSSL_SYS_MSDOS)
    int at_eof = 0;
#endif
    int read_buf_len = 0;
    int fallback_scsv = 0;
    OPTION_CHOICE o;
#ifndef OPENSSL_NO_DTLS
    int enable_timeouts = 0;
    long socket_mtu = 0;
#endif
#if defined(OPENSSL_SYS_WINDOWS) || defined(OPENSSL_SYS_MSDOS)
    struct timeval tv;
#endif
    char *servername = NULL;
    int noservername = 0;
    const char *alpn_in = NULL;
    const char *ssl_config = NULL;
#define MAX_SI_TYPES 100
    unsigned short serverinfo_types[MAX_SI_TYPES];
    int serverinfo_count = 0, start = 0, len;
#ifndef OPENSSL_NO_NEXTPROTONEG
    const char *next_proto_neg_in = NULL;
#endif
#ifndef OPENSSL_NO_SRP
    char *srppass = NULL;
    int srp_lateuser = 0;
#endif
#ifndef OPENSSL_NO_CT
    char *ctlog_file = NULL;
    int ct_validation = 0;
#endif
    int min_version = 0, max_version = 0, prot_opt = 0, no_prot_opt = 0;
    int async = 0;
    unsigned int max_send_fragment = 0;
    unsigned int split_send_fragment = 0, max_pipelines = 0;
    enum { use_inet, use_unix, use_unknown } connect_type = use_unknown;
    int count4or6 = 0;
    int maxfraglen = 0;
    int c_nbio = 0, c_msg = 0, c_ign_eof = 0, c_brief = 0;
    int c_tlsextdebug = 0;
#ifndef OPENSSL_NO_OCSP
    int c_status_req = 0;
#endif
    const char *keylog_file = NULL, *early_data_file = NULL;
#ifndef OPENSSL_NO_DTLS
    int isdtls = 0;
#endif
    char *psksessf = NULL;

    prog = opt_progname(argv[0]);
    c_quiet = 0;
    c_debug = 0;
    c_showcerts = 0;
    c_nbio = 0;

    
    while ((o = opt_next()) != OPT_EOF) {
        /* Check for intermixing flags. */
        if (connect_type == use_unix && IS_INET_FLAG(o)) {
            BIO_printf(bio_err,
                       "%s: Intermixed protocol flags (unix and internet domains)\n",
                       prog);
            goto end;
        }
        if (connect_type == use_inet && IS_UNIX_FLAG(o)) {
            BIO_printf(bio_err,
                       "%s: Intermixed protocol flags (internet and unix domains)\n",
                       prog);
            goto end;
        }

        if (IS_PROT_FLAG(o) && ++prot_opt > 1) {
            BIO_printf(bio_err, "Cannot supply multiple protocol flags\n");
            goto end;
        }
        if (IS_NO_PROT_FLAG(o))
            no_prot_opt++;
        if (prot_opt == 1 && no_prot_opt) {
            BIO_printf(bio_err,
                       "Cannot supply both a protocol flag and '-no_<prot>'\n");
            goto end;
        }

        switch (o) {
        case OPT_EOF:
        case OPT_ERR:
 opthelp:
            BIO_printf(bio_err, "%s: Use -help for summary.\n", prog);
            goto end;
        
        case OPT_HOST:
            connect_type = use_inet;
            freeandcopy(&host, opt_arg());
            break;

        case OPT_SERVERNAME:
            servername = opt_arg(); /* don't free */
            break;
        case OPT_NOSERVERNAME:
            noservername = 1;
            break;
        }
    }
    if (count4or6 >= 2) {
        BIO_printf(bio_err, "%s: Can't use both -4 and -6\n", prog);
        goto opthelp;
    }
    if (noservername) {
        if (servername != NULL) {
            BIO_printf(bio_err,
                       "%s: Can't use -servername and -noservername together\n",
                       prog);
            goto opthelp;
        }
        if (dane_tlsa_domain != NULL) {
            BIO_printf(bio_err,
               "%s: Can't use -dane_tlsa_domain and -noservername together\n",
               prog);
            goto opthelp;
        }
    }
    argc = opt_num_rest();
    if (argc == 1) {
        /* If there's a positional argument, it's the equivalent of
         * OPT_CONNECT.
         * Don't allow -connect and a separate argument.
         */
        if (connectstr != NULL) {
            BIO_printf(bio_err,
                       "%s: must not provide both -connect option and target parameter\n",
                       prog);
            goto opthelp;
        }
        connect_type = use_inet;
    } else if (argc != 0) {
        goto opthelp;
    }
    if (sess_in != NULL) {
        SSL_SESSION *sess;
        BIO *stmp = BIO_new_file(sess_in, "r");
        if (stmp == NULL) {
            BIO_printf(bio_err, "Can't open session file %s\n", sess_in);
            ERR_print_errors(bio_err);
            goto end;
        }
        sess = PEM_read_bio_SSL_SESSION(stmp, NULL, 0, NULL);
        BIO_free(stmp);
        if (sess == NULL) {
            BIO_printf(bio_err, "Can't open session file %s\n", sess_in);
            ERR_print_errors(bio_err);
            goto end;
        }
        if (!SSL_set_session(con, sess)) {
            BIO_printf(bio_err, "Can't set session\n");
            ERR_print_errors(bio_err);
            goto end;
        }
        /* By default the SNI should be the same as was set in the session */
        if (!noservername && servername == NULL) {
            const char *sni = SSL_SESSION_get0_hostname(sess);

            if (sni != NULL) {
                servername = OPENSSL_strdup(sni); /* allocation site */
                if (servername == NULL) {
                    BIO_printf(bio_err, "Can't set server name\n");
                    ERR_print_errors(bio_err);
                    goto end;
                }
            } else {
                /*
                 * Force no SNI to be sent so we are consistent with the
                 * session.
                 */
                noservername = 1;
            }
        }
        SSL_SESSION_free(sess);
    }


    if (!noservername && (servername != NULL || dane_tlsa_domain == NULL)) {
        if (servername == NULL)
            servername = (host == NULL) ? "localhost" : host;
        if (!SSL_set_tlsext_host_name(con, servername)) {
            BIO_printf(bio_err, "Unable to set TLS servername extension.\n");
            ERR_print_errors(bio_err);
            goto end;
        }
    }
 re_start:
    ret = 0;
 shut:
    if (in_init)
        print_stuff(bio_c_out, con, full_log);
    do_ssl_shutdown(con);
 end:
    if (con != NULL) {
        if (prexit != 0)
            print_stuff(bio_c_out, con, 1);
    }
    OPENSSL_free(host);
    return ret; /* memory leak */
}




