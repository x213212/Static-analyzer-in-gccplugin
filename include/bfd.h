#define bfd_get_32(abfd, ptr) \
	ptr[0]; __USE(abfd)
#define bfd_get_64(abfd, ptr) \
	ptr[0]; __USE(abfd)
#define bfd_put_32(abfd, val, ptr) \
	ptr[0] + val; __USE(abfd); 
#define bfd_put_64(abfd, val, ptr) \
	ptr[0] + val; __USE(abfd); 
#define bfd_get_flavour(abfd) ((abfd->xvec->flavour))
#define bfd_get_section(x) ((x)->section)
#define bfd_get_section_size(ptr) ((ptr)->size)



#define EI_NIDENT	16		/* Size of e_ident[] */

typedef int bool;

typedef FILE *file_ptr;

enum bfd_flavour
{
  /* N.B. Update bfd_flavour_name if you change this.  */
  bfd_target_unknown_flavour,
  bfd_target_aout_flavour,
  bfd_target_coff_flavour,
  bfd_target_ecoff_flavour,
  bfd_target_xcoff_flavour,
  bfd_target_elf_flavour,
  bfd_target_ieee_flavour,
  bfd_target_nlm_flavour,
  bfd_target_oasys_flavour,
  bfd_target_tekhex_flavour,
  bfd_target_srec_flavour,
  bfd_target_verilog_flavour,
  bfd_target_ihex_flavour,
  bfd_target_som_flavour,
  bfd_target_os9k_flavour,
  bfd_target_versados_flavour,
  bfd_target_msdos_flavour,
  bfd_target_ovax_flavour,
  bfd_target_evax_flavour,
  bfd_target_mmo_flavour,
  bfd_target_mach_o_flavour,
  bfd_target_pef_flavour,
  bfd_target_pef_xlib_flavour,
  bfd_target_sym_flavour
};

typedef struct bfd_target
{
	char *name;

	enum bfd_flavour flavour;
};

struct bfd {
	char *filename;
	int direction;
	struct bfd *archive_next;	

	const struct bfd_target *xvec;


	void *memory;	
	bool cacheable;
};

typedef struct bfd bfd;

typedef struct bfd_section *sec_ptr;

