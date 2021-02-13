#define SSH_ERR_SUCCESS             0
#define SSH_ERR_INTERNAL_ERROR          -1
#define SSH_ERR_ALLOC_FAIL          -2
#define SSH_ERR_MESSAGE_INCOMPLETE      -3
#define SSH_ERR_INVALID_FORMAT          -4
#define SSH_ERR_BIGNUM_IS_NEGATIVE      -5
#define SSH_ERR_STRING_TOO_LARGE        -6
#define SSH_ERR_BIGNUM_TOO_LARGE        -7
#define SSH_ERR_ECPOINT_TOO_LARGE       -8
#define SSH_ERR_NO_BUFFER_SPACE         -9
#define SSH_ERR_INVALID_ARGUMENT        -10
#define SSH_ERR_KEY_BITS_MISMATCH       -11
#define SSH_ERR_EC_CURVE_INVALID        -12
#define SSH_ERR_KEY_TYPE_MISMATCH       -13
#define SSH_ERR_KEY_TYPE_UNKNOWN        -14 /* XXX UNSUPPORTED? */
#define SSH_ERR_EC_CURVE_MISMATCH       -15
#define SSH_ERR_EXPECTED_CERT           -16
#define SSH_ERR_KEY_LACKS_CERTBLOB      -17
#define SSH_ERR_KEY_CERT_UNKNOWN_TYPE       -18
#define SSH_ERR_KEY_CERT_INVALID_SIGN_KEY   -19
#define SSH_ERR_KEY_INVALID_EC_VALUE        -20
#define SSH_ERR_SIGNATURE_INVALID       -21
#define SSH_ERR_LIBCRYPTO_ERROR         -22
#define SSH_ERR_UNEXPECTED_TRAILING_DATA    -23
#define SSH_ERR_SYSTEM_ERROR            -24
#define SSH_ERR_KEY_CERT_INVALID        -25
#define SSH_ERR_AGENT_COMMUNICATION     -26
#define SSH_ERR_AGENT_FAILURE           -27
#define SSH_ERR_DH_GEX_OUT_OF_RANGE     -28
#define SSH_ERR_DISCONNECTED            -29
#define SSH_ERR_MAC_INVALID         -30
#define SSH_ERR_NO_CIPHER_ALG_MATCH     -31
#define SSH_ERR_NO_MAC_ALG_MATCH        -32
#define SSH_ERR_NO_COMPRESS_ALG_MATCH       -33
#define SSH_ERR_NO_KEX_ALG_MATCH        -34
#define SSH_ERR_NO_HOSTKEY_ALG_MATCH        -35
#define SSH_ERR_NO_HOSTKEY_LOADED       -36
#define SSH_ERR_PROTOCOL_MISMATCH       -37
#define SSH_ERR_NO_PROTOCOL_VERSION     -38
#define SSH_ERR_NEED_REKEY          -39
#define SSH_ERR_PASSPHRASE_TOO_SHORT        -40
#define SSH_ERR_FILE_CHANGED            -41
#define SSH_ERR_KEY_UNKNOWN_CIPHER      -42
#define SSH_ERR_KEY_WRONG_PASSPHRASE        -43
#define SSH_ERR_KEY_BAD_PERMISSIONS     -44
#define SSH_ERR_KEY_CERT_MISMATCH       -45
#define SSH_ERR_KEY_NOT_FOUND           -46
#define SSH_ERR_AGENT_NOT_PRESENT       -47
#define SSH_ERR_AGENT_NO_IDENTITIES     -48
#define SSH_ERR_BUFFER_READ_ONLY        -49
#define SSH_ERR_KRL_BAD_MAGIC           -50
#define SSH_ERR_KEY_REVOKED         -51
#define SSH_ERR_CONN_CLOSED         -52
#define SSH_ERR_CONN_TIMEOUT            -53
#define SSH_ERR_CONN_CORRUPT            -54
#define SSH_ERR_PROTOCOL_ERROR          -55

/* ranges */

#define SSH2_MSG_TRANSPORT_MIN				1
#define SSH2_MSG_TRANSPORT_MAX				49
#define SSH2_MSG_USERAUTH_MIN				50
#define SSH2_MSG_USERAUTH_MAX				79
#define SSH2_MSG_USERAUTH_PER_METHOD_MIN		60
#define SSH2_MSG_USERAUTH_PER_METHOD_MAX		SSH2_MSG_USERAUTH_MAX
#define SSH2_MSG_CONNECTION_MIN				80
#define SSH2_MSG_CONNECTION_MAX				127
#define SSH2_MSG_RESERVED_MIN				128
#define SSH2_MSG_RESERVED_MAX				191
#define SSH2_MSG_LOCAL_MIN				192
#define SSH2_MSG_LOCAL_MAX				255
#define SSH2_MSG_MIN					1
#define SSH2_MSG_MAX					255

/* transport layer: generic */

#define SSH2_MSG_DISCONNECT				1
#define SSH2_MSG_IGNORE					2
#define SSH2_MSG_UNIMPLEMENTED				3
#define SSH2_MSG_DEBUG					4
#define SSH2_MSG_SERVICE_REQUEST			5
#define SSH2_MSG_SERVICE_ACCEPT				6

/* transport layer: alg negotiation */

#define SSH2_MSG_KEXINIT				20
#define SSH2_MSG_NEWKEYS				21

/* transport layer: kex specific messages, can be reused */

#define SSH2_MSG_KEXDH_INIT				30
#define SSH2_MSG_KEXDH_REPLY				31

/* dh-group-exchange */
#define SSH2_MSG_KEX_DH_GEX_REQUEST_OLD			30
#define SSH2_MSG_KEX_DH_GEX_GROUP			31
#define SSH2_MSG_KEX_DH_GEX_INIT			32
#define SSH2_MSG_KEX_DH_GEX_REPLY			33
#define SSH2_MSG_KEX_DH_GEX_REQUEST			34

/* ecdh */
#define SSH2_MSG_KEX_ECDH_INIT				30
#define SSH2_MSG_KEX_ECDH_REPLY				31

/* user authentication: generic */

#define SSH2_MSG_USERAUTH_REQUEST			50
#define SSH2_MSG_USERAUTH_FAILURE			51
#define SSH2_MSG_USERAUTH_SUCCESS			52
#define SSH2_MSG_USERAUTH_BANNER			53

