/*
	buggy parent : c753e71
	commit id : 865886553dfb842464600e76b6e8f127b377a1ca
*/

#include <stdio.h>
#include <stdlib.h>

#include "../stdio.h"
#include "../openssl.h"

/* Function codes. */
#define ASN1_F_A2D_ASN1_OBJECT				 100
#define ASN1_F_A2I_ASN1_ENUMERATED			 101
#define ASN1_F_A2I_ASN1_INTEGER				 102
#define ASN1_F_A2I_ASN1_STRING				 103
#define ASN1_F_APPEND_EXP				 176
#define ASN1_F_ASN1_BIT_STRING_SET_BIT			 183
#define ASN1_F_ASN1_CB					 177
#define ASN1_F_ASN1_CHECK_TLEN				 104
#define ASN1_F_ASN1_COLLATE_PRIMITIVE			 105
#define ASN1_F_ASN1_COLLECT				 106
#define ASN1_F_ASN1_D2I_EX_PRIMITIVE			 108
#define ASN1_F_ASN1_D2I_FP				 109
#define ASN1_F_ASN1_D2I_READ_BIO			 107
#define ASN1_F_ASN1_DIGEST				 184
#define ASN1_F_ASN1_DO_ADB				 110
#define ASN1_F_ASN1_DUP					 111
#define ASN1_F_ASN1_ENUMERATED_SET			 112
#define ASN1_F_ASN1_ENUMERATED_TO_BN			 113
#define ASN1_F_ASN1_EX_C2I				 204
#define ASN1_F_ASN1_FIND_END				 190
#define ASN1_F_ASN1_GENERALIZEDTIME_ADJ			 216
#define ASN1_F_ASN1_GENERALIZEDTIME_SET			 185
#define ASN1_F_ASN1_GENERATE_V3				 178
#define ASN1_F_ASN1_GET_OBJECT				 114
#define ASN1_F_ASN1_HEADER_NEW				 115
#define ASN1_F_ASN1_I2D_BIO				 116
#define ASN1_F_ASN1_I2D_FP				 117
#define ASN1_F_ASN1_INTEGER_SET				 118
#define ASN1_F_ASN1_INTEGER_TO_BN			 119
#define ASN1_F_ASN1_ITEM_D2I_FP				 206
#define ASN1_F_ASN1_ITEM_DUP				 191
#define ASN1_F_ASN1_ITEM_EX_COMBINE_NEW			 121
#define ASN1_F_ASN1_ITEM_EX_D2I				 120
#define ASN1_F_ASN1_ITEM_I2D_BIO			 192
#define ASN1_F_ASN1_ITEM_I2D_FP				 193
#define ASN1_F_ASN1_ITEM_PACK				 198
#define ASN1_F_ASN1_ITEM_SIGN				 195
#define ASN1_F_ASN1_ITEM_SIGN_CTX			 220
#define ASN1_F_ASN1_ITEM_UNPACK				 199
#define ASN1_F_ASN1_ITEM_VERIFY				 197
#define ASN1_F_ASN1_MBSTRING_NCOPY			 122
#define ASN1_F_ASN1_OBJECT_NEW				 123
#define ASN1_F_ASN1_OUTPUT_DATA				 214
#define ASN1_F_ASN1_PACK_STRING				 124
#define ASN1_F_ASN1_PCTX_NEW				 205
#define ASN1_F_ASN1_PKCS5_PBE_SET			 125
#define ASN1_F_ASN1_SCTX_NEW				 221
#define ASN1_F_ASN1_SEQ_PACK				 126
#define ASN1_F_ASN1_SEQ_UNPACK				 127
#define ASN1_F_ASN1_SIGN				 128
#define ASN1_F_ASN1_STR2TYPE				 179
#define ASN1_F_ASN1_STRING_SET				 186
#define ASN1_F_ASN1_STRING_TABLE_ADD			 129
#define ASN1_F_ASN1_STRING_TYPE_NEW			 130
#define ASN1_F_ASN1_TEMPLATE_EX_D2I			 132
#define ASN1_F_ASN1_TEMPLATE_NEW			 133
#define ASN1_F_ASN1_TEMPLATE_NOEXP_D2I			 131
#define ASN1_F_ASN1_TIME_ADJ				 217
#define ASN1_F_ASN1_TIME_SET				 175
#define ASN1_F_ASN1_TYPE_GET_INT_OCTETSTRING		 134
#define ASN1_F_ASN1_TYPE_GET_OCTETSTRING		 135
#define ASN1_F_ASN1_UNPACK_STRING			 136
#define ASN1_F_ASN1_UTCTIME_ADJ				 218
#define ASN1_F_ASN1_UTCTIME_SET				 187
#define ASN1_F_ASN1_VERIFY				 137
#define ASN1_F_B64_READ_ASN1				 209
#define ASN1_F_B64_WRITE_ASN1				 210
#define ASN1_F_BIO_NEW_NDEF				 208
#define ASN1_F_BITSTR_CB				 180
#define ASN1_F_BN_TO_ASN1_ENUMERATED			 138
#define ASN1_F_BN_TO_ASN1_INTEGER			 139
#define ASN1_F_C2I_ASN1_BIT_STRING			 189
#define ASN1_F_C2I_ASN1_INTEGER				 194
#define ASN1_F_C2I_ASN1_OBJECT				 196
#define ASN1_F_COLLECT_DATA				 140
#define ASN1_F_D2I_ASN1_BIT_STRING			 141
#define ASN1_F_D2I_ASN1_BOOLEAN				 142
#define ASN1_F_D2I_ASN1_BYTES				 143
#define ASN1_F_D2I_ASN1_GENERALIZEDTIME			 144
#define ASN1_F_D2I_ASN1_HEADER				 145
#define ASN1_F_D2I_ASN1_INTEGER				 146
#define ASN1_F_D2I_ASN1_OBJECT				 147
#define ASN1_F_D2I_ASN1_SET				 148
#define ASN1_F_D2I_ASN1_TYPE_BYTES			 149
#define ASN1_F_D2I_ASN1_UINTEGER			 150
#define ASN1_F_D2I_ASN1_UTCTIME				 151
#define ASN1_F_D2I_AUTOPRIVATEKEY			 207
#define ASN1_F_D2I_NETSCAPE_RSA				 152
#define ASN1_F_D2I_NETSCAPE_RSA_2			 153
#define ASN1_F_D2I_PRIVATEKEY				 154
#define ASN1_F_D2I_PUBLICKEY				 155
#define ASN1_F_D2I_RSA_NET				 200
#define ASN1_F_D2I_RSA_NET_2				 201
#define ASN1_F_D2I_X509					 156
#define ASN1_F_D2I_X509_CINF				 157
#define ASN1_F_D2I_X509_PKEY				 159
#define ASN1_F_DO_TCREATE				 222
#define ASN1_F_I2D_ASN1_BIO_STREAM			 211
#define ASN1_F_I2D_ASN1_SET				 188
#define ASN1_F_I2D_ASN1_TIME				 160
#define ASN1_F_I2D_DSA_PUBKEY				 161
#define ASN1_F_I2D_EC_PUBKEY				 181
#define ASN1_F_I2D_PRIVATEKEY				 163
#define ASN1_F_I2D_PUBLICKEY				 164
#define ASN1_F_I2D_RSA_NET				 162
#define ASN1_F_I2D_RSA_PUBKEY				 165
#define ASN1_F_LONG_C2I					 166
#define ASN1_F_OID_MODULE_INIT				 174
#define ASN1_F_PARSE_TAGGING				 182
#define ASN1_F_PKCS5_PBE2_SET_IV			 167
#define ASN1_F_PKCS5_PBE_SET				 202
#define ASN1_F_PKCS5_PBE_SET0_ALGOR			 215
#define ASN1_F_PKCS5_PBKDF2_SET				 219
#define ASN1_F_SMIME_READ_ASN1				 212
#define ASN1_F_SMIME_TEXT				 213
#define ASN1_F_STBL_MODULE_INIT				 223
#define ASN1_F_X509_CINF_NEW				 168
#define ASN1_F_X509_CRL_ADD0_REVOKED			 169
#define ASN1_F_X509_INFO_NEW				 170
#define ASN1_F_X509_NAME_ENCODE				 203
#define ASN1_F_X509_NAME_EX_D2I				 158
#define ASN1_F_X509_NAME_EX_NEW				 171
#define ASN1_F_X509_NEW					 172
#define ASN1_F_X509_PKEY_NEW				 173

