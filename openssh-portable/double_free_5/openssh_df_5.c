/*
    commit id : 056ca1eb473e2d4bd9fa867feb26f94a36f5d877
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../openssh.h"

#define USE_PAM

/*
 * Definition of message types.  New values can be added, but old values
 * should not be removed or without careful consideration of the consequences
 * for compatibility.  The maximum value is 254; value 255 is reserved for
 * future extension.
 */
/* Message name */			/* msg code */	/* arguments */
#define SSH_MSG_NONE				0	/* no message */
#define SSH_MSG_DISCONNECT			1	/* cause (string) */
#define SSH_SMSG_PUBLIC_KEY			2	/* ck,msk,srvk,hostk */
#define SSH_CMSG_SESSION_KEY			3	/* key (BIGNUM) */
#define SSH_CMSG_USER				4	/* user (string) */
#define SSH_CMSG_AUTH_RHOSTS			5	/* user (string) */
#define SSH_CMSG_AUTH_RSA			6	/* modulus (BIGNUM) */
#define SSH_SMSG_AUTH_RSA_CHALLENGE		7	/* int (BIGNUM) */
#define SSH_CMSG_AUTH_RSA_RESPONSE		8	/* int (BIGNUM) */
#define SSH_CMSG_AUTH_PASSWORD			9	/* pass (string) */
#define SSH_CMSG_REQUEST_PTY		        10	/* TERM, tty modes */
#define SSH_CMSG_WINDOW_SIZE		        11	/* row,col,xpix,ypix */
#define SSH_CMSG_EXEC_SHELL			12	/* */
#define SSH_CMSG_EXEC_CMD			13	/* cmd (string) */
#define SSH_SMSG_SUCCESS			14	/* */
#define SSH_SMSG_FAILURE			15	/* */
#define SSH_CMSG_STDIN_DATA			16	/* data (string) */
#define SSH_SMSG_STDOUT_DATA			17	/* data (string) */
#define SSH_SMSG_STDERR_DATA			18	/* data (string) */
#define SSH_CMSG_EOF				19	/* */
#define SSH_SMSG_EXITSTATUS			20	/* status (int) */
#define SSH_MSG_CHANNEL_OPEN_CONFIRMATION	21	/* channel (int) */
#define SSH_MSG_CHANNEL_OPEN_FAILURE		22	/* channel (int) */
#define SSH_MSG_CHANNEL_DATA			23	/* ch,data (int,str) */
#define SSH_MSG_CHANNEL_CLOSE			24	/* channel (int) */
#define SSH_MSG_CHANNEL_CLOSE_CONFIRMATION	25	/* channel (int) */
/*      SSH_CMSG_X11_REQUEST_FORWARDING         26         OBSOLETE */
#define SSH_SMSG_X11_OPEN			27	/* channel (int) */
#define SSH_CMSG_PORT_FORWARD_REQUEST		28	/* p,host,hp (i,s,i) */
#define SSH_MSG_PORT_OPEN			29	/* ch,h,p (i,s,i) */
#define SSH_CMSG_AGENT_REQUEST_FORWARDING	30	/* */
#define SSH_SMSG_AGENT_OPEN			31	/* port (int) */
#define SSH_MSG_IGNORE				32	/* string */
#define SSH_CMSG_EXIT_CONFIRMATION		33	/* */
#define SSH_CMSG_X11_REQUEST_FORWARDING		34	/* proto,data (s,s) */
#define SSH_CMSG_AUTH_RHOSTS_RSA		35	/* user,mod (s,mpi) */
#define SSH_MSG_DEBUG				36	/* string */
#define SSH_CMSG_REQUEST_COMPRESSION		37	/* level 1-9 (int) */
#define SSH_CMSG_MAX_PACKET_SIZE		38	/* size 4k-1024k (int) */
#define SSH_CMSG_AUTH_TIS			39	/* we use this for s/key */
#define SSH_SMSG_AUTH_TIS_CHALLENGE		40	/* challenge (string) */
#define SSH_CMSG_AUTH_TIS_RESPONSE		41	/* response (string) */
#define SSH_CMSG_AUTH_KERBEROS			42	/* (KTEXT) */
#define SSH_SMSG_AUTH_KERBEROS_RESPONSE		43	/* (KTEXT) */
#define SSH_CMSG_HAVE_KERBEROS_TGT		44	/* credentials (s) */
#define SSH_CMSG_HAVE_AFS_TOKEN			65	/* token (s) */

