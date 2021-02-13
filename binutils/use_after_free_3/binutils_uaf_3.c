/*
    buggy parent : 09ec4d3 
    commit id : 54064fdb792313355c92d9880680fad825d71ebd
*/

#include "../include/common.h"
#include "../include/binutils.h"

typedef unsigned int CGEN_INSN_INT;
typedef int64_t CGEN_INSN_LGSINT; /* large/long SINT */
typedef uint64_t CGEN_INSN_LGUINT; /* large/long UINT */
typedef unsigned char *CGEN_INSN_BYTES;
typedef unsigned char *CGEN_INSN_BYTES_PTR;

#define X(b) (((unsigned char *) (b))[0] & 0xf0)
#define CGEN_DIS_HASH(buffer, value) \
(X (buffer) | \
 (X (buffer) == 0x40 || X (buffer) == 0xe0 || X (buffer) == 0x60 || X (buffer) == 0x50 ? 0 \
   : X (buffer) == 0x70 || X (buffer) == 0xf0 ? (((unsigned char *) (buffer))[0] & 0xf) \
     : X (buffer) == 0x30 ? ((((unsigned char *) (buffer))[1] & 0x70) >> 4) \
	   : ((((unsigned char *) (buffer))[1] & 0xf0) >> 4)))


enum cgen_endian
{
  CGEN_ENDIAN_UNKNOWN,
  CGEN_ENDIAN_LITTLE,
  CGEN_ENDIAN_BIG
};

/* Forward decl.  */
typedef struct cgen_insn CGEN_INSN;

/* Opaque pointer version for use by external world.  */
typedef struct cgen_cpu_desc *CGEN_CPU_DESC;

/* Type of attribute values.  */
typedef struct cgen_bitset
{
  unsigned length;
  char *bits;
} CGEN_BITSET;

typedef CGEN_BITSET     CGEN_ATTR_VALUE_BITSET_TYPE;
typedef int             CGEN_ATTR_VALUE_ENUM_TYPE;
typedef union
{
  CGEN_ATTR_VALUE_BITSET_TYPE bitset;
  CGEN_ATTR_VALUE_ENUM_TYPE   nonbitset;
} CGEN_ATTR_VALUE_TYPE;

/* Struct to record attribute information.  */
typedef struct
{
  /* Boolean attributes.  */
  unsigned int bool_;
  /* Non-boolean integer attributes.  */
  CGEN_ATTR_VALUE_TYPE nonbool[1];
} CGEN_ATTR;

typedef struct cgen_insn_list
{
  struct cgen_insn_list *next;
  const CGEN_INSN *insn;
} CGEN_INSN_LIST;


typedef struct cgen_cpu_desc
{
  /* Copy of CGEN_INT_INSN_P.  */
  int int_insn_p;
  
  /* Disassembler instruction hash table.  */
  CGEN_INSN_LIST **dis_hash_table;

} CGEN_CPU_TABLE;

void
bfd_put_bits (int64_t data, void *p, int bits, bool big_p)
{
  bfd_byte *addr = (bfd_byte *) p;
  int i;
  int bytes;

  if (bits % 8 != 0)
    abort ();

  bytes = bits / 8;
  for (i = 0; i < bytes; i++)
    {
      int addr_index = big_p ? bytes - i - 1 : i;

      addr[addr_index] = data & 0xff;
      data >>= 8;
    }
}

/* Cover function to read and properly byteswap an insn value.  */
CGEN_INSN_INT
cgen_get_insn_value (CGEN_CPU_DESC cd, unsigned char *buf, int length)
{
  int big_p;
  int insn_chunk_bitsize;
  CGEN_INSN_INT value = 0;

  if (insn_chunk_bitsize != 0 && insn_chunk_bitsize < length)
    {
      int i;
      for (i = 0; i < length; i += insn_chunk_bitsize) /* NB: i == bits */
	{
	  int bit_index;
	  bfd_vma this_value;

	  bit_index = i; /* NB: not dependent on endianness; opposite of cgen_put_insn_value! */
	  this_value = bfd_get_bits (& buf[bit_index / 8], insn_chunk_bitsize, big_p);
	  value = (value << insn_chunk_bitsize) | this_value;
	}
    }
  else
    {
      value = bfd_get_bits (buf, length, big_p);
    }

  return value;
}

/* Cover function to store an insn value properly byteswapped.  */

void
cgen_put_insn_value (CGEN_CPU_DESC cd,
		     unsigned char *buf,
		     int length,
		     CGEN_INSN_INT value)
{
  int big_p;
  int insn_chunk_bitsize; 

  if (insn_chunk_bitsize != 0 && insn_chunk_bitsize < length)
    {
	  int i;
      for (i = 0; i < length; i += insn_chunk_bitsize) /* NB: i == bits */
	{
	  int bit_index;

	  bit_index = (length - insn_chunk_bitsize - i); /* NB: not dependent on endianness! */
	  bfd_put_bits ((bfd_vma) value, & buf[bit_index / 8], insn_chunk_bitsize, big_p);
	  value >>= insn_chunk_bitsize;
	}
    }
  else
    {
      bfd_put_bits ((bfd_vma) value, buf, length, big_p);
    }
}

CGEN_INSN_LIST *
cgen_dis_lookup_insn (CGEN_CPU_DESC cd, const char * buf, CGEN_INSN_INT value)
{
	unsigned int hash;
  
    if (cd->dis_hash_table == NULL)
    build_dis_hash_table (cd);

	hash = CGEN_DIS_HASH(buf, value);
  	return cd->dis_hash_table[hash];
}

const CGEN_INSN *
cgen_lookup_insn (CGEN_CPU_DESC cd,
		  const CGEN_INSN *insn,
		  CGEN_INSN_INT insn_int_value,
		  unsigned char *insn_bytes_value)
{
  int length;
  unsigned char *buf;
  CGEN_INSN_INT base_insn;

  if (cd->int_insn_p)
    {
      buf = (unsigned char *) xmalloc (4);	/* allocation site */
      cgen_put_insn_value (cd, buf, length, insn_int_value);
      base_insn = insn_int_value;
      free (buf);							/* freed */
    }
  else
    {
      buf = insn_bytes_value;
      base_insn = cgen_get_insn_value (cd, buf, length);
    }

  if (!insn)
    {
      const CGEN_INSN_LIST *insn_list;

      insn_list = cgen_dis_lookup_insn (cd, (char *) buf, base_insn);	/* use-after-free */
      while (insn_list != NULL)
		{
	      insn = insn_list->insn;
	  	  insn_list = insn_list->next;
		}
    }

  return NULL;
}

int main()
{
    CGEN_CPU_DESC cd = malloc(sizeof(*cd));
    unsigned char *insn_bytes_value = strdup("insn_value");
	cd->int_insn_p = __RANDBOOL;
	cgen_lookup_insn(cd, NULL, 0, insn_bytes_value);
    free(insn_bytes_value);
	free(cd);
	return 0;
}


