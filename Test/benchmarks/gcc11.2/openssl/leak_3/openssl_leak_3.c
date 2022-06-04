/*
    buggy parent : 590bbdf
    commit id : 04761b557a53f026630dd5916b2b8522d94579db
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../openssl.h"

/* Field is optional */
# define ASN1_TFLG_OPTIONAL      (0x1)

/* Field is a SET OF */
# define ASN1_TFLG_SET_OF        (0x1 << 1)

/* Field is a SEQUENCE OF */
# define ASN1_TFLG_SEQUENCE_OF   (0x2 << 1)

/*
 * Special case: this refers to a SET OF that will be sorted into DER order
 * when encoded *and* the corresponding STACK will be modified to match the
 * new order.
 */
# define ASN1_TFLG_SET_ORDER     (0x3 << 1)

/* Mask for SET OF or SEQUENCE OF */
# define ASN1_TFLG_SK_MASK       (0x3 << 1)

/*
 * These flags mean the tag should be taken from the tag field. If EXPLICIT
 * then the underlying type is used for the inner tag.
 */

/* IMPLICIT tagging */
# define ASN1_TFLG_IMPTAG        (0x1 << 3)

/* EXPLICIT tagging, inner tag from underlying type */
# define ASN1_TFLG_EXPTAG        (0x2 << 3)

# define ASN1_TFLG_TAG_MASK      (0x3 << 3)

/* context specific IMPLICIT */
# define ASN1_TFLG_IMPLICIT      (ASN1_TFLG_IMPTAG|ASN1_TFLG_CONTEXT)

/* context specific EXPLICIT */
# define ASN1_TFLG_EXPLICIT      (ASN1_TFLG_EXPTAG|ASN1_TFLG_CONTEXT)

/*
 * If tagging is in force these determine the type of tag to use. Otherwise
 * the tag is determined by the underlying type. These values reflect the
 * actual octet format.
 */

/* Universal tag */
# define ASN1_TFLG_UNIVERSAL     (0x0<<6)
/* Application tag */
# define ASN1_TFLG_APPLICATION   (0x1<<6)
/* Context specific tag */
# define ASN1_TFLG_CONTEXT       (0x2<<6)
/* Private tag */
# define ASN1_TFLG_PRIVATE       (0x3<<6)

# define ASN1_TFLG_TAG_CLASS     (0x3<<6)

/*
 * These are for ANY DEFINED BY type. In this case the 'item' field points to
 * an ASN1_ADB structure which contains a table of values to decode the
 * relevant type
 */

# define ASN1_TFLG_ADB_MASK      (0x3<<8)

# define ASN1_TFLG_ADB_OID       (0x1<<8)

# define ASN1_TFLG_ADB_INT       (0x1<<9)

/*
 * This flag when present in a SEQUENCE OF, SET OF or EXPLICIT causes
 * indefinite length constructed encoding to be used if required.
 */

# define ASN1_TFLG_NDEF          (0x1<<11)

/* Field is embedded and not a pointer */
# define ASN1_TFLG_EMBED         (0x1 << 12)


# define ASN1_ITYPE_PRIMITIVE            0x0

# define ASN1_ITYPE_SEQUENCE             0x1

# define ASN1_ITYPE_CHOICE               0x2

# define ASN1_ITYPE_EXTERN               0x4

# define ASN1_ITYPE_MSTRING              0x5

# define ASN1_ITYPE_NDEF_SEQUENCE        0x6

/* Use a reference count */
# define ASN1_AFLG_REFCOUNT      1
/* Save the encoding of structure (useful for signatures) */
# define ASN1_AFLG_ENCODING      2
/* The Sequence length is invalid */
# define ASN1_AFLG_BROKEN        4

/* operation values for asn1_cb */

# define ASN1_OP_NEW_PRE         0
# define ASN1_OP_NEW_POST        1
# define ASN1_OP_FREE_PRE        2
# define ASN1_OP_FREE_POST       3
# define ASN1_OP_D2I_PRE         4
# define ASN1_OP_D2I_POST        5
# define ASN1_OP_I2D_PRE         6
# define ASN1_OP_I2D_POST        7
# define ASN1_OP_PRINT_PRE       8
# define ASN1_OP_PRINT_POST      9
# define ASN1_OP_STREAM_PRE      10
# define ASN1_OP_STREAM_POST     11
# define ASN1_OP_DETACHED_PRE    12
# define ASN1_OP_DETACHED_POST   13


#  define ASN1_ITEM_rptr(ref) (&(ref##_it))

# define IMPLEMENT_ASN1_FUNCTIONS(stname) IMPLEMENT_ASN1_FUNCTIONS_fname(stname, stname, stname)