typedef struct bfd_section
{
  /* The name of the section; the name isn't a copy, the pointer is
     the same as that passed to bfd_make_section.  */
  const char *name;

  /* A unique sequence number.  */
  unsigned int id;

  /* Which section in the bfd; 0..n-1 as sections are created in a bfd.  */
  unsigned int index;

  /* The next section in the list belonging to the BFD, or NULL.  */
  struct bfd_section *next;

  /* The previous section in the list belonging to the BFD, or NULL.  */
  struct bfd_section *prev;

  /* The field flags contains attributes of the section. Some
     flags are read in from the object file, and some are
     synthesized from other information.  */
  flagword flags;

#define SEC_NO_FLAGS   0x000

  /* Tells the OS to allocate space for this section when loading.
     This is clear for a section containing debug information only.  */
#define SEC_ALLOC      0x001

  /* Tells the OS to load the section from the file when loading.
     This is clear for a .bss section.  */
#define SEC_LOAD       0x002

  /* The section contains data still to be relocated, so there is
     some relocation information too.  */
#define SEC_RELOC      0x004

  /* A signal to the OS that the section contains read only data.  */
#define SEC_READONLY   0x008

  /* The section contains code only.  */
#define SEC_CODE       0x010

  /* The section contains data only.  */
#define SEC_DATA       0x020

  /* The section will reside in ROM.  */
#define SEC_ROM        0x040

  /* The section contains constructor information. This section
     type is used by the linker to create lists of constructors and
     destructors used by <<g++>>. When a back end sees a symbol
     which should be used in a constructor list, it creates a new
     section for the type of name (e.g., <<__CTOR_LIST__>>), attaches
     the symbol to it, and builds a relocation. To build the lists
     of constructors, all the linker has to do is catenate all the
     sections called <<__CTOR_LIST__>> and relocate the data
     contained within - exactly the operations it would peform on
     standard data.  */
#define SEC_CONSTRUCTOR 0x080

  /* The section has contents - a data section could be
     <<SEC_ALLOC>> | <<SEC_HAS_CONTENTS>>; a debug section could be
     <<SEC_HAS_CONTENTS>>  */
#define SEC_HAS_CONTENTS 0x100

  /* An instruction to the linker to not output the section
     even if it has information which would normally be written.  */
#define SEC_NEVER_LOAD 0x200

  /* The section contains thread local data.  */
#define SEC_THREAD_LOCAL 0x400

  /* The section has GOT references.  This flag is only for the
     linker, and is currently only used by the elf32-hppa back end.
     It will be set if global offset table references were detected
     in this section, which indicate to the linker that the section
     contains PIC code, and must be handled specially when doing a
     static link.  */
#define SEC_HAS_GOT_REF 0x800

  /* The section contains common symbols (symbols may be defined
     multiple times, the value of a symbol is the amount of
     space it requires, and the largest symbol value is the one
     used).  Most targets have exactly one of these (which we
     translate to bfd_com_section_ptr), but ECOFF has two.  */
#define SEC_IS_COMMON 0x1000

  /* The section contains only debugging information.  For
     example, this is set for ELF .debug and .stab sections.
     strip tests this flag to see if a section can be
     discarded.  */
#define SEC_DEBUGGING 0x2000

  /* The contents of this section are held in memory pointed to
     by the contents field.  This is checked by bfd_get_section_contents,
     and the data is retrieved from memory if appropriate.  */
#define SEC_IN_MEMORY 0x4000

  /* The contents of this section are to be excluded by the
     linker for executable and shared objects unless those
     objects are to be further relocated.  */
#define SEC_EXCLUDE 0x8000

  /* The contents of this section are to be sorted based on the sum of
     the symbol and addend values specified by the associated relocation
     entries.  Entries without associated relocation entries will be
     appended to the end of the section in an unspecified order.  */
#define SEC_SORT_ENTRIES 0x10000

  /* When linking, duplicate sections of the same name should be
     discarded, rather than being combined into a single section as
     is usually done.  This is similar to how common symbols are
     handled.  See SEC_LINK_DUPLICATES below.  */
#define SEC_LINK_ONCE 0x20000

  /* If SEC_LINK_ONCE is set, this bitfield describes how the linker
     should handle duplicate sections.  */
#define SEC_LINK_DUPLICATES 0xc0000

  /* This value for SEC_LINK_DUPLICATES means that duplicate
     sections with the same name should simply be discarded.  */
#define SEC_LINK_DUPLICATES_DISCARD 0x0

  /* This value for SEC_LINK_DUPLICATES means that the linker
     should warn if there are any duplicate sections, although
     it should still only link one copy.  */
#define SEC_LINK_DUPLICATES_ONE_ONLY 0x40000

  /* This value for SEC_LINK_DUPLICATES means that the linker
     should warn if any duplicate sections are a different size.  */
#define SEC_LINK_DUPLICATES_SAME_SIZE 0x80000

  /* This value for SEC_LINK_DUPLICATES means that the linker
     should warn if any duplicate sections contain different
     contents.  */
#define SEC_LINK_DUPLICATES_SAME_CONTENTS \
  (SEC_LINK_DUPLICATES_ONE_ONLY | SEC_LINK_DUPLICATES_SAME_SIZE)

  /* This section was created by the linker as part of dynamic
     relocation or other arcane processing.  It is skipped when
     going through the first-pass output, trusting that someone
     else up the line will take care of it later.  */
#define SEC_LINKER_CREATED 0x100000

  /* This section should not be subject to garbage collection.
     Also set to inform the linker that this section should not be
     listed in the link map as discarded.  */
#define SEC_KEEP 0x200000

  /* This section contains "short" data, and should be placed
     "near" the GP.  */
#define SEC_SMALL_DATA 0x400000

  /* Attempt to merge identical entities in the section.
     Entity size is given in the entsize field.  */
#define SEC_MERGE 0x800000

  /* If given with SEC_MERGE, entities to merge are zero terminated
     strings where entsize specifies character size instead of fixed
     size entries.  */
#define SEC_STRINGS 0x1000000

  /* This section contains data about section groups.  */
#define SEC_GROUP 0x2000000

  /* The section is a COFF shared library section.  This flag is
     only for the linker.  If this type of section appears in
     the input file, the linker must copy it to the output file
     without changing the vma or size.  FIXME: Although this
     was originally intended to be general, it really is COFF
     specific (and the flag was renamed to indicate this).  It
     might be cleaner to have some more general mechanism to
     allow the back end to control what the linker does with
     sections.  */
#define SEC_COFF_SHARED_LIBRARY 0x4000000

  /* This input section should be copied to output in reverse order
     as an array of pointers.  This is for ELF linker internal use
     only.  */
#define SEC_ELF_REVERSE_COPY 0x4000000

  /* This section contains data which may be shared with other
     executables or shared objects. This is for COFF only.  */
#define SEC_COFF_SHARED 0x8000000

  /* This section should be compressed.  This is for ELF linker
     internal use only.  */
#define SEC_ELF_COMPRESS 0x8000000

  /* When a section with this flag is being linked, then if the size of
     the input section is less than a page, it should not cross a page
     boundary.  If the size of the input section is one page or more,
     it should be aligned on a page boundary.  This is for TI
     TMS320C54X only.  */
#define SEC_TIC54X_BLOCK 0x10000000

  /* This section should be renamed.  This is for ELF linker
     internal use only.  */
#define SEC_ELF_RENAME 0x10000000

  /* Conditionally link this section; do not link if there are no
     references found to any symbol in the section.  This is for TI
     TMS320C54X only.  */
#define SEC_TIC54X_CLINK 0x20000000

  /* This section contains vliw code.  This is for Toshiba MeP only.  */
#define SEC_MEP_VLIW 0x20000000

  /* Indicate that section has the no read flag set. This happens
     when memory read flag isn't set. */
#define SEC_COFF_NOREAD 0x40000000

  /* Indicate that section has the purecode flag set.  */
#define SEC_ELF_PURECODE 0x80000000

  /*  End of section flags.  */

  /* Some internal packed boolean fields.  */

  /* See the vma field.  */
  unsigned int user_set_vma : 1;

  /* A mark flag used by some of the linker backends.  */
  unsigned int linker_mark : 1;

  /* Another mark flag used by some of the linker backends.  Set for
     output sections that have an input section.  */
  unsigned int linker_has_input : 1;

  /* Mark flag used by some linker backends for garbage collection.  */
  unsigned int gc_mark : 1;

  /* Section compression status.  */
  unsigned int compress_status : 2;
#define COMPRESS_SECTION_NONE    0
#define COMPRESS_SECTION_DONE    1
#define DECOMPRESS_SECTION_SIZED 2

  /* The following flags are used by the ELF linker. */

  /* Mark sections which have been allocated to segments.  */
  unsigned int segment_mark : 1;

  /* Type of sec_info information.  */
  unsigned int sec_info_type:3;
#define SEC_INFO_TYPE_NONE      0
#define SEC_INFO_TYPE_STABS     1
#define SEC_INFO_TYPE_MERGE     2
#define SEC_INFO_TYPE_EH_FRAME  3
#define SEC_INFO_TYPE_JUST_SYMS 4
#define SEC_INFO_TYPE_TARGET    5
#define SEC_INFO_TYPE_EH_FRAME_ENTRY 6

  /* Nonzero if this section uses RELA relocations, rather than REL.  */
  unsigned int use_rela_p:1;

  /* Bits used by various backends.  The generic code doesn't touch
     these fields.  */

  unsigned int sec_flg0:1;
  unsigned int sec_flg1:1;
  unsigned int sec_flg2:1;
  unsigned int sec_flg3:1;
  unsigned int sec_flg4:1;
  unsigned int sec_flg5:1;

  /* End of internal packed boolean fields.  */

  /*  The virtual memory address of the section - where it will be
      at run time.  The symbols are relocated against this.  The
      user_set_vma flag is maintained by bfd; if it's not set, the
      backend can assign addresses (for example, in <<a.out>>, where
      the default address for <<.data>> is dependent on the specific
      target and various flags).  */
  bfd_vma vma;

  /*  The load address of the section - where it would be in a
      rom image; really only used for writing section header
      information.  */
  bfd_vma lma;

  /* The size of the section in *octets*, as it will be output.
     Contains a value even if the section has no contents (e.g., the
     size of <<.bss>>).  */
  bfd_size_type size;

  /* For input sections, the original size on disk of the section, in
     octets.  This field should be set for any section whose size is
     changed by linker relaxation.  It is required for sections where
     the linker relaxation scheme doesn't cache altered section and
     reloc contents (stabs, eh_frame, SEC_MERGE, some coff relaxing
     targets), and thus the original size needs to be kept to read the
     section multiple times.  For output sections, rawsize holds the
     section size calculated on a previous linker relaxation pass.  */
  bfd_size_type rawsize;

  /* The compressed size of the section in octets.  */
  bfd_size_type compressed_size;

  /* Relaxation table. */
  struct relax_table *relax;

  /* Count of used relaxation table entries. */
  int relax_count;


  /* If this section is going to be output, then this value is the
     offset in *bytes* into the output section of the first byte in the
     input section (byte ==> smallest addressable unit on the
     target).  In most cases, if this was going to start at the
     100th octet (8-bit quantity) in the output section, this value
     would be 100.  However, if the target byte size is 16 bits
     (bfd_octets_per_byte is "2"), this value would be 50.  */
  bfd_vma output_offset;

  /* The output section through which to map on output.  */
  struct bfd_section *output_section;

  /* The alignment requirement of the section, as an exponent of 2 -
     e.g., 3 aligns to 2^3 (or 8).  */
  unsigned int alignment_power;

  /* If an input section, a pointer to a vector of relocation
     records for the data in this section.  */
  struct reloc_cache_entry *relocation;

  /* If an output section, a pointer to a vector of pointers to
     relocation records for the data in this section.  */
  struct reloc_cache_entry **orelocation;

  /* The number of relocation records in one of the above.  */
  unsigned reloc_count;

  /* Information below is back end specific - and not always used
     or updated.  */

  /* File position of section data.  */
  file_ptr filepos;

  /* File position of relocation info.  */
  file_ptr rel_filepos;

  /* File position of line data.  */
  file_ptr line_filepos;

  /* Pointer to data for applications.  */
  void *userdata;

  /* If the SEC_IN_MEMORY flag is set, this points to the actual
     contents.  */
  unsigned char *contents;

  /* Number of line number records.  */
  unsigned int lineno_count;

  /* Entity size for merging purposes.  */
  unsigned int entsize;

  /* Points to the kept section if this section is a link-once section,
     and is discarded.  */
  struct bfd_section *kept_section;

  /* When a section is being output, this value changes as more
     linenumbers are written out.  */
  file_ptr moving_line_filepos;

  /* What the section number is in the target world.  */
  int target_index;

  void *used_by_bfd;

  /* If this is a constructor section then here is a list of the
     relocations created to relocate items within it.  */
  struct relent_chain *constructor_chain;

  /* The BFD which owns the section.  */
  bfd *owner;

  /* A symbol which points at this section only.  */
  struct bfd_symbol *symbol;
  struct bfd_symbol **symbol_ptr_ptr;

  /* Early in the link process, map_head and map_tail are used to build
     a list of input sections attached to an output section.  Later,
     output sections use these fields for a list of bfd_link_order
     structs.  */
  union {
    struct bfd_link_order *link_order;
    struct bfd_section *s;
  } map_head, map_tail;
} asection;
typedef struct elf_internal_ehdr {
  unsigned char		e_ident[EI_NIDENT]; /* ELF "magic number" */
  bfd_vma		e_entry;	/* Entry point virtual address */
  bfd_size_type		e_phoff;	/* Program header table file offset */
  bfd_size_type		e_shoff;	/* Section header table file offset */
  unsigned long		e_version;	/* Identifies object file version */
  unsigned long		e_flags;	/* Processor-specific flags */
  unsigned short	e_type;		/* Identifies object file type */
  unsigned short	e_machine;	/* Specifies required architecture */
  unsigned int		e_ehsize;	/* ELF header size in bytes */
  unsigned int		e_phentsize;	/* Program header table entry size */
  unsigned int		e_phnum;	/* Program header table entry count */
  unsigned int		e_shentsize;	/* Section header table entry size */
  unsigned int		e_shnum;	/* Section header table entry count */
  unsigned int		e_shstrndx;	/* Section header string table index */
} Elf_Internal_Ehdr;