/* user authentication: method specific, can be reused */

#define SSH2_MSG_USERAUTH_PK_OK				60
#define SSH2_MSG_USERAUTH_PASSWD_CHANGEREQ		60
#define SSH2_MSG_USERAUTH_INFO_REQUEST			60
#define SSH2_MSG_USERAUTH_INFO_RESPONSE			61

/* connection protocol: generic */

#define SSH2_MSG_GLOBAL_REQUEST				80
#define SSH2_MSG_REQUEST_SUCCESS			81
#define SSH2_MSG_REQUEST_FAILURE			82

/* channel related messages */

#define SSH2_MSG_CHANNEL_OPEN				90
#define SSH2_MSG_CHANNEL_OPEN_CONFIRMATION		91
#define SSH2_MSG_CHANNEL_OPEN_FAILURE			92
#define SSH2_MSG_CHANNEL_WINDOW_ADJUST			93
#define SSH2_MSG_CHANNEL_DATA				94
#define SSH2_MSG_CHANNEL_EXTENDED_DATA			95
#define SSH2_MSG_CHANNEL_EOF				96
#define SSH2_MSG_CHANNEL_CLOSE				97
#define SSH2_MSG_CHANNEL_REQUEST			98
#define SSH2_MSG_CHANNEL_SUCCESS			99
#define SSH2_MSG_CHANNEL_FAILURE			100

/* disconnect reason code */

#define SSH2_DISCONNECT_HOST_NOT_ALLOWED_TO_CONNECT	1
#define SSH2_DISCONNECT_PROTOCOL_ERROR			2
#define SSH2_DISCONNECT_KEY_EXCHANGE_FAILED		3
#define SSH2_DISCONNECT_HOST_AUTHENTICATION_FAILED	4
#define SSH2_DISCONNECT_RESERVED			4
#define SSH2_DISCONNECT_MAC_ERROR			5
#define SSH2_DISCONNECT_COMPRESSION_ERROR		6
#define SSH2_DISCONNECT_SERVICE_NOT_AVAILABLE		7
#define SSH2_DISCONNECT_PROTOCOL_VERSION_NOT_SUPPORTED	8
#define SSH2_DISCONNECT_HOST_KEY_NOT_VERIFIABLE		9
#define SSH2_DISCONNECT_CONNECTION_LOST			10
#define SSH2_DISCONNECT_BY_APPLICATION			11
#define SSH2_DISCONNECT_TOO_MANY_CONNECTIONS		12
#define SSH2_DISCONNECT_AUTH_CANCELLED_BY_USER		13
#define SSH2_DISCONNECT_NO_MORE_AUTH_METHODS_AVAILABLE	14
#define SSH2_DISCONNECT_ILLEGAL_USER_NAME		15

/* misc */

#define SSH2_OPEN_ADMINISTRATIVELY_PROHIBITED		1
#define SSH2_OPEN_CONNECT_FAILED			2
#define SSH2_OPEN_UNKNOWN_CHANNEL_TYPE			3
#define SSH2_OPEN_RESOURCE_SHORTAGE			4

#define SSH2_EXTENDED_DATA_STDERR			1

/* kex messages for resume@appgate.com */
#define SSH2_MSG_KEX_ROAMING_RESUME			30
#define SSH2_MSG_KEX_ROAMING_AUTH_REQUIRED		31
#define SSH2_MSG_KEX_ROAMING_AUTH			32
#define SSH2_MSG_KEX_ROAMING_AUTH_OK			33
#define SSH2_MSG_KEX_ROAMING_AUTH_FAIL			34

/* Certificate types for OpenSSH certificate keys extension */
#define SSH2_CERT_TYPE_USER				1
#define SSH2_CERT_TYPE_HOST				2

/* conditional algorithm support */

#ifdef OPENSSL_HAS_ECC
#ifdef OPENSSL_HAS_NISTP521
# define KEX_ECDH_METHODS \
	"ecdh-sha2-nistp256," \
	"ecdh-sha2-nistp384," \
	"ecdh-sha2-nistp521,"
# define HOSTKEY_ECDSA_CERT_METHODS \
	"ecdsa-sha2-nistp256-cert-v01@openssh.com," \
	"ecdsa-sha2-nistp384-cert-v01@openssh.com," \
	"ecdsa-sha2-nistp521-cert-v01@openssh.com,"
# define HOSTKEY_ECDSA_METHODS \
	"ecdsa-sha2-nistp256," \
	"ecdsa-sha2-nistp384," \
	"ecdsa-sha2-nistp521,"
#else
# define KEX_ECDH_METHODS \
	"ecdh-sha2-nistp256," \
	"ecdh-sha2-nistp384,"
# define HOSTKEY_ECDSA_CERT_METHODS \
	"ecdsa-sha2-nistp256-cert-v01@openssh.com," \
	"ecdsa-sha2-nistp384-cert-v01@openssh.com,"
# define HOSTKEY_ECDSA_METHODS \
	"ecdsa-sha2-nistp256," \
	"ecdsa-sha2-nistp384,"
#endif
#else
# define KEX_ECDH_METHODS
# define HOSTKEY_ECDSA_CERT_METHODS
# define HOSTKEY_ECDSA_METHODS
#endif

#ifdef OPENSSL_HAVE_EVPGCM
# define AESGCM_CIPHER_MODES \
	",aes128-gcm@openssh.com,aes256-gcm@openssh.com"
#else
# define AESGCM_CIPHER_MODES
#endif

#ifdef HAVE_EVP_SHA256
# define KEX_SHA2_METHODS \
	"diffie-hellman-group-exchange-sha256," \
	"diffie-hellman-group16-sha512," \
	"diffie-hellman-group18-sha512,"
# define KEX_SHA2_GROUP14 \
	"diffie-hellman-group14-sha256,"
#define	SHA2_HMAC_MODES \
	"hmac-sha2-256," \
	"hmac-sha2-512,"
#else
# define KEX_SHA2_METHODS
# define KEX_SHA2_GROUP14
# define SHA2_HMAC_MODES
#endif

#ifdef WITH_OPENSSL
# ifdef HAVE_EVP_SHA256
#  define KEX_CURVE25519_METHODS \
	"curve25519-sha256," \
	"curve25519-sha256@libssh.org,"