/* Reason codes. */
#define ASN1_R_ADDING_OBJECT				 171
#define ASN1_R_ASN1_PARSE_ERROR				 203
#define ASN1_R_ASN1_SIG_PARSE_ERROR			 204
#define ASN1_R_AUX_ERROR				 100
#define ASN1_R_BAD_CLASS				 101
#define ASN1_R_BAD_OBJECT_HEADER			 102
#define ASN1_R_BAD_PASSWORD_READ			 103
#define ASN1_R_BAD_TAG					 104
#define ASN1_R_BMPSTRING_IS_WRONG_LENGTH		 214
#define ASN1_R_BN_LIB					 105
#define ASN1_R_BOOLEAN_IS_WRONG_LENGTH			 106
#define ASN1_R_BUFFER_TOO_SMALL				 107
#define ASN1_R_CIPHER_HAS_NO_OBJECT_IDENTIFIER		 108
#define ASN1_R_CONTEXT_NOT_INITIALISED			 217
#define ASN1_R_DATA_IS_WRONG				 109
#define ASN1_R_DECODE_ERROR				 110
#define ASN1_R_DECODING_ERROR				 111
#define ASN1_R_DEPTH_EXCEEDED				 174
#define ASN1_R_DIGEST_AND_KEY_TYPE_NOT_SUPPORTED	 198
#define ASN1_R_ENCODE_ERROR				 112
#define ASN1_R_ERROR_GETTING_TIME			 173
#define ASN1_R_ERROR_LOADING_SECTION			 172
#define ASN1_R_ERROR_PARSING_SET_ELEMENT		 113
#define ASN1_R_ERROR_SETTING_CIPHER_PARAMS		 114
#define ASN1_R_EXPECTING_AN_INTEGER			 115
#define ASN1_R_EXPECTING_AN_OBJECT			 116
#define ASN1_R_EXPECTING_A_BOOLEAN			 117
#define ASN1_R_EXPECTING_A_TIME				 118
#define ASN1_R_EXPLICIT_LENGTH_MISMATCH			 119
#define ASN1_R_EXPLICIT_TAG_NOT_CONSTRUCTED		 120
#define ASN1_R_FIELD_MISSING				 121
#define ASN1_R_FIRST_NUM_TOO_LARGE			 122
#define ASN1_R_HEADER_TOO_LONG				 123
#define ASN1_R_ILLEGAL_BITSTRING_FORMAT			 175
#define ASN1_R_ILLEGAL_BOOLEAN				 176
#define ASN1_R_ILLEGAL_CHARACTERS			 124
#define ASN1_R_ILLEGAL_FORMAT				 177
#define ASN1_R_ILLEGAL_HEX				 178
#define ASN1_R_ILLEGAL_IMPLICIT_TAG			 179
#define ASN1_R_ILLEGAL_INTEGER				 180
#define ASN1_R_ILLEGAL_NESTED_TAGGING			 181
#define ASN1_R_ILLEGAL_NULL				 125
#define ASN1_R_ILLEGAL_NULL_VALUE			 182
#define ASN1_R_ILLEGAL_OBJECT				 183
#define ASN1_R_ILLEGAL_OPTIONAL_ANY			 126
#define ASN1_R_ILLEGAL_OPTIONS_ON_ITEM_TEMPLATE		 170
#define ASN1_R_ILLEGAL_TAGGED_ANY			 127
#define ASN1_R_ILLEGAL_TIME_VALUE			 184
#define ASN1_R_INTEGER_NOT_ASCII_FORMAT			 185
#define ASN1_R_INTEGER_TOO_LARGE_FOR_LONG		 128
#define ASN1_R_INVALID_BMPSTRING_LENGTH			 129
#define ASN1_R_INVALID_DIGIT				 130
#define ASN1_R_INVALID_MIME_TYPE			 205
#define ASN1_R_INVALID_MODIFIER				 186
#define ASN1_R_INVALID_NUMBER				 187
#define ASN1_R_INVALID_OBJECT_ENCODING			 216
#define ASN1_R_INVALID_SEPARATOR			 131
#define ASN1_R_INVALID_STRING_TABLE_VALUE		 218
#define ASN1_R_INVALID_TIME_FORMAT			 132
#define ASN1_R_INVALID_UNIVERSALSTRING_LENGTH		 133
#define ASN1_R_INVALID_UTF8STRING			 134
#define ASN1_R_INVALID_VALUE				 219
#define ASN1_R_IV_TOO_LARGE				 135
#define ASN1_R_LENGTH_ERROR				 136
#define ASN1_R_LIST_ERROR				 188
#define ASN1_R_MIME_NO_CONTENT_TYPE			 206
#define ASN1_R_MIME_PARSE_ERROR				 207
#define ASN1_R_MIME_SIG_PARSE_ERROR			 208
#define ASN1_R_MISSING_EOC				 137
#define ASN1_R_MISSING_SECOND_NUMBER			 138
#define ASN1_R_MISSING_VALUE				 189
#define ASN1_R_MSTRING_NOT_UNIVERSAL			 139
#define ASN1_R_MSTRING_WRONG_TAG			 140
#define ASN1_R_NESTED_ASN1_STRING			 197
#define ASN1_R_NON_HEX_CHARACTERS			 141
#define ASN1_R_NOT_ASCII_FORMAT				 190
#define ASN1_R_NOT_ENOUGH_DATA				 142
#define ASN1_R_NO_CONTENT_TYPE				 209
#define ASN1_R_NO_DEFAULT_DIGEST			 201
#define ASN1_R_NO_MATCHING_CHOICE_TYPE			 143
#define ASN1_R_NO_MULTIPART_BODY_FAILURE		 210
#define ASN1_R_NO_MULTIPART_BOUNDARY			 211
#define ASN1_R_NO_SIG_CONTENT_TYPE			 212
#define ASN1_R_NULL_IS_WRONG_LENGTH			 144
#define ASN1_R_OBJECT_NOT_ASCII_FORMAT			 191
#define ASN1_R_ODD_NUMBER_OF_CHARS			 145
#define ASN1_R_PRIVATE_KEY_HEADER_MISSING		 146
#define ASN1_R_SECOND_NUMBER_TOO_LARGE			 147
#define ASN1_R_SEQUENCE_LENGTH_MISMATCH			 148
#define ASN1_R_SEQUENCE_NOT_CONSTRUCTED			 149
#define ASN1_R_SEQUENCE_OR_SET_NEEDS_CONFIG		 192
#define ASN1_R_SHORT_LINE				 150
#define ASN1_R_SIG_INVALID_MIME_TYPE			 213
#define ASN1_R_STREAMING_NOT_SUPPORTED			 202
#define ASN1_R_STRING_TOO_LONG				 151
#define ASN1_R_STRING_TOO_SHORT				 152
#define ASN1_R_TAG_VALUE_TOO_HIGH			 153
#define ASN1_R_THE_ASN1_OBJECT_IDENTIFIER_IS_NOT_KNOWN_FOR_THIS_MD 154
#define ASN1_R_TIME_NOT_ASCII_FORMAT			 193
#define ASN1_R_TOO_LONG					 155
#define ASN1_R_TYPE_NOT_CONSTRUCTED			 156
#define ASN1_R_UNABLE_TO_DECODE_RSA_KEY			 157
#define ASN1_R_UNABLE_TO_DECODE_RSA_PRIVATE_KEY		 158
#define ASN1_R_UNEXPECTED_EOC				 159
#define ASN1_R_UNIVERSALSTRING_IS_WRONG_LENGTH		 215
#define ASN1_R_UNKNOWN_FORMAT				 160
#define ASN1_R_UNKNOWN_MESSAGE_DIGEST_ALGORITHM		 161
#define ASN1_R_UNKNOWN_OBJECT_TYPE			 162
#define ASN1_R_UNKNOWN_PUBLIC_KEY_TYPE			 163
#define ASN1_R_UNKNOWN_SIGNATURE_ALGORITHM		 199
#define ASN1_R_UNKNOWN_TAG				 194
#define ASN1_R_UNSUPPORTED_ANY_DEFINED_BY_TYPE		 164
#define ASN1_R_UNSUPPORTED_CIPHER			 165
#define ASN1_R_UNSUPPORTED_ENCRYPTION_ALGORITHM		 166
#define ASN1_R_UNSUPPORTED_PUBLIC_KEY_TYPE		 167
#define ASN1_R_UNSUPPORTED_TYPE				 196
#define ASN1_R_WRONG_PUBLIC_KEY_TYPE			 200
#define ASN1_R_WRONG_TAG				 168
#define ASN1_R_WRONG_TYPE				 169