/* Program header */

struct elf_internal_phdr {
  unsigned long	p_type;			/* Identifies program segment type */
  unsigned long	p_flags;		/* Segment flags */
  bfd_vma	p_offset;		/* Segment file offset */
  bfd_vma	p_vaddr;		/* Segment virtual address */
  bfd_vma	p_paddr;		/* Segment physical address */
  bfd_vma	p_filesz;		/* Segment size in file */
  bfd_vma	p_memsz;		/* Segment size in memory */
  bfd_vma	p_align;		/* Segment alignment, file & memory */
};

typedef struct elf_internal_phdr Elf_Internal_Phdr;
/* Section header */
typedef struct elf_internal_shdr {
  unsigned int	sh_name;		/* Section name, index in string tbl */
  unsigned int	sh_type;		/* Type of section */
  bfd_vma	sh_flags;		/* Miscellaneous section attributes */
  bfd_vma	sh_addr;		/* Section virtual addr at execution */
  file_ptr	sh_offset;		/* Section file offset */
  bfd_size_type	sh_size;		/* Size of section in bytes */
  unsigned int	sh_link;		/* Index of another section */
  unsigned int	sh_info;		/* Additional section information */
  bfd_vma	sh_addralign;		/* Section alignment */
  bfd_size_type	sh_entsize;		/* Entry size if section holds table */

  /* The internal rep also has some cached info associated with it. */
  asection *	bfd_section;		/* Associated BFD section.  */
  unsigned char *contents;		/* Section contents.  */
} Elf_Internal_Shdr;