# else
#  define KEX_CURVE25519_METHODS ""
# endif
#define KEX_COMMON_KEX \
	KEX_CURVE25519_METHODS \
	KEX_ECDH_METHODS \
	KEX_SHA2_METHODS

#define KEX_SERVER_KEX KEX_COMMON_KEX \
	KEX_SHA2_GROUP14 \
	"diffie-hellman-group14-sha1" \

#define KEX_CLIENT_KEX KEX_COMMON_KEX \
	"diffie-hellman-group-exchange-sha1," \
	KEX_SHA2_GROUP14 \
	"diffie-hellman-group14-sha1"

#define	KEX_DEFAULT_PK_ALG	\
	HOSTKEY_ECDSA_CERT_METHODS \
	"ssh-ed25519-cert-v01@openssh.com," \
	"ssh-rsa-cert-v01@openssh.com," \
	HOSTKEY_ECDSA_METHODS \
	"ssh-ed25519," \
	"rsa-sha2-512," \
	"rsa-sha2-256," \
	"ssh-rsa"

/* the actual algorithms */

#define KEX_SERVER_ENCRYPT \
	"chacha20-poly1305@openssh.com," \
	"aes128-ctr,aes192-ctr,aes256-ctr" \
	AESGCM_CIPHER_MODES

#define KEX_CLIENT_ENCRYPT KEX_SERVER_ENCRYPT "," \
	"aes128-cbc,aes192-cbc,aes256-cbc"

#define KEX_SERVER_MAC \
	"umac-64-etm@openssh.com," \
	"umac-128-etm@openssh.com," \
	"hmac-sha2-256-etm@openssh.com," \
	"hmac-sha2-512-etm@openssh.com," \
	"hmac-sha1-etm@openssh.com," \
	"umac-64@openssh.com," \
	"umac-128@openssh.com," \
	"hmac-sha2-256," \
	"hmac-sha2-512," \
	"hmac-sha1"

#define KEX_CLIENT_MAC KEX_SERVER_MAC

#else /* WITH_OPENSSL */

#define KEX_SERVER_KEX		\
	"curve25519-sha256," \
	"curve25519-sha256@libssh.org"
#define	KEX_DEFAULT_PK_ALG	\
	"ssh-ed25519-cert-v01@openssh.com," \
	"ssh-ed25519"
#define	KEX_SERVER_ENCRYPT \
	"chacha20-poly1305@openssh.com," \
	"aes128-ctr,aes192-ctr,aes256-ctr"
#define	KEX_SERVER_MAC \
	"umac-64-etm@openssh.com," \
	"umac-128-etm@openssh.com," \
	"hmac-sha2-256-etm@openssh.com," \
	"hmac-sha2-512-etm@openssh.com," \
	"hmac-sha1-etm@openssh.com," \
	"umac-64@openssh.com," \
	"umac-128@openssh.com," \
	"hmac-sha2-256," \
	"hmac-sha2-512," \
	"hmac-sha1"

#define KEX_CLIENT_KEX KEX_SERVER_KEX
#define	KEX_CLIENT_ENCRYPT KEX_SERVER_ENCRYPT
#define KEX_CLIENT_MAC KEX_SERVER_MAC

#endif /* WITH_OPENSSL */

#define	KEX_DEFAULT_COMP	"none,zlib@openssh.com"
#define	KEX_DEFAULT_LANG	""

#define KEX_CLIENT \
	KEX_CLIENT_KEX, \
	KEX_DEFAULT_PK_ALG, \
	KEX_CLIENT_ENCRYPT, \
	KEX_CLIENT_ENCRYPT, \
	KEX_CLIENT_MAC, \
	KEX_CLIENT_MAC, \
	KEX_DEFAULT_COMP, \
	KEX_DEFAULT_COMP, \
	KEX_DEFAULT_LANG, \
	KEX_DEFAULT_LANG

#define KEX_SERVER \
	KEX_SERVER_KEX, \
	KEX_DEFAULT_PK_ALG, \
	KEX_SERVER_ENCRYPT, \
	KEX_SERVER_ENCRYPT, \
	KEX_SERVER_MAC, \
	KEX_SERVER_MAC, \
	KEX_DEFAULT_COMP, \
	KEX_DEFAULT_COMP, \
	KEX_DEFAULT_LANG, \
	KEX_DEFAULT_LANG

#define SSHBUF_SIZE_MAX     0x8000000   /* Hard maximum size */
#define SSHBUF_REFS_MAX     0x100000    /* Max child buffers */
#define SSHBUF_MAX_BIGNUM   (16384 / 8) /* Max bignum *bytes* */
#define SSHBUF_MAX_ECPOINT  ((528 * 2 / 8) + 1) /* Max EC point *bytes* */

struct sshbuf {
    u_char *d;      /* Data */
    const u_char *cd;   /* Const data */
    size_t off;     /* First available byte is buf->d + buf->off */
    size_t size;        /* Last byte is buf->d + buf->size - 1 */
    size_t max_size;    /* Maximum size of buffer */
    size_t alloc;       /* Total bytes allocated to buf->d */
    int readonly;       /* Refers to external, const data */
    int dont_free;      /* Kludge to support sshbuf_init */
    u_int refcount;     /* Tracks self and number of child buffers */
    struct sshbuf *parent;  /* If child, pointer to parent */
};

typedef u_int BN_ULONG;
typedef struct {
	u_char	*buf;		/* Buffer for data. */
	u_int	 alloc;		/* Number of bytes allocated for data. */
	u_int	 offset;	/* Offset of first byte containing data. */
	u_int	 end;		/* Offset of last byte containing data. */
}       Buffer;

struct bignum_st {
    BN_ULONG *d;                /* Pointer to an array of 'BN_BITS2' bit
                                 * chunks. */
    int top;                    /* Index of last used d +1. */
    /* The next are internal book keeping for bn_expand. */
    int dmax;                   /* Size of the d array. */
    int neg;                    /* one if the number is negative */
    int flags;
};

