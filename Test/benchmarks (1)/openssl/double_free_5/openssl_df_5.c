/*
	commit id : 229bd12487f8576fc088dc4f641950ac33c62033
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../openssl.h"

# define STACK_OF(type) struct stack_st_##type
# define LHASH_OF(type) struct lhash_st_##type
# define DEFINE_LHASH_OF(type) \
    LHASH_OF(type) { union lh_##type##_dummy { void* d1; unsigned long d2; int d3; } dummy; }; \

typedef struct lhash_node_st OPENSSL_LH_NODE;
typedef int (*OPENSSL_LH_COMPFUNC) (const void *, const void *);
typedef unsigned long (*OPENSSL_LH_HASHFUNC) (const void *);
typedef void (*OPENSSL_LH_DOALL_FUNC) (void *);
typedef void (*OPENSSL_LH_DOALL_FUNCARG) (void *, void *);
typedef struct lhash_st OPENSSL_LHASH;
struct lhash_node_st {
    void *data;
    struct lhash_node_st *next;
    unsigned long hash;
};

struct lhash_st {
    OPENSSL_LH_NODE **b;
    OPENSSL_LH_COMPFUNC comp;
    OPENSSL_LH_HASHFUNC hash;
    unsigned int num_nodes;
    unsigned int num_alloc_nodes;
    unsigned int p;
    unsigned int pmax;
    unsigned long up_load;      /* load times 256 */
    unsigned long down_load;    /* load times 256 */
    unsigned long num_items;
    unsigned long num_expands;
    unsigned long num_expand_reallocs;
    unsigned long num_contracts;
    unsigned long num_contract_reallocs;
    unsigned long num_hash_calls;
    unsigned long num_comp_calls;
    unsigned long num_insert;
    unsigned long num_replace;
    unsigned long num_delete;
    unsigned long num_no_delete;
    unsigned long num_retrieve;
    unsigned long num_retrieve_miss;
    unsigned long num_hash_comps;
    int error;
};

typedef struct {
    char *section;
    char *name;
    char *value;
} CONF_VALUE;

DEFINE_LHASH_OF(CONF_VALUE)

typedef struct conf_method_st {
    const char *name;
    void *(*create) (struct conf_method_st *meth);
    int (*init) (CONF *conf);
    int (*destroy) (CONF *conf);
    int (*destroy_data) (CONF *conf);
    int (*load_bio) (CONF *conf, BIO *bp, long *eline);
    int (*dump) (const CONF *conf, BIO *bp);
    int (*is_number) (const CONF *conf, char c);
    int (*to_int) (const CONF *conf, char c);
    int (*load) (CONF *conf, const char *name, long *eline);
}CONF_METHOD;

struct conf_st {
    CONF_METHOD *meth;
    void *meth_data;
    LHASH_OF(CONF_VALUE) *data;
};

static OPENSSL_LH_NODE **getrn(OPENSSL_LHASH *lh,
                               const void *data, unsigned long *rhash)
{
    OPENSSL_LH_NODE **ret, *n1;
    unsigned long hash, nn;
    OPENSSL_LH_COMPFUNC cf;

    hash = (*(lh->hash)) (data);
    lh->num_hash_calls++;
    *rhash = hash;

    nn = hash % lh->pmax;
    if (nn < lh->p)
        nn = hash % lh->num_alloc_nodes;

    cf = lh->comp;
    ret = &(lh->b[(int)nn]);
    for (n1 = *ret; n1 != NULL; n1 = n1->next) {
        lh->num_hash_comps++;
        if (n1->hash != hash) {
            ret = &(n1->next);
            continue;
        }
        lh->num_comp_calls++;
        if (cf(n1->data, data) == 0)
            break;
        ret = &(n1->next);
    }
    return (ret);
}

CONF_VALUE *lh_CONF_VALUE_retrieve(OPENSSL_LHASH *lh, const void *data)
{
    unsigned long hash;
    OPENSSL_LH_NODE **rn;
    void *ret;

    lh->error = 0;
    rn = getrn(lh, data, &hash);

    if (*rn == NULL) {
        lh->num_retrieve_miss++;
        return (NULL);
    } else {
        ret = (*rn)->data;
        lh->num_retrieve++;
    }
    return (ret);
}
/* Up until OpenSSL 0.9.5a, this was get_section */
CONF_VALUE *_CONF_get_section(const CONF *conf, const char *section)
{
    CONF_VALUE *v, vv;

    if ((conf == NULL) || (section == NULL))
        return (NULL);
    vv.name = NULL;
    vv.section = (char *)section;
    v = lh_CONF_VALUE_retrieve(conf->data, &vv);
    return (v);
}

