#include "config.h"
#include "system.h"
#include <coretypes.h>
#include "insn-constants.h"

//#include "config/i386/i386.h"
#include "is-a.h"
#include "options.h"
#include "backend.h"
#include "rtl.h"
#include "tree-pass.h"
#include "tree.h"
#include "gimple.h"
#include "alloc-pool.h"
#include "ssa.h"
#include "cgraph.h"
#include "tree-pretty-print.h"
#include "diagnostic-core.h"
#include "fold-const.h"
#include "stor-layout.h"
#include "stmt.h"
#include "gimple-iterator.h"
#include "tree-into-ssa.h"
#include "tree-dfa.h"
#include "params.h"
#include "gimple-walk.h"
#include "varasm.h"
#include "options.h"
#include "gimple-pretty-print.h"
#include "print-tree.h"
#include <string.h>
#include "intl.h"
#include "dominance.h"
#include <et-forest.h>
#include <cgraph.h>
/*Algorithm 3*/
#include "DFS.h"
#include "cfgloop.h"
#include <vector>
// #include <omp.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fstream>
#define FOR_EACH_TABLE(TABLE, TAR)                           \
	for ((TAR) = ((TABLE) == NULL) ? NULL : (TABLE)->target; \
		 (TAR) != NULL;                                      \
		 (TABLE) = (TABLE)->next, (TAR) = ((TABLE) == NULL) ? NULL : (TABLE)->target)
#define FOR_EACH_USE_TABLE(USE, STMT) \
	for ((STMT) = ((USE) == NULL) ? NULL : (USE)->stmt; (USE) != NULL; (USE) = (USE)->next, (STMT) = ((USE) == NULL) ? NULL : (USE)->stmt)

/*allocation and deallocation table*/
struct ptb
{
	tree target;
	location_t loc;
	ptb *next;
	int state;
	bool may_realloc, is_phi_stmt;
	gimple *last_stmt, *dealloc_stmt;
	basic_block bb;
	cgraph_node *node;
	gimple *caller;
	function *fun;
	bool removed;
	bool inbranch;
	int size = 0;
};

ptb start1, start2, start3, start4, start5, start6, start7;
ptb *ptable, *retable, *ftable, *phitable, *return_table, *use_table, *fopen_table;

struct gimple_array
{
	gimple *stmt;
	tree aptr;
	tree target;
	gimple_array *next;
	int size = 0;
};

/*function decl tree and rhs tree*/
struct fdecl
{
	tree fndecl;
	tree fn;
};

/*function decl and location*/
struct var_points_to
{
	vector<vector<pair<fdecl, location_t>>> may_malloc_path;
	tree decl;
};

/*record each DFS graph*/
hash_map<cgraph_node *, Graph> *fDFS;

/*use function tree search cgraph_node*/
hash_map<tree, cgraph_node *> *fnode;

/*rhs function search function decl and location*/
hash_map<tree, var_points_to> *tvpt;
hash_map<tree, gimple_array> *tvpt2;

/*state*/
unsigned int POINTER_NOT_EXIST = 0;
unsigned int POINTER_STATE_IS_FREE = 1;
unsigned int POINTER_STATE_IS_NORMAL = 2;
unsigned int POINTER_STATE_IS_MALLOC = 3;
unsigned int POINTER_STATE_IS_FILE = 4;
unsigned int POINTER_STATE_MAY_IS_FREE = 5;

const unsigned int POINTER_NOT = 0;
const unsigned int POINTER_MAY = 1;
const unsigned int POINTER_MUST = 2;
/*dump file */
FILE *fp;

/*interprocedural analysis*/
bool ipa = true;

/*main function*/
function *main_fun;

bool bb_in_branch_p(ptb *table)
{
	return !dominated_by_p(CDI_DOMINATORS, table->fun->cfg->x_exit_block_ptr->prev_bb, table->bb);
}

bool bb_in_branch_p(gimple *stmt)
{
	//function* fn = DECL_STRUCT_FUNCTION(gimple_get_lhs(stmt));
	// fprintf(stderr, "backkkkkkkbackkkkkkkbackkkkkkkbackkkkkkkbackkkkkkkbackkkkkkk\n");
	// debug_gimple_stmt(stmt);
	debug_bb(stmt->bb);
	// fprintf(stderr, "prev_bbprev_bbprev_bbprev_bbprev_bbprev_bbprev_bbprev_bb\n");
	// debug_bb(cfun->cfg->x_exit_block_ptr->prev_bb);
	// debug_bb(cfun->cfg->x_entry_block_ptr->next_bb);
	// debug_bb(stmt->bb);
	// fprintf(stderr,"fuck %d\n",dominated_by_p(CDI_DOMINATORS,stmt->bb,cfun->cfg->x_exit_block_ptr->prev_bb));

	return !dominated_by_p(CDI_DOMINATORS, stmt->bb, cfun->cfg->x_exit_block_ptr->prev_bb);
}

void init_table()
{
	//fprintf(stderr,"init_table.... \n");
	start1.target = NULL_TREE;
	start1.next = NULL;
	start1.state = POINTER_NOT_EXIST;
	ptable = &start1;

	start2.target = NULL_TREE;
	start2.next = NULL;
	start2.state = POINTER_NOT_EXIST;
	ftable = &start2;

	start3.target = NULL_TREE;
	start3.next = NULL;
	start3.state = POINTER_NOT_EXIST;
	retable = &start3;

	start4.target = NULL_TREE;
	start4.next = NULL;
	start4.state = POINTER_NOT_EXIST;
	phitable = &start4;

	start5.target = NULL_TREE;
	start5.next = NULL;
	start5.state = POINTER_NOT_EXIST;
	return_table = &start5;

	start6.target = NULL_TREE;
	start6.next = NULL;
	start6.state = POINTER_NOT_EXIST;
	use_table = &start6;

	start7.target = NULL_TREE;
	start7.next = NULL;
	start7.state = POINTER_NOT_EXIST;
	fopen_table = &start7;
}

/*set allocation and deallocation table*/
void set_ptb(basic_block b, ptb *table, tree t, location_t l, int s, gimple *stmt, cgraph_node *node)
{
	if (table->target == NULL_TREE)
	{
		table->bb = b;
		table->target = t;
		table->next = NULL;
		table->loc = l;
		table->state = s;
		table->may_realloc = false;
		table->last_stmt = stmt;
		table->node = node;
		table->inbranch = bb_in_branch_p(stmt);
		table->fun = node->get_fun();
		if (gimple_code(stmt) == GIMPLE_PHI)
		{
			//fprintf(stderr, "add phi stmt ");debug_head(table->target);
		}
		table->removed = false;
		table->size = table->size + 1;
	}
	else
	{
		while (table->next != NULL)
		{
			table = table->next;
		}
		table->next = new ptb();
		table = table->next;
		table->bb = b;
		table->target = t;
		table->next = NULL;
		table->loc = l;
		table->state = s;
		table->may_realloc = false;
		table->last_stmt = stmt;
		table->node = node;
		table->inbranch = bb_in_branch_p(stmt);
		table->fun = node->get_fun();
		if (gimple_code(stmt) == GIMPLE_PHI)
		{
			//fprintf(stderr, "add phi stmt ");debug_head(table->target);
			table->is_phi_stmt = true;
		}
		else
			table->is_phi_stmt = false;
		table->removed = false;
		table->size = table->size + 1;
	}
}

void set_gimple_array(gimple_array *table, gimple *used_stmt, tree a, tree target)
{
	// fprintf(stderr, "set_gimple_array----------------\n");
	// 	gimple_array *user_tmp ;
	// 	user_tmp = tvpt2->get(a);
	// 	if(user_tmp != NULL)
	// 	return ;
	// bool find = false;
	// gimple *u_stmt;
	// gimple_array table_tmp = table;
	// FOR_EACH_USE_TABLE(table_tmp, u_stmt) 			{
	// 	if(table_tmp->aptr == a)
	// 		fprintf(stderr , "222222222222222\n");
	// }

	if (table->stmt == NULL)
	{
		table->stmt = used_stmt;
		table->aptr = a;
		table->target = target;
		table->size = table->size + 1;
		table->next = NULL;
	}
	else
	{
		bool same = false;
		while (table->next != NULL)
		{
			//  fprintf(stderr, "set_gimple_array----------------\n");
			//  debug(table->stmt);
			//  debug(used_stmt);
			if (table->stmt == used_stmt)
			// if (table->stmt == used_stmt && LOCATION_LINE(gimple_location(table->stmt)) == LOCATION_LINE(gimple_location(used_stmt)))
			{

				same = true;
				break;
			}
			table = table->next;
		}
		if (!same)
		{
			table->next = new gimple_array();
			table = table->next;
			table->stmt = used_stmt;
			table->target = target;
			table->size = table->size + 1;
			table->aptr = a;
			table->next = NULL;
		}
	}
}
/*Algorithm 4*/
void search_imm_use(gimple_array *used_stmt, tree target)
{
	imm_use_iterator imm_iter;
	gimple_array *used2 = used_stmt;
	gimple *use_stmt;
	gimple *gc;
	fprintf(stderr, "debug_tree---------------------------------\n");
	debug_tree(target);
	fprintf(stderr, "debug_tree end---------------------------------\n");
	int count = 0;
	FOR_EACH_IMM_USE_STMT(use_stmt, imm_iter, target)
	{
		// fprintf(stderr,"---------------------------------\n") 	;
		// debug_gimple_stmt(use_stmt);
		// fprintf(stderr,"---------------------------------\n");
		fprintf(stderr, "-------------------------%d--------sssssssss\n", count);
		fprintf(stderr, "---------------------------------\n");
		debug_gimple_stmt(use_stmt);
		warning_at(gimple_location(use_stmt), 0, "use location");
		fprintf(stderr, "---------------------------------\n");
		if (gimple_code(use_stmt) == GIMPLE_ASSIGN)
		{
			//debug_tree(gimple_assign_lhs(use_stmt));
			debug_gimple_stmt(use_stmt);
			if (gimple_assign_lhs(use_stmt) && TREE_CODE(gimple_assign_lhs(use_stmt)) == SSA_NAME && gimple_assign_single_p(use_stmt))
			{
				// set_gimple_array(used_stmt, use_stmt, target);
				fprintf(stderr, "lhshslhslhslhslhslhslshlshlshlshlsh------------------\n");
				search_imm_use(used_stmt, gimple_assign_lhs(use_stmt));
			}
		}
		else if (gimple_code(use_stmt) == GIMPLE_PHI)
		{
			fprintf(stderr, "-----------------hiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii------------------\n");
			if (gimple_phi_result(use_stmt) && TREE_CODE(gimple_phi_result(use_stmt)) == SSA_NAME)
			{
				//debug_gimple_stmt(use_stmt);
				debug_gimple_stmt(use_stmt);
				//debug_tree(gimple_phi_result(use_stmt));
				bool exist = false;
				FOR_EACH_USE_TABLE(used2, gc)
				{

					//debug_gimple_stmt(gc);
					if (gc == use_stmt)
					{
						exist = true;
						fprintf(stderr, "----------------what r u loolking for------------------\n");
						debug_tree(gimple_phi_result(use_stmt));
					}
				}
				if (!exist)
				{
					// set_gimple_array(used_stmt, use_stmt, target);
					fprintf(stderr, "-----------------hiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii------------------\n");
					debug_tree(gimple_phi_result(use_stmt));
					search_imm_use(used_stmt, gimple_phi_result(use_stmt));
				}
			}
		}
		else
		{
			fprintf(stderr, "-------always in therealways in therealways in there--------------------------\n");
			debug_gimple_stmt(use_stmt);

			// set_gimple_array(used_stmt, use_stmt, target);
		}
		count++;
	}
}

void print_path(vector<pair<fdecl, location_t>> *path)
{
	fprintf(stderr, "malloc from ->\n");
	for (int i = 0; i < (*path).size(); i++)
		fprintf(stderr, "	function ->%s in loc %d \n", IDENTIFIER_POINTER(DECL_NAME((*path)[i].first.fndecl)), LOCATION_LINE((*path)[i].second));
}

bool path_intersection(vector<pair<fdecl, location_t>> *path, tree t, location_t loc)
{
	for (int i = 0; i < (*path).size(); i++)
	{
		if ((*path)[i].first.fndecl == t && (*path)[i].second == loc)
		{
			//fprintf(stderr,"intersection\n");
			return true;
		}
	}
	return false;
}