#define RSA_METHOD void
#define ENGINE void
#define RSA_PSS_PARAMS void
#define CRYPTO_EX_DATA int
#define CRYPTO_REF_COUNT int
#define BN_MONT_CTX void
#define BN_BLINDING void
#define CRYPTO_RWLOCK void
typedef struct bignum_st BIGNUM;
typedef struct rsa_st {
    /*
     * The first parameter is used to pickup errors where this is passed
     * instead of aEVP_PKEY, it is set to 0
     */
    int pad;
    int32_t version;
    const RSA_METHOD *meth;
    /* functional reference if 'meth' is ENGINE-provided */
    ENGINE *engine;
    BIGNUM *n;
    BIGNUM *e;
    BIGNUM *d;
    BIGNUM *p;
    BIGNUM *q;
    BIGNUM *dmp1;
    BIGNUM *dmq1;
    BIGNUM *iqmp;
    /* If a PSS only key this contains the parameter restrictions */
    RSA_PSS_PARAMS *pss;
    /* be careful using this if the RSA structure is shared */
    CRYPTO_EX_DATA ex_data;
    CRYPTO_REF_COUNT references;
    int flags;
    /* Used to cache montgomery values */
    BN_MONT_CTX *_method_mod_n;
    BN_MONT_CTX *_method_mod_p;
    BN_MONT_CTX *_method_mod_q;
    /*
     * all BIGNUM values are actually in the following data, if it is not
     * NULL
     */
    char *bignum_data;
    BN_BLINDING *blinding;
    BN_BLINDING *mt_blinding;
    CRYPTO_RWLOCK *lock;
} RSA;

# define EC_KEY		void
# define EC_GROUP	void
# define EC_POINT	void
# define DH         void

typedef struct dsa_st {
    /*
     * This first variable is used to pick up errors where a DSA is passed
     * instead of of a EVP_PKEY
     */
    int pad;
    int32_t version;
    BIGNUM *p;
    BIGNUM *q;                  /* == 20 */
    BIGNUM *g;
    BIGNUM *pub_key;            /* y public key */
    BIGNUM *priv_key;           /* x private key */
    int flags;
} DSA;

#define KEX_COOKIE_LEN	16

#define	KEX_DH1				"diffie-hellman-group1-sha1"
#define	KEX_DH14_SHA1			"diffie-hellman-group14-sha1"
#define	KEX_DH14_SHA256			"diffie-hellman-group14-sha256"
#define	KEX_DH16_SHA512			"diffie-hellman-group16-sha512"
#define	KEX_DH18_SHA512			"diffie-hellman-group18-sha512"
#define	KEX_DHGEX_SHA1			"diffie-hellman-group-exchange-sha1"
#define	KEX_DHGEX_SHA256		"diffie-hellman-group-exchange-sha256"
#define	KEX_ECDH_SHA2_NISTP256		"ecdh-sha2-nistp256"
#define	KEX_ECDH_SHA2_NISTP384		"ecdh-sha2-nistp384"
#define	KEX_ECDH_SHA2_NISTP521		"ecdh-sha2-nistp521"
#define	KEX_CURVE25519_SHA256		"curve25519-sha256"
#define	KEX_CURVE25519_SHA256_OLD	"curve25519-sha256@libssh.org"

#define COMP_NONE	0
#define COMP_ZLIB	1
#define COMP_DELAYED	2

#define CURVE25519_SIZE 32

/*******************************************************************/

/* Cipher used for encrypting authentication files. */
#define SSH_AUTHFILE_CIPHER	SSH_CIPHER_3DES

/* Default port number. */
#define SSH_DEFAULT_PORT	22

/*
 * Maximum number of certificate files that can be specified
 * in configuration files or on the command line.
 */
#define SSH_MAX_CERTIFICATE_FILES	100

/*
 * Maximum number of RSA authentication identity files that can be specified
 * in configuration files or on the command line.
 */
#define SSH_MAX_IDENTITY_FILES		100

/*
 * Maximum length of lines in authorized_keys file.
 * Current value permits 16kbit RSA and RSA1 keys and 8kbit DSA keys, with
 * some room for options and comments.
 */
#define SSH_MAX_PUBKEY_BYTES		16384

/*
 * Major protocol version.  Different version indicates major incompatibility
 * that prevents communication.
 *
 * Minor protocol version.  Different version indicates minor incompatibility
 * that does not prevent interoperation.
 */
#define PROTOCOL_MAJOR_1	1
#define PROTOCOL_MINOR_1	5

/* We support both SSH1 and SSH2 */
#define PROTOCOL_MAJOR_2	2
#define PROTOCOL_MINOR_2	0

/*
 * Name for the service.  The port named by this service overrides the
 * default port if present.
 */
#define SSH_SERVICE_NAME	"ssh"

/*
 * Name of the environment variable containing the process ID of the
 * authentication agent.
 */
#define SSH_AGENTPID_ENV_NAME	"SSH_AGENT_PID"

/*
 * Name of the environment variable containing the pathname of the
 * authentication socket.
 */
#define SSH_AUTHSOCKET_ENV_NAME "SSH_AUTH_SOCK"

/*
 * Environment variable for overwriting the default location of askpass
 */
#define SSH_ASKPASS_ENV		"SSH_ASKPASS"

/*
 * Force host key length and server key length to differ by at least this
 * many bits.  This is to make double encryption with rsaref work.
 */
#define SSH_KEY_BITS_RESERVED		128

/*
 * Length of the session key in bytes.  (Specified as 256 bits in the
 * protocol.)
 */
#define SSH_SESSION_KEY_LENGTH		32

/* Used to identify ``EscapeChar none'' */
#define SSH_ESCAPECHAR_NONE		-2

/*
 * unprivileged user when UsePrivilegeSeparation=yes;
 * sshd will change its privileges to this user and its
 * primary group.
 */
#ifndef SSH_PRIVSEP_USER
#define SSH_PRIVSEP_USER		"sshd"
#endif

/* Minimum modulus size (n) for RSA keys. */
#define SSH_RSA_MINIMUM_MODULUS_SIZE	768

/* Listen backlog for sshd, ssh-agent and forwarding sockets */
#define SSH_LISTEN_BACKLOG		128

/***********************************************************/

enum kex_init_proposals {
	PROPOSAL_KEX_ALGS,
	PROPOSAL_SERVER_HOST_KEY_ALGS,
	PROPOSAL_ENC_ALGS_CTOS,
	PROPOSAL_ENC_ALGS_STOC,
	PROPOSAL_MAC_ALGS_CTOS,
	PROPOSAL_MAC_ALGS_STOC,
	PROPOSAL_COMP_ALGS_CTOS,
	PROPOSAL_COMP_ALGS_STOC,
	PROPOSAL_LANG_CTOS,
	PROPOSAL_LANG_STOC,
	PROPOSAL_MAX
};