typedef int (*equal_fn)(const unsigned char *pattern, size_t pattern_len,
			const unsigned char *subject, size_t subject_len,
			unsigned int flags);

/* Lookup table to convert tags to character widths,
 * 0 = UTF8 encoded, -1 is used for non string types
 * otherwise it is the number of bytes per character
 */

static const signed char tag2nbyte[] = {
	-1, -1, -1, -1, -1,	/* 0-4 */
	-1, -1, -1, -1, -1,	/* 5-9 */
	-1, -1, 0, -1,		/* 10-13 */
	-1, -1, -1, -1,		/* 15-17 */
	-1, 1, 1,		/* 18-20 */
	-1, 1, 1, 1,		/* 21-24 */
	-1, 1, -1,		/* 25-27 */
	4, -1, 2		/* 28-30 */
};
ASN1_STRING *ASN1_STRING_type_new(int type)
	{
	ASN1_STRING *ret;

	ret=(ASN1_STRING *)OPENSSL_malloc(sizeof(ASN1_STRING));
	if (ret == NULL)
		{
		ASN1err(ASN1_F_ASN1_STRING_TYPE_NEW,ERR_R_MALLOC_FAILURE);
		return(NULL);
		}
	ret->length=0;
	ret->type=type;
	ret->data=NULL;
	ret->flags=0;
	return(ret);
	}