bool path_intersection_tvpt(vector<pair<fdecl, location_t>> path, vector<pair<fdecl, location_t>> path2)
{
	vector<pair<fdecl, location_t>> temp;
	int ii = 0;
	if (path.size() < path2.size())
	{
		temp = path2;
		path2 = path;
		path = temp;
	}
	for (int i = 0; i < path.size();)
	{
		if (path.size() - i < path2.size())
			return false;

		fprintf(stderr, "=path_intersection_tvpt= ii=%d i=%d\n", ii, i);
		fprintf(stderr, "=path1ptr  %s  %d=\n", IDENTIFIER_POINTER(DECL_NAME(path[i + ii].first.fndecl)), LOCATION_LINE(path[i + ii].second));
		fprintf(stderr, "=path2ptr  %s  %d=\n", IDENTIFIER_POINTER(DECL_NAME(path2[ii].first.fndecl)), LOCATION_LINE(path2[ii].second));

		if (path[i + ii].first.fndecl == path2[ii].first.fndecl)
		{
			if (path[i + ii].second == path2[ii].second)
			{
				if (ii == path2.size() - 1)
					return true;
				ii++;
				continue;
			}
		}
		if (ii > 0)
			ii = 0;
		return false;
	}
	return false;
}

void print_tvpt(tree a)
{
	if (tvpt->get(a) == NULL)
		return;
	var_points_to vpt = *(tvpt->get(a));
	vector<vector<pair<fdecl, location_t>>> path = vpt.may_malloc_path;
	vector<pair<fdecl, location_t>> loc;
	fprintf(stderr, "			=======print_tvpt %d   %d========\n", a, path.size());
	for (int i = 0; i < path.size(); i++)
	{
		print_path(&path[i]);
	}
}

bool same_path(tree t1, tree t2)
{
	if (tvpt->get(t1) == NULL || tvpt->get(t2) == NULL)
		return false;
	var_points_to vpt = *(tvpt->get(t1));
	var_points_to vpt2 = *(tvpt->get(t2));
	int ii = 0;
	vector<vector<pair<fdecl, location_t>>> path = vpt.may_malloc_path;
	vector<vector<pair<fdecl, location_t>>> path2 = vpt2.may_malloc_path;

	for (int i = 0; i < path.size(); i++)
	{
		for (int j = 0; j < path2.size(); j++)
		{
			if (path_intersection_tvpt(path[i], path2[j]))
			{

				return true;
			}
		}
	}
	return false;
}
void tracer_caller(cgraph_node *dest, cgraph_node *current, vector<pair<fdecl, location_t>> *path)
{
	//fprintf(stderr,"======dest:%s current:%s\n",function_name (DECL_STRUCT_FUNCTION(dest->decl)),function_name (DECL_STRUCT_FUNCTION(current->decl)));
	//debug_tree(current->decl);
	cgraph_node *ftn;
	cgraph_node *node2;

	for (cgraph_edge *e = current->callers; e; e = e->next_caller)
	{
		function *f;
		fdecl fd;
		location_t loc;
		tree current_fncall;
		tree current_fndecl;
		gimple *current_ga = e->call_stmt;
		debug_gimple_stmt(current_ga);

		//debug_tree(a);
		f = DECL_STRUCT_FUNCTION(gimple_call_fndecl(current_ga));
		current_fndecl = gimple_call_fndecl(current_ga);
		current_fncall = gimple_call_fn(current_ga);
		fd.fn = current_fncall;
		fd.fndecl = current_fndecl;

		loc = gimple_location(current_ga);

		//debug_head(a);
		if (f != NULL)
		{
			if (!path_intersection(path, current_fndecl, loc))
			{
				vector<pair<fdecl, location_t>> next = *path;
				next.push_back(make_pair(fd, loc));

				//print_path(&next);
				//debug_head(a);

				node2 = *(fnode->get(current_fncall));
				ftn = node2;
				if (dest->decl == ftn->decl)
				{

					tree lhs = gimple_call_lhs(current_ga);
					print_path(&next);
					debug_head(gimple_call_lhs(current_ga));

					var_points_to a = *(tvpt->get(lhs));
					a.may_malloc_path.push_back(next);
					tvpt->put(lhs, a);

					continue;
				}
				if (gimple_call_lhs(current_ga) != NULL)
				{
					// fprintf(stderr ,"wqejwpoqekpoqwkeopqwkepoqwkepoqwek\n");
					// debug(dest->decl );
					// fprintf(stderr ,"wqejwpoqekpoqwkeopqwkepoqwkepoqwek\n");
					tree lhs = gimple_call_lhs(current_ga);
					gimple_array start;
					gimple *stmt;
					start.stmt = NULL;
					gimple_array *used_stmt = &start;
					search_imm_use(used_stmt, lhs);
					FOR_EACH_USE_TABLE(used_stmt, stmt)
					{
						//gimple_bb(SSA_NAME_DEF_STMT (gimple_phi_arg_def(phi, i))
						if (gimple_code(stmt) == GIMPLE_RETURN)
						{
							tracer_caller(dest, ftn, &next);
						}
					}
				}
			}
		}
	}
}

void search_all_path(ptb *free_t, ptb *malloc_t)
{
	vector<pair<fdecl, location_t>> path;

	tracer_caller(free_t->node, malloc_t->node, &path);
}

bool bb_in_same_loop_alloc_free(basic_block m, basic_block f)
{
	if (m->loop_father->header->index == f->loop_father->header->index)
		if (m->loop_father->header->index != 0)
			return true;
	return false;
}

bool bb_in_loop_p(basic_block bb)
{
	return bb->loop_father->header->index != 0;
}

bool Location_b(gimple *a, gimple *b, basic_block bb)
{
	gimple *gc;
	//debug_gimple_stmt(a);
	//debug_gimple_stmt(b);
	for (gimple_stmt_iterator gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi))
	{
		gc = gsi_stmt(gsi);
		//debug_gimple_stmt(gc);
		if (gc == a)
		{
			return true;
		}
		else if (gc == b)
		{
			return false;
		}
	}
	return false;
}
/*Algorithm 5*/
void new_use_after_free_analysis(ptb *ptable, ptb *ftable)
{

	ptb *table1 = ptable;
	ptb *table3 = ftable;
	bool to_remove = false;
	tree t;
	struct ptr_info_def *pi;
	struct pt_solution *pt;
	fprintf(stderr, "start use after free analysis\n");
	FOR_EACH_TABLE(table3, t)
	{
		if (table3->removed || !TREE_CODE(t) == SSA_NAME)
			continue;
		pi = SSA_NAME_PTR_INFO(table3->target);
		pt = &pi->pt;
		//if(pt->null)
		//continue;
		//fprintf(stderr,"---free_table-----\n");
		//debug_head(table3->target);
		gimple_array start;
		start.stmt = NULL;
		gimple_array *used_stmt = &start;
		fprintf(stderr, "noewnoewnoewnoewnoew -----------------------------------------------\n");
		//  search_imm_use(used_stmt, table3->target);
		tree t2;
		FOR_EACH_TABLE(table1, t2)
		{
			if (TREE_CODE(table1->target) != SSA_NAME)
			{

				continue;
			}
			if (ptr_derefs_may_alias_p(table3->target, table1->target))
			{
				fprintf(stderr, "testing-----------------------------------------------\n");
				warning_at(table3->loc, 0, "Use after free error!: free location");
				warning_at(table1->loc, 0, "Use after free error!: free location");
				//warning_at(gimple_location(used_stmt->stmt), 0, "use location"); ///ppp
				debug_tree(table1->target);
				debug_tree(table3->target);
				fprintf(stderr, "testing-----------------------------------------------\n");
				search_imm_use(used_stmt, table1->target);
			}
		}
		table1 = ptable;

		//pointer1
		gimple *u_stmt;
		//fprintf(stderr,"=====free: ");
		//debug_head(table3->target);
		FOR_EACH_USE_TABLE(used_stmt, u_stmt)
		{

			//debug_gimple_stmt(u_stmt);
			if (gimple_code(used_stmt->stmt) == GIMPLE_PHI)
				continue;
			if (gimple_call_builtin_p(used_stmt->stmt, BUILT_IN_FREE))
				continue;
			//fprintf(stderr,"-----  %d ~ %d \n",table3->bb->index,gimple_bb(used_stmt->stmt)->index);
			//fprintf(stderr,"free line : %d ,use line : %d \n",LOCATION_LINE((table3->loc)),LOCATION_LINE(gimple_location(used_stmt->stmt)));
			//fprintf(stderr,"free bb : %d ,use bb : %d \n",table3->bb->index,gimple_bb(used_stmt->stmt)->index);
			if (table3->bb->index == gimple_bb(used_stmt->stmt)->index)
			{
				if (Location_b(table3->last_stmt, used_stmt->stmt, table3->bb))
				{ //LOCATION_LINE((table3->loc))<=LOCATION_LINE(gimple_location(used_stmt->stmt))
					//warning_at(table3->loc, 0, "");
					//debug_head(table3->target);
					//fprintf(stderr,"--index : %d %d---\n",table3->bb->index,gimple_bb(used_stmt->stmt)->index);
					if (!table3->removed)
					{
						warning_at(table3->loc, 0, "Use after free error!: free location");
						warning_at(gimple_location(used_stmt->stmt), 0, "use location"); ///ppp
																						 //fprintf(fp,"%s use after free\n",gimple_filename (table3->last_stmt));
																						 //remove_pointer_stmt(table3->last_stmt);
																						 //table3->removed=true;
					}
				}
			}
			else if (fDFS->get(table3->node)->is_succ(table3->bb, gimple_bb(used_stmt->stmt)))
			{
				//fprintf(stderr,"----- is_succ\n");
				if (!table3->removed)
				{
					warning_at(table3->loc, 0, "Use after free error!: free location");
					warning_at(gimple_location(used_stmt->stmt), 0, "use location");
					//fprintf(fp,"%s use after free \n",DECL_SOURCE_FILE (table3->fun->decl));
					//remove_pointer_stmt(table3->last_stmt);
					//table3->removed=true;
				}
			}
		}
		//fprintf(stderr,"=====use table end \n");
	}
}
bool analyze_type(tree tree_typed, tree trace_tree)
{
	fprintf(stderr, "%s ", get_tree_code_name(TREE_CODE(tree_typed)));
	tree tree_typed_tmp = tree_typed;
	while (true)
	{
		switch (TREE_CODE(tree_typed_tmp))
		{
		case FUNCTION_TYPE:
		{
			fprintf(stderr, "receives ");
			tree args = TYPE_ARG_TYPES(tree_typed);
			if (args == NULL_TREE)
				fprintf(stderr, "nothing ");
			else
				while (args != NULL_TREE && TREE_VALUE(args))
				{
					//!= void_type_node
					tree_typed_tmp=TREE_VALUE(args);
					//analyze_type(TREE_VALUE(args), &trace_tree);
					if (TREE_CHAIN(args) != NULL_TREE)
						fprintf(stderr, "and ");
					else
						fprintf(stderr, "... "); // this node is not void_type_node and the next node is NULL_TREE, thus variadic arguments
												 // debug_tree(args);
					args = TREE_CHAIN(args);
					fprintf(stderr, "\n then returns2 \n");
					// debug_tree(args);
				}
			fprintf(stderr, "then returns ");
		
			// analyze_type(TREE_TYPE(tree_typed), &trace_tree);
			break;
		}
		case VOID_TYPE:
		{
			// debug_tree(tree_typed);
		}
		case RECORD_TYPE:
		{
			fprintf(stderr, "named ");
			tree type_name = TYPE_NAME(tree_typed);
			if (TREE_CODE(type_name) == IDENTIFIER_NODE)
			{
				fprintf(stderr, "%s ", IDENTIFIER_POINTER(type_name));
			}
			else if (TREE_CODE(type_name) == TYPE_DECL)
			{
				fprintf(stderr, "%s ", IDENTIFIER_POINTER(DECL_NAME(type_name)));
			}
			else
			{
				fprintf(stderr, "**** UNRECOGNIZED TYPE NODE ****");
			}
			break;
		}
		case POINTER_TYPE:
		{
			fprintf(stderr, "\n--------------------------------------------\n");
			debug_tree(TREE_TYPE(tree_typed_tmp));
			// debug_tree(trace_tree);
			struct ptr_info_def *pi1;
			struct pt_solution *pt1;
			struct ptr_info_def *pi2;
			struct pt_solution *pt2;
			pi1 = SSA_NAME_PTR_INFO(TREE_TYPE(tree_typed_tmp));
			pt1 = &pi1->pt;
			pi2 = SSA_NAME_PTR_INFO(TREE_TYPE(TREE_TYPE(trace_tree)));
			pt2 = &pi2->pt;
			//  analyze_type(tree_typed, TREE_TYPE(trace_tree));
			debug_tree(TREE_TYPE(TREE_TYPE(trace_tree)));
			if (TREE_TYPE(tree_typed_tmp) == TREE_TYPE(TREE_TYPE(trace_tree)))
			{
				fprintf(stderr, "\a");
				fprintf(stderr, "\nfuckkkkkkddddddddddddddddddddddddddddddddddddddddddddddddkkkkker\n");
				return true;
			}
			// if (pt1->vars == NULL || pt2->vars == NULL)
			// 	return false;
			// if (bitmap_intersect_p(pt1->vars, pt2->vars)) 			{
			// 	fprintf(stderr,"\a");
			// 			fprintf(stderr,"\nfuckkkkkkddddddddddddddddddddddddddddddddddddddddddddddddkkkkker\n"); 				return true;
			// }
			// if( ptr_derefs_may_alias_p(tree_typed, trace_tree)){
			// 	fprintf(stderr,"\nfuckkkkkkkkkkker\n");
			// 	return true;
			// 	}
			fprintf(stderr, "----------------------------------------------\n");
			tree_typed_tmp=TREE_TYPE(tree_typed);
			//analyze_type(TREE_TYPE(tree_typed), trace_tree);
			break;
		}
		case INTEGER_TYPE:
		{
			unsigned int precision = TYPE_PRECISION(tree_typed);
			fprintf(stderr, "%s integer of %d bits ", TYPE_UNSIGNED(tree_typed) ? "unsigned" : "signed", precision);
			break;
		}
		case ARRAY_TYPE:
		{
			fprintf(stderr, "of ");
			tree_typed_tmp=TREE_TYPE(tree_typed);
			//analyze_type(TREE_TYPE(tree_typed), &trace_tree);
			break;
		}
		default:
			break;
		}
	}

	return false;
}