enum kex_modes {
	MODE_IN,
	MODE_OUT,
	MODE_MAX
};

enum kex_exchange {
	KEX_DH_GRP1_SHA1,
	KEX_DH_GRP14_SHA1,
	KEX_DH_GRP14_SHA256,
	KEX_DH_GRP16_SHA512,
	KEX_DH_GRP18_SHA512,
	KEX_DH_GEX_SHA1,
	KEX_DH_GEX_SHA256,
	KEX_ECDH_SHA2,
	KEX_C25519_SHA256,
	KEX_MAX
};

#define KEX_INIT_SENT	0x0001
struct sshenc {
	char	*name;
	char     *cipher;
	int	enabled;
	u_int	key_len;
	u_int	iv_len;
	u_int	block_size;
	u_char	*key;
	u_char	*iv;
};
struct sshcomp {
	u_int	type;
	int	enabled;
	char	*name;
};
struct newkeys {
    char 	*enc;
	char 	*mac;
	char    *comp;
};


/* key is stored in external hardware */
#define SSHKEY_FLAG_EXT		0x0001

#define SSHKEY_CERT_MAX_PRINCIPALS	256
struct sshkey_cert {
	struct sshbuf	*certblob; /* Kept around for use on wire */
	u_int		 type; /* SSH2_CERT_TYPE_USER or SSH2_CERT_TYPE_HOST */
	u_int64_t	 serial;
	char		*key_id;
	u_int		 nprincipals;
	char		**principals;
	u_int64_t	 valid_after, valid_before;
	struct sshbuf	*critical;
	struct sshbuf	*extensions;
	struct sshkey	*signature_key;
};

/* XXX opaquify? */
struct sshkey {
	int	 type;
	int	 flags;
	RSA	*rsa;
	DSA	*dsa;
	int	 ecdsa_nid;	/* NID of curve */
	EC_KEY	*ecdsa;
	u_char	*ed25519_sk;
	u_char	*ed25519_pk;
	struct sshkey_cert *cert;
};

struct kex {
	u_char	*session_id;
	size_t	session_id_len;
	struct newkeys	*newkeys[MODE_MAX];
	u_int	we_need;
	u_int	dh_need;
	int	server;
	char	*name;
	char	*hostkey_alg;
	int	hostkey_type;
	int	hostkey_nid;
	u_int	kex_type;
	int	rsa_sha2;
	int	ext_info_c;
	struct sshbuf *my;
	struct sshbuf *peer;
	sig_atomic_t done;
	u_int	flags;
	int	hash_alg;
	int	ec_nid;
	char	*client_version_string;
	char	*server_version_string;
	char	*failed_choice;
	int	(*verify_host_key)(struct sshkey *, struct ssh *);
	struct sshkey *(*load_host_public_key)(int, int, struct ssh *);
	struct sshkey *(*load_host_private_key)(int, int, struct ssh *);
	int	(*host_key_index)(struct sshkey *, int, struct ssh *);
	int	(*sign)(struct sshkey *, struct sshkey *, u_char **, size_t *,
	    const u_char *, size_t, const char *, u_int);
	int	(*kex[KEX_MAX])(struct ssh *);
	/* kex specific state */
	DH	*dh;			/* DH */
	u_int	min, max, nbits;	/* GEX */
	EC_KEY	*ec_client_key;		/* ECDH */
	const EC_GROUP *ec_group;	/* ECDH */
	u_char c25519_client_key[CURVE25519_SIZE]; /* 25519 */
	u_char c25519_client_pubkey[CURVE25519_SIZE]; /* 25519 */
};





#define SSH_RSA_MINIMUM_MODULUS_SIZE	768
#define SSH_KEY_MAX_SIGN_DATA_SIZE	(1 << 20)

/* Key types */
enum sshkey_types {
	KEY_RSA1,
	KEY_RSA,
	KEY_DSA,
	KEY_ECDSA,
	KEY_ED25519,
	KEY_RSA_CERT,
	KEY_DSA_CERT,
	KEY_ECDSA_CERT,
	KEY_ED25519_CERT,
	KEY_RSA_CERT_V00,
	KEY_DSA_CERT_V00,
	KEY_UNSPEC
};

/* Default fingerprint hash */
#define SSH_FP_HASH_DEFAULT	SSH_DIGEST_SHA256

/* Fingerprint representation formats */
enum sshkey_fp_rep {
	SSH_FP_DEFAULT = 0,
	SSH_FP_HEX,
	SSH_FP_BASE64,
	SSH_FP_BUBBLEBABBLE,
	SSH_FP_RANDOMART
};

struct hostkeys_update_ctx {
	/* The hostname and (optionally) IP address string for the server */
	char *host_str, *ip_str;

	/*
	 * Keys received from the server and a flag for each indicating
	 * whether they already exist in known_hosts.
	 * keys_seen is filled in by hostkeys_find() and later (for new
	 * keys) by client_global_hostkeys_private_confirm().
	 */
	struct sshkey **keys;
	int *keys_seen;
	size_t nkeys;

	size_t nnew;

	/*
	 * Keys that are in known_hosts, but were not present in the update
	 * from the server (i.e. scheduled to be deleted).
	 * Filled in by hostkeys_find().
	 */
	struct sshkey **old_keys;
	size_t nold;
};

struct Forward {
	char	 *listen_host;		/* Host (address) to listen on. */
	int	  listen_port;		/* Port to forward. */
	char	 *listen_path;		/* Path to bind domain socket. */
	char	 *connect_host;		/* Host to connect. */
	int	  connect_port;		/* Port to connect on connect_host. */
	char	 *connect_path;		/* Path to connect domain socket. */
	int	  allocated_port;	/* Dynamically allocated listen port */
	int	  handle;		/* Handle for dynamic listen ports */
};

struct ForwardOptions {
	int	 gateway_ports; /* Allow remote connects to forwarded ports. */
	mode_t	 streamlocal_bind_mask; /* umask for streamlocal binds */
	int	 streamlocal_bind_unlink; /* unlink socket before bind */
};