static int equal_nocase(const unsigned char *pattern, size_t pattern_len,
			const unsigned char *subject, size_t subject_len,
			unsigned int flags)
	{
	skip_prefix(&pattern, &pattern_len, subject, subject_len, flags);
	if (pattern_len != subject_len)
		return 0;
	while (pattern_len)
		{
		unsigned char l = *pattern;
		unsigned char r = *subject;
		/* The pattern must not contain NUL characters. */
		if (l == 0)
			return 0;
		if (l != r)
			{
			if ('A' <= l && l <= 'Z')
				l = (l - 'A') + 'a';
			if ('A' <= r && r <= 'Z')
				r = (r - 'A') + 'a';
			if (l != r)
				return 0;
			}
		++pattern;
		++subject;
		--pattern_len;
		}
	return 1;
	}

int ASN1_mbstring_copy(ASN1_STRING **out, const unsigned char *in, int len,
					int inform, unsigned long mask)
{
	return ASN1_mbstring_ncopy(out, in, len, inform, mask, 0, 0);
}

int ASN1_mbstring_ncopy(ASN1_STRING **out, const unsigned char *in, int len,
					int inform, unsigned long mask, 
					long minsize, long maxsize)
{
	int str_type;
	int ret;
	char free_out;
	int outform, outlen = 0;
	ASN1_STRING *dest;
	unsigned char *p;
	int nchar;
	char strbuf[32];
	int (*cpyfunc)(unsigned long,void *) = NULL;
	if(len == -1) len = strlen((const char *)in);
	if(!mask) mask = DIRSTRING_TYPE;

	/* First do a string check and work out the number of characters */
	switch(inform) {

		case MBSTRING_BMP:
		if(len & 1) {
			ASN1err(ASN1_F_ASN1_MBSTRING_NCOPY,
					 ASN1_R_INVALID_BMPSTRING_LENGTH);
			return -1;
		}
		nchar = len >> 1;
		break;

		case MBSTRING_UNIV:
		if(len & 3) {
			ASN1err(ASN1_F_ASN1_MBSTRING_NCOPY,
					 ASN1_R_INVALID_UNIVERSALSTRING_LENGTH);
			return -1;
		}
		nchar = len >> 2;
		break;

		case MBSTRING_UTF8:
		nchar = 0;
		/* This counts the characters and does utf8 syntax checking */
		break;

		case MBSTRING_ASC:
		nchar = len;
		break;

		default:
		ASN1err(ASN1_F_ASN1_MBSTRING_NCOPY, ASN1_R_UNKNOWN_FORMAT);
		return -1;
	}

	if((minsize > 0) && (nchar < minsize)) {
		ASN1err(ASN1_F_ASN1_MBSTRING_NCOPY, ASN1_R_STRING_TOO_SHORT);
		BIO_snprintf(strbuf, sizeof strbuf, "%ld", minsize);
		ERR_add_error_data(2, "minsize=", strbuf);
		return -1;
	}

	if((maxsize > 0) && (nchar > maxsize)) {
		ASN1err(ASN1_F_ASN1_MBSTRING_NCOPY, ASN1_R_STRING_TOO_LONG);
		BIO_snprintf(strbuf, sizeof strbuf, "%ld", maxsize);
		ERR_add_error_data(2, "maxsize=", strbuf);
		return -1;
	}


	/* Now work out output format and string type */
	outform = MBSTRING_ASC;
	if(mask & B_ASN1_PRINTABLESTRING) str_type = V_ASN1_PRINTABLESTRING;
	else if(mask & B_ASN1_IA5STRING) str_type = V_ASN1_IA5STRING;
	else if(mask & B_ASN1_T61STRING) str_type = V_ASN1_T61STRING;
	else if(mask & B_ASN1_BMPSTRING) {
		str_type = V_ASN1_BMPSTRING;
		outform = MBSTRING_BMP;
	} else if(mask & B_ASN1_UNIVERSALSTRING) {
		str_type = V_ASN1_UNIVERSALSTRING;
		outform = MBSTRING_UNIV;
	} else {
		str_type = V_ASN1_UTF8STRING;
		outform = MBSTRING_UTF8;
	}
	if(!out) return str_type;
	if(*out) {
		free_out = 0;
		dest = *out;
		if(dest->data) {
			dest->length = 0;
			OPENSSL_free(dest->data);
			dest->data = NULL;
		}
		dest->type = str_type;
	} else {
		free_out = 1;
		dest = ASN1_STRING_type_new(str_type);
		if(!dest) {
			ASN1err(ASN1_F_ASN1_MBSTRING_NCOPY,
							ERR_R_MALLOC_FAILURE);
			return -1;
		}
		*out = dest;
	}
	/* If both the same type just copy across */
	if(inform == outform) {
		if(!ASN1_STRING_set(dest, in, len)) {
			ASN1err(ASN1_F_ASN1_MBSTRING_NCOPY,ERR_R_MALLOC_FAILURE);
			return -1;
		}
		return str_type;
	} 

	if(!(p = OPENSSL_malloc(outlen + 1))) {				/* allocation site */
		if(free_out) ASN1_STRING_free(dest);
		ASN1err(ASN1_F_ASN1_MBSTRING_NCOPY,ERR_R_MALLOC_FAILURE);
		return -1;
	}
	dest->length = outlen;
	dest->data = p;
	p[outlen] = 0;
	traverse_string(in, len, inform, cpyfunc, &p);
	return str_type;	
}