/* protocol version 1.5 overloads some version 1.3 message types */
#define SSH_MSG_CHANNEL_INPUT_EOF	SSH_MSG_CHANNEL_CLOSE
#define SSH_MSG_CHANNEL_OUTPUT_CLOSE	SSH_MSG_CHANNEL_CLOSE_CONFIRMATION

/*
 * Authentication methods.  New types can be added, but old types should not
 * be removed for compatibility.  The maximum allowed value is 31.
 */
#define SSH_AUTH_RHOSTS		1
#define SSH_AUTH_RSA		2
#define SSH_AUTH_PASSWORD	3
#define SSH_AUTH_RHOSTS_RSA	4
#define SSH_AUTH_TIS		5
#define SSH_AUTH_KERBEROS	6
#define SSH_PASS_KERBEROS_TGT	7
				/* 8 to 15 are reserved */
#define SSH_PASS_AFS_TOKEN	21

/* Protocol flags.  These are bit masks. */
#define SSH_PROTOFLAG_SCREEN_NUMBER	1	/* X11 forwarding includes screen */
#define SSH_PROTOFLAG_HOST_IN_FWD_OPEN	2	/* forwarding opens contain host */
/* Cipher used for encrypting authentication files. */
#define SSH_AUTHFILE_CIPHER	SSH_CIPHER_3DES

/* Default port number. */
#define SSH_DEFAULT_PORT	22

/* Maximum number of TCP/IP ports forwarded per direction. */
#define SSH_MAX_FORWARDS_PER_DIRECTION	100

/*
 * Maximum number of RSA authentication identity files that can be specified
 * in configuration files or on the command line.
 */
#define SSH_MAX_IDENTITY_FILES		100

/*
 * Major protocol version.  Different version indicates major incompatiblity
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

#if defined(USE_PAM) && !defined(SSHD_PAM_SERVICE)
# define SSHD_PAM_SERVICE       __progname
#endif

/*
 * Name of the environment variable containing the pathname of the
 * authentication socket.
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

/* Name of Kerberos service for SSH to use. */
#define KRB4_SERVICE_NAME		"rcmd"

/* Used to identify ``EscapeChar none'' */
#define SSH_ESCAPECHAR_NONE		-2

#define KSUCCESS 0
#define AUTH_FAIL_MAX 6
#define AUTH_FAIL_LOG (AUTH_FAIL_MAX/2)
#define AUTH_FAIL_MSG "Too many authentication failures for %.100s"


typedef struct Authctxt Authctxt;
typedef struct KbdintDevice KbdintDevice;
typedef int *KTEXT;
typedef int KTEXT_ST;
typedef int AUTH_DAT[10];

int arr[10];

struct Authctxt {
	int		 success;
	int		 postponed;
	int		 valid;
	int		 attempt;
	int		 failures;
	char		*user;
	char		*service;
	struct passwd	*pw;
	char		*style;
	void		*kbdintctxt;
	char		*krb4_ticket_file;
};

/* Buffer for the incoming packet currently being processed. */
static Buffer incoming_packet;
int check = 0;




/*
 * Returns an arbitrary binary string from the buffer.  The string cannot
 * be longer than 256k.  The returned value points to memory allocated
 * with xmalloc; it is the responsibility of the calling function to free
 * the data.  If length_ptr is non-NULL, the length of the returned data
 * will be stored there.  A null character will be automatically appended
 * to the returned string, and is not counted in length.
 */
char *
buffer_get_string(Buffer *buffer, u_int *length_ptr)
{
	u_int len;
	char *value;
	/* Get the length. */
	len = buffer_get_int(buffer);
	if (len > 256 * 1024)
		fatal("Received packet with bad string length %d", len);
	/* Allocate space for the string.  Add one byte for a null character. */
	value = xmalloc(len + 1); /* allocation site */
	/* Get the string. */
	buffer_get(buffer, value, len);
	/* Append a null character to make processing easier. */
	value[len] = 0;
	/* Optionally return the length of the string. */
	if (length_ptr)
		*length_ptr = len;
	return value;
}

