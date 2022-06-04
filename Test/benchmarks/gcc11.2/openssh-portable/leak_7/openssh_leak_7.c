/*
    commit id : aae07e2e2000dd318418fd7fd4597760904cae32
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../openssh.h"

#define ASN1_ITYPE_PRIMITIVE		0x0
#define ASN1_ITYPE_SEQUENCE		0x1

typedef struct ASN1_ITEM_st ASN1_ITEM;
typedef struct ASN1_TEMPLATE_st ASN1_TEMPLATE;
typedef void ASN1_VALUE;

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define OPENSSL_malloc(size) malloc(size)
#define ASN1_EX_TYPE(flags, tag, stname, field, type) { \
    (flags), (tag), offsetof(stname, field),\
    #field, (&(type##_it)) }

#define ASN1_ITEM_start(itname) \
	const ASN1_ITEM itname##_it = {

#define ASN1_ITEM_end(itname) \
		};

/* Plain simple type */
#define ASN1_SIMPLE(stname, field, type) ASN1_EX_TYPE(0,0, stname, field, type)

#define ASN1_SEQUENCE(tname) static const ASN1_TEMPLATE tname##_seq_tt[] 

#define ASN1_SEQUENCE_END(stname) ASN1_SEQUENCE_END_name(stname, stname)

#define ASN1_SEQUENCE_END_name(stname, tname) \
	;\
	ASN1_ITEM_start(tname) \
		ASN1_ITYPE_SEQUENCE,\
		0,\
		tname##_seq_tt,\
		sizeof(tname##_seq_tt) / sizeof(ASN1_TEMPLATE),\
		NULL,\
		sizeof(stname),\
		#stname \
	ASN1_ITEM_end(tname)

struct ASN1_TEMPLATE_st {
	unsigned long flags;        /* Various flags */
	long tag;           /* tag, not used if no tagging */
	unsigned long offset;       /* Offset of this field in structure */
	#ifndef NO_ASN1_FIELD_NAMES
	const char *field_name;     /* Field name */
	#endif
	ASN1_ITEM *item;        /* Relevant ASN1_ITEM or ASN1_ADB */
};

struct ASN1_ITEM_st {
	char itype;			/* The item type, primitive, SEQUENCE, CHOICE or extern */
	long utype;			/* underlying type */
	const ASN1_TEMPLATE *templates;	/* If SEQUENCE or CHOICE this contains the contents */
	long tcount;			/* Number of templates if SEQUENCE or CHOICE */
	const void *funcs;		/* functions that handle this type */
	long size;			/* Structure size (usually)*/
	const char *sname;		/* Structure name */
};

ASN1_ITEM_start(BIGNUM)
    ASN1_ITYPE_PRIMITIVE, 0, NULL, 0, NULL, 1, "BIGNUM"
ASN1_ITEM_end(BIGNUM)

ASN1_SEQUENCE(ECDSA_SIG) = {
	ASN1_SIMPLE(ECDSA_SIG, r, BIGNUM),
	ASN1_SIMPLE(ECDSA_SIG, s, BIGNUM)
} ASN1_SEQUENCE_END(ECDSA_SIG)

int ASN1_item_new(ASN1_VALUE **pval, ASN1_ITEM *it)
{
	int i;
	ASN1_VALUE *pseqval;
	ASN1_TEMPLATE *tt = it->templates;
	switch(it->itype) {
	
	case ASN1_ITYPE_PRIMITIVE:
		*pval = OPENSSL_malloc(it->size);	/* allocation site */ 		
		if (!*pval)
			goto memerr;
		memset(*pval, 0, it->size);
	break;
	case ASN1_ITYPE_SEQUENCE:
		*pval = OPENSSL_malloc(it->size);
		if (!*pval)
			goto memerr;
		memset(*pval, 0, it->size);
        for (i = 0, tt = it->templates; i < it->tcount; tt++, i++)
            {
            pseqval = *pval + tt->offset;
            if (!ASN1_item_new(pseqval, tt->item))
                goto memerr;
            }
	break;
	}
	return 1;

memerr:
	printf("ASN1_ITEM_new ERROR\n");
	return 0;
} 

ECDSA_SIG *ECDSA_SIG_new(void)
{
	ECDSA_SIG *ret;
	ASN1_item_new(&ret, &ECDSA_SIG_it);
	return ret;
}

void ECDSA_SIG_free(ECDSA_SIG *p)
{
	free(p->r);
	free(p->s);
	free(p);
}

BIGNUM *BN_new(void)
{
	return malloc(sizeof(BIGNUM));
}

int main() {
	ECDSA_SIG *sig;
	
	/* parse signature */
	if ((sig = ECDSA_SIG_new()) == NULL)
		fatal("%s: ECDSA_SIG_new failed", __func__);

    if ((sig->r = BN_new()) == NULL ||	/* memory leak */	
	    (sig->s = BN_new()) == NULL)	/* memory leak */	
		fatal("%s: BN_new failed", __func__);

	ECDSA_SIG_free(sig);
    return 0;
}