/* Utility function: convert any string type to UTF8, returns number of bytes
 * in output string or a negative error code
 */

int ASN1_STRING_to_UTF8(unsigned char **out, ASN1_STRING *in)
{
	ASN1_STRING stmp, *str = &stmp;
	int mbflag, type, ret;
	if(!in) return -1;
	type = in->type;
	if((type < 0) || (type > 30)) return -1;
	mbflag = tag2nbyte[type];
	if(mbflag == -1) return -1;
	mbflag |= MBSTRING_FLAG;
	stmp.data = NULL;
	stmp.length = 0;
	ret = ASN1_mbstring_copy(&str, in->data, in->length, mbflag, B_ASN1_UTF8STRING);
	if(ret < 0) return ret;
	*out = stmp.data;
	return stmp.length;
}
/* Compare an ASN1_STRING to a supplied string. If they match
 * return 1. If cmp_type > 0 only compare if string matches the
 * type, otherwise convert it to UTF8.
 */

static int do_check_string(ASN1_STRING *a, int cmp_type, equal_fn equal,
				unsigned int flags, const char *b, size_t blen,
				char **peername)
	{
	int rv = 0;

	if (!a->data || !a->length)
		return 0;
	if (cmp_type > 0)
		{
		if (cmp_type != a->type)
			return 0;
		if (cmp_type == V_ASN1_IA5STRING)
			rv = equal(a->data, a->length,
				   (unsigned char *)b, blen, flags);
		else if (a->length == (int)blen && !memcmp(a->data, b, blen))
			rv = 1;
		if (rv > 0 && peername)
			*peername = BUF_strndup((char *)a->data, a->length);
		}
	else
		{
		int astrlen;
		unsigned char *astr;
		astrlen = ASN1_STRING_to_UTF8(&astr, a);
		if (astrlen < 0)
			return -1;
		rv = equal(astr, astrlen, (unsigned char *)b, blen, flags);
		OPENSSL_free(astr);							
		if (rv > 0 && peername)
			*peername = BUF_strndup((char *)astr, astrlen);	/* use-after-free */
		}
	return rv;
	}

int main ()
{
	ASN1_STRING a;
	int cmp_type;
	unsigned int flags;
	const char *b = "bbbb";
	char *peername = b;
	size_t blen = strlen(b);
	do_check_string(&a, cmp_type, equal_nocase, flags, b, blen, &peername);
	return 0;
}