/* Compression header */

typedef struct elf_internal_chdr {
  unsigned int	ch_type;		/* Type of compression */
  bfd_size_type	ch_size;		/* Size of uncompressed data in bytes */
  bfd_vma	ch_addralign;		/* Alignment of uncompressed data */
} Elf_Internal_Chdr;

/* Symbol table entry */

struct elf_internal_sym {
  bfd_vma	st_value;		/* Value of the symbol */
  bfd_vma	st_size;		/* Associated symbol size */
  unsigned long	st_name;		/* Symbol name, index in string tbl */
  unsigned char	st_info;		/* Type and binding attributes */
  unsigned char	st_other;		/* Visibilty, and target specific */
  unsigned char st_target_internal;	/* Internal-only information */
  unsigned int  st_shndx;		/* Associated section index */
};

typedef struct elf_internal_sym Elf_Internal_Sym;

typedef struct {
  unsigned char	ch_type[4];		/* Type of compression */
  unsigned char	ch_size[4];		/* Size of uncompressed data in bytes */
  unsigned char	ch_addralign[4];	/* Alignment of uncompressed data  */
} Elf32_External_Chdr;

typedef struct {
  unsigned char	ch_type[4];		/* Type of compression */
  unsigned char	ch_reserved[4];		/* Padding */
  unsigned char	ch_size[8];		/* Size of uncompressed data in bytes */
  unsigned char	ch_addralign[8];	/* Alignment of uncompressed data  */
} Elf64_External_Chdr;

