/*
	buggy parent: e13cb30
	commit id : 7ed1acafa0b5d135342f9dcc0eb0387dff95005a
*/

#include "../include/common.h"
#include "../include/vec.h"

typedef int btrace_insn;

DEF_VEC_I(btrace_insn);

struct btrace_function {
	struct btrace_function *prev;
	struct btrace_function *next;

	VEC(btrace_insn)* insn;
};

struct btrace_thread_info {
	struct btrace_function *begin; 
	struct btrace_function *end; 
};

void btrace_clear(struct btrace_thread_info *btp)
{
	struct btrace_function *it, *trash;
	it = btp->begin;

	while (it != NULL) {
		trash = it;
		it = it->next;
		/* VFD_free (btrace_insn_s, trash->insn); */
		free (trash);
	}

	btp->begin = NULL;
	btp->end = NULL; /* memory leak */
}

int main()
{
	time_t t;

	struct btrace_thread_info btinfo;
	struct btrace_function *it, *next, *bfunc;
	VEC_btrace_insn *insn_vec_p;

	srand(time(&t));

	bfunc = xmalloc(sizeof(struct btrace_function));
	bfunc->insn = VEC_alloc (btrace_insn, 10); /* allocation site */
	bfunc->prev = bfunc->next = NULL;

	btinfo.begin = btinfo.end = bfunc;
	
	it = btinfo.begin;
	while (__RANDBOOL) {
		next = xmalloc(sizeof(struct btrace_function));
		insn_vec_p = VEC_alloc (btrace_insn, 10); /* allocation site */

		next->insn = insn_vec_p;

		it->next = next;
		next->prev = it;
		next->next = NULL;

		btinfo.end = next;
		it = it->next;
	}

	it = btinfo.begin;
	while (it != NULL) {
		insn_vec_p = it->insn;
		VEC_safe_push (btrace_insn, insn_vec_p, 55);
		it = it->next;
	}	

	btrace_clear (&btinfo);
}