#define SSH_MAX_IDENTITY_FILES		100
#define MAX_SEND_ENV		256
#define SSH_MAX_HOSTS_FILES	32
#define MAX_CANON_DOMAINS	32
#define PATH_MAX_SUN		(sizeof((struct sockaddr_un *)0)->sun_path)

typedef enum {
	SYSLOG_FACILITY_DAEMON,
	SYSLOG_FACILITY_USER,
	SYSLOG_FACILITY_AUTH,
#ifdef LOG_AUTHPRIV
	SYSLOG_FACILITY_AUTHPRIV,
#endif
	SYSLOG_FACILITY_LOCAL0,
	SYSLOG_FACILITY_LOCAL1,
	SYSLOG_FACILITY_LOCAL2,
	SYSLOG_FACILITY_LOCAL3,
	SYSLOG_FACILITY_LOCAL4,
	SYSLOG_FACILITY_LOCAL5,
	SYSLOG_FACILITY_LOCAL6,
	SYSLOG_FACILITY_LOCAL7,
	SYSLOG_FACILITY_NOT_SET = -1
}       SyslogFacility;

typedef enum {
	SYSLOG_LEVEL_QUIET,
	SYSLOG_LEVEL_FATAL,
	SYSLOG_LEVEL_ERROR,
	SYSLOG_LEVEL_INFO,
	SYSLOG_LEVEL_VERBOSE,
	SYSLOG_LEVEL_DEBUG1,
	SYSLOG_LEVEL_DEBUG2,
	SYSLOG_LEVEL_DEBUG3,
	SYSLOG_LEVEL_NOT_SET = -1
}       LogLevel;



struct allowed_cname {
	char *source_list;
	char *target_list;
};

typedef struct {
	int     forward_agent;	/* Forward authentication agent. */
	int     forward_x11;	/* Forward X11 display. */
	int     forward_x11_timeout;	/* Expiration for Cookies */
	int     forward_x11_trusted;	/* Trust Forward X11 display. */
	int     exit_on_forward_failure;	/* Exit if bind(2) fails for -L/-R */
	char   *xauth_location;	/* Location for xauth program */
	struct ForwardOptions fwd_opts;	/* forwarding options */
	int     use_privileged_port;	/* Don't use privileged port if false. */
	int     rhosts_rsa_authentication;	/* Try rhosts with RSA
						 * authentication. */
	int     rsa_authentication;	/* Try RSA authentication. */
	int     pubkey_authentication;	/* Try ssh2 pubkey authentication. */
	int     hostbased_authentication;	/* ssh2's rhosts_rsa */
	int     challenge_response_authentication;
					/* Try S/Key or TIS, authentication. */
	int     gss_authentication;	/* Try GSS authentication */
	int     gss_deleg_creds;	/* Delegate GSS credentials */
	int     password_authentication;	/* Try password
						 * authentication. */
	int     kbd_interactive_authentication; /* Try keyboard-interactive auth. */
	char	*kbd_interactive_devices; /* Keyboard-interactive auth devices. */
	int     batch_mode;	/* Batch mode: do not ask for passwords. */
	int     check_host_ip;	/* Also keep track of keys for IP address */
	int     strict_host_key_checking;	/* Strict host key checking. */
	int     compression;	/* Compress packets in both directions. */
	int     compression_level;	/* Compression level 1 (fast) to 9
					 * (best). */
	int     tcp_keep_alive;	/* Set SO_KEEPALIVE. */
	int	ip_qos_interactive;	/* IP ToS/DSCP/class for interactive */
	int	ip_qos_bulk;		/* IP ToS/DSCP/class for bulk traffic */
	LogLevel log_level;	/* Level for logging. */

	int     port;		/* Port to connect. */
	int     address_family;
	int     connection_attempts;	/* Max attempts (seconds) before
					 * giving up */
	int     connection_timeout;	/* Max time (seconds) before
					 * aborting connection attempt */
	int     number_of_password_prompts;	/* Max number of password
						 * prompts. */
	int     cipher;		/* Cipher to use. */
	char   *ciphers;	/* SSH2 ciphers in order of preference. */
	char   *macs;		/* SSH2 macs in order of preference. */
	char   *hostkeyalgorithms;	/* SSH2 server key types in order of preference. */
	char   *kex_algorithms;	/* SSH2 kex methods in order of preference. */
	int	protocol;	/* Protocol in order of preference. */
	char   *hostname;	/* Real host to connect. */
	char   *host_key_alias;	/* hostname alias for .ssh/known_hosts */
	char   *proxy_command;	/* Proxy command for connecting the host. */
	char   *user;		/* User to log in as. */
	int     escape_char;	/* Escape character; -2 = none */

	u_int	num_system_hostfiles;	/* Paths for /etc/ssh/ssh_known_hosts */
	char   *system_hostfiles[SSH_MAX_HOSTS_FILES];
	u_int	num_user_hostfiles;	/* Path for $HOME/.ssh/known_hosts */
	char   *user_hostfiles[SSH_MAX_HOSTS_FILES];
	char   *preferred_authentications;
	char   *bind_address;	/* local socket address for connection to sshd */
	char   *pkcs11_provider; /* PKCS#11 provider */
	int	verify_host_key_dns;	/* Verify host key using DNS */

	int     num_identity_files;	/* Number of files for RSA/DSA identities. */
	char   *identity_files[SSH_MAX_IDENTITY_FILES];
	int    identity_file_userprovided[SSH_MAX_IDENTITY_FILES];
	struct sshkey *identity_keys[SSH_MAX_IDENTITY_FILES];

	/* Local TCP/IP forward requests. */
	int     num_local_forwards;
	struct Forward *local_forwards;

	/* Remote TCP/IP forward requests. */
	int     num_remote_forwards;
	struct Forward *remote_forwards;
	int	clear_forwardings;

	int	enable_ssh_keysign;
	int64_t rekey_limit;
	int	rekey_interval;
	int	no_host_authentication_for_localhost;
	int	identities_only;
	int	server_alive_interval;
	int	server_alive_count_max;

	int     num_send_env;
	char   *send_env[MAX_SEND_ENV];

	char	*control_path;
	int	control_master;
	int     control_persist; /* ControlPersist flag */
	int     control_persist_timeout; /* ControlPersist timeout (seconds) */

	int	hash_known_hosts;

	int	tun_open;	/* tun(4) */
	int     tun_local;	/* force tun device (optional) */
	int     tun_remote;	/* force tun device (optional) */

	char	*local_command;
	int	permit_local_command;
	int	visual_host_key;

	int	use_roaming;

	int	request_tty;

	int	proxy_use_fdpass;

	int	num_canonical_domains;
	char	*canonical_domains[MAX_CANON_DOMAINS];
	int	canonicalize_hostname;
	int	canonicalize_max_dots;
	int	canonicalize_fallback_local;
	int	num_permitted_cnames;
	struct allowed_cname permitted_cnames[MAX_CANON_DOMAINS];

	char	*revoked_host_keys;

	int	 fingerprint_hash;

	int	 update_hostkeys; /* one of SSH_UPDATE_HOSTKEYS_* */

	char	*hostbased_key_types;

	char	*ignored_unknown; /* Pattern list of unknown tokens to ignore */
}       Options;