# define IMPLEMENT_ASN1_ALLOC_FUNCTIONS_fname(stname, itname, fname) \
        stname *fname##_new(void) \
        { \
                return (stname *)ASN1_item_new(ASN1_ITEM_rptr(itname)); \
        } \
        void fname##_free(stname *a) \
        { \
                ASN1_item_free((ASN1_VALUE *)a, ASN1_ITEM_rptr(itname)); \
        }

# define IMPLEMENT_ASN1_FUNCTIONS_fname(stname, itname, fname) \
        IMPLEMENT_ASN1_ALLOC_FUNCTIONS_fname(stname, itname, fname)

/* Macro to implement a primitive type */
# define IMPLEMENT_ASN1_TYPE(stname) IMPLEMENT_ASN1_TYPE_ex(stname, stname, 0)
# define IMPLEMENT_ASN1_TYPE_ex(itname, vname, ex) \
                                ASN1_ITEM_start(itname) \
                                        ASN1_ITYPE_PRIMITIVE, V_##vname, NULL, 0, NULL, ex, #itname \
                                ASN1_ITEM_end(itname)

#  define ASN1_ITEM_start(itname) \
const ASN1_ITEM itname##_it = {

#  define ASN1_ITEM_end(itname)                 \
                };

static int asn1_primitive_new(ASN1_VALUE **pval, const ASN1_ITEM *it,
                              int embed)
{
    ASN1_TYPE *typ;
    ASN1_STRING *str;
    int utype;

    if (!it)
        return 0;

    if (it->funcs) {
        const ASN1_PRIMITIVE_FUNCS *pf = it->funcs;
        if (embed) {
            if (pf->prim_clear) {
                pf->prim_clear(pval, it);
                return 1;
            }
        } else if (pf->prim_new) {
            return pf->prim_new(pval, it);
        }
    }

    if (it->itype == ASN1_ITYPE_MSTRING)
        utype = -1;
    else
        utype = it->utype;
    switch (utype) {
    default:
        str = ASN1_STRING_type_new(utype); /* allocation site */
        *pval = (ASN1_VALUE *)str;
        if (it->itype == ASN1_ITYPE_MSTRING && str)
            str->flags |= ASN1_STRING_FLAG_MSTRING;
        break;
    }
    if (*pval)
        return 1;
    return 0;
}

ASN1_VALUE *ASN1_item_new(const ASN1_ITEM *it)
{
    ASN1_VALUE *ret = NULL;
    if (ASN1_item_ex_new(&ret, it) > 0)
        return ret;
    return NULL;
}

/* Allocate an ASN1 structure */

int ASN1_item_ex_new(ASN1_VALUE **pval, const ASN1_ITEM *it)
{
    return asn1_item_embed_new(pval, it, 0);
}

int asn1_item_embed_new(ASN1_VALUE **pval, const ASN1_ITEM *it, int embed)
{
    const ASN1_TEMPLATE *tt = NULL;
    const ASN1_EXTERN_FUNCS *ef;
    const ASN1_AUX *aux = it->funcs;
    ASN1_aux_cb *asn1_cb;
    ASN1_VALUE **pseqval;
    int i;
    if (aux && aux->asn1_cb)
        asn1_cb = aux->asn1_cb;
    else
        asn1_cb = 0;

    switch (it->itype) {

    case ASN1_ITYPE_PRIMITIVE:
        if (it->templates) {
            if (!asn1_template_new(pval, it->templates))
                goto memerr;
        } else if (!asn1_primitive_new(pval, it, embed))
            goto memerr;
        break;

    case ASN1_ITYPE_NDEF_SEQUENCE:
    case ASN1_ITYPE_SEQUENCE:
        if (asn1_cb) {
            i = asn1_cb(ASN1_OP_NEW_PRE, pval, it, NULL);
            if (!i)
                goto auxerr;
            if (i == 2) {
                return 1;
            }
        }
        if (embed) {
            memset(*pval, 0, it->size);
        } else {
            *pval = OPENSSL_zalloc(it->size);
            if (*pval == NULL)
                goto memerr;
        }
        for (i = 0, tt = it->templates; i < it->tcount; tt++, i++) {
            pseqval = asn1_get_field_ptr(pval, tt);
            if (!asn1_template_new(pseqval, tt))
                goto memerr2;
        }
        if (asn1_cb && !asn1_cb(ASN1_OP_NEW_POST, pval, it, NULL))
            goto auxerr2;
        break;
    }
    return 1;

 memerr2:
    if (!embed)
        ASN1_item_ex_free(pval, it);
 memerr:
    return 0;

 auxerr2:
    if (!embed)
        ASN1_item_ex_free(pval, it);
 auxerr:
    return 0;

}

