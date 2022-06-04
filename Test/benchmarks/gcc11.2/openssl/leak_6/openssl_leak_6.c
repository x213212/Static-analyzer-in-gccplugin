/*
	buggy parent : 141e470
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../openssl.h"

extern const BIGNUM _bignum_ffdhe2048_p;
extern const BIGNUM _bignum_ffdhe3072_p;
extern const BIGNUM _bignum_ffdhe4096_p;
extern const BIGNUM _bignum_ffdhe6144_p;
extern const BIGNUM _bignum_ffdhe8192_p;
extern const BIGNUM _bignum_const_2;

int BN_cmp(const BIGNUM *a, const BIGNUM *b)
{
    int i;
    int gt, lt;
    BN_ULONG t1, t2;

    if ((a == NULL) || (b == NULL)) {
        if (a != NULL)
            return (-1);
        else if (b != NULL)
            return 1;
        else
            return (0);
    }

    bn_check_top(a);
    bn_check_top(b);

    if (a->neg != b->neg) {
        if (a->neg)
            return (-1);
        else
            return 1;
    }
    if (a->neg == 0) {
        gt = 1;
        lt = -1;
    } else {
        gt = -1;
        lt = 1;
    }

    if (a->top > b->top)
        return (gt);
    if (a->top < b->top)
        return (lt);
    for (i = a->top - 1; i >= 0; i--) {
        t1 = a->d[i];
        t2 = b->d[i];
        if (t1 > t2)
            return (gt);
        if (t1 < t2)
            return (lt);
    }
    return (0);
}

int BN_rshift1(BIGNUM *r, const BIGNUM *a)
{
    BN_ULONG *ap, *rp, t, c;
    int i, j;

    bn_check_top(r);
    bn_check_top(a);

    if (BN_is_zero(a)) {
        BN_zero(r);
        return 1;
    }
    i = a->top;
    ap = a->d;
    j = i - (ap[i - 1] == 1);
    if (a != r) {
        if (bn_wexpand(r, j) == NULL)
            return (0);
        r->neg = a->neg;
    }
    rp = r->d;
    t = ap[--i];
    c = (t & 1) ? BN_TBIT : 0;
    if (t >>= 1)
        rp[i] = t;
    while (i > 0) {
        t = ap[--i];
        rp[i] = ((t >> 1) & BN_MASK2) | c;
        c = (t & 1) ? BN_TBIT : 0;
    }
    r->top = j;
    if (!r->top)
        r->neg = 0; /* don't allow negative zero */
    bn_check_top(r);
    return 1;
}

int DH_get_nid(const DH *dh)
{
    int nid;

    if (BN_get_word(dh->g) != 2)
        return NID_undef;
    if (!BN_cmp(dh->p, &_bignum_ffdhe2048_p))
        nid = NID_ffdhe2048;
    else if (!BN_cmp(dh->p, &_bignum_ffdhe3072_p))
        nid = NID_ffdhe3072;
    else if (!BN_cmp(dh->p, &_bignum_ffdhe4096_p))
        nid = NID_ffdhe4096;
    else if (!BN_cmp(dh->p, &_bignum_ffdhe6144_p))
        nid = NID_ffdhe6144;
    else if (!BN_cmp(dh->p, &_bignum_ffdhe8192_p))
        nid = NID_ffdhe8192;
    else
        return NID_undef;
    if (dh->q != NULL) {
        BIGNUM *q = BN_dup(dh->p);	/* allocation site */

        /* Check q = p * 2 + 1 we already know q is odd, so just shift right */
        if (q == NULL || !BN_rshift1(q, q) || !BN_cmp(dh->q, q)) {
            BN_free(q);
            return NID_undef;
        }
    }
    return nid;	/* memory leak */
}

int main()
{
	DH dh;
	DH_get_nid(&dh);
	return 0;
}