#define SSH_CANONICALISE_NO	0
#define SSH_CANONICALISE_YES	1
#define SSH_CANONICALISE_ALWAYS	2

#define SSHCTL_MASTER_NO	0
#define SSHCTL_MASTER_YES	1
#define SSHCTL_MASTER_AUTO	2
#define SSHCTL_MASTER_ASK	3
#define SSHCTL_MASTER_AUTO_ASK	4

#define REQUEST_TTY_AUTO	0
#define REQUEST_TTY_NO		1
#define REQUEST_TTY_YES		2
#define REQUEST_TTY_FORCE	3

#define SSHCONF_CHECKPERM	1  /* check permissions on config file */
#define SSHCONF_USERCONF	2  /* user provided config file not system */
#define SSHCONF_POSTCANON	4  /* After hostname canonicalisation */

#define SSH_UPDATE_HOSTKEYS_NO	0
#define SSH_UPDATE_HOSTKEYS_YES	1
#define SSH_UPDATE_HOSTKEYS_ASK	2


#define RP_ECHO			0x0001
#define RP_ALLOW_STDIN		0x0002
#define RP_ALLOW_EOF		0x0004
#define RP_USE_ASKPASS		0x0008

/*
 * Iterate through a hostkeys file, optionally parsing keys and matching
 * hostnames. Allows access to the raw keyfile lines to allow
 * streaming edits to the file to take place.
 */
#define HKF_WANT_MATCH		(1)	/* return only matching hosts/addrs */
#define HKF_WANT_PARSE_KEY	(1<<1)	/* need key parsed */

#define HKF_STATUS_OK		0	/* Line parsed, didn't match host */
#define HKF_STATUS_INVALID	1	/* line had parse error */
#define HKF_STATUS_COMMENT	2	/* valid line contained no key */
#define HKF_STATUS_MATCHED	3	/* hostname or IP matched */

#define HKF_MATCH_HOST		(1)	/* hostname matched */
#define HKF_MATCH_IP		(1<<1)	/* address matched */
#define HKF_MATCH_HOST_HASHED	(1<<2)	/* hostname was hashed */
#define HKF_MATCH_IP_HASHED	(1<<3)	/* address was hashed */

struct hostkey_foreach_line {
	const char *path; /* Path of file */
	u_long linenum;	/* Line number */
	u_int status;	/* One of HKF_STATUS_* */
	u_int match;	/* Zero or more of HKF_MATCH_* OR'd together */
	char *line;	/* Entire key line; mutable by callback */
	int marker;	/* CA/revocation markers; indicated by MRK_* value */
	const char *hosts; /* Raw hosts text, may be hashed or list multiple */
	const char *rawkey; /* Text of key and any comment following it */
	int keytype;	/* Type of key; KEY_UNSPEC for invalid/comment lines */
	struct sshkey *key; /* Key, if parsed ok and HKF_WANT_MATCH_HOST set */
	const char *comment; /* Any comment following the key */
};


typedef enum {
	HOST_OK, HOST_NEW, HOST_CHANGED, HOST_REVOKED, HOST_FOUND
}       HostStatus;

typedef enum {
	MRK_ERROR, MRK_NONE, MRK_REVOKE, MRK_CA
}	HostkeyMarker;

struct hostkey_entry {
	char *host;
	char *file;
	u_long line;
	struct sshkey *key;
	HostkeyMarker marker;
};
struct hostkeys;

#ifndef OPENSSH
struct ssh {
    /* Session state */
    struct session_state *state;

    /* Key exchange */
    struct kex *kex;

    /* cached local and remote ip addresses and ports */
    char *remote_ipaddr;
    int remote_port;
    char *local_ipaddr;
    int local_port;

    /* Optional preamble for log messages (e.g. username) */
    char *log_preamble;

    /* Dispatcher table */
//    dispatch_fn *dispatch[DISPATCH_MAX];
    /* number of packets to ignore in the dispatcher */
    int dispatch_skip_packets;

    /* datafellows */
    int compat;

    /* Lists for private and public keys */
    TAILQ_HEAD(, key_entry) private_keys;
    TAILQ_HEAD(, key_entry) public_keys;

    /* APP data */
    void *app_data;
};
#endif

#define PACKET_MAX_SIZE (256 * 1024)

struct packet_state {
	u_int32_t seqnr;
	u_int32_t packets;
	u_int64_t blocks;
	u_int64_t bytes;
};

struct packet {
	TAILQ_ENTRY(packet) next;
	u_char type;
	struct sshbuf *payload;
};

typedef int (ssh_packet_hook_fn)(struct ssh *, struct sshbuf *,
    u_char *, void *);

typedef int z_stream;


struct session_state {
	/*
	 * This variable contains the file descriptors used for
	 * communicating with the other side.  connection_in is used for
	 * reading; connection_out for writing.  These can be the same
	 * descriptor, in which case it is assumed to be a socket.
	 */
	int connection_in;
	int connection_out;

	/* Protocol flags for the remote side. */
	u_int remote_protocol_flags;

	/* Encryption context for receiving data.  Only used for decryption. */
	struct sshcipher_ctx *receive_context;

	/* Encryption context for sending data.  Only used for encryption. */
	struct sshcipher_ctx *send_context;

	/* Buffer for raw input data from the socket. */
	struct sshbuf *input;

	/* Buffer for raw output data going to the socket. */
	struct sshbuf *output;

	/* Buffer for the partial outgoing packet being constructed. */
	struct sshbuf *outgoing_packet;

