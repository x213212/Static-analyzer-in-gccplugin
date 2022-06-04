/*
	buggy parent : 43a0449
	commit id : 59099d6b8a3aec77f7d9f310ebf8e31b09c2d518
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../openssl.h"

#define va_list char

/*
 * OSSL_STORE function codes.
 */
# define OSSL_STORE_F_FILE_CTRL                           129
# define OSSL_STORE_F_FILE_GET_PASS                       118
# define OSSL_STORE_F_FILE_LOAD                           119
# define OSSL_STORE_F_FILE_LOAD_TRY_DECODE                124
# define OSSL_STORE_F_FILE_NAME_TO_URI                    126
# define OSSL_STORE_F_FILE_OPEN                           120
# define OSSL_STORE_F_OSSL_STORE_ATTACH_PEM_BIO           127
# define OSSL_STORE_F_OSSL_STORE_FILE_ATTACH_PEM_BIO_INT  128
# define OSSL_STORE_F_OSSL_STORE_GET0_LOADER_INT          100
# define OSSL_STORE_F_OSSL_STORE_INFO_GET1_CERT           101
# define OSSL_STORE_F_OSSL_STORE_INFO_GET1_CRL            102
# define OSSL_STORE_F_OSSL_STORE_INFO_GET1_NAME           103
# define OSSL_STORE_F_OSSL_STORE_INFO_GET1_NAME_DESCRIPTION 135
# define OSSL_STORE_F_OSSL_STORE_INFO_GET1_PARAMS         104
# define OSSL_STORE_F_OSSL_STORE_INFO_GET1_PKEY           105
# define OSSL_STORE_F_OSSL_STORE_INFO_NEW_CERT            106
# define OSSL_STORE_F_OSSL_STORE_INFO_NEW_CRL             107
# define OSSL_STORE_F_OSSL_STORE_INFO_NEW_EMBEDDED        123
# define OSSL_STORE_F_OSSL_STORE_INFO_NEW_NAME            109
# define OSSL_STORE_F_OSSL_STORE_INFO_NEW_PARAMS          110
# define OSSL_STORE_F_OSSL_STORE_INFO_NEW_PKEY            111
# define OSSL_STORE_F_OSSL_STORE_INFO_SET0_NAME_DESCRIPTION 134
# define OSSL_STORE_F_OSSL_STORE_INIT_ONCE                112
# define OSSL_STORE_F_OSSL_STORE_LOADER_NEW               113
# define OSSL_STORE_F_OSSL_STORE_OPEN                     114
# define OSSL_STORE_F_OSSL_STORE_OPEN_INT                 115
# define OSSL_STORE_F_OSSL_STORE_REGISTER_LOADER_INT      117
# define OSSL_STORE_F_OSSL_STORE_UNREGISTER_LOADER_INT    116
# define OSSL_STORE_F_TRY_DECODE_PARAMS                   121
# define OSSL_STORE_F_TRY_DECODE_PKCS12                   122
# define OSSL_STORE_F_TRY_DECODE_PKCS8ENCRYPTED           125

/*
 * OSSL_STORE reason codes.
 */
# define OSSL_STORE_R_AMBIGUOUS_CONTENT_TYPE              107
# define OSSL_STORE_R_BAD_PASSWORD_READ                   115
# define OSSL_STORE_R_ERROR_VERIFYING_PKCS12_MAC          113
# define OSSL_STORE_R_INVALID_SCHEME                      106
# define OSSL_STORE_R_IS_NOT_A                            112
# define OSSL_STORE_R_LOADER_INCOMPLETE                   116
# define OSSL_STORE_R_NOT_A_CERTIFICATE                   100
# define OSSL_STORE_R_NOT_A_CRL                           101
# define OSSL_STORE_R_NOT_A_KEY                           102
# define OSSL_STORE_R_NOT_A_NAME                          103
# define OSSL_STORE_R_NOT_PARAMETERS                      104
# define OSSL_STORE_R_PASSPHRASE_CALLBACK_ERROR           114
# define OSSL_STORE_R_PATH_MUST_BE_ABSOLUTE               108
# define OSSL_STORE_R_UI_PROCESS_INTERRUPTED_OR_CANCELLED 109
# define OSSL_STORE_R_UNREGISTERED_SCHEME                 105
# define OSSL_STORE_R_UNSUPPORTED_CONTENT_TYPE            110
# define OSSL_STORE_R_URI_AUTHORITY_UNSUPPORED            111