/*new_search_imm_use */
void new_search_imm_use(gimple_array *used_stmt, tree target, tree target2)
{
	int count = 0;
	tree may_alias_p;
	tree may_alias_p2;
	may_alias_p = target2;
	may_alias_p2 = target2;
	gimple *def_stmt_start = SSA_NAME_DEF_STMT(target);
	gimple *def_stmt_end = SSA_NAME_DEF_STMT(target);
	int start = 0;
	int end = 0;
	imm_use_iterator imm_iter;
	gimple_array *used2 = used_stmt;
	gimple *use_stmt;
	gimple *gc;
	const char *name;
	gimple *def_stmt = SSA_NAME_DEF_STMT(target);
	gimple *def_org = SSA_NAME_DEF_STMT(target);
	gimple *def_head = SSA_NAME_DEF_STMT(target);
	gimple *def_stmt2 = SSA_NAME_DEF_STMT(target);


	gimple *def_stmt = SSA_NAME_DEF_STMT(table3->target);
	debug_gimple_stmt(def_stmt); 					
	fprintf(stderr, "coooooooooooooooooooooooooooooooooooool \n"); 	
	if (gimple_code(def_stmt) == GIMPLE_PHI) 	{ 						
		for (unsigned i = 0; i < gimple_phi_num_args(def_stmt); i++) 	{ 
			tree argu = gimple_phi_arg_def(def_stmt, i); 
			debug_tree(argu);
	}	

	
						//fprintf(stderr,"phi_argu:%d ",i); 			
										//debug_tree(argu); 					
					// pi3 = get_ptr_info(argu); 						
					// pt3 = &pi3->pt; 						
					// if (pt3->vars == NULL) 				
					// continue; 					
					// 	if (!bitmap_intersect_p(pt2->vars, pt3->vars)) 				
					// 				{ 						
					// 						table1->state = POINTER_STATE_MAY_IS_FREE; 		
					// 											} 						} 		
																			}
	// debug_tree(target);

	// if( !strcmp(get_tree_code_name(TREE_CODE(target)), "addr_expr")) 	{
	// 		 		// debug_tree(target);
	// 	  	return ; 	}
	// if((int)gimple_call_num_args(def_stmt) ==-1)
	// return;
	// new_search_imm_use(used_stmt, gimple_assign_lhs(use_stmt),target2);

	// fprintf(stderr, "有趣了幹---------------------------------- calllllll\n");
	// debug_gimple_stmt(def_stmt);
	// tree lhs = gimple_call_lhs(def_stmt);
	// set_gimple_array(used_stmt, def_stmt, lhs, target2);
	// debug_tree(lhs);
	// tree lhs2 = gimple_call_lhs(def_stmt->prev);
	// debug_gimple_stmt(def_stmt->prev);
	// set_gimple_array(used_stmt, def_stmt->prev, lhs2, target2);

	do
	{
		// def_stmt_start = (def_stmt_start->prev);
		start++;
		def_stmt_start = (def_stmt_start->prev);
	} while (def_stmt_start == NULL);

	do
	{
		end++;
		def_stmt_end = (def_stmt_end->next);
	} while (def_stmt_end == NULL);
	// for ( int i = 0 ; i < 20 ;i++){
	// def_stmt = def_stmt->next;
	// }
	// fprintf(stderr, "---------------------fuckddddddddddddddddddddddddddddddddd\n");
	// debug_gimple_stmt(def_stmt_start);
	// debug_gimple_stmt(def_stmt_end);
	// start = end =10;
	// fprintf(stderr,  "start %d \n",start);

	// fprintf(stderr, "end %d\n",end);
	// fprintf(stderr, "---------------------fuckddddddddddddddddddddddddddddddddd\n");

	do
	{
		// fprintf(stderr, "SEEEEEEEEEEEEEEEEEEEEEEEEEEE\n");

		// if(gimple_call_num_args(def_stmt) ==-1) {
		// 			count += 1;
		// 					def_stmt = def_stmt->prev;

		// 					if ((def_stmt == def_stmt_start) || count >= start)
		// 						break;
		// 					else
		// 						continue;
		// }
		// debug(def_stmt);
		if (def_stmt == NULL)
			break;
		if (gimple_num_ops(def_stmt) > 1000)
			break;
		// debug_tree(may_alias_p);

		// fprintf(stderr,"幹%s\n",get_tree_code_name(TREE_CODE(may_alias_p)));
		// if(!strcmp(get_tree_code_name(TREE_CODE(target)), "SSA_NAME")
		// fprintf(stderr,  "count %d \n",count);
		if (!strcmp(get_tree_code_name(TREE_CODE(may_alias_p)), "addr_expr"))
		{ //debug_tree(target);
			tree fundecl = TREE_OPERAND(may_alias_p, 0);
			may_alias_p2 = may_alias_p; // debug(def_stmt);
			may_alias_p = target;
			fprintf(stderr, "addr_expraddr_expraddr_expraddr_expraddr_expraddr_expraddr_expr\n");
			set_gimple_array(used_stmt, def_stmt, may_alias_p, may_alias_p2);
			count += 1; // def_stmt = def_stmt->prev->prev; 				// def_stmt = def_stmt2->prev; 			// fprintf(stderr, "test\n"); 			def_stmt = def_stmt->prev; 			if (def_stmt == def_stmt_start) 				break;
		}
		// }else
		// debug(def_stmt);
		// if (def_stmt == def_stmt_start) 				break;
		else if (is_gimple_assign(def_stmt))
		{
			if (target2 == gimple_assign_rhs1(def_stmt))
			{

				//  may_alias_ex=may_alias_p;
				may_alias_p2 = may_alias_p;
				// debug(def_stmt);
				may_alias_p = gimple_assign_rhs1(def_stmt);

				// if(may_alias_p !=may_alias_p2)
				// if( ptr_derefs_may_alias_p(may_alias_p, may_alias_p2))
				set_gimple_array(used_stmt, def_stmt, may_alias_p, may_alias_p2);
			}
			else if (target2 == gimple_assign_lhs(def_stmt))
			{
				//  may_alias_ex=may_alias_p;
				may_alias_p2 = may_alias_p;
				// debug(def_stmt);
				may_alias_p = gimple_assign_lhs(def_stmt);

				// if(may_alias_p !=may_alias_p2)
				// if( ptr_derefs_may_alias_p(may_alias_p, may_alias_p2))
				set_gimple_array(used_stmt, def_stmt, may_alias_p, may_alias_p2);
			}
		}
		else if (is_gimple_call(def_stmt))
		{

			if (target2 == gimple_call_lhs(def_stmt) && ptr_derefs_may_alias_p(may_alias_p, may_alias_p2))
			{
				// fprintf(stderr, "gimple_call_lhsgimple_call_lhsgimple_call_lhsgimple_call_lhs\n");
				debug(def_stmt);
				// may_alias_ex=may_alias_p;
				may_alias_p2 = may_alias_p;

				may_alias_p = gimple_call_lhs(def_stmt);
				// debug(def_stmt);

				// debug_tree(may_alias_p);
				// debug_tree(may_alias_p2);
				// if( ptr_derefs_may_alias_p(may_alias_p, may_alias_p2))
				set_gimple_array(used_stmt, def_stmt, may_alias_p, may_alias_p2);
				// new_search_imm_use(used_stmt, may_alias_p,target2);
			}
			// else if (gimple_call_arg(def_stmt,0) && TREE_CODE(gimple_call_fn(def_stmt)) == SSA_NAME && ptr_derefs_may_alias_p(may_alias_p, may_alias_p2)) 			{
			else if (target2 == gimple_call_arg(def_stmt, 0) && ptr_derefs_may_alias_p(may_alias_p, may_alias_p2))
			{
				// fprintf(stderr, "gimple_call_lhsgimple_call_lhsgimple_call_lhsgimple_call_lhs\n");
				debug(def_stmt);
				may_alias_p2 = may_alias_p;
				may_alias_p = gimple_call_arg(def_stmt, 0);
				// if( ptr_derefs_may_alias_p(may_alias_p, may_alias_p2))
				set_gimple_array(used_stmt, def_stmt, may_alias_p, may_alias_p2);
			}
			if (gimple_call_fn(def_stmt) == NULL)
			{
				return;
				debug(def_stmt);
				name = get_name(gimple_call_fn(def_stmt));
				if (!strcmp(name, "free") || !strcmp(name, "xfree"))
				{
					fprintf(stderr, "is_gimple_callis_gimple_callis_gimple_callis_gimple_callis_gimple_call\n");
					tree fundecl = TREE_OPERAND(gimple_call_fn(def_stmt), 0);
					if (ptr_derefs_may_alias_p(may_alias_p, fundecl))
					{
						fprintf(stderr, "ptr_derefs_may_alias_pptr_derefs_may_alias_pptr_derefs_may_alias_p\n");
						debug(fundecl);
						tree type_tree = TREE_TYPE(fundecl);
						debug_tree(type_tree);
						fprintf(stderr, "analyze_typeanalyze_typeanalyze_typeanalyze_typeanalyze_typeanalyze_typeanalyze_typeanalyze_type\n");
						//analyze_type(type_tree, &target2);
						fprintf(stderr, "name: %s \n", DECL_NAME(fundecl) != NULL_TREE ? IDENTIFIER_POINTER(DECL_NAME(fundecl)) : "<unamed>");
						fprintf(stderr, "\nanalyze_typeanalyze_typeanalyze_typeanalyze_typeanalyze_typeanalyze_typeanalyze_typeanalyze_type\n");
						// fprintf(stderr, "ptr_derefs_may_alias_pptr_derefs_may_alias_pptr_derefs_may_alias_p\n");
						// debug_tree(  (TREE_OPERAND (fundecl, 0)));
						if (analyze_type(type_tree, target2) == true)
						{
							fprintf(stderr, "fuck\n");
							may_alias_p2 = may_alias_p;
							may_alias_p = fundecl;
							set_gimple_array(used_stmt, def_stmt, fundecl, may_alias_p2);
						}
						// may_alias_p2 = may_alias_p;
						// may_alias_p = fundecl;
						// set_gimple_array(used_stmt, def_stmt, fundecl, may_alias_p2);
						// fprintf(stderr, "SEEEEEEEEEEEEEEEEEEEEEEEEEEE %d\n" ,TREE_CODE(gimple_call_fn(def_stmt2)) == SSA_NAME); 				// if(ptr_derefs_may_alias_p(may_alias_p, may_alias_p2)) 				set_gimple_array(used_stmt, def_stmt2, may_alias_p, may_alias_p2);
						// fprintf(stderr, "%s\n", gimple_call_num_args(def_stmt2));
						// if(gimple_call_num_args(gimple_call_arg(def_stmt2, 0) )))
						// 	tree fundecl = TREE_OPERAND(gimple_assign_lhs(def_stmt2), 0);
						// 		fprintf(stderr, "is_gimple_callis_gimple_callis_gimple_callis_gimple_callis_gimple_call\n");
						// 		debug(def_stmt2);
					}
				}
			}
		}

		// fprintf(stderr,  "count %d \n",count);
		// def_stmt = def_stmt->prev->prev;
		// def_stmt = def_stmt2->prev;
		// fprintf(stderr, "test\n");
		// if(start==0)
		// break;
		def_stmt = def_stmt->prev;
		if (def_stmt != NULL)
			def_head = def_stmt;
		if (def_stmt == def_head)
			count += 1;
		else
			count = 0;
		//def_stmt = def_stmt->prev;

		if (count > 3)
			break;
		// if (def_stmt == NULL)
		// 	break;

		// ptr_derefs_may_alias_p(may_alias_p,target2)!=false
	} while (true);

	count = 0;
	// tree may_alias_p=NULL;
	// tree may_alias_p2=NULL;
	// def_stmt2 = def_stmt2->next;
	do
	{
		// if(def_stmt2==def_org)
		// fprintf(stderr,"幹%d\n",ptr_derefs_may_alias_p(may_alias_p, may_alias_p2));

		// if( !strcmp(get_tree_code_name(TREE_CODE(target)), "addr_expr")) 	{ 		//debug_tree(target);
		// 	return ; 	}
		// if(gimple_call_num_args(def_stmt2) ==-1) {
		// 		count += 1;
		// 						def_stmt2 = def_stmt->next;
		// 					if ((def_stmt2 == def_stmt_end) || count >= end)
		// 												break;
		// 												else
		// 												continue;
		// 		}
		// debug(def_stmt2);
		if (def_stmt2 == NULL)
			break;
		//if(gimple_call_internal_p(def_stmt2))
		//fprintf(stderr,"dqwwwwwwwwww%s\n",get_tree_code_name(TREE_CODE(gimple_call_lhs(def_stmt2))));
		// fprintf(stderr,"幹幹幹幹幹幹幹幹幹幹幹幹幹幹幹幹幹幹幹幹%d\n",gimple_num_ops (def_stmt2));
		if (gimple_num_ops(def_stmt2) > 1000)
			break;
		if (!strcmp(get_tree_code_name(TREE_CODE(may_alias_p)), "addr_expr"))
		{ //debug_tree(target);
			// fprintf(stderr,"幹幹幹幹幹幹幹幹幹幹幹幹幹幹幹幹幹幹幹幹%d\n",gimple_num_ops (def_stmt2->bb));
			tree fundecl = TREE_OPERAND(may_alias_p, 0);
			may_alias_p2 = may_alias_p; // debug(def_stmt);
			may_alias_p = fundecl;

			// debug_tree(may_alias_p);
			set_gimple_array(used_stmt, def_stmt2, may_alias_p, may_alias_p2);
			// count += 1; 			// def_stmt = def_stmt->prev->prev; 				// def_stmt = def_stmt2->prev; 			// fprintf(stderr, "test\n"); 			def_stmt = def_stmt->prev; 			if (def_stmt == def_stmt_start) 				break;
		}
		// if(is_gimple_assign(def_stmt2)){

		else if (gimple_code(def_stmt2) == GIMPLE_ASSIGN)
		{
			// debug(def_stmt2);
			// fprintf(stderr,"%s\n",get_tree_code_name(TREE_CODE(gimple_assign_lhs(def_stmt2))));
			// debug_tree(gimple_assign_lhs(def_stmt2));
			if (!strcmp(get_tree_code_name(TREE_CODE(gimple_assign_lhs(def_stmt2))), "mem_ref"))
			{
				tree fundecl = TREE_OPERAND(gimple_assign_lhs(def_stmt2), 0);
				// debug(def_stmt2);
				// debug_tree(fundecl);
				if (ptr_derefs_may_alias_p(fundecl, target2))
					// // debug(gimple_assign_lhs(def_stmt2));
					// // debug_tree(gimple_assign_lhs(def_stmt2));
					// 		may_alias_p2 = may_alias_p;
					// 			may_alias_p = gimple_assign_lhs(def_stmt2);
					//  if(ptr_derefs_may_alias_p(may_alias_p, may_alias		//
					set_gimple_array(used_stmt, def_stmt2, may_alias_p, may_alias_p2);
				//  fprintf(stderr,"幹幹幹幹幹幹幹幹幹幹幹幹幹幹幹幹幹幹幹幹\n");
			}
			// debug(def_stmt2);
			if (target2 == gimple_assign_rhs1(def_stmt2))
			{
				//  may_alias_ex=may_alias_p;
				may_alias_p2 = may_alias_p;

				may_alias_p = gimple_assign_rhs1(def_stmt2);

				// if(may_alias_p !=may_alias_p2)
				//  if(ptr_derefs_may_alias_p(may_alias_p, may_alias_p2))
				set_gimple_array(used_stmt, def_stmt2, may_alias_p, may_alias_p2);
			}

			else if (target2 == gimple_assign_lhs(def_stmt2))
			{
				// debug(def_stmt2);
				may_alias_p2 = may_alias_p;
				may_alias_p = gimple_assign_lhs(def_stmt2);
				//  if(ptr_derefs_may_alias_p(may_alias_p, may_alias_p2))
				set_gimple_array(used_stmt, def_stmt2, may_alias_p, may_alias_p2);
			}
		}
		// }
		else if (is_gimple_call(def_stmt2))
		{

			if (target2 == gimple_call_lhs(def_stmt2) && ptr_derefs_may_alias_p(may_alias_p, may_alias_p2))
			{

				// may_alias_ex=may_alias_p;
				may_alias_p2 = may_alias_p;
				may_alias_p = gimple_call_lhs(def_stmt2);
				//  if(ptr_derefs_may_alias_p(may_alias_p, may_alias_p2))
				// if(may_alias_p !=may_alias_p2)

				set_gimple_array(used_stmt, def_stmt2, may_alias_p, may_alias_p2);
			}
			else if (target2 == gimple_call_arg(def_stmt2, 0) && ptr_derefs_may_alias_p(may_alias_p, may_alias_p2))
			{

				may_alias_p2 = may_alias_p;

				may_alias_p = gimple_call_arg(def_stmt2, 0);
				// fprintf(stderr, "SEEEEEEEEEEEEEEEEEEEEEEEEEEE %d\n" ,TREE_CODE(gimple_call_fn(def_stmt2)) == SSA_NAME);
				// if(ptr_derefs_may_alias_p(may_alias_p, may_alias_p2))
				set_gimple_array(used_stmt, def_stmt2, may_alias_p, may_alias_p2);
			}
			if (gimple_call_fn(def_stmt2) == NULL)
			{
				return;
				debug(def_stmt2);
				name = get_name(gimple_call_fn(def_stmt2));
				if (!strcmp(name, "free") || !strcmp(name, "xfree"))
				{
					fprintf(stderr, "is_gimple_callis_gimple_callis_gimple_callis_gimple_callis_gimple_call\n");
					tree fundecl = TREE_OPERAND(gimple_call_fn(def_stmt2), 0);
					if (ptr_derefs_may_alias_p(may_alias_p, fundecl))
					{
						// fprintf(stderr, "ptr_derefs_may_alias_pptr_derefs_may_alias_pptr_derefs_may_alias_p\n");
						// debug_tree(gimple_call_fn(def_stmt2));
						// debug_tree(fundecl);
						// debug_tree(may_alias_p);
						// debug_tree(target2);
						//debug_tree(  get_tree_code_name(TREE_CODE(tree_typed)));
						// fprintf(stderr, "ptr_derefs_may_alias_pptr_derefs_may_alias_pptr_derefs_may_alias_p\n");
						//analyze_type(target);
						tree type_tree = TREE_TYPE(fundecl);
						// debug_tree(type_tree);
						fprintf(stderr, "analyze_typeanalyze_typeanalyze_typeanalyze_typeanalyze_typeanalyze_typeanalyze_typeanalyze_type\n");
						// analyze_type(type_tree, &target2);
						fprintf(stderr, "name: %s \n", DECL_NAME(fundecl) != NULL_TREE ? IDENTIFIER_POINTER(DECL_NAME(fundecl)) : "<unamed>");
						fprintf(stderr, "\nanalyze_typeanalyze_typeanalyze_typeanalyze_typeanalyze_typeanalyze_typeanalyze_typeanalyze_type\n");
						// fprintf(stderr, "ptr_derefs_may_alias_pptr_derefs_may_alias_pptr_derefs_may_alias_p\n");
						// debug_tree(  (TREE_OPERAND (fundecl, 0)));
						if (analyze_type(type_tree, target2) == true)
						{
							fprintf(stderr, "fuck\n");
							may_alias_p2 = may_alias_p;
							may_alias_p = fundecl;
							set_gimple_array(used_stmt, def_stmt2, fundecl, may_alias_p2);
						}
						// fprintf(stderr, "SEEEEEEEEEEEEEEEEEEEEEEEEEEE %d\n" ,TREE_CODE(gimple_call_fn(def_stmt2)) == SSA_NAME); 				// if(ptr_derefs_may_alias_p(may_alias_p, may_alias_p2)) 				set_gimple_array(used_stmt, def_stmt2, may_alias_p, may_alias_p2);
						// fprintf(stderr, "%s\n", gimple_call_num_args(def_stmt2));
						// if(gimple_call_num_args(gimple_call_arg(def_stmt2, 0) )))
						// 	tree fundecl = TREE_OPERAND(gimple_assign_lhs(def_stmt2), 0);
						// 		fprintf(stderr, "is_gimple_callis_gimple_callis_gimple_callis_gimple_callis_gimple_call\n");
						// 		debug(def_stmt2);
					}
				}
			}
		}
		count += 1;
		// fprintf(stderr,  "count %d \n",count);
		// if(end==0) 			return;
		// debug(def_stmt2);
		def_stmt2 = def_stmt2->next;
		if (def_stmt2 != NULL)
			count = 0;

		// def_stmt2 = def_stmt->next;

		if (count > 5)
			break;
		// fprintf(stderr, "test\n"); // ptr_derefs_may_alias_p(may_alias_p,target2)!=false
		// if (def_stmt2 == NULL)
		// 	break;

	} while (true);

	//  gimple_call_lhs(def_stmt)?may_alias_p=gimple_call_lhs(def_stmt) :gimple_assign_rhs1(def_stmt->prev->prev);
	//  do{
	//  }
	// while (ptr_derefs_may_alias_p(may_alias_p,target2)!=false);
	// gimple *def_stmt2 = SSA_NAME_DEF_STMT(lhs2);
	//  		fprintf(stderr, "有趣了幹---222222222------------------------------- calllllll\n");
	// 		 tree rhs = gimple_assign_rhs1(def_stmt->prev->prev);
	// 		 set_gimple_array(used_stmt, def_stmt->prev->prev, rhs, target2);
	// 		 set_gimple_array(used_stmt, def_stmt->prev->prev->prev, rhs, target2);
	// debug_tree(rhs);

	// fprintf(stderr, "debug_tree end---------------------------------\n");
	// FOR_EACH_IMM_USE_STMT(use_stmt, imm_iter, target)
	// {
	// debug(use_stmt);
	// }
	// fprintf(stderr, "-------always in therealways in therealways in there--------------------------\n");
}
// void new_search_imm_use2(gimple_array *used_stmt, tree target, tree target2)
// {
// 		int count = 0;
// 		tree may_alias_p;
// 		tree may_alias_p2;
// 		may_alias_p=target2;
// 		may_alias_p2=target2;
// 		gimple *def_stmt_start = SSA_NAME_DEF_STMT(target);
// 		gimple *def_stmt_end = SSA_NAME_DEF_STMT(target);
// 		int start = 0;
// 		int end = 0;
// 		imm_use_iterator imm_iter;
// 		gimple_array *used2 = used_stmt;
// 		gimple *use_stmt;
// 		gimple *gc;

