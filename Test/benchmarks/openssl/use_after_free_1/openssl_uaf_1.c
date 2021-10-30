/* 
    buggy parent : f1b8b00 
    commit id : 54e5ba058b4f2c6042c14d44868077e9ffcff818
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../openssl.h"

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


struct conf_st {
    void *meth_data;
    LHASH_OF(CONF_VALUE) *data;
};

typedef struct {
    char *section;
    char *name;
    char *value;
} CONF_VALUE;


static const char *save_rand_file;

int CRYPTO_atomic_add(int *val, int amount, int *ret)
{
    *val += amount;
    *ret  = *val;

    return 1;
}

static OPENSSL_LH_NODE **getrn(OPENSSL_LHASH *lh,
                               const void *data, unsigned long *rhash)
{
    OPENSSL_LH_NODE **ret, *n1;
    unsigned long hash, nn;
    OPENSSL_LH_COMPFUNC cf;
    int scratch;

    hash = (*(lh->hash)) (data);
    CRYPTO_atomic_add(&lh->num_hash_calls, 1, &scratch);
    *rhash = hash;

    nn = hash % lh->pmax;
    if (nn < lh->p)
        nn = hash % lh->num_alloc_nodes;

    cf = lh->comp;
    ret = &(lh->b[(int)nn]);
    for (n1 = *ret; n1 != NULL; n1 = n1->next) {
        CRYPTO_atomic_add(&lh->num_hash_comps, 1, &scratch);
        if (n1->hash != hash) {
            ret = &(n1->next);
            continue;
        }
        CRYPTO_atomic_add(&lh->num_comp_calls, 1, &scratch);
        if (cf(n1->data, data) == 0)
            break;
        ret = &(n1->next);
    }
    return (ret);
}


void lh_CONF_VALUE_free(OPENSSL_LHASH *lh)
{
    unsigned int i;
    OPENSSL_LH_NODE *n, *nn;

    if (lh == NULL)
        return;

    for (i = 0; i < lh->num_nodes; i++) {
        n = lh->b[i];
        while (n != NULL) {
            nn = n->next;
            OPENSSL_free(n);
            n = nn;
        }
    }
    OPENSSL_free(lh->b);
    OPENSSL_free(lh);
}

CONF_VALUE *lh_CONF_VALUE_new()
{
    OPENSSL_LHASH *ret;

    if ((ret = OPENSSL_zalloc(sizeof(*ret))) == NULL)
        return NULL;
    if ((ret->b = OPENSSL_zalloc(sizeof(*ret->b) * 1)) == NULL)
        goto err;
    return (ret);

err:
    OPENSSL_free(ret->b);
    OPENSSL_free(ret);
    return NULL;
}

void *lh_CONF_VALUE_retrieve(OPENSSL_LHASH *lh, const void *data)
{
    unsigned long hash;
    OPENSSL_LH_NODE **rn;
    void *ret;
    int scratch;

    lh->error = 0;
    rn = getrn(lh, data, &hash);

    if (*rn == NULL) {
        CRYPTO_atomic_add(&lh->num_retrieve_miss, 1, &scratch);
        return NULL;
    } else {
        ret = (*rn)->data;
        CRYPTO_atomic_add(&lh->num_retrieve, 1, &scratch);
    }
    return ret;
}

char *_CONF_get_string(const CONF *conf, const char *section,
                       const char *name)
{
    CONF_VALUE *v, vv;
    char *p;

    if (name == NULL)
        return (NULL);
    if (conf != NULL) {
        if (section != NULL) {
            vv.name = (char *)name;
            vv.section = (char *)section;
            v = lh_CONF_VALUE_retrieve(conf->data, &vv);
            if (v != NULL)
                return (v->value);
            return NULL;
        }
        vv.section = "default";
        vv.name = (char *)name;
        v = lh_CONF_VALUE_retrieve(conf->data, &vv);
        if (v != NULL)
            return (v->value);
        else
            return (NULL);
    } else
        return (getenv(name));
}

char *NCONF_get_string(const CONF *conf, const char *group, const char *name)
{
    char *s = _CONF_get_string(conf, group, name);

    if (s)
        return s;

    if (conf == NULL) {
        return NULL;
    }
    return NULL;
}

void app_RAND_load_conf(CONF *c, const char *section)
{
    const char *randfile = NCONF_get_string(c, section, "RANDFILE");

    if (randfile == NULL) {
        return;
    }
    
    if (save_rand_file == NULL)
        save_rand_file = randfile;
}

void app_RAND_write(void)
{
    if (save_rand_file == NULL)
        return;
    if (RAND_write_file(save_rand_file) == -1) {
        printf("RAND_write_file error\n");
    }
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

void CONF_free(CONF *conf)
{
    if (conf == NULL || conf->data == NULL)
        return;
    
	lh_CONF_VALUE_free(conf->data);
	OPENSSL_free(conf);
}

CONF *CONF_new()
{
    CONF *ret;

    ret = OPENSSL_malloc(sizeof(*ret));
    return ret;
}

void foo()
{
    CONF *conf = NULL;
    if ((conf = CONF_new()) == NULL)
        goto end;
    conf->data = NULL;
    if (_CONF_new_data(conf) == 0) 
		goto end;	
	app_RAND_load_conf(conf, "section");

end:
    CONF_free(conf);
}

int main()
{
	foo();
	app_RAND_write(); // use-after-free
    return 0;
}