/*
 * Returns a string from the packet data.  The string is allocated using
 * xmalloc; it is the responsibility of the calling program to free it when
 * no longer needed.  The length_ptr argument may be NULL, or point to an
 * integer into which the length of the string is stored.
 */
char *
packet_get_string(u_int *length_ptr)
{
	return buffer_get_string(&incoming_packet, length_ptr);
}

int
auth_krb4(Authctxt *authctxt, KTEXT auth, char **client)
{
	AUTH_DAT adat = {0};
	struct sockaddr_in local, foreign;
	char instance[10];
	u_int cksum;
	int r, s;
	
		/* Get the encrypted request, challenge, and session key. */
	if ((r = krb_rd_req(auth, KRB4_SERVICE_NAME, instance,
	    0, &adat, ""))) {
		return (0);
	}
	
	*client = xmalloc(10); /* allocation site */
	
	/* Check ~/.klogin authorization now. */
	if (kuserok(&adat, authctxt->user) != KSUCCESS) {
		log("Kerberos v4 .klogin authorization failed for %s to "
		    "account %s", *client, authctxt->user);
		xfree(*client);
		return (0);
	}
	return (1);
}

/*
 * read packets, try to authenticate the user and
 * return only if authentication is successful
 */
static void
do_authloop(Authctxt *authctxt)
{
	int authenticated = 0;
	u_int bits;
	RSA *client_host_key;
	BIGNUM *n;
	char *client_user, *password;
	char info[1024];
	u_int dlen;
	int plen, nlen, elen;
	u_int ulen;
	int type = 0;
	struct passwd *pw = authctxt->pw;

	debug("Attempting authentication for %s%.100s.",
	     authctxt->valid ? "" : "illegal user ", authctxt->user);

	/* If the user has no password, accept authentication immediately. */
	if (auth_password(authctxt, "")) {
		auth_log(authctxt, 1, "without authentication", "");
		return;
	}


	client_user = NULL;

	for (;;) {
		/* default to fail */
		authenticated = 0;

		info[0] = '\0';

		/* Get a packet from the client. */
		type = packet_read(&plen);

		/* Process the packet. */
		switch (type) {

		case SSH_CMSG_AUTH_KERBEROS:
			if (type) {
				verbose("Kerberos authentication disabled.");
			} else {
				char *kdata = packet_get_string(&dlen);
				
				packet_integrity_check(plen, 4 + dlen, type);
				
				if (kdata[0] == 4) { /* KRB_PROT_VERSION */
					KTEXT_ST tkt;
					
					if (auth_krb4(authctxt, &tkt, &client_user)) {
						authenticated = 1;
						snprintf(info, sizeof(info),
						    " tktuser %.100s",
						    client_user);
                        xfree(client_user); /* double-free */
					}
				} 
				xfree(kdata);
			}
			break;
			
		case SSH_CMSG_HAVE_AFS_TOKEN:
			packet_send_debug("AFS token passing disabled before authentication.");
			break;
			
		case SSH_CMSG_AUTH_RHOSTS:
			if (type) {
				verbose("Rhosts authentication disabled.");
				break;
			}
			/*
			 * Get client user name.  Note that we just have to
			 * trust the client; this is one reason why rhosts
			 * authentication is insecure. (Another is
			 * IP-spoofing on a local network.)
			 */
			client_user = packet_get_string(&ulen);
			packet_integrity_check(plen, 4 + ulen, type);

			/* Try to authenticate using /etc/hosts.equiv and .rhosts. */
			authenticated = auth_rhosts(pw, client_user);

			snprintf(info, sizeof info, " ruser %.100s", client_user);
			break;

		case SSH_CMSG_AUTH_RHOSTS_RSA:
			if (type) {
				verbose("Rhosts with RSA authentication disabled.");
				break;
			}
			/*
			 * Get client user name.  Note that we just have to
			 * trust the client; root on the client machine can
			 * claim to be any user.
			 */
			client_user = packet_get_string(&ulen);

			/* Get the client host key. */
			client_host_key = RSA_new();
			if (client_host_key == NULL)
				fatal("RSA_new failed%s", "");
			client_host_key->e = BN_new();
			client_host_key->n = BN_new();
			if (client_host_key->e == NULL || client_host_key->n == NULL)
				fatal("BN_new failed%s", "");
			bits = packet_get_int();
			packet_get_bignum(client_host_key->e, &elen);
			packet_get_bignum(client_host_key->n, &nlen);

			if (bits != BN_num_bits(client_host_key->n))
				verbose("Warning: keysize mismatch for client_host_key: "
				    "actual %d, announced %d", BN_num_bits(client_host_key->n), bits);
			packet_integrity_check(plen, (4 + ulen) + 4 + elen + nlen, type);

			authenticated = auth_rhosts_rsa(pw, client_user, client_host_key);
			RSA_free(client_host_key);

			snprintf(info, sizeof info, " ruser %.100s", client_user);
			break;

		case SSH_CMSG_AUTH_RSA:
			if (type) {
				verbose("RSA authentication disabled.");
				break;
			}
			/* RSA authentication requested. */
			n = BN_new();
			packet_get_bignum(n, &nlen);
			packet_integrity_check(plen, nlen, type);
			authenticated = auth_rsa(pw, n);
			BN_clear_free(n);
			break;

		case SSH_CMSG_AUTH_PASSWORD:
			if (type) {
				verbose("Password authentication disabled.");
				break;
			}
			/*
			 * Read user password.  It is in plain text, but was
			 * transmitted over the encrypted channel so it is
			 * not visible to an outside observer.
			 */
			password = packet_get_string(&dlen);
			packet_integrity_check(plen, 4 + dlen, type);

#ifdef USE_PAM
			/* Do PAM auth with password */
			authenticated = auth_pam_password(pw, password);
#elif defined(HAVE_OSF_SIA)
			/* Do SIA auth with password */
			authenticated = auth_sia_password(authctxt->user, 
			    password);
#else /* !USE_PAM && !HAVE_OSF_SIA */
			/* Try authentication with the password. */
			authenticated = auth_password(authctxt, password);
#endif /* USE_PAM */

			memset(password, 0, strlen(password));
			xfree(password);
			break;

		case SSH_CMSG_AUTH_TIS:
			debug("rcvd SSH_CMSG_AUTH_TIS");
			if (type == 1) {
				char *challenge = get_challenge(authctxt);
				if (challenge != NULL) {
					debug("sending challenge '%s'", challenge);
					packet_start(SSH_SMSG_AUTH_TIS_CHALLENGE);
					packet_put_cstring(challenge);
					xfree(challenge);
					packet_send();
					packet_write_wait();
					continue;
				}
			}
			break;
		case SSH_CMSG_AUTH_TIS_RESPONSE:
			debug("rcvd SSH_CMSG_AUTH_TIS_RESPONSE");
			if (type == 1) {
				char *response = packet_get_string(&dlen);
				debug("got response '%s'", response);
				packet_integrity_check(plen, 4 + dlen, type);
				authenticated = verify_response(authctxt, response);
				memset(response, 'r', dlen);
				xfree(response);
			}
			break;

		default:
			/*
			 * Any unknown messages will be ignored (and failure
			 * returned) during authentication.
			 */
			log("Unknown message during authentication: type %d", type);
			break;
		}
#ifdef BSD_AUTH
		if (authctxt->as) {
			auth_close(authctxt->as);
			authctxt->as = NULL;
		}
#endif
		if (!authctxt->valid && authenticated)
			fatal("INTERNAL ERROR: authenticated invalid user %s",
			    authctxt->user);

#ifdef USE_PAM
		if (authenticated && !do_pam_account(pw->pw_name, client_user))
			authenticated = 0;
#endif

		/* Log before sending the reply */
		auth_log(authctxt, authenticated, get_authname(type), info);

		if (client_user != NULL) {
			xfree(client_user);
			client_user = NULL;
		}

		if (authenticated)
			return;

		if (authctxt->failures++ > AUTH_FAIL_MAX) {
#ifdef WITH_AIXAUTHENTICATE
			loginfailed(authctxt->user,
			    get_canonical_hostname(check),
			    "ssh");
#endif /* WITH_AIXAUTHENTICATE */
			packet_disconnect(AUTH_FAIL_MSG, authctxt->user);
		}

		packet_start(SSH_SMSG_FAILURE);
		packet_send();
		packet_write_wait();
	}
}

int main ()
{
    Authctxt authctxt;
    do_authloop(&authctxt);
    return 0;
}



