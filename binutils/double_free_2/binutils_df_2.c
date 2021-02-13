#include "../include/common.h"
#include "../include/binutils.h"
#include "../include/bfd.h"

/* buggy parent: cc917fd
	commit id: be74fad95edc8827516e144cf38d135b503249cd
*/

typedef int bool;
typedef int bfd_vma;
typedef bool bfd_boolean;
typedef int Elf_Internal_Rela;

struct link_info {
	bool keep_memory;
};

struct alpha_relax_info {
	struct bfd *abfd;
	struct link_info* link_info;
};

typedef struct bfd bfd;

Elf_Internal_Rela *
_bfd_elf_link_read_relocs (bfd *abfd,
				Elf_Internal_Rela *internal_relocs,
				bfd_boolean keep_memory)
{
	Elf_Internal_Rela *alloc1, *alloc2 = NULL;
	Elf_Internal_Rela *internal_rela_relocs;

	/* cached relocs may be returned */
	if (__RANDBOOL && abfd->memory) {
		printf("cached memory(abfd->memory) is returned\n");
		return abfd->memory;
	}

	if (internal_relocs == NULL)
		{
			internal_relocs = alloc2
				= (Elf_Internal_Rela *)malloc(sizeof(Elf_Internal_Rela)); /* allocation site */

			if (keep_memory)
				abfd->memory = alloc2;
			
			if (internal_relocs == NULL)
				goto error_return;
		}

	if (alloc1 == NULL)


	return internal_relocs;
	error_return:
		if (keep_memory) {
			abfd->memory = NULL;
		}
	return NULL;
}

static bfd_vma
elf64_alpha_relax_opt_call (struct alpha_relax_info *info,
		bfd_vma symval)
{
	Elf_Internal_Rela *tsec_relocs, *tsec_free;

	tsec_relocs = _bfd_elf_link_read_relocs(info->abfd,
			(Elf_Internal_Rela *)NULL,
			info->link_info->keep_memory);

	if (tsec_relocs == NULL)
		return 0;

	*((int *)tsec_relocs) = 1;

	tsec_free = (info->link_info->keep_memory ? NULL : tsec_relocs);
	
	if (tsec_free)
		free(tsec_free);	/* double-free */

	return 0;
}

static bfd_boolean
elf64_alpha_relax_with_lituse (struct alpha_relax_info *info, bfd_vma symval)
{
  bfd *abfd = info->abfd;
	Elf_Internal_Rela *internal_relocs = _bfd_elf_link_read_relocs(abfd,
			(Elf_Internal_Rela *)NULL, info->link_info->keep_memory);

	bfd_vma optdest = elf64_alpha_relax_opt_call(info, symval);
	free(internal_relocs);	/* double-free */
}

int main() {
	time_t t;
	srand((unsigned) time(&t));

	bfd_vma symval = 1;
	struct alpha_relax_info info;
	bfd bfd;
	struct link_info link_info;

	bfd.memory = malloc(sizeof(Elf_Internal_Rela));
	link_info.keep_memory = __RANDBOOL;

	info.abfd = &bfd;
	info.link_info = &link_info;

	printf ("keep_memory: %d\n", link_info.keep_memory);
	elf64_alpha_relax_with_lituse(&info, symval);

	*((int *)bfd.memory) = 1;	
	free(bfd.memory);
}

