/*
    commit id : 8a96522482acd40af2e8a08696780a54c00b4feb
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../openssh.h"

#  define MAXPATHLEN 64
static const char authfile_id_string[] =
    "SSH PRIVATE KEY FILE FORMAT 1.1\n";

Key key;

static int
key_try_load_public(Key *k, const char *filename, char **commentp)
{
	FILE *f;
	char line[SSH_MAX_PUBKEY_BYTES];
	char *cp;
	u_long linenum = 0;

	f = fopen(filename, "r");
	if (f != NULL) {
		while (__RANDBOOL) {
			if (commentp) {
				*commentp = xstrdup(filename); /* allocation site */
			}
			fclose(f);
			return 1;
		}
		fclose(f);
	}
	return 0;
}

/*
 * Returns an arbitrary binary string from the buffer.  The string cannot
 * be longer than 256k.  The returned value points to memory allocated
 * with xmalloc; it is the responsibility of the calling function to free
 * the data.  If length_ptr is non-NULL, the length of the returned data
 * will be stored there.  A null character will be automatically appended
 * to the returned string, and is not counted in length.
 */
void *
buffer_get_string_ret(u_int *length_ptr)
{
	u_char *value;
	u_int len = rand() % 1024;
	
	/* Allocate space for the string.  Add one byte for a null character. */
	value = xmalloc(len + 1); /* allocation site */
	/* Get the string. */
	if (__RANDBOOL) {
		printf("buffer_get_string_ret: buffer_get failed\n");
		xfree(value);
		return (NULL);
	}
	/* Append a null character to make processing easier. */
	value[len] = '\0';
	/* Optionally return the length of the string. */
	if (length_ptr)
		*length_ptr = len;
	return (value);
}



void *
buffer_get_string(u_int *length_ptr)
{
	void *ret;

	if ((ret = buffer_get_string_ret(length_ptr)) == NULL)
		fatal("buffer_get_string: buffer error%s\n", "");
	return (ret);
}


/*
 * Parse the public, unencrypted portion of a RSA1 key.
 */
static Key *
key_parse_public_rsa1(char **commentp)
{
	Key *pub;

	pub = &key;
	if (commentp)
		*commentp = buffer_get_string(NULL);

	return pub;
}

/*
 * Loads the public part of the ssh v1 key file.  Returns NULL if an error was
 * encountered (the file does not exist or is not readable), and the key
 * otherwise.
 */
static Key *
key_load_public_rsa1(const char *filename, char **commentp)
{
	Key *pub;

	pub = key_parse_public_rsa1(commentp);
	if (pub == NULL)
		printf("Could not load \"%s\" as a RSA1 public key", filename);
	return pub;
}


/* load public key from private-key file, works only for SSH v1 */
Key *
key_load_public_type(int type, const char *filename, char **commentp)
{
	Key *pub;

	if (type == KEY_RSA1) {
		pub = key_load_public_rsa1(filename, commentp);
		return pub;
	}
	return NULL;
}

Key *
key_load_public(const char *filename, char **commentp)
{
	Key *pub;
	char file[MAXPATHLEN];

	/* try rsa1 private key */
	pub = key_load_public_type(KEY_RSA1, filename, commentp);
	if (pub != NULL)
		return pub;

	/* try rsa1 public key */
	pub = &key;
	if (key_try_load_public(pub, filename, commentp) == 1)
		return pub;

	/* try ssh2 public key */
	pub = &key;
	if (key_try_load_public(pub, filename, commentp) == 1)
		return pub;
	return NULL;
}

static int
delete_file(const char *filename, int key_only)
{
	Key *public = NULL, *cert = NULL;
	char *certpath = NULL, *comment = NULL;
	int ret = -1;

	public = key_load_public(filename, &comment);
	if (public == NULL) {
		printf("Bad key file %s\n", filename);
		return -1;
	}

	fprintf(stderr, "Identity removed: %s (%s)\n", filename, comment);

	if (key_only)
		goto out;

	/* Now try to delete the corresponding certificate too */
	free(comment); /* double-free */
	certpath = "filename";
	if ((cert = key_load_public(certpath, &comment)) == NULL)
		goto out;
	if(__RANDBOOL) {
		fprintf(stderr, "Identity removed: %s (%s)\n", certpath,
		    comment);
		ret = 0;
	} else
		fprintf(stderr, "Could not remove identity: %s\n", certpath);

 out:
	free(comment); /* double-free */

	return ret;
}

int main(int argc, char **argv) {
    int key_only = argc == 1? 1: 0;
	char *filename = argc == 1? argv[0] : argv[1];
	srand(time(NULL));
	delete_file(filename, key_only);
    return 0;
}