static void asn1_item_clear(ASN1_VALUE **pval, const ASN1_ITEM *it)
{
    const ASN1_EXTERN_FUNCS *ef;

    switch (it->itype) {

    case ASN1_ITYPE_EXTERN:
        ef = it->funcs;
        if (ef && ef->asn1_ex_clear)
            ef->asn1_ex_clear(pval, it);
        else
            *pval = NULL;
        break;

    case ASN1_ITYPE_PRIMITIVE:
        if (it->templates)
            asn1_template_clear(pval, it->templates);
        else
            asn1_primitive_clear(pval, it);
        break;

    case ASN1_ITYPE_MSTRING:
        asn1_primitive_clear(pval, it);
        break;

    case ASN1_ITYPE_CHOICE:
    case ASN1_ITYPE_SEQUENCE:
    case ASN1_ITYPE_NDEF_SEQUENCE:
        *pval = NULL;
        break;
    }
}

static int asn1_template_new(ASN1_VALUE **pval, const ASN1_TEMPLATE *tt)
{
    const ASN1_ITEM *it = ASN1_ITEM_ptr(tt->item);
    int embed = tt->flags & ASN1_TFLG_EMBED;
    ASN1_VALUE *tval;
    int ret;
    if (embed) {
        tval = (ASN1_VALUE *)pval;
        pval = &tval;
    }
    if (tt->flags & ASN1_TFLG_OPTIONAL) {
        asn1_template_clear(pval, tt);
        return 1;
    }
    /* If ANY DEFINED BY nothing to do */

    if (tt->flags & ASN1_TFLG_ADB_MASK) {
        *pval = NULL;
        return 1;
    }
#ifndef OPENSSL_NO_CRYPTO_MDEBUG
    OPENSSL_mem_debug_push(tt->field_name
            ? tt->field_name : "asn1_template_new");
#endif
    /* If SET OF or SEQUENCE OF, its a STACK */
    if (tt->flags & ASN1_TFLG_SK_MASK) {
        STACK_OF(ASN1_VALUE) *skval;
        skval = sk_ASN1_VALUE_new_null();
        if (!skval) {
            ret = 0;
            goto done;
        }
        *pval = (ASN1_VALUE *)skval;
        ret = 1;
        goto done;
    }
    /* Otherwise pass it back to the item routine */
    ret = asn1_item_embed_new(pval, it, embed);
 done:
#ifndef OPENSSL_NO_CRYPTO_MDEBUG
    OPENSSL_mem_debug_pop();
#endif
    return ret;
}

static void asn1_template_clear(ASN1_VALUE **pval, const ASN1_TEMPLATE *tt)
{
    /* If ADB or STACK just NULL the field */
    if (tt->flags & (ASN1_TFLG_ADB_MASK | ASN1_TFLG_SK_MASK))
        *pval = NULL;
    else
        asn1_item_clear(pval, ASN1_ITEM_ptr(tt->item));
}

static void asn1_primitive_clear(ASN1_VALUE **pval, const ASN1_ITEM *it)
{
    int utype;
    if (it && it->funcs) {
        const ASN1_PRIMITIVE_FUNCS *pf = it->funcs;
        if (pf->prim_clear)
            pf->prim_clear(pval, it);
        else
            *pval = NULL;
        return;
    }
    if (!it || (it->itype == ASN1_ITYPE_MSTRING))
        utype = -1;
    else
        utype = it->utype;
    if (utype == V_ASN1_BOOLEAN)
        *(ASN1_BOOLEAN *)pval = it->size;
    else
        *pval = NULL;
}
#define V_OTHERNAME 0
IMPLEMENT_ASN1_TYPE(OTHERNAME)
IMPLEMENT_ASN1_FUNCTIONS(OTHERNAME)

void GENERAL_NAME_set0_value(GENERAL_NAME *a, int type, void *value)
{
    switch (type) {
    case GEN_X400:
    case GEN_EDIPARTY:
        a->d.other = value;
        break;

    case GEN_OTHERNAME:
        a->d.otherName = value;
        break;

    case GEN_EMAIL:
    case GEN_DNS:
    case GEN_URI:
        a->d.ia5 = value;
        break;

    case GEN_DIRNAME:
        a->d.dirn = value;
        break;

    case GEN_IPADD:
        a->d.ip = value;
        break;

    case GEN_RID:
        a->d.rid = value;
        break;
    }
    a->type = type;
}


int GENERAL_NAME_set0_othername(GENERAL_NAME *gen,
                                ASN1_OBJECT *oid, ASN1_TYPE *value)
{
    OTHERNAME *oth;
    oth = OTHERNAME_new();
    if (oth == NULL)
        return 0;
    oth->type_id = oid;
    oth->value = value; /* memory leak */
    GENERAL_NAME_set0_value(gen, GEN_OTHERNAME, oth);
    return 1;
}

int main ()
{
   ASN1_TYPE *value = malloc(sizeof(ASN1_TYPE));  
   GENERAL_NAME_set0_othername(NULL, NULL, value);
   free(value);
   return 0;
}