// 	gimple *def_stmt = SSA_NAME_DEF_STMT(target);
// 	gimple *def_stmt2 = SSA_NAME_DEF_STMT(target);
// 	// debug_tree(target);

// 	if( !strcmp(get_tree_code_name(TREE_CODE(target)), "addr_expr")) 	{ 		//debug_tree(target);
// 	return ; 	}
// 	if((int)gimple_call_num_args(def_stmt) ==-1)
// 	return;
// 	// new_search_imm_use(used_stmt, gimple_assign_lhs(use_stmt),target2);
// 	if (gimple_code(def_stmt) == GIMPLE_CALL)
// 	{
// 		// fprintf(stderr, "有趣了幹---------------------------------- calllllll\n");
// 		// debug_gimple_stmt(def_stmt);
// 		// tree lhs = gimple_call_lhs(def_stmt);
// 		// set_gimple_array(used_stmt, def_stmt, lhs, target2);
// 		// debug_tree(lhs);
// 		// tree lhs2 = gimple_call_lhs(def_stmt->prev);
// 		// debug_gimple_stmt(def_stmt->prev);
// 		// set_gimple_array(used_stmt, def_stmt->prev, lhs2, target2);

// 		while (def_stmt_start == NULL)
// 		{
// 			// def_stmt_start = (def_stmt_start->prev);
// 			def_stmt_start = (def_stmt_start->prev);
// 			start++;
// 		}