	/* Buffer for the incoming packet currently being processed. */
	struct sshbuf *incoming_packet;

	/* Scratch buffer for packet compression/decompression. */
	struct sshbuf *compression_buffer;

	/* Incoming/outgoing compression dictionaries */
	z_stream compression_in_stream;
	z_stream compression_out_stream;
	int compression_in_started;
	int compression_out_started;
	int compression_in_failures;
	int compression_out_failures;

	/*
	 * Flag indicating whether packet compression/decompression is
	 * enabled.
	 */
	int packet_compression;

	/* default maximum packet size */
	u_int max_packet_size;

	/* Flag indicating whether this module has been initialized. */
	int initialized;

	/* Set to true if the connection is interactive. */
	int interactive_mode;

	/* Set to true if we are the server side. */
	int server_side;

	/* Set to true if we are authenticated. */
	int after_authentication;

	int keep_alive_timeouts;

	/* The maximum time that we will wait to send or receive a packet */
	int packet_timeout_ms;

	/* Session key information for Encryption and MAC */
	struct newkeys *newkeys[MODE_MAX];
	struct packet_state p_read, p_send;

	/* Volume-based rekeying */
	u_int64_t max_blocks_in, max_blocks_out, rekey_limit;

	/* Time-based rekeying */
	u_int32_t rekey_interval;	/* how often in seconds */
	time_t rekey_time;	/* time of last rekeying */

	/* Session key for protocol v1 */
	u_char ssh1_key[SSH_SESSION_KEY_LENGTH];
	u_int ssh1_keylen;

	/* roundup current message to extra_pad bytes */
	u_char extra_pad;

	/* XXX discard incoming data after MAC error */
	u_int packet_discard;
	size_t packet_discard_mac_already;
	struct sshmac *packet_discard_mac;

	/* Used in packet_read_poll2() */
	u_int packlen;

	/* Used in packet_send2 */
	int rekeying;

	/* Used in ssh_packet_send_mux() */
	int mux;

	/* Used in packet_set_interactive */
	int set_interactive_called;

	/* Used in packet_set_maxsize */
	int set_maxsize_called;

	/* One-off warning about weak ciphers */
	int cipher_warning_done;

	/* SSH1 CRC compensation attack detector */
	char *deattack;

	/* Hook for fuzzing inbound packets */
	ssh_packet_hook_fn *hook_in;
	void *hook_in_ctx;

	TAILQ_HEAD(, packet) outgoing;
};
typedef struct Key Key;

enum fp_type {
	SSH_FP_SHA1,
	SSH_FP_MD5,
	SSH_FP_SHA256
};

/* key is stored in external hardware */
#define KEY_FLAG_EXT		0x0001

#define CERT_MAX_PRINCIPALS	256
struct KeyCert {
	Buffer		 certblob; /* Kept around for use on wire */
	u_int		 type; /* SSH2_CERT_TYPE_USER or SSH2_CERT_TYPE_HOST */
	u_int64_t	 serial;
	char		*key_id;
	u_int		 nprincipals;
	char		**principals;
	u_int64_t	 valid_after, valid_before;
	Buffer		 critical;
	Buffer		 extensions;
	Key		*signature_key;
};

struct Key {
	int	 type;
	int	 flags;
	RSA	*rsa;
	DSA	*dsa;
	int	 ecdsa_nid;	/* NID of curve */
#ifdef OPENSSL_HAS_ECC
	EC_KEY	*ecdsa;
#else
	void	*ecdsa;
#endif
	struct KeyCert *cert;
	u_char	*ed25519_sk;
	u_char	*ed25519_pk;
};

#define	ED25519_SK_SZ	crypto_sign_ed25519_SECRETKEYBYTES
#define	ED25519_PK_SZ	crypto_sign_ed25519_PUBLICKEYBYTES

typedef struct {
    BIGNUM *r;
    BIGNUM *s;
} ECDSA_SIG;

/* Messages for the authentication agent connection. */
#define SSH_AGENTC_REQUEST_RSA_IDENTITIES	1
#define SSH_AGENT_RSA_IDENTITIES_ANSWER		2
#define SSH_AGENTC_RSA_CHALLENGE		3
#define SSH_AGENT_RSA_RESPONSE			4
#define SSH_AGENT_FAILURE			5
#define SSH_AGENT_SUCCESS			6
#define SSH_AGENTC_ADD_RSA_IDENTITY		7
#define SSH_AGENTC_REMOVE_RSA_IDENTITY		8
#define SSH_AGENTC_REMOVE_ALL_RSA_IDENTITIES	9

/* private OpenSSH extensions for SSH2 */
#define SSH2_AGENTC_REQUEST_IDENTITIES		11
#define SSH2_AGENT_IDENTITIES_ANSWER		12
#define SSH2_AGENTC_SIGN_REQUEST		13
#define SSH2_AGENT_SIGN_RESPONSE		14
#define SSH2_AGENTC_ADD_IDENTITY		17
#define SSH2_AGENTC_REMOVE_IDENTITY		18
#define SSH2_AGENTC_REMOVE_ALL_IDENTITIES	19

/* smartcard */
#define SSH_AGENTC_ADD_SMARTCARD_KEY		20
#define SSH_AGENTC_REMOVE_SMARTCARD_KEY		21

/* lock/unlock the agent */
#define SSH_AGENTC_LOCK				22
#define SSH_AGENTC_UNLOCK			23

/* add key with constraints */
#define SSH_AGENTC_ADD_RSA_ID_CONSTRAINED	24
#define SSH2_AGENTC_ADD_ID_CONSTRAINED		25
#define SSH_AGENTC_ADD_SMARTCARD_KEY_CONSTRAINED 26

#define	SSH_AGENT_CONSTRAIN_LIFETIME		1
#define	SSH_AGENT_CONSTRAIN_CONFIRM		2

/* extended failure messages */
#define SSH2_AGENT_FAILURE			30

/* additional error code for ssh.com's ssh-agent2 */
#define SSH_COM_AGENT2_FAILURE			102

#define	SSH_AGENT_OLD_SIGNATURE			0x01

typedef struct {
	int	fd;
	Buffer	identities;
	int	howmany;
}	AuthenticationConnection;

typedef struct Sensitive Sensitive;
struct Sensitive {
	Key	**keys;
	int	nkeys;
	int	external_keysign;
};