typedef struct engine_st {} ENGINE;
typedef struct ossl_store_loader_st OSSL_STORE_LOADER;
const ENGINE *OSSL_STORE_LOADER_get0_engine(const OSSL_STORE_LOADER *loader);
const char *OSSL_STORE_LOADER_get0_scheme(const OSSL_STORE_LOADER *loader);
/* struct ossl_store_loader_ctx_st is defined differently by each loader */
typedef struct ossl_store_loader_ctx_st OSSL_STORE_LOADER_CTX;
typedef OSSL_STORE_LOADER_CTX *(*OSSL_STORE_open_fn)(const OSSL_STORE_LOADER
                                                     *loader,
                                                     const char *uri,
                                                     const UI_METHOD *ui_method,
                                                     void *ui_data);
int OSSL_STORE_LOADER_set_open(OSSL_STORE_LOADER *loader,
                               OSSL_STORE_open_fn open_function);
typedef int (*OSSL_STORE_ctrl_fn)(OSSL_STORE_LOADER_CTX *ctx, int cmd,
                                  va_list args);
int OSSL_STORE_LOADER_set_ctrl(OSSL_STORE_LOADER *loader,
                               OSSL_STORE_ctrl_fn ctrl_function);
typedef OSSL_STORE_INFO *(*OSSL_STORE_load_fn)(OSSL_STORE_LOADER_CTX *ctx,
                                               const UI_METHOD *ui_method,
                                               void *ui_data);
int OSSL_STORE_LOADER_set_load(OSSL_STORE_LOADER *loader,
                               OSSL_STORE_load_fn load_function);
typedef int (*OSSL_STORE_eof_fn)(OSSL_STORE_LOADER_CTX *ctx);
int OSSL_STORE_LOADER_set_eof(OSSL_STORE_LOADER *loader,
                              OSSL_STORE_eof_fn eof_function);
typedef int (*OSSL_STORE_error_fn)(OSSL_STORE_LOADER_CTX *ctx);
int OSSL_STORE_LOADER_set_error(OSSL_STORE_LOADER *loader,
                                OSSL_STORE_error_fn error_function);
typedef int (*OSSL_STORE_close_fn)(OSSL_STORE_LOADER_CTX *ctx);
int OSSL_STORE_LOADER_set_close(OSSL_STORE_LOADER *loader,
                                OSSL_STORE_close_fn close_function);
void OSSL_STORE_LOADER_free(OSSL_STORE_LOADER *loader);

int OSSL_STORE_register_loader(OSSL_STORE_LOADER *loader);
OSSL_STORE_LOADER *OSSL_STORE_unregister_loader(const char *scheme);


struct ossl_store_loader_st {
    const char *scheme;
    ENGINE *engine;
    OSSL_STORE_open_fn open;
    OSSL_STORE_ctrl_fn ctrl;
    OSSL_STORE_load_fn load;
    OSSL_STORE_eof_fn eof;
    OSSL_STORE_error_fn error;
    OSSL_STORE_close_fn close;
};


/*
 *  Functions for manipulating OSSL_STORE_LOADERs
 */

OSSL_STORE_LOADER *OSSL_STORE_LOADER_new(ENGINE *e, const char *scheme)
{
    OSSL_STORE_LOADER *res = OPENSSL_zalloc(sizeof(*res));	/* allocation site */

    if (res == NULL) {
        OSSL_STOREerr(OSSL_STORE_F_OSSL_STORE_LOADER_NEW, ERR_R_MALLOC_FAILURE);
        return NULL;										
    }

    /*
     * We usually don't check NULL arguments.  For loaders, though, the
     * scheme is crucial and must never be NULL, or the user will get
     * mysterious errors when trying to register the created loader
     * later on.
     */
    if (scheme == NULL) {
        OSSL_STOREerr(OSSL_STORE_F_OSSL_STORE_LOADER_NEW,
                      OSSL_STORE_R_INVALID_SCHEME);
        return NULL;											/* memory leak */
    }

    res->engine = e;
    res->scheme = scheme;
    return res;
}

int main()
{
	ENGINE e;
	OSSL_STORE_LOADER *new = OSSL_STORE_LOADER_new(&e, "scheme");
	printf("%s", new->scheme);
	OPENSSL_free(new);
	return 0;
}