// 		while (def_stmt_end == NULL)
// 			;
// 		{
// 			def_stmt_end = (def_stmt_end->next);
// 			end++;
// 		}
// 		// for ( int i = 0 ; i < 20 ;i++){
// 		// def_stmt = def_stmt->next;
// 		// }
// 		// fprintf(stderr, "---------------------fuckddddddddddddddddddddddddddddddddd\n");
// 		// debug_gimple_stmt(def_stmt_start);
// 		// debug_gimple_stmt(def_stmt_end);
// 		// fprintf(stderr, start + "\n");

// 		// fprintf(stderr, end + "\n");
// 	// 	fprintf(stderr, "---------------------fuckddddddddddddddddddddddddddddddddd\n");
// 	// debug(def_stmt);
// 		do
// 		{
// 				if( !strcmp(get_tree_code_name(TREE_CODE(target)), "addr_expr")) 	{ 		//debug_tree(target);
// 				return ; 	}

// 		if(gimple_call_num_args(def_stmt) ==-1) {
// 					count += 1;
// 							def_stmt = def_stmt->prev;
// 							if ((def_stmt == def_stmt_start) || count >= start)
// 								break;
// 							else
// 								continue;
// 		}
// 				// fprintf(stderr,"%s 去你媽",get_tree_code_name(TREE_CODE(may_alias_p)));
// 				// debug_tree(may_alias_p);
// 				//	fprintf(stderr,"%d 去你媽",gimple_call_num_args(def_stmt));
// 				// debug(def_stmt);
// 			// fprintf(stderr,"幹%d\n",ptr_derefs_may_alias_p(may_alias_p, may_alias_p2));
// 			// if (def_stmt == def_stmt_start) 				break;

// 			if (gimple_call_lhs(def_stmt) && TREE_CODE(gimple_call_lhs(def_stmt)) == SSA_NAME && ptr_derefs_may_alias_p(may_alias_p, may_alias_p2))
// 			{
// 				// may_alias_ex=may_alias_p;
// 				may_alias_p2 = may_alias_p;

// 				may_alias_p = gimple_call_lhs(def_stmt);
// 				// debug(def_stmt);

// 				// debug_tree(may_alias_p);
// 				// debug_tree(may_alias_p2);

// 				set_gimple_array(used_stmt, def_stmt, may_alias_p, may_alias_p2);

// 			}
// 			else if (gimple_assign_rhs1(def_stmt) && TREE_CODE(gimple_assign_rhs1(def_stmt)) == SSA_NAME && ptr_derefs_may_alias_p(may_alias_p, may_alias_p2))
// 			{
// 				//  may_alias_ex=may_alias_p;
// 				may_alias_p2 = may_alias_p;
// 				// debug(def_stmt);
// 				may_alias_p = gimple_assign_rhs1(def_stmt);

// 				// if(may_alias_p !=may_alias_p2)
// 				set_gimple_array(used_stmt, def_stmt, may_alias_p, may_alias_p2);
// 			}

// 			count += 1;
// 			// def_stmt = def_stmt->prev->prev;
// 				// def_stmt = def_stmt2->prev;
// 			// fprintf(stderr, "test\n");
// 			def_stmt = def_stmt->prev;
// 			if (def_stmt == def_stmt_start)
// 				break;

// 			// ptr_derefs_may_alias_p(may_alias_p,target2)!=false
// 		} while (count != start);

// 		count = 0;
// 		// tree may_alias_p=NULL;
// 		// tree may_alias_p2=NULL;
// 		// def_stmt2 = def_stmt2->next;
// 		do
// 		{

// 			if( !strcmp(get_tree_code_name(TREE_CODE(target)), "addr_expr")) 	{ 		//debug_tree(target);
// 				return ; 	}

// 			if (gimple_call_lhs(def_stmt2) && TREE_CODE(gimple_call_lhs(def_stmt2)) == SSA_NAME && ptr_derefs_may_alias_p(may_alias_p, may_alias_p2))
// 			{
// 				// may_alias_ex=may_alias_p;
// 				may_alias_p2 = may_alias_p;

// 				may_alias_p = gimple_call_lhs(def_stmt2);
// 					// if(may_alias_p !=may_alias_p2)
// 				set_gimple_array(used_stmt, def_stmt2, may_alias_p, may_alias_p2);
// 			}
// 			else if (gimple_assign_rhs1(def_stmt2) && TREE_CODE(gimple_assign_rhs1(def_stmt2)) == SSA_NAME && ptr_derefs_may_alias_p(may_alias_p, may_alias_p2))
// 			{
// 				//  may_alias_ex=may_alias_p;
// 				may_alias_p2 = may_alias_p;

// 				may_alias_p = gimple_assign_rhs1(def_stmt2);
// 					// if(may_alias_p !=may_alias_p2)
// 				set_gimple_array(used_stmt, def_stmt2, may_alias_p, may_alias_p2);
// 			}

// 			count += 1;
// 			def_stmt2 = def_stmt2->next;
// 			// fprintf(stderr, "test\n"); // ptr_derefs_may_alias_p(may_alias_p,target2)!=false
// 			if (def_stmt2 == def_stmt_end)
// 				break;

// 		} while (count != end);

// 		//  gimple_call_lhs(def_stmt)?may_alias_p=gimple_call_lhs(def_stmt) :gimple_assign_rhs1(def_stmt->prev->prev);
// 		//  do{
// 		//  }
// 		// while (ptr_derefs_may_alias_p(may_alias_p,target2)!=false);
// 		// gimple *def_stmt2 = SSA_NAME_DEF_STMT(lhs2);
// 		//  		fprintf(stderr, "有趣了幹---222222222------------------------------- calllllll\n");
// 		// 		 tree rhs = gimple_assign_rhs1(def_stmt->prev->prev);
// 		// 		 set_gimple_array(used_stmt, def_stmt->prev->prev, rhs, target2);
// 		// 		 set_gimple_array(used_stmt, def_stmt->prev->prev->prev, rhs, target2);
// 		// debug_tree(rhs);
// 	}
// 	// fprintf(stderr, "debug_tree end---------------------------------\n");

// 	//gimple *def_stmt = SSA_NAME_DEF_STMT(table3->target);
// 		if( !strcmp(get_tree_code_name(TREE_CODE(target)), "addr_expr")) 	{ 			return ; 	}
// 	FOR_EACH_IMM_USE_STMT(use_stmt, imm_iter, target)
// 	{
// 		if(ptr_derefs_may_alias_p(target, target2)){
// 		// fprintf(stderr,"---------------------------------\n") 	;
// 		// debug_gimple_stmt(use_stmt);
// 		// fprintf(stderr,"---------------------------------\n");
// 		// fprintf(stderr, "-------------------------%d--------sssssssss\n", count);
// 		// fprintf(stderr, "---------------------------------\n");
// 		// debug_gimple_stmt(use_stmt);

// 		// warning_at(gimple_location(use_stmt), 0, "use location<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
// 		// warning_at(gimple_code(use_stmt), 0, "\ngimple code");
// 		// set_gimple_array(used_stmt, use_stmt, target);

// 		// 	if (TREE_CODE(gimple_assign_lhs(use_stmt)) == SSA_NAME){
// 		// fprintf(stderr, "去你媽的\n");
// 		// 	}

// 		if (gimple_code(use_stmt) == GIMPLE_ASSIGN)
// 		{
// 			// fprintf(stderr, "有趣了\n");
// 			//debug_tree(gimple_assign_lhs(use_stmt));
// 			// debug_gimple_stmt(use_stmt);
// 			if (gimple_assign_lhs(use_stmt) && TREE_CODE(gimple_assign_lhs(use_stmt)) == SSA_NAME && gimple_assign_single_p(use_stmt))
// 			{
// 				set_gimple_array(used_stmt, use_stmt, target, target2);
// 				// fprintf(stderr, "lhshslhslhslhslhslhslshlshlshlshlsh------------------\n");
// 				new_search_imm_use(used_stmt, gimple_assign_lhs(use_stmt), target2);
// 			}
// 		}
// 		else if (gimple_code(use_stmt) == GIMPLE_PHI)
// 		{
// 			// fprintf(stderr, "-----------------hiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii------------------\n");
// 			if (gimple_phi_result(use_stmt) && TREE_CODE(gimple_phi_result(use_stmt)) == SSA_NAME)
// 			{
// 				//debug_gimple_stmt(use_stmt);
// 				// debug_gimple_stmt(use_stmt);
// 				//debug_tree(gimple_phi_result(use_stmt));
// 				bool exist = false;
// 				FOR_EACH_USE_TABLE(used2, gc)
// 				{

// 					//debug_gimple_stmt(gc);
// 					if (gc == use_stmt)
// 					{
// 						exist = true;
// 						// fprintf(stderr, "----------------what r u loolking for------------------\n");
// 						// debug_tree(gimple_phi_result(use_stmt));
// 					}
// 				}
// 				if (!exist)
// 				{
// 					set_gimple_array(used_stmt, use_stmt, target, target2);
// 					// fprintf(stderr, "-----------------hiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii------------------\n");
// 					// debug_tree(gimple_phi_result(use_stmt));
// 					new_search_imm_use(used_stmt, gimple_phi_result(use_stmt), target2);
// 				}
// 			}
// 		}
// 		else
// 		{
// 			// fprintf(stderr, "-------always in therealways in therealways in there--------------------------\n");
// 			// debug_gimple_stmt(use_stmt);

// 			set_gimple_array(used_stmt, use_stmt, target, target2);
// 		}
// 		}
// 	}
// 	// fprintf(stderr, "-------always in therealways in therealways in there--------------------------\n");
// }
void printfPointerConstraint(ptb *ptable)
{
	gimple *u_stmt;
	gimple_array start;
	start.stmt = NULL;
	gimple_array *used_stmt = &start;
	ptb *table_temp = ptable;
	tree t;
	// table1 = ftable;
	gimple_array *user_tmp = used_stmt;
	FOR_EACH_TABLE(table_temp, t)
	{
		fprintf(stderr, "\n ------------------------------------------\n");
		if (table_temp->target != NULL)
		{
			debug_tree(table_temp->target);
			user_tmp = tvpt2->get(table_temp->target);
			fprintf(stderr, " \n Pointer to set  is size %d :[ \n", user_tmp->size);
			// fprintf(stderr,"%d \n",user_tmp->size);
			if (user_tmp->size > 0)
				FOR_EACH_USE_TABLE(user_tmp, u_stmt)
				{

					if (!strcmp(get_tree_code_name(TREE_CODE(table_temp->target)), "addr_expr"))
					{
						tree fndecl = TREE_OPERAND(table_temp->target, 0); //	debug_tree(table_temp->target);
																		   //    debug_tree(fndecl);
																		   // fprintf(stderr,"%s\n",	 get_tree_code_name(TREE_CODE(fndecl)));
																		   // fprintf(stderr,"%-16s in %s line %-4d column %-4d ", get_tree_code_name(TREE_CODE(fndecl)), DECL_SOURCE_FILE (fndecl), DECL_SOURCE_LINE (fndecl), DECL_SOURCE_COLUMN (fndecl));
																		   //  fprintf(stderr,"name: %s \n", DECL_NAME(fndecl) != NULL_TREE ? IDENTIFIER_POINTER(DECL_NAME(fndecl)) : "<unamed>");
					}
					// if(u_stmt != NULL && user_tmp->aptr!=NULL ){
					// try{
					// 	debug(user_tmp->aptr);
					// 	}catch(...){ 		break		 			}
					// debug_gimple_stmt(u_stmt);
					// if (ptr_derefs_may_alias_p(table_temp->target, user_tmp->target)) 				{
					// if(user_tmp->stmt != table_temp->last_stmt)
					debug(u_stmt);
					warning_at(gimple_location(u_stmt), 0, "use location");
					// warning_at(gimple_location(table_temp->last_stmt), 0, "use location");

					// fprintf(stderr, ", ");
					// }
				}
			fprintf(stderr, "] \n");
		}
	}
}

