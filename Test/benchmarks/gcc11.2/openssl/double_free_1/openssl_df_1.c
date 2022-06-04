/*
	buggy parent : f5791af
	commit id : 432f8688bb72e21939845ac7a69359ca718c6676
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../openssl.h"

#define HDR_NAME 0x01
#define HDR_VALUE 0x02

struct stack_st {
	int num;
	const void **data;
	int sorted;
	int num_alloc;
};

typedef struct stack_st OPENSSL_STACK; /* Use STACK_OF(...) instead */

typedef struct {
char *section;
char *name;
char *value;
} CONF_VALUE;

# define DEFINE_STACK_OF(t) SKM_DEFINE_STACK_OF(t, t, t)

# define STACK_OF(type) struct stack_st_##type

# define SKM_DEFINE_STACK_OF(t1, t2, t3) \
    STACK_OF(t1); \
    typedef void (*sk_##t1##_freefunc)(t3 *a); \
    static STACK_OF(t1) *sk_##t1##_new_null(void) \
    { \
        return (STACK_OF(t1) *)OPENSSL_sk_new_null(); \
    } \
    static void sk_##t1##_free(STACK_OF(t1) *sk) \
    { \
        OPENSSL_sk_free((OPENSSL_STACK *)sk); \
    } \
    static int sk_##t1##_push(STACK_OF(t1) *sk, t2 *ptr) \
    { \
        return OPENSSL_sk_push((OPENSSL_STACK *)sk, (const void *)ptr); \
    } \
    static void sk_##t1##_pop_free(STACK_OF(t1) *sk, sk_##t1##_freefunc freefunc) \
    { \
        OPENSSL_sk_pop_free((OPENSSL_STACK *)sk, (OPENSSL_sk_freefunc)freefunc); \
    } \
    static int sk_##t1##_insert(STACK_OF(t1) *sk, t2 *ptr, int idx) \
    { \
        return OPENSSL_sk_insert((OPENSSL_STACK *)sk, (const void *)ptr, idx); \
    } \

typedef void (*OPENSSL_sk_freefunc)(void *);

OPENSSL_STACK *OPENSSL_sk_new_null(void)
{
    return OPENSSL_zalloc(sizeof(OPENSSL_STACK));   /* allocation site */
}

int OPENSSL_sk_push(OPENSSL_STACK *st, const void *data)
{
    if (st == NULL)
        return -1;
    return OPENSSL_sk_insert(st, data, st->num);
}

int OPENSSL_sk_insert(OPENSSL_STACK *st, const void *data, int loc)
{
    if (st == NULL || st->num == 255)
        return 0;

    if (!sk_reserve(st, 1, 0))
        return 0;

    if ((loc >= st->num) || (loc < 0)) {
        st->data[st->num] = data;
    } else {
        memmove(&st->data[loc + 1], &st->data[loc],
                sizeof(st->data[0]) * (st->num - loc));
        st->data[loc] = data;
    }
    st->num++;
    st->sorted = 0;
    return st->num;
}

void OPENSSL_sk_pop_free(OPENSSL_STACK *st, OPENSSL_sk_freefunc func)
{
    int i;

    if (st == NULL)
        return;
    for (i = 0; i < st->num; i++)
        if (st->data[i] != NULL)
            func((char *)st->data[i]);
    OPENSSL_sk_free(st);
}

void OPENSSL_sk_free(OPENSSL_STACK *st)
{
    if (st == NULL)
        return;
    OPENSSL_free(st->data);
    OPENSSL_free(st);
}

DEFINE_STACK_OF(CONF_VALUE)

void X509V3_conf_free(CONF_VALUE *conf)
{
    if (!conf)
        return;
    OPENSSL_free(conf->name);
    OPENSSL_free(conf->value);
    OPENSSL_free(conf->section);
    OPENSSL_free(conf);
}


/* Add a CONF_VALUE name value pair to stack */

int X509V3_add_value(const char *name, const char *value,
                     STACK_OF(CONF_VALUE) **extlist)
{
    CONF_VALUE *vtmp = NULL;
    char *tname = NULL, *tvalue = NULL;
    int sk_allocated = (*extlist == NULL);

    if (name && (tname = OPENSSL_strdup(name)) == NULL)
        goto err;
    if (value && (tvalue = OPENSSL_strdup(value)) == NULL)
        goto err;
    if ((vtmp = OPENSSL_malloc(sizeof(*vtmp))) == NULL)
        goto err;
    if (sk_allocated && (*extlist = sk_CONF_VALUE_new_null()) == NULL) /* allocation site */
        goto err;
    vtmp->section = NULL;
    vtmp->name = tname;
    vtmp->value = tvalue;
    if (!sk_CONF_VALUE_push(*extlist, vtmp))
        goto err;
    return 1;
 err:
    if (sk_allocated) {
        sk_CONF_VALUE_free(*extlist);   /* freed */
    }
    OPENSSL_free(vtmp);
    OPENSSL_free(tname);
    OPENSSL_free(tvalue);
    return 0;
}

STACK_OF(CONF_VALUE) *X509V3_parse_list(const char *line)
{
    char *p, *q, c;
    char *ntmp, *vtmp;
    STACK_OF(CONF_VALUE) *values = NULL;
    char *linebuf;
    int state;
    /* We are going to modify the line so copy it first */
    linebuf = OPENSSL_strdup(line);
    if (linebuf == NULL) {
        goto err;
    }
    state = HDR_NAME;
    ntmp = NULL;
    /* Go through all characters */
    for (p = linebuf, q = linebuf; (c = *p) && (c != '\r') && (c != '\n');
         p++) {

        switch (state) {
        case HDR_NAME:
            if (c == ':') {
                state = HDR_VALUE;
                *p = 0;
                ntmp = strip_spaces(q);
                if (!ntmp) {
                    goto err;
                }
                q = p + 1;
            } else if (c == ',') {
                *p = 0;
                ntmp = strip_spaces(q);
                q = p + 1;
                if (!ntmp) {
                    goto err;
                }
                X509V3_add_value(ntmp, NULL, &values);
            }
            break;

        case HDR_VALUE:
            if (c == ',') {
                state = HDR_NAME;
                *p = 0;
                vtmp = strip_spaces(q);
                if (!vtmp) {
                    goto err;
                }
                X509V3_add_value(ntmp, vtmp, &values);
                ntmp = NULL;
                q = p + 1;
            }

        }
    }

    if (state == HDR_VALUE) {
        vtmp = strip_spaces(q);
        if (!vtmp) {
            goto err;
        }
        X509V3_add_value(ntmp, vtmp, &values);
    } else {
        ntmp = strip_spaces(q);
        if (!ntmp) {
            goto err;
        }
        X509V3_add_value(ntmp, NULL, &values);
    }
    OPENSSL_free(linebuf);
    return values;

 err:
    OPENSSL_free(linebuf);
    sk_CONF_VALUE_pop_free(values, X509V3_conf_free);   /* double-free */
    return NULL;

}

int main(int argc, char **argv)
{
    STACK_OF(CONF_VALUE) *values = NULL;
	values = X509V3_parse_list(argv[0]);
    if(values != NULL) 
		sk_CONF_VALUE_pop_free(values, X509V3_conf_free);   /* double-free */	
	return 0;
}