/* Symbol table entry */

typedef struct {
  unsigned char	st_name[4];		/* Symbol name, index in string tbl */
  unsigned char	st_value[4];		/* Value of the symbol */
  unsigned char	st_size[4];		/* Associated symbol size */
  unsigned char	st_info[1];		/* Type and binding attributes */
  unsigned char	st_other[1];		/* No defined meaning, 0 */
  unsigned char	st_shndx[2];		/* Associated section index */
} Elf32_External_Sym;

typedef struct {
  unsigned char	st_name[4];		/* Symbol name, index in string tbl */
  unsigned char	st_info[1];		/* Type and binding attributes */
  unsigned char	st_other[1];		/* No defined meaning, 0 */
  unsigned char	st_shndx[2];		/* Associated section index */
  unsigned char	st_value[8];		/* Value of the symbol */
  unsigned char	st_size[8];		/* Associated symbol size */
} Elf64_External_Sym;

typedef struct {
  unsigned char est_shndx[4];		/* Section index */
} Elf_External_Sym_Shndx;

/* Note segments */

typedef struct {
  unsigned char	namesz[4];		/* Size of entry's owner string */
  unsigned char	descsz[4];		/* Size of the note descriptor */
  unsigned char	type[4];		/* Interpretation of the descriptor */
  char		name[1];		/* Start of the name+desc data */
} Elf_External_Note;

/* Relocation Entries */
typedef struct {
  unsigned char r_offset[4];	/* Location at which to apply the action */
  unsigned char	r_info[4];	/* index and type of relocation */
} Elf32_External_Rel;

typedef struct {
  unsigned char r_offset[4];	/* Location at which to apply the action */
  unsigned char	r_info[4];	/* index and type of relocation */
  unsigned char	r_addend[4];	/* Constant addend used to compute value */
} Elf32_External_Rela;

typedef struct {
  unsigned char r_offset[8];	/* Location at which to apply the action */
  unsigned char	r_info[8];	/* index and type of relocation */
} Elf64_External_Rel;

typedef struct {
  unsigned char r_offset[8];	/* Location at which to apply the action */
  unsigned char	r_info[8];	/* index and type of relocation */
  unsigned char	r_addend[8];	/* Constant addend used to compute value */
} Elf64_External_Rela;

enum bfd_endian { BFD_ENDIAN_BIG, BFD_ENDIAN_LITTLE, BFD_ENDIAN_UNKNOWN };

struct relax_table {
  /* Address where bytes may be deleted. */
  bfd_vma addr;

  /* Number of bytes to be deleted.  */
  int size;
};