/*Pointer Constraint  5*/
void PointerConstraint(ptb *ptable, ptb *ftable)
{

	ptb *table1 = ptable;
	ptb *table3 = ftable;
	bool to_remove = false;
	tree t;

	struct ptr_info_def *pi1;
	struct pt_solution *pt1;
	struct ptr_info_def *pi2;
	struct pt_solution *pt2;
	fprintf(stderr, "start PointerConstraint\n");
	fprintf(stderr, "%d \n", ftable->size);
	fprintf(stderr, "%d \n", ptable->size);
	// if (ptable->size > ftable->size)
	// {
	// 	table1 = ftable;
	// 	table3 = ptable;
	// }
	gimple *u_stmt;
	gimple_array *used_stmt = NULL;
	if (ftable->size >= 0)
	{
		FOR_EACH_TABLE(table3, t)
		{
			if (table3->removed || !TREE_CODE(t) == SSA_NAME)
				continue;

			gimple_array start;
			start.stmt = NULL;
			used_stmt = &start;
			// pi = SSA_NAME_PTR_INFO(table3->target);
			// pt = &pi->pt;
			//if(pt->null)
			//continue;
			//fprintf(stderr,"---free_table-----\n");
			// debug_head(table3->target);

			// fprintf(stderr, "noewnoewnoewnoewnoew -----------------------------------------------\n");
			// debug_tree(table3->target);
			new_search_imm_use(used_stmt, table3->target, table3->target);
			// pi = SSA_NAME_PTR_INFO(table3->target);
			// pt = &pi->pt;
			// if (pt->vars == NULL) 				continue;
			FOR_EACH_USE_TABLE(used_stmt, u_stmt)
			{
				// if (gimple_code(used_stmt->stmt) == GIMPLE_PHI) 				continue;
				// if (gimple_call_builtin_p(used_stmt->stmt, BUILT_IN_FREE)) 				continue;

				// pi2 = SSA_NAME_PTR_INFO(used_stmt->target);
				// pt2 = &pi2->pt;
				// 		if (pt2->vars == NULL) 				continue;
				// if (!bitmap_intersect_p(pt1->vars, pt2->vars)){
				// 	tvpt2.erase (myvector.begin()+5);
				// }
				if (ptr_derefs_may_alias_p(table3->target, used_stmt->target))
				{
					// debug_tree(table3->target);
					// fprintf(stderr, " ----------fuckkkkkkkkkkkkkkkkkkkkkkkk----------------------------\n");
					// debug(u_stmt);
					// fprintf(stderr, ", ");
					gimple_array *user_tmp2;
					user_tmp2 = tvpt2->get(table3->target);
					if (user_tmp2 != NULL)
						break;
					;
					tvpt2->put(table3->target, *used_stmt);
					break;
				}

				//	debug(used_stmt->stmt);
				//	warning_at(gimple_location(used_stmt->stmt), 0, "use location<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
				// fprintf(stderr, "used_stmt is %d \n",count);
				//debug_gimple_stmt(u_stmt);
			}
		}
		// table1 = ptable;

		// warning_at(	used_stmt->size, 0, "Use after free error!: free location");
		// fprintf(stderr, "used_stmt is %d \n", used_stmt->size);

		// 			// warning_at(	used_stmt->size, 0, "Use after free error!: free location");

		// }
		//	 	if (gimple_code(used_stmt->stmt) == GIMPLE_PHI) 				continue; 			if (gimple_call_builtin_p(used_stmt->stmt, BUILT_IN_FREE)) 				continue; 			//fprintf(stderr,"-----  %d ~ %d \n",table3->bb->index,gimple_bb(used_stmt->stmt)->index); 			//fprintf(stderr,"free line : %d ,use line : %d \n",LOCATION_LINE((table3->loc)),LOCATION_LINE(gimple_location(used_stmt->stmt))); 			//fprintf(stderr,"free bb : %d ,use bb : %d \n",table3->bb->index,gimple_bb(used_stmt->stmt)->index); 			if (table3->bb->index == gimple_bb(used_stmt->stmt)->index) 			{ 				if (Location_b(table3->last_stmt, used_stmt->stmt, table3->bb)) 				{ //LOCATION_LINE((table3->loc))<=LOCATION_LINE(gimple_location(used_stmt->stmt)) 					//warning_at(table3->loc, 0, ""); 					//debug_head(table3->target); 					//fprintf(stderr,"--index : %d %d---\n",table3->bb->index,gimple_bb(used_stmt->stmt)->index); 					if (!table3->removed) 					{ 						warning_at(table3->loc, 0, "Use after free error!: free location"); 						warning_at(gimple_location(used_stmt->stmt), 0, "use location"); ///ppp 																						 //fprintf(fp,"%s use after free\n",gimple_filename (table3->last_stmt)); 																						 //remove_pointer_stmt(table3->last_stmt); 																						 //table3->removed=true; 					} 				} 			} 			else if (fDFS->get(table3->node)->is_succ(table3->bb, gimple_bb(used_stmt->stmt))) 			{ 				//fprintf(stderr,"----- is_succ\n"); 				if (!table3->removed) 				{ 					warning_at(table3->loc, 0, "Use after free error!: free location"); 					warning_at(gimple_location(used_stmt->stmt), 0, "use location"); 					//fprintf(fp,"%s use after free \n",DECL_SOURCE_FILE (table3->fun->decl)); 					//remove_pointer_stmt(table3->last_stmt); 					//table3->removed=true; 				} 			} 		}
		// //pointer1
		// gimple *u_stmt;
		// //fprintf(stderr,"=====free: ");
		// //debug_head(table3->target);
		// FOR_EACH_USE_TABLE(used_stmt, u_stmt)
		// {

		// 	//debug_gimple_stmt(u_stmt);
		// 	if (gimple_code(used_stmt->stmt) == GIMPLE_PHI)
		// 		continue;
		// 	if (gimple_call_builtin_p(used_stmt->stmt, BUILT_IN_FREE))
		// 		continue;
		// 	//fprintf(stderr,"-----  %d ~ %d \n",table3->bb->index,gimple_bb(used_stmt->stmt)->index);
		// 	//fprintf(stderr,"free line : %d ,use line : %d \n",LOCATION_LINE((table3->loc)),LOCATION_LINE(gimple_location(used_stmt->stmt)));
		// 	//fprintf(stderr,"free bb : %d ,use bb : %d \n",table3->bb->index,gimple_bb(used_stmt->stmt)->index);
		// 	if (table3->bb->index == gimple_bb(used_stmt->stmt)->index)
		// 	{
		// 		if (Location_b(table3->last_stmt, used_stmt->stmt, table3->bb))
		// 		{ //LOCATION_LINE((table3->loc))<=LOCATION_LINE(gimple_location(used_stmt->stmt))
		// 			//warning_at(table3->loc, 0, "");
		// 			//debug_head(table3->target);
		// 			//fprintf(stderr,"--index : %d %d---\n",table3->bb->index,gimple_bb(used_stmt->stmt)->index);
		// 			if (!table3->removed)
		// 			{
		// 				warning_at(table3->loc, 0, "Use after free error!: free location");
		// 				warning_at(gimple_location(used_stmt->stmt), 0, "use location"); ///ppp
		// 																				 //fprintf(fp,"%s use after free\n",gimple_filename (table3->last_stmt));
		// 																				 //remove_pointer_stmt(table3->last_stmt);
		// 																				 //table3->removed=true;
		// 			}
		// 		}
		// 	}
		// 	else if (fDFS->get(table3->node)->is_succ(table3->bb, gimple_bb(used_stmt->stmt)))
		// 	{
		// 		//fprintf(stderr,"----- is_succ\n");
		// 		if (!table3->removed)
		// 		{
		// 			warning_at(table3->loc, 0, "Use after free error!: free location");
		// 			warning_at(gimple_location(used_stmt->stmt), 0, "use location");
		// 			//fprintf(fp,"%s use after free \n",DECL_SOURCE_FILE (table3->fun->decl));
		// 			//remove_pointer_stmt(table3->last_stmt);
		// 			//table3->removed=true;
		// 		}
		// 	}
		// }
		//fprintf(stderr,"=====use table end \n");
	}

	table3 = ptable;
	gimple_array *used_stmt2 = used_stmt;

	FOR_EACH_TABLE(table3, t)
	{
		if (table3->removed || !TREE_CODE(t) == SSA_NAME)
			continue;
		// pi1 = SSA_NAME_PTR_INFO(table3->target);
		// pt1 = &pi1->pt;
		// used_stmt = used_stmt2;
		//  debug(table3->target);
		// fprintf(stderr, "\n Pointer to set  is :[ \n");
	}
	tree t2;
	if (ptable->size >= 0)
	{
		FOR_EACH_TABLE(table1, t2)
		{
			if (table1->removed || !TREE_CODE(t) == SSA_NAME)
				continue;
			gimple_array start2;
			start2.stmt = NULL;
			used_stmt = &start2;
			// fprintf(stderr, "noewnoewnoewnoewnoew222222222222222 -----------------------------------------------\n");
			new_search_imm_use(used_stmt, table1->target, table1->target);
			// pi1 = SSA_NAME_PTR_INFO(table1->target);
			// pt1 = &pi1->pt;
			// used_stmt = *used_stmt2;
			//  debug(table3->target);
			// fprintf(stderr, "\n Pointer to set  is :[ \n");

			FOR_EACH_USE_TABLE(used_stmt, u_stmt)
			{
				// if (gimple_code(used_stmt->stmt) == GIMPLE_PHI) 				continue;
				// if (gimple_call_builtin_p(used_stmt->stmt, BUILT_IN_FREE)) 				continue;

				// pi2 = SSA_NAME_PTR_INFO(used_stmt->target);
				// pt2 = &pi2->pt;
				if (ptr_derefs_may_alias_p(table1->target, used_stmt->target))
				{

					// debug_tree(table3->target);
					// debug(u_stmt);
					// fprintf(stderr, ", ");
					gimple_array *user_tmp2;
					user_tmp2 = tvpt2->get(table1->target);
					if (user_tmp2 != NULL)
						break;
					;
					tvpt2->put(table1->target, *used_stmt);
					break;
				}

				//	debug(used_stmt->stmt);
				//	warning_at(gimple_location(used_stmt->stmt), 0, "use location<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
				// fprintf(stderr, "used_stmt is %d \n",count);
				//debug_gimple_stmt(u_stmt);
			}
		}
		// debug_head(table1->target); 			if (TREE_CODE(table1->target) != SSA_NAME) 			{ 				continue; 			} 			if (ptr_derefs_may_alias_p(table3->target, table1->target)) 			{ 				fprintf(stderr, "testing-----------------------------------------------\n"); 				warning_at(table3->loc, 0, "Use after free error!: free location"); 				warning_at(table1->loc, 0, "Use after free error!: free location"); 				//warning_at(gimple_location(used_stmt->stmt), 0, "use location"); ///ppp 				debug_tree(table1->target); 				debug_tree(table3->target); 				fprintf(stderr, "testing-----------------------------------------------\n"); 				new_search_imm_use(used_stmt, table1->target, table1->target); 			} 		}
		// table1 = ftable;
		// gimple_array *used_stmt2 = used_stmt;
		// FOR_EACH_TABLE(table1, t)
		// {
		// 	if (table1->removed || !TREE_CODE(t) == SSA_NAME)
		// 		continue;
		// }
	}
	// fprintf(stderr, "] \n");
	// printfPointerConstraint(ptable);
	// printfPointerConstraint(ftable);

	// table1 = ftable;
	// gimple_array *user_tmp = used_stmt;
	// FOR_EACH_TABLE(table1, t)
	// {
	// 	fprintf(stderr, "\n ------------------------------------------\n");
	// 	debug_tree(table1->target);
	// 	fprintf(stderr, " \n Pointer to set  is :[ \n");
	// 	user_tmp = tvpt2->get(table1->target);
	// 	FOR_EACH_USE_TABLE(user_tmp, u_stmt)
	// 	{
	// 		debug(u_stmt);
	// 		fprintf(stderr, ", ");
	// 	}
	// 	fprintf(stderr, "] \n");
	// }

	// FOR_EACH_USE_TABLE(used_stmt, u_stmt)
	// {
	// 	// if (gimple_code(used_stmt->stmt) == GIMPLE_PHI) 				continue;
	// 	// if (gimple_call_builtin_p(used_stmt->stmt, BUILT_IN_FREE)) 				continue;
	// 	debug(u_stmt);
	// 	debug(used_stmt->target);
	// 	// debug(u_stmt);

	// 	//	debug(used_stmt->stmt);
	// 	//	warning_at(gimple_location(used_stmt->stmt), 0, "use location<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
	// 	// fprintf(stderr, "used_stmt is %d \n",count);
	// 	//debug_gimple_stmt(u_stmt);
	// }
}