/* Up until OpenSSL 0.9.5a, this was CONF_get_section */
STACK_OF(CONF_VALUE) *_CONF_get_section_values(const CONF *conf,
                                               const char *section)
{
    CONF_VALUE *v;

    v = _CONF_get_section(conf, section);
    if (v != NULL)
        return ((STACK_OF(CONF_VALUE) *)v->value);
    else
        return (NULL);
}


STACK_OF(CONF_VALUE) *CONF_get_section(const CONF *conf, const char *section)
{
    if (conf == NULL) {
        return NULL;
    }

    if (section == NULL) {
        return NULL;
    }

    return _CONF_get_section_values(conf, section);
}

CONF_VALUE *lh_CONF_VALUE_new()
{
	CONF_VALUE *ret;
	if((ret = OPENSSL_zalloc(sizeof(*ret))) == NULL)	/* allocation site */
		return NULL;
	return ret;
}

int _CONF_new_data(CONF *conf)
{
    if (conf == NULL) {
        return 0;
    }
    if (conf->data == NULL) {
        conf->data = lh_CONF_VALUE_new();
        if (conf->data == NULL)
            return 0;
    }
    return 1;
}

int NCONF_load_bio(CONF *conf, BIO *bp, long *eline)
{
    if (conf == NULL) {
        return 0;
    }

	if(_CONF_new_data(conf) == 0)
		return 0;

	return 1;
}

LHASH_OF(CONF_VALUE) *CONF_load_bio(LHASH_OF(CONF_VALUE) *conf, BIO *bp,
                                    long *eline)
{
    CONF ctmp;
    int ret;

    CONF_set_nconf(&ctmp, conf);

    ret = NCONF_load_bio(&ctmp, bp, eline);
    if (ret)
        return ctmp.data;
    return NULL;
}

LHASH_OF(CONF_VALUE) *CONF_load(LHASH_OF(CONF_VALUE) *conf, const char *file,
                                long *eline)
{
    LHASH_OF(CONF_VALUE) *ltmp;
    BIO *in = NULL;

    in = BIO_new_file(file, "r");
    if (in == NULL) {
        return NULL;
    }

    ltmp = CONF_load_bio(conf, in, eline);
    BIO_free(in);

    return ltmp;
}
static int certify_spkac(const char *infile, unsigned long chtype)
{
    STACK_OF(CONF_VALUE) *sk = NULL;
    LHASH_OF(CONF_VALUE) *parms = NULL;
    CONF_VALUE *cv = NULL;
    char *type, *buf;
    X509_NAME *n = NULL;
    X509_NAME_ENTRY *ne = NULL;
    int ok = -1, i, j;
    long errline;
    int nid;

    /*
     * Load input file into a hash table.  (This is just an easy
     * way to read and parse the file, then put it into a convenient
     * STACK format).
     */
    parms = CONF_load(NULL, infile, &errline);
    if (parms == NULL) {
        goto end;
    }

    sk = CONF_get_section(parms, "default");
    if (sk_CONF_VALUE_num(sk) == 0) {
        CONF_free(parms);	/* freed */
        goto end;
    }

    /*
     * Build up the subject name set.
     */

    for (i = 0;; i++) {
        if (sk_CONF_VALUE_num(sk) <= i)
            break;

        cv = sk_CONF_VALUE_value(sk, i);
        type = cv->name;
        /*
         * Skip past any leading X. X: X, etc to allow for multiple instances
         */
        for (buf = cv->name; *buf; buf++)
            if ((*buf == ':') || (*buf == ',') || (*buf == '.')) {
                buf++;
                if (*buf)
                    type = buf;
                break;
            }

        if (!X509_NAME_add_entry_by_NID(n, nid, chtype,
                                        (unsigned char *)buf, -1, -1, 0))
            goto end;
    }

end:
    CONF_free(parms);	/* double-free */
    return (ok);
}

int main(int argc, char **argv)
{
	certify_spkac(argv[0], argc);
	return 0;
}