void new_memory_leak_analysis(ptb *ptable, ptb *ftable)
{
	fprintf(stderr, "start memory_leak_analysis\n");
	ptb *table1 = ptable;
	ptb *table3 = ftable;
	tree t, t2;
	int may = 0;
	int must = 0;
	int total = 0;
	struct ptr_info_def *pi;
	struct pt_solution *pt;

	struct ptr_info_def *pi1;
	struct pt_solution *pt1;

	struct ptr_info_def *pi2;
	struct pt_solution *pt2;

	struct ptr_info_def *pi3;
	struct pt_solution *pt3;
	//phase1 check every alloc pair with one free
	FOR_EACH_TABLE(table1, t)
	{
		if (TREE_CODE(TREE_TYPE(t)) == METHOD_TYPE || TREE_CODE(TREE_TYPE(t)) == FUNCTION_TYPE || TREE_CODE(TREE_TYPE(t)) == RECORD_TYPE || !(TREE_CODE(t) == SSA_NAME))
		{
			continue;
		}
		//debug_head(t);
		fprintf(stderr, "=======ftable start========\n");
		pi1 = SSA_NAME_PTR_INFO(table1->target);
		pt1 = &pi1->pt;
		debug(pt1);
		debug_tree(table1->target);
		fprintf(stderr, "=======ftable end========\n");
		//if(MAIN_NAME_P(DECL_NAME (table1->node->decl)))
		if (pt1 == NULL)
			continue;
		/*Algorithm 6*/
		FOR_EACH_TABLE(table3, t2)
		{
			if (table3->removed)
			{
				continue;
			}
			if (t2 == NULL)
			{
				break;
			}
			if (TREE_CODE(table3->target) != SSA_NAME)
			{
				continue;
			}
			// fprintf(stderr,"=======ftable2 before========\n");
			pi2 = SSA_NAME_PTR_INFO(table3->target);
			pt2 = &pi2->pt;

			// debug(pt2);
			// fprintf(stderr,"=======ftable2 before========\n");
			//if(pt->null)
			//continue;
			//if(table1->state == POINTER_STATE_IS_FREE)
			//	warning_at(table1->loc, 0,"Memory Leak error");

			if (pt2->vars == NULL)
				continue;
			if (bitmap_intersect_p(pt1->vars, pt2->vars))
			{
				if (bb_in_loop_p(table1->bb) && !bb_in_same_loop_alloc_free(table1->bb, table3->bb))
				{
					warning_at(table1->loc, 1, "May Memory-Leak Error!: This alloc statement is in a loop which may cause a memory-leak error.");
					break;
				}

				//warning_at(table1->loc,0,"");
				//fprintf(stderr,"malloc ");debug(pt1);
				//debug_head(t);

				//warning_at(table3->loc,0,"");
				//fprintf(stderr,"free ");debug(pt2);
				//debug_head(t2);

				//fprintf(stderr,"\n\n");

				gimple *def_stmt = SSA_NAME_DEF_STMT(table3->target);
				//debug_tree(table3->target);
				if (table3->inbranch)
				{
					if (table1->state != POINTER_STATE_IS_FREE)
					{
						// fprintf(stderr,"inbranch99999999999999999999999999999999999999999999999999999999 \n");
						table1->state = POINTER_STATE_MAY_IS_FREE;
					}
				}
				else
				{
					debug_gimple_stmt(def_stmt);
					fprintf(stderr, "coooooooooooooooooooooooooooooooooooool \n");
					if (gimple_code(def_stmt) == GIMPLE_PHI)
					{
						for (unsigned i = 0; i < gimple_phi_num_args(def_stmt); i++)
						{
							tree argu = gimple_phi_arg_def(def_stmt, i);
							//fprintf(stderr,"phi_argu:%d ",i);
							//debug_tree(argu);
							pi3 = get_ptr_info(argu);
							pt3 = &pi3->pt;
							if (pt3->vars == NULL)
								continue;
							if (!bitmap_intersect_p(pt2->vars, pt3->vars))
							{
								table1->state = POINTER_STATE_MAY_IS_FREE;
							}
						}
					}
					else
					{
						table1->state = POINTER_STATE_IS_FREE;
					}
				}
			}
		}
		if (table1->state != POINTER_STATE_IS_FREE)
		{
			if (table1->state != POINTER_STATE_MAY_IS_FREE)
			{
				warning_at(table1->loc, 0, "Memory Leak Error!");
				must++;
				//fprintf(fp,"%s Memory Leak Error \n",DECL_SOURCE_FILE (table1->fun->decl)); //ppp
				//add_free_stmt(table1, table1->target);
				table1->state = POINTER_STATE_IS_FREE;
			}
			else
			{
				warning_at(table1->loc, 0, "May Memory Leak Error!");
				may++;
				//fprintf(fp,"%s Memory Leak Error \n",DECL_SOURCE_FILE (table1->fun->decl));
				table1->state = POINTER_STATE_IS_FREE;
			}
		}
		//fprintf(fp,"malloc: %d ",table1->loc);
		total++;
		table3 = ftable;
	} //phase1 end
	table1 = ptable;
	table3 = ftable;
	if (ptable->target != NULL)
	{
		//fprintf(fp,"%s Memory Leak Error \n",DECL_SOURCE_FILE (table1->fun->decl));
		//fprintf(fp,"%s Memory Leak Error \n		Total:%d\n		May:%d\n		Must:%d\n",DECL_SOURCE_FILE (ptable->fun->decl),total,may,must);
	}
	else
	{
		//fprintf(fp,"Memory Leak Error \n		Total:%d\n		May:%d\n		Must:%d\n",total,may,must);
	}
}

void new_double_free_analysis(ptb *ptable, ptb *ftable)
{

	fprintf(stderr, "start double_free_analysis\n");
	ptb *malloc_t = ftable;
	ptb *free_t = ftable;
	ptb *free3_t = ftable;
	ptb *free2_t;
	tree t, t2, t3;

	int may = 0;
	int must = 0;
	int total = 0;
	gimple_array start;
	start.stmt = NULL;
	gimple_array *used_stmt = &start;
	struct ptr_info_def *pi;
	struct pt_solution *pt;
	struct ptr_info_def *pi2;
	struct pt_solution *pt2;
	//search_imm_use(used_stmt, malloc_t->target);
	FOR_EACH_TABLE(free_t, t)
	{
		//debug_head(t);
		total++;
		if (TREE_CODE(free_t->target) != SSA_NAME)
		{
			if (free_t->next != NULL)
			{
				continue;
			}
			else
			{
				break;
			}
		}
		fprintf(stderr, "=======ftable before========\n");
		pi = SSA_NAME_PTR_INFO(free_t->target);
		pt = &pi->pt;
		// debug(pt);
		dump_points_to_solution(stderr, pt);
		fprintf(stderr, "=======ftable before========\n");
		if (pt->vars == NULL)
		{

			fprintf(stderr, "points to is null\n");
			continue;
		}
		if (free_t->bb->loop_father->header->index != 0)
		{
			warning_at(free_t->loc, 0, "May Double Free Error!: This free statement is in a loop which may cause a double-free error.");
			may++;
			//fprintf(fp,"%s May Double Free Error: This free statement is in a loop which may cause a double-free error.\n",DECL_SOURCE_FILE (main_fun->decl));
			//continue;
		}
		//fprintf(stderr,"=======loop after========\n");
		if (free_t->next != NULL)
		{
			//fprintf(stderr,"=======free2_t=free_t->next========\n");
			free2_t = free_t->next;
		}
		else
		{
			//fprintf(stderr,"=======break========\n");
			break;
		}
		//fprintf(stderr,"=======ftable2 before========\n");
		FOR_EACH_TABLE(free2_t, t2)
		{
			//debug_head(t2);
			if (TREE_CODE(free2_t->target) != SSA_NAME)
			{
				if (free2_t->next != NULL)
				{
					continue;
				}
				else
				{
					break;
				}
			}
			pi2 = SSA_NAME_PTR_INFO(free2_t->target);
			pt2 = &pi2->pt;
			fprintf(stderr, "=======ftable2 before========\n");
			// debug(pt2);
			dump_points_to_solution(stderr, pt2);
			fprintf(stderr, "=======ftable2 before========\n");
			//if(pt->null)
			//continue;
			unsigned int errorm = POINTER_NOT;
			if (free_t->bb->loop_father->header->index != 0)
			{
				//fprintf(stderr,"bb_loop_header_p is exist\n");
				//warning_at(free_t->loc,0,"May Double-Free Error: This free statement is in a loop which may cause a double-free error.");
			}

			//fprintf(stderr,"	=======search ftable2========\n");
			errorm = POINTER_NOT;
			if (bitmap_intersect_p(pt->vars, pt2->vars))
			{
				//fprintf(stderr,"	=======bitmap_intersect_p========\n");

				if (free_t->fun == free2_t->fun)
				{
					push_cfun(free_t->fun);
					calculate_dominance_info(CDI_DOMINATORS);
					dominated_by_p(CDI_DOMINATORS, free2_t->bb, free_t->bb);
					dominated_by_p(CDI_DOMINATORS, free_t->bb, free_t->fun->cfg->x_exit_block_ptr->prev_bb);
					// if (dominated_by_p(CDI_DOMINATORS,free_t->bb,free_t->fun->cfg->x_exit_block_ptr->prev_bb) ^ dominated_by_p(CDI_DOMINATORS,free2_t->bb,free2_t->fun->cfg->x_exit_block_ptr->prev_bb)){
					// 	errorm=POINTER_MAY;
					// }
					// else if (dominated_by_p(CDI_DOMINATORS,free_t->bb,free_t->fun->cfg->x_exit_block_ptr->prev_bb) && dominated_by_p(CDI_DOMINATORS,free2_t->bb,free2_t->fun->cfg->x_exit_block_ptr->prev_bb)){
					// 	errorm=POINTER_MUST;
					// }
					// else if (!dominated_by_p(CDI_DOMINATORS,free_t->bb,free_t->fun->cfg->x_exit_block_ptr->prev_bb) && !dominated_by_p(CDI_DOMINATORS,free2_t->bb,free2_t->fun->cfg->x_exit_block_ptr->prev_bb)){

					// 	if(fDFS->get(free_t->node)->is_succ(free_t->bb,free2_t->bb)){
					// 		errorm=POINTER_MAY;
					// 	}
					// }
					free_dominance_info(CDI_DOMINATORS);
					pop_cfun();
					fprintf(stderr, "do interprocedural allocation check		=======search ptable %d %d========\n", free_t->fun, free2_t->fun);

					if (ipa)
					{

						FOR_EACH_TABLE(malloc_t, t3)
						{

							if (ptr_derefs_may_alias_p(free_t->target, malloc_t->target))
							{
								fprintf(stderr, "klkkkkkkkkkkkkkkkkkkkkkkkkk\n");
								if (free_t->node->get_fun() != malloc_t->node->get_fun())
								{

									//fprintf(stderr,"			=======free:");
									//debug_head(free_t->target);
									//fprintf(stderr,"			=======malloc:");
									//debug_head(malloc_t->target);
									//fprintf(stderr,"			=======search_all_path ========\n");
									search_all_path(free_t, malloc_t);
									//fprintf(stderr,"			=======print_tvpt %d========\n",free_t->target);
									//debug_head(free_t->target);
									print_tvpt(free_t->target);
									//fprintf(stderr,"			=======print_tvpt %d========\n",free2_t->target);
									//debug_head(free2_t->target);
									print_tvpt(free2_t->target);
									if (!same_path(free_t->target, free2_t->target))
									{
										errorm = POINTER_NOT; //problem
									}
								}
							}
						}
					}
					malloc_t = ptable;
					//fprintf(stderr,"		=======search ptable end %d========\n",errorm);

					//fprintf(stderr,"	=======search ftable2 end========\n");
				}
				else
				{
					errorm = POINTER_MAY;
				}
				switch (errorm)
				{
				case POINTER_MUST:
					warning_at(free_t->loc, 0, "Double free error!");
					warning_at(free2_t->loc, 0, "");
					//fprintf(fp,"%s Double-free must\n",DECL_SOURCE_FILE (main_fun->decl));

					must++;
					break;
				case POINTER_MAY:
					warning_at(free_t->loc, 0, "May Double free error!");
					warning_at(free2_t->loc, 0, "");
					//fprintf(fp,"%s Double-free may\n",DECL_SOURCE_FILE (main_fun->decl));
					may++;
					break;
				default:
					break;
				}
				FOR_EACH_TABLE(malloc_t, t3)
				{
					pi2 = SSA_NAME_PTR_INFO(malloc_t->target);
					pt2 = &pi2->pt;
					if (bitmap_intersect_p(pt->vars, pt2->vars))
					{
						//warning_at(malloc_t->loc,0,"malloc location \n");
					}
				}
				malloc_t = ptable;
				/*
				fprintf(stderr,"\n%d:%d\n",free_t->bb->index,free2_t->bb->index);
				warning_at(free_t->loc,0,"loc: %d",free_t->bb->index);
				warning_at(free2_t->loc,0,"loc: %d",free2_t->bb->index);
				if (dominated_by_p(CDI_DOMINATORS,free2_t->bb,free_t->bb)){
					if()
					warning_at(free_t->loc,0,"Double-free %d",free_t->bb->index);
					warning_at(free2_t->loc,0,"Double-free %d",free2_t->bb->index);
					debug_head(free_t->target);
					struct pt_solution *pt=&pi1->pt;
					//debug(&pi1->pt);
					FILE *file;
					dump_decl_set (stderr,pt->vars);
					fprintf(stderr,"\n");
					debug_head(free2_t->target);
					debug(&pi2->pt);
					fprintf(stderr,"\n");
					if(pt_solutions_intersect (&pi1->pt,&pi2->pt)){
						
						gimple_array *used_stmt;
						gimple_array gstart;
	      					gstart.stmt = NULL;
	      					used_stmt = &gstart;
	      					//search_ptr_use(used_stmt, free_t->target);
						fprintf(stderr,"~.~\n");
	      					//search_ptr_use(used_stmt, free2_t->target);
					}
					fprintf(stderr,"----------------\n");
					break;
				}
				if(et_below(n2,n1)){
					warning_at(free_t->loc,0,"May Double-free %d",free_t->bb->index);
					warning_at(free2_t->loc,0,"May Double-free %d",free2_t->bb->index);
				}
				if(g.is_succ(free_t->bb,free2_t->bb)){
					warning_at(free_t->loc,0,"May Double-free %d",free_t->bb->index);
					warning_at(free2_t->loc,0,"May Double-free %d",free2_t->bb->index);
				}
				*/
			}
		}
	}
	if (main_fun != NULL)
	{
		//fprintf(fp,"%s Memory Leak Error \n",DECL_SOURCE_FILE (table1->fun->decl));
		//fprintf(fp,"%s Double free Error \n		Total free:%d\n		May df:%d\n		Must df:%d\n",DECL_SOURCE_FILE (main_fun->decl),total,may,must);
	}
	else
	{
		//fprintf(fp,"Double free Error \n		Total free:%d\n		May df:%d\n		Must df:%d\n",total,may,must);
	}
}

void collect_malloc(gimple *gc, cgraph_node *node, basic_block bb)
{
	location_t loc = gimple_location(gc);
	tree a;
	cgraph_node *node2;
	const char *name;

	if (gimple_call_fn(gc) == NULL)
		return;

	name = get_name(gimple_call_fn(gc));
	if (!strcmp(name, "free") || !strcmp(name, "xfree"))
	{
		set_ptb(bb, ftable, gimple_call_arg(gc, 0), loc, 0, gc, node);
	}
	else if (!strcmp(name, "malloc") || !strcmp(get_name(gimple_call_fn(gc)), "calloc") || !strcmp(name, "xmalloc") || !strcmp(name, "strdup"))
	{
		set_ptb(bb, ptable, gimple_call_lhs(gc), loc, 0, gc, node);
	}
	// a = gimple_call_fn(gc);
	// fnode->put(a, node);

	// var_points_to vpt;
	// vector<vector<pair<fdecl, location_t>>> may_malloc_path;
	// vpt.may_malloc_path = may_malloc_path;
	// tvpt->put(gimple_call_lhs(gc), vpt);
}

// void ompfucntio()
// {
// #pragma omp parallel
// 	{
// 		fprintf(stderr, "plugin run\n");
// 	}
// 	int i;
// 	int n = 10;
// 	double s = 0.0;
// 	omp_set_num_threads(4);
// #pragma omp parallel for reduction(+ \
// 								   : s)
// 	for (i = 0; i < n; ++i)
// 	{
// 		fprintf(stderr, "threads %d\n", omp_get_thread_num());
// 		// printf("threads %d\n", omp_get_thread_num());
// 		s += i;
// 	}
// 	fprintf(stderr, "%f\n", s);
// }

void detect()
{

	struct rusage ru;
	struct timeval utime;
	struct timeval stime;
	struct cgraph_node *node;
	struct var_points_to vpt;
	const int N = 1e2;
	tree ptr;
	unsigned i;
	function *ofun;
	function *fn;
	basic_block bb;
	ipa = true;
	tvpt = new hash_map<tree, var_points_to>;
	tvpt2 = new hash_map<tree, gimple_array>;
	fDFS = new hash_map<cgraph_node *, Graph>;
	fnode = new hash_map<tree, cgraph_node *>;
	fprintf(stderr, "=======ipa_pta=========\n");
	//ompfucntio();
	/*initialization table*/
	init_table();
	// omp_set_num_threads(4);
	double s = 0.0;
	clock_t start, end;
	start = clock();
	getrusage(RUSAGE_SELF, &ru);
	FOR_EACH_DEFINED_FUNCTION(node)
	{
		if (!ipa)
			init_table();
		push_cfun(node->get_fun());
		if (cfun == NULL)
			continue;
		if (strcmp(get_name(cfun->decl), "main") == 0)
			main_fun = cfun;
		//fprintf(stderr,"=======node_fun:%s=========\n",get_name(cfun->decl));

		/*initialization DFS graph*/
		Graph DFS;
		DFS.init_Graph(cfun->cfg->x_last_basic_block);
		/*calculate dominator tree*/
		calculate_dominance_info(CDI_DOMINATORS);

		/*create DFS graph, Algorithm 1 and 2*/
		FOR_EACH_BB_FN(bb, cfun)
		{
			if (bb != cfun->cfg->x_exit_block_ptr->prev_bb)
			{
				edge e;
				edge_iterator ei;
				//fprintf(stderr,"node:= %d \n succs:=",bb->index);
				FOR_EACH_EDGE(e, ei, bb->succs)
				{
					DFS.addEdge(bb->index, e->dest->index);
					//fprintf(stderr,"%d",e->dest->index);
				}
			}
			for (gimple_stmt_iterator gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi))
			{
				gimple *gc = gsi_stmt(gsi);
				if (is_gimple_call(gc))
				{

					/*collect malloc and free information*/
					collect_malloc(gc, node, bb);
					// fprintf(stderr, "add collect_malloc\n");
				}
			}
		}
		/*DFS of this function put set "fDfS" */
		fDFS->put(node, DFS);
		if (!ipa)
		{
			// new_memory_leak_analysis (ptable,ftable);
			// new_double_free_analysis(ptable,ftable);
			// new_use_after_free_analysis(ptable, ftable);
		}
		pop_cfun();
	}

	if (ipa)
	{

		// new_memory_leak_analysis(ptable, ftable);
		PointerConstraint(ptable, ftable);
		// new_double_free_analysis(ptable,ftable);
		// new_use_after_free_analysis(ptable, ftable);
	}
	end = clock();
	//printf("time: %f s\n", ((double)(end - start)) / CLOCKS_PER_SEC / N); 	// fprintf(stderr,"time: %f s\n",((double)(end - start)) / CLOCKS_PER_SEC / N);
	fprintf(stderr, "time: %f s\n", ((double)(end - start)) / CLOCKS_PER_SEC);
	utime = ru.ru_utime;
	stime = ru.ru_stime;

	double utime_used = utime.tv_sec + (double)utime.tv_usec / 1000000.0;
	double stime_used = stime.tv_sec + (double)stime.tv_usec / 1000000.0;
	ofstream myfile("example.txt");
	if (myfile.is_open())
	{ //
		myfile << "utime_used: " << utime_used << "s;\n";
		myfile << "stime_used: " << stime_used << "s;\n";

		myfile.close();
	}
	else
		cout << "Unable to open file";
}
void insert_always_inline()
{
	cgraph_node *node;
	const char *name;
	bool always_inline;
	//fprintf(stderr,"=======inline=========\n");

	FOR_EACH_DEFINED_FUNCTION(node)
	{
		basic_block bb;
		cgraph_edge *e;

		//fprintf(stderr,"=======node_fun:%s %d=========\n",get_name(node->decl),node->decl);
		//fprintf(stderr,"attribute:%s \n",IDENTIFIER_POINTER (node->decl));

		//fprintf(stderr,"attribute:%s \n",IDENTIFIER_POINTER (DECL_ASSEMBLER_NAME (node->decl)));
		//node->debug();
		//fprintf(stderr,"=======fun:%d=========\n",node->decl);
		tree attr;
		enum availability avail;
		for (e = node->callees; e; e = e->next_callee)
		{
			cgraph_node *caller = e->caller->global.inlined_to ? e->caller->global.inlined_to : e->caller;
			cgraph_node *callee = e->callee->ultimate_alias_target(&avail, caller);
			tree callee_tree = callee ? DECL_FUNCTION_SPECIFIC_OPTIMIZATION(callee->decl) : NULL;
			//DECL_DISREGARD_INLINE_LIMITS (callee->decl)=1;
			if (DECL_ATTRIBUTES(callee->decl) != NULL)
			{
				attr = get_attribute_name(DECL_ATTRIBUTES(callee->decl));
				//debug_tree(attr);
			}
			else
			{
				//if(callee->decl)
				//DECL_ATTRIBUTES (callee->decl) = tree_cons (get_identifier ("always_inline"),
				//NULL, DECL_ATTRIBUTES (callee->decl));
			}
			always_inline = (DECL_DISREGARD_INLINE_LIMITS(callee->decl) && lookup_attribute("always_inline", DECL_ATTRIBUTES(callee->decl)));
			//fprintf(stderr,"=======%s 's address:%d %d=========\n",get_name(callee->decl),callee->decl,always_inline);
		}

		//node->debug();
		push_cfun(DECL_STRUCT_FUNCTION(node->decl));
		if (cfun == NULL)
		{
			//fprintf(stderr,"=======NULL=========\n");
		}
		FOR_EACH_BB_FN(bb, cfun)
		{

			for (gimple_stmt_iterator gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi))
			{
				gimple *gc = gsi_stmt(gsi);
				//debug_gimple_stmt(gc);
				if (is_gimple_call(gc))
				{

					if (gimple_call_fn(gc) == NULL)
						continue;
					name = get_name(gimple_call_fn(gc));
					if (name == NULL)
						continue;
					//fprintf(stderr, "functionname %s\n",name);
					if (!strcmp(name, "free") ||
						!strcmp(name, "xfree") ||
						!strcmp(name, "malloc") ||
						!strcmp(name, "xmalloc") ||
						!strcmp(name, "calloc") ||
						!strcmp(name, "xcalloc") ||
						!strcmp(name, "strdup"))
					{

						//fprintf(stderr,"=======add_always:%d=========\n",node->decl);
						always_inline = (DECL_DISREGARD_INLINE_LIMITS(node->decl) && lookup_attribute("always_inline", DECL_ATTRIBUTES(node->decl)));
						if (!always_inline && !MAIN_NAME_P(DECL_NAME(node->decl)))
						{
							DECL_ATTRIBUTES(node->decl) = tree_cons(get_identifier("always_inline"), NULL, DECL_ATTRIBUTES(node->decl));
							DECL_DISREGARD_INLINE_LIMITS(node->decl) = 1;
							//printf("always_inline\n");
						}
					}
				}
			}
		}
		pop_cfun();
	}
}