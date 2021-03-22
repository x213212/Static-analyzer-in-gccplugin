#include "system.h"
#include "config.h"
#include <coretypes.h>
#include "insn-constants.h"
#include <pthread.h>
#include <unistd.h>
//#include "config/i386/i386.h"
#include "is-a.h"
#include "options.h"
#include "backend.h"
#include "rtl.h"
#include "tree.h"
#include "tree-pass.h"
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
#include "intl.h"
#include "opts.h"
#include "ssa.h"
#include "alloc-pool.h"
#include "tree-pass.h"
#include "context.h"
#include "tree.h"
#include "tree-iterator.h"
#include <iostream>
#include <stack>
using namespace std;
// #include "ipa-inline.h"
// #include "symbol-summary.h"
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

/*ptb declaration*/
ptb start1, start2, start3, start4, start5, start6, start7;
ptb *ptable, *retable, *ftable;
ptb *phitable, *return_table, *use_table, *fopen_table;
ptb *locktable;
ptb *unlocktable;


struct gimple_array
{
	gimple *stmt;
	gimple *ret_stmt;
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

/*

int return_type
return type
normal 0
pointer 0
function pointer
*/
struct function_path
{
	//gimple *stmt;
	//n/p treee
	cgraph_node *cgnext;
	tree prev;
	tree next;

	//int return_type;
};
struct return_type
{
	gimple *stmt;
	tree return_tree;

	//int return_type;
};
struct assign_type
{
	gimple *stmt;
	tree assign_tree;
	// int reutnr_type_num = 0;
	//int return_type;
};

struct free_type
{
	gimple *stmt;
	tree free_tree;
	// int reutnr_type_num = 0;
	//int return_type;
};


/*define return_type struct*/
struct function_return_array
{
	int return_type_num = 0;

	vector<return_type> return_type_array;
};

/*define return_type struct*/
struct function_path_array
{
	int funtion_type_num = 0;
	vector<function_path> function_path_array;
};

/*define assign_type struct*/
struct function_assign_array
{
	vector<assign_type> assign_type_array;
};

/*define free_type struct*/
struct function_free_array
{
	vector<free_type> free_type_array;
};

struct function_graph_array 
{
	Graph graph_type_array;
	cgraph_node *  graph_node ;
};
/*collect function return types */
hash_map<tree, function_return_array> *function_return_collect;
/*collect function var decl ssa name */
hash_map<tree, function_assign_array> *function_assign_collect;
/*collect function path */
hash_map<tree, function_path_array> *function_path_collect;
/*collect function free */
hash_map<tree, function_free_array> *function_free_collect;
/*collect fucntion graph_array*/
hash_map<tree, function_graph_array> *function_graph_collect;

/*record each DFS graph*/
hash_map<cgraph_node *, Graph> *fDFS;

/*use function tree search cgraph_node*/
hash_map<tree, cgraph_node *> *fnode;

/*rhs function search function decl and location*/
hash_map<tree, var_points_to> *tvpt;
hash_map<tree, gimple_array> *treeGimpleArray;

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
class CpathStack : public std::stack<tree>
{
public:
	using std::stack<tree>::c; // expose the container
};
CpathStack pathStack;
//stack<tree> pathStack;  // 建立堆疊
function *main_fun;
void dump_fucntion(cgraph_node *node, ptb *ptable, gimple_array *user_tmp);
void tracefree_fucntion(cgraph_node *node, ptb *ptable, gimple_array *user_tmp);
void record_fucntion(cgraph_node *node);
void printfBasicblock();
void print_function_return(tree function_tree);
void print_function_return2(tree function_tree);
void collect_FunctionMapping_Assign(gimple *gc, cgraph_node *node, basic_block bb);

void collect_FunctionMapping_Ret(tree function_tree, gimple *u_stmt, gimple_array *user_tmp, ptb *table_temp, ptb *ptable);
function_assign_array ret_function_varstmt(tree function_tree);
bool bb_in_branch_p(ptb *table)
{
	return !dominated_by_p(CDI_DOMINATORS, table->fun->cfg->x_exit_block_ptr->prev_bb, table->bb);
}

bool bb_in_branch_p(gimple *stmt)
{
	//function* fn = DECL_STRUCT_FUNCTION(gimple_get_lhs(stmt));
	// fprintf(stderr, "backkkkkkkbackkkkkkkbackkkkkkkbackkkkkkkbackkkkkkkbackkkkkkk\n");
	// debug_gimple_stmt(stmt);
	// debug_bb(stmt->bb);
	// fprintf(stderr, "prev_bbprev_bbprev_bbprev_bbprev_bbprev_bbprev_bbprev_bb\n");
	// debug_bb(cfun->cfg->x_exit_block_ptr->prev_bb);
	// debug_bb(cfun->cfg->x_entry_block_ptr->next_bb);
	// debug_bb(stmt->bb);
	// fprintf(stderr," %d\n",dominated_by_p(CDI_DOMINATORS,stmt->bb,cfun->cfg->x_exit_block_ptr->prev_bb));

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
		bool same = false;
		while (table->next != NULL)
		{
			if (table->target == t)
			// if (table->stmt == used_stmt && LOCATION_LINE(gimple_location(table->stmt)) == LOCATION_LINE(gimple_location(used_stmt)))
			{

				same = true;
				break;
			}
			table = table->next;
		}
		if (!same)
		{
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
}

void set_gimple_array(gimple_array *table, gimple *used_stmt, tree a, tree target, gimple *ret_stmt)
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
	// debug_tree(a);

	if (table->stmt == NULL)
	{
		table->stmt = used_stmt;
		table->ret_stmt = ret_stmt;
		table->aptr = a;
		table->target = target;
		table->size = 1;
		table->next = NULL;
	}
	else
	{
		bool same = false;
		int size_tmp = 0;
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
			size_tmp = size_tmp + 1;
			table = table->next;
		}
		if (!same)
		{
			table->next = new gimple_array();
			table = table->next;
			table->stmt = used_stmt;
			table->ret_stmt = ret_stmt;
			table->target = target;
			table->size = size_tmp;
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
					if (dominated_by_p(CDI_DOMINATORS, free_t->bb, free_t->fun->cfg->x_exit_block_ptr->prev_bb) ^ dominated_by_p(CDI_DOMINATORS, free2_t->bb, free2_t->fun->cfg->x_exit_block_ptr->prev_bb))
					{
						errorm = POINTER_MAY;
					}
					else if (dominated_by_p(CDI_DOMINATORS, free_t->bb, free_t->fun->cfg->x_exit_block_ptr->prev_bb) && dominated_by_p(CDI_DOMINATORS, free2_t->bb, free2_t->fun->cfg->x_exit_block_ptr->prev_bb))
					{
						errorm = POINTER_MUST;
					}
					else if (!dominated_by_p(CDI_DOMINATORS, free_t->bb, free_t->fun->cfg->x_exit_block_ptr->prev_bb) && !dominated_by_p(CDI_DOMINATORS, free2_t->bb, free2_t->fun->cfg->x_exit_block_ptr->prev_bb))
					{

						if (fDFS->get(free_t->node)->is_succ(free_t->bb, free2_t->bb))
						{
							errorm = POINTER_MAY;
						}
					}
					free_dominance_info(CDI_DOMINATORS);
					pop_cfun();
					fprintf(stderr, "do interprocedural allocation check		=======search ptable %d %d========\n", free_t->fun, free2_t->fun);

					if (ipa)
					{

						FOR_EACH_TABLE(malloc_t, t3)
						{

							if (ptr_derefs_may_alias_p(free_t->target, malloc_t->target))
							{
								// fprintf(stderr, "klkkkkkkkkkkkkkkkkkkkkkkkkk\n");
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

void collect_function_call(gimple *gc, cgraph_node *node, basic_block bb)
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

		if (!strcmp(get_tree_code_name(TREE_CODE(gimple_call_arg(gc, 0))), "addr_expr"))
		{
			// debug_gimple_stmt(gc);
			// debug_tree(gimple_call_arg(gc, 0));
			fprintf(stderr, "addr_expraddr_expraddr_expraddr_expraddr_expr--------\n");
		}
		else
		{
			// debug_tree(gimple_call_arg(gc, 0));
			set_ptb(bb, ftable, gimple_call_arg(gc, 0), loc, 0, gc, node);

			// tree getreturnFucntionDecl = TREE_OPERAND(get_function_return_tree, 0);
			tree getFucntionDecl = (node->get_fun()->decl);
			if (function_free_collect->get(getFucntionDecl) == NULL)
			{
				fprintf(stderr, "--------------------collect_Function free------------------\n");
				return;
			}

			vector<free_type> free_type_array;
			function_free_array fun_array;
			if (function_free_collect->get(getFucntionDecl) == NULL)
			{
				fun_array.free_type_array = free_type_array;
			}
			else
			{
				fun_array = *(function_free_collect->get(getFucntionDecl));
				free_type_array = fun_array.free_type_array;
			}

			struct free_type free_type;
			free_type.stmt = gc;
			free_type.free_tree = gimple_call_arg(gc, 0);
			// ret_type.return_tree = get_function_return_tree;

			fun_array.free_type_array.push_back(free_type);
			function_free_collect->put(node->get_fun()->decl, fun_array);
		}
	}
	else if (!strcmp(name, "malloc") || !strcmp(get_name(gimple_call_fn(gc)), "calloc") || !strcmp(name, "xmalloc") || !strcmp(name, "strdup"))
	{
		set_ptb(bb, ptable, gimple_call_lhs(gc), loc, 0, gc, node);
	}
	
	else  if (!strcmp(name, "pthread_mutex_lock") )
	{
			fprintf(stderr, "================================================\n");
		debug(gc);
		debug_tree( gimple_call_arg(gc, 0));
		// set_ptb(bb, locktable, gimple_call_arg(gc,0), loc, 0, gc, node);

	}else if (!strcmp(name, "pthread_mutex_unlock") ){
			fprintf(stderr, "================================================\n");
		debug(gc);
		debug_tree(  gimple_call_arg(gc, 0));
		// set_ptb(bb,	unlocktable, gimple_call_arg(gc,0), loc, 0, gc, node);

	}
}

void indent(int level)
{
	while (level--)
		printf(" ");
}

void analyze_func_body(tree t, int level)
{
	indent(level);
	printf("%s\n", get_tree_code_name(TREE_CODE(t)));
	switch (TREE_CODE(t))
	{
	case STATEMENT_LIST:
	{
		fprintf(stderr, "receives \n");
		// tree_stmt_iterator it = tsi_start(t);
		// while (!tsi_end_p(it))
		// {
		//     analyze_func_body(tsi_stmt(it), level + 4);
		//     tsi_next(&it);
		// }
		// break;
	}
	case BIND_EXPR:
	{
		// analyze_func_body(BIND_EXPR_BODY(t), level + 4);
		// break;
	}
	default:
		break;
	}
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

	gimple *u_stmt;

	fprintf(stderr, "START NEW FOR_EACH_IMM_USE_STMT -------------------------------\n");
	debug_tree(target);
	fprintf(stderr, "TARGET NEW FOR_EACH_IMM_USE_STMT -------------------------------\n");
	if (!strcmp(get_tree_code_name(TREE_CODE(target)), "addr_expr"))
	{
		fprintf(stderr, "GIMPLE CODE :addr_expr--------\n");
		return;
	}

	FOR_EACH_IMM_USE_STMT(use_stmt, imm_iter, target)
	{
		fprintf(stderr, "			FOR_EACH_IMM_USE_STMT INSIDE-------------------------------\n");
		debug(use_stmt);

		// fprintf(stderr,"			FOR_EACH_IMM_USE_STMT INSIDE-------------------------------\n");

		if (gimple_code(use_stmt) == GIMPLE_RETURN)
		{
			fprintf(stderr, "GIMPLE_RETURN\n");
			fprintf(stderr, "------------------GIMPLE_RETURN : LHS------------------\n");
			tree return_tree = gimple_return_retval(as_a<greturn *>(use_stmt));

			set_gimple_array(used_stmt, use_stmt, return_tree, return_tree, NULL);
			//可能產生無窮迴圈
			// if (TREE_CODE(return_tree) == SSA_NAME )
			// 	new_search_imm_use(used_stmt, return_tree, return_tree);
		}
		else if (gimple_code(use_stmt) == GIMPLE_ASSIGN)
		{
			fprintf(stderr, "GIMPLE ASSIGN\n");
			//debug_tree(gimple_assign_lhs(use_stmt));
			// debug_gimple_stmt(use_stmt);
			debug_tree(gimple_assign_lhs(use_stmt));
			// debug_tree(gimple_assign_lhs(use_stmt));
			fprintf(stderr, "%s\n", get_tree_code_name(TREE_CODE(gimple_assign_lhs(use_stmt))));
			if (gimple_assign_lhs(use_stmt) && TREE_CODE(gimple_assign_lhs(use_stmt)) == SSA_NAME)
			{
				debug_tree(gimple_assign_lhs(use_stmt));

				debug(use_stmt);
				set_gimple_array(used_stmt, use_stmt, gimple_assign_lhs(use_stmt), target, NULL);
				fprintf(stderr, "------------------SSA_NAME : LHS------------------\n");
				debug(use_stmt);
				debug_tree(gimple_assign_lhs(use_stmt));
				fprintf(stderr, "------------------SSA_NAME : LHS------------------\n");

				new_search_imm_use(used_stmt, gimple_assign_lhs(use_stmt), gimple_assign_lhs(use_stmt));
			}
			else if (gimple_assign_lhs(use_stmt) && TREE_CODE(gimple_assign_lhs(use_stmt)) == VAR_DECL)
			{
				/*
					int *foo(int z)
					{
					int *b = malloc(1);
					int *p2 = malloc(1);

					p2 = &b;
					b[0] = 2;
					p2[0] = 1;
					
					free(p2);
					return p2;
					}
				*/
				// debug(use_stmt);
				fprintf(stderr, "------------------VAR_DECL : LHS------------------\n");
				// debug_tree( gimple_assign_rhs1(use_stmt));
				// debug(use_stmt);
				// debug_tree (gimple_assign_lhs(use_stmt));
				//fprintf(stderr, "VAR_DECLVAR_DECLVAR_DECL------------------\n");
				// debug_gimple_stmt(use_stmt);
				debug_tree(gimple_assign_lhs(use_stmt));
				//fprintf(stderr, "VAR_DECLVAR_DECLVAR_DECL------------------\n");
				fprintf(stderr, "------------------VAR_DECL : LHS-----------------2-\n");

				// set_gimple_array(used_stmt, use_stmt, gimple_assign_lhs(use_stmt), target, NULL);

				// tree testttt = gimple_assign_lhs(use_stmt);
				// debug(use_stmt);
				tree getFunctionAssignVAR = gimple_assign_lhs(use_stmt);

				function_assign_array assign_array = ret_function_varstmt(getFunctionAssignVAR);
				fprintf(stderr, "=======print_function_var %d   %d========\n", getFunctionAssignVAR, assign_array.assign_type_array.size());
				// if (assign_array.assign_type_array.size() )
				// {

				for (int i = 0; i < assign_array.assign_type_array.size(); i++)
				{

					// for (int i = 0; i < (ret_type_array).size(); i++)
					// {
					debug((assign_array.assign_type_array)[i].stmt);
					set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, gimple_assign_lhs((assign_array.assign_type_array)[i].stmt), target, NULL);
					if (gimple_code((assign_array.assign_type_array)[i].stmt) == GIMPLE_ASSIGN)
						if (TREE_CODE(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt)) == SSA_NAME)
							//{
							new_search_imm_use(used_stmt, gimple_assign_lhs((assign_array.assign_type_array)[i].stmt), gimple_assign_lhs((assign_array.assign_type_array)[i].stmt));

					//}
					//else
					//{
					// gimple *gcassign = *function_assign_collect->get(gimple_assign_lhs(use_stmt));

					//}
					// debug((assign_array.assign_type_array)[i].stmt);
					// fprintf(stderr, ",\n");
					// }
					// fprintf(stderr, "]\n");
					// print_function_path(&ret_type_array);
				}

				// debug_tree(gimple_assign_lhs(gcassign));
				// MEM[(int *)&b ] = 1;
				// MEM[(int *)&b + 4B] = 1;
				// if (TREE_CODE(gimple_assign_lhs(gcassign)) == SSA_NAME)
				// 	new_search_imm_use(used_stmt, gimple_assign_lhs(gcassign), gimple_assign_lhs(gcassign));
				// }
				// }
			}
			else if (gimple_assign_lhs(use_stmt) && TREE_CODE(gimple_assign_lhs(use_stmt)) == MEM_REF)
			{

				// debug(use_stmt);
				// fprintf(stderr, "MEM_REF ------------------\n");
				fprintf(stderr, "------------------MEM_REF : LHS------------------\n");

				// debug_tree( gimple_assign_rhs1(use_stmt));
				// debug(use_stmt);
				// debug_tree (gimple_assign_lhs(use_stmt));
				//fprintf(stderr, "VAR_DECLVAR_DECLVAR_DECL------------------\n");
				// debug_gimple_stmt(use_stmt);
				debug_tree(gimple_assign_lhs(use_stmt));
				tree fundecl = TREE_OPERAND(gimple_assign_lhs(use_stmt), 0);
				debug_tree(fundecl);
				debug(use_stmt);
				//tree fundecl = TREE_OPERAND(gc, 0);

				//fprintf(stderr, "VAR_DECLVAR_DECLVAR_DECL------------------\n");
				fprintf(stderr, "------------------MEM_REF : LHS------------------\n");
				set_gimple_array(used_stmt, use_stmt, fundecl, fundecl, NULL);
				// if (TREE_CODE(fundecl) == SSA_NAME)

				// 	new_search_imm_use(used_stmt, fundecl, fundecl);
				// new_search_imm_use(used_stmt,  gimple_assign_rhs1(use_stmt),   gimple_assign_lhs(use_stmt));
			}
		}
		else if (gimple_code(use_stmt) == GIMPLE_PHI)
		{
			fprintf(stderr, "GIMPLE_PHI\n");
			fprintf(stderr, "-----------------GIMPLE_PHI : LOOP------------------\n");
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
						// fprintf(stderr, "----------------what r u loolking for------------------\n");
						debug_tree(gimple_phi_result(use_stmt));
					}
				}
				if (!exist)
				{
					debug(use_stmt);
					debug_tree(gimple_phi_result(use_stmt));
					set_gimple_array(used_stmt, use_stmt, gimple_phi_result(use_stmt), gimple_phi_result(use_stmt), NULL);
					fprintf(stderr, "-----------------GIMPLE_PHI : INSERT------------------\n");
					// gimple *def_stmt = SSA_NAME_DEF_STMT( gimple_phi_result(use_stmt));
					// debug(def_stmt);
					// debug_tree(gimple_assign_lhs(def_stmt));
					fprintf(stderr, "-----------------GIMPLE_PHI : INSERT------------------\n");
					if (TREE_CODE(gimple_phi_result(use_stmt)) == SSA_NAME)
						new_search_imm_use(used_stmt, gimple_phi_result(use_stmt), gimple_phi_result(use_stmt));
				}
			}
		}
		else if (gimple_code(use_stmt) == GIMPLE_CALL)
		{
			fprintf(stderr, "GIMPLE_CALL2\n");
			if (gimple_call_fn(use_stmt) == NULL)
				return;
			// fprintf(stderr, "-----------------GIMPLE_CALL : FIND------------------\n");
			// debug(use_stmt);
			// 	fprintf(stderr, "-----------------GIMPLE_CALL : FIN22D------------------\n");
			// 		fprintf(stderr, "-----------------GIMPLE_CALL : FIN22D------------------\n");
			// 			fprintf(stderr, "-----------------GIMPLE_CALL : FIN22D------------------\n");
			// 				fprintf(stderr, "-----------------GIMPLE_CALL : FIN22D------------------\n");
			// 					fprintf(stderr, "-----------------GIMPLE_CALL : FIN22D------------------\n");
			// 						fprintf(stderr, "-----------------GIMPLE_CALL : FIN22D------------------\n");
			// 							fprintf(stderr, "-----------------GIMPLE_CALL : FIN22D------------------\n");
			// 								fprintf(stderr, "-----------------GIMPLE_CALL : FIN22D------------------\n");

			// 									fprintf(stderr, "-----------------GIMPLE_CALL : FIN22D------------------\n");
			name = get_name(gimple_call_fn(use_stmt));
			if (!strcmp(name, "free") || !strcmp(name, "xfree"))
			{

				debug(use_stmt);
				debug_tree(target);
				debug_tree(gimple_call_fn(use_stmt));
				fprintf(stderr, "-----------------GIMPLE_CALL : FIN22D------------------\n");

				fprintf(stderr, "-------always in therealways in therealways in there--------------------------\n");
				set_gimple_array(used_stmt, use_stmt, gimple_call_fn(use_stmt), target, NULL);
				if (TREE_CODE(gimple_call_fn(use_stmt)) == SSA_NAME)
					new_search_imm_use(used_stmt, gimple_call_fn(use_stmt), gimple_call_fn(use_stmt));
			}
			else
			{
				fprintf(stderr, "-------always in therealways in therealways in there--------------------------\n");
				set_gimple_array(used_stmt, use_stmt, gimple_call_fn(use_stmt), target, NULL);
				if (TREE_CODE(gimple_call_fn(use_stmt)) == SSA_NAME)
					new_search_imm_use(used_stmt, gimple_call_fn(use_stmt), gimple_call_fn(use_stmt));
			}
		}
		else
		{
			fprintf(stderr, "-------always in therealways in therealways in there--------------------------\n");
			debug_gimple_stmt(use_stmt);

			set_gimple_array(used_stmt, use_stmt, target, target2, NULL);
		}
	}
}

void printfPointerConstraint(ptb *ptable, gimple_array *user_tmp)
{
	gimple *u_stmt;
	gimple_array start;
	start.stmt = NULL;
	gimple_array *used_stmt = &start;
	ptb *table_temp = ptable;
	tree t;
	//

	FOR_EACH_TABLE(table_temp, t)
	{
		fprintf(stderr, "------------------- stmt -----------------------\n");
		// debug_tree(table_temp->target);
		if (table_temp->target != NULL)
		{
			debug(table_temp->last_stmt);

			location_t loc = gimple_location(table_temp->last_stmt);
			fprintf(stderr, "------------------------------------------\n");
			warning_at(loc, 0, "loc");

			debug_tree(table_temp->target);
			fprintf(stderr, "------------------------------------------\n");
			user_tmp = treeGimpleArray->get(table_temp->target);
			start.stmt = NULL;
			used_stmt = &start;
			if (user_tmp != NULL)
			{
				fprintf(stderr, " \n Pointer to set  is size %d :[ \n", user_tmp->size);
				if (user_tmp->size > 0)
					FOR_EACH_USE_TABLE(user_tmp, u_stmt)
					{
						fprintf(stderr, "------------------------------------------\n");
						if (user_tmp->ret_stmt != NULL)
						{

							debug(user_tmp->ret_stmt);

							location_t loc = gimple_location(user_tmp->ret_stmt);
							warning_at(loc, 0, "loc");
						}
						else
						{
							debug(u_stmt);
							location_t loc = gimple_location(u_stmt);
							warning_at(loc, 0, "loc");
						}
						// user_tmp->
					}
			}
			else
			{
				fprintf(stderr, " \n Pointer to set  is size 0 :[ \n");
			}
			fprintf(stderr, "] \n");
		}
	}
}

void collect_FunctionMapping_Assign(gimple *gc, cgraph_node *node, basic_block bb)
{
	location_t loc = gimple_location(gc);
	tree a;
	cgraph_node *node2;
	const char *name;
	//https://doc.ecoscentric.com/gnutools/doc/gccint/Expression-trees.html#Expression-trees
	//https://doc.ecoscentric.com/gnutools/doc/gccint/Storage-References.html
	if (gimple_code(gc) == GIMPLE_CALL)
	{
		// if (gimple_call_fn(gc) == NULL)
		// 	return;

		fprintf(stderr, "GIMPLE_CALL\n");
		name = get_name(gimple_call_fn(gc));
		fprintf(stderr, "--------------------collect_FunctionMapping------------------\n");
		fprintf(stderr, "%s\n", name);

		debug(gc);
		tree get_function_return_tree = gimple_call_fn(gc);
		if (!strcmp(name, "free") || !strcmp(name, "xfree"))
		{

			debug_tree(get_function_return_tree);
			debug_tree(gimple_call_arg(gc, 0));
			// fprintf(stderr, "--------------------wmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmmm------------------\n");
			// if (function_return_collect->get(node->get_fun()->decl) == NULL)
			// {

			// 	fprintf(stderr, "--------------------collect_FunctionMappingENNNNNNNNDDDDDDD------------------\n");
			// 	return;
			// }
			if (!strcmp(get_tree_code_name(TREE_CODE(gimple_call_arg(gc, 0))), "addr_expr"))
			{
				tree second = TREE_OPERAND(gimple_call_arg(gc, 0), 0);
				if (TREE_CODE(second) == VAR_DECL)
				{
					debug_tree(second);
					// debug_tree(gimple_assign_lhs(gc));

					function_assign_array assign_array;
					vector<assign_type> assign_type_array;

					if (function_assign_collect->get(second) == NULL)
					{
						// fprintf(stderr, "firstfirstfirstfirstfirstmappinggggggggggggggggggggggggggggggg-------\n");
						assign_array.assign_type_array = assign_type_array;
						// debug(gc);
					}
					else
					{
						fprintf(stderr, "mappinggggggggggggggggggggggggggggggg-------\n");
						debug(gc);
						assign_array = *(function_assign_collect->get(second));
						assign_type_array = assign_array.assign_type_array;
					}

					struct assign_type assign_type;

					assign_type.stmt = gc;
					assign_type.assign_tree = gimple_call_fn(gc);
					// ret_type.reutnr_type_num = 0;
					assign_array.assign_type_array.push_back(assign_type);
					function_assign_collect->put(second, assign_array);
				}
			}
			// tree getFucntionDecl = (node->get_fun()->decl);
			// debug_tree(getFucntionDecl);
			// vector<return_type> ret_type_array;
			// function_return_array fun_array;
			// if (function_return_collect->get(getFucntionDecl) == NULL)
			// {
			// 	fun_array.return_type_array = ret_type_array;
			// }
			// else
			// {
			// 	fun_array = *(function_return_collect->get(getFucntionDecl));
			// 	ret_type_array = fun_array.return_type_array;
			// }
			// tree function_return_lhstree = gimple_call_arg(gc, 0);
			// struct return_type ret_type;
			// ret_type.stmt = gc;
			// ret_type.return_tree = function_return_lhstree;
			// ret_type.reutnr_type_num = 0;
			// fun_array.return_type_array.push_back(ret_type);
			// function_return_collect->put(node->get_fun()->decl, fun_array);
		}
		else
		{

			tree getreturnFucntionDecl = TREE_OPERAND(get_function_return_tree, 0);
			if (function_return_collect->get(getreturnFucntionDecl) == NULL)
			{
				fprintf(stderr, "--------------------collect_FunctionMappingENNNNNNNNDDDDDDD------------------\n");
				return;
			}
			tree getFucntionDecl = (node->get_fun()->decl);
			debug_tree(getFucntionDecl);
			vector<return_type> ret_type_array;
			function_return_array fun_array;

			if (function_return_collect->get(getFucntionDecl) == NULL)
			{
				fun_array.return_type_array = ret_type_array;
			}
			else
			{
				fun_array = *(function_return_collect->get(getFucntionDecl));
				ret_type_array = fun_array.return_type_array;
			}

			struct return_type ret_type;
			ret_type.stmt = gc;
			ret_type.return_tree = getreturnFucntionDecl;

			fun_array.return_type_array.push_back(ret_type);
			function_return_collect->put(node->get_fun()->decl, fun_array);

			debug(gc);

			fprintf(stderr, "--------------------wwwwwwwwwwwwwSSSSSSSSSSSSSSSSSwwwwwwwwwwwwwwww------------------\n");
			// 分歧點
			// foo()
			// p1 = foo2()
			debug_tree(gimple_call_arg(gc, 0));
			fprintf(stderr, "--------------------wwwwwwwwwwwwwSSSSSSSSSSSSSSSSSwwwwwwwwwwwwwwww------------------\n");

			debug_tree(gimple_call_lhs(gc));
			fprintf(stderr, "--------------------wwwwwwwwwwwwwSSSSSSSSSSSSSSSSSwwwwwwwwwwwwwwww------------------\n");

			debug_tree(gimple_call_fn(gc));
			fprintf(stderr, "--------------------wwwwwwwwwwwwSSSSSSSSSSSSSSSSSwwwwwwwwwwwwwwwww------------------\n");
			tree checklhstree = gimple_call_lhs(gc);
			// if lhs tree is null this is fucntion call
			if (checklhstree != NULL)
			{
				if (TREE_CODE(gimple_call_lhs(gc)) == SSA_NAME)
					set_ptb(bb, ptable, gimple_call_lhs(gc), loc, 0, gc, node);
			}
			else if (gimple_code(gc) == GIMPLE_CALL)
			{
				name = get_name(gimple_call_fn(gc));
				if (name != NULL)
				{
					fprintf(stderr, "functionname %s\n", name);
					fprintf(stderr, "functionname %s\n", name);
					fprintf(stderr, "functionname %s\n", name);
					fprintf(stderr, "functionname %s\n", name);
					fprintf(stderr, "functionname %s\n", name);
					fprintf(stderr, "functionname %s\n", name);
					fprintf(stderr, "functionname %s\n", name);
					fprintf(stderr, "functionname %s\n", name);

					fprintf(stderr, "functionname %s\n", name);
					fprintf(stderr, "functionname %s\n", name);
					// if (
					// 	!strcmp(name, "malloc") ||
					// 	!strcmp(name, "xmalloc") ||
					// 	!strcmp(name, "calloc") ||
					// 	!strcmp(name, "xcalloc") ||
					// 	!strcmp(name, "strdup"))
					// {

					fprintf(stderr, "--------------------collect_FunctionMappingENNNNNNNNDDDDDDD------------------\n");
					fprintf(stderr, "--------------------collect_FunctionMappingENNNNNNNNDDDDDDD------------------\n");
					fprintf(stderr, "--------------------collect_FunctionMappingENNNNNNNNDDDDDDD------------------\n");
					fprintf(stderr, "--------------------collect_FunctionMappingENNNNNNNNDDDDDDD------------------\n");
					fprintf(stderr, "--------------------collect_FunctionMappingENNNNNNNNDDDDDDD------------------\n");
					fprintf(stderr, "--------------------collect_FunctionMappingENNNNNNNNDDDDDDD------------------\n");
					fprintf(stderr, "--------------------collect_FunctionMappingENNNNNNNNDDDDDDD------------------\n");
					fprintf(stderr, "--------------------collect_FunctionMappingENNNNNNNNDDDDDDD------------------\n");
					fprintf(stderr, "--------------------collect_FunctionMappingENNNNNNNNDDDDDDD------------------\n");
					fprintf(stderr, "--------------------collect_FunctionMappingENNNNNNNNDDDDDDD------------------\n");

					set_ptb(bb, ptable, gimple_call_arg(gc, 0), loc, 0, gc, node);
					// }
				}
			}

			// set_ptb(bb, ptable, gimple_call_lhs(gc), loc, 0, gc, node);
		}
		// debug_tree(function_return_lhstree);
		fprintf(stderr, "--------------------collect_FunctionMappingENNNNNNNNDDDDDDD------------------\n");
	}
	else if (gimple_code(gc) == GIMPLE_ASSIGN)
	{
		// if (TREE_CODE(gimple_assign_lhs(gc)) == VAR_DECL== NULL)
		fprintf(stderr, "GIMPLE_ASSIGN\n");

		if ((gimple_assign_lhs(gc) == NULL) || (gimple_assign_rhs1(gc)) == NULL)
			return;
		debug(gc);
		debug_tree(gimple_assign_lhs(gc));
		debug_tree(gimple_assign_rhs1(gc));
		// struct assign_type tmp ;
		// tmp.stmt = gc;
		// tmp.assign_tree =gimple_assign_lhs(gc);
		///MEM[(int *)&b] = 1;
		// tree getFunctionAssignSSANAME = gimple_assign_rhs1(gc);
		// tree getFunctionAssignVAR = gimple_assign_lhs(gc);
		tree getFunctionAssignRHS = gimple_assign_rhs1(gc);
		tree getFunctionAssignLHS = gimple_assign_lhs(gc);
		if (TREE_CODE(getFunctionAssignRHS) == SSA_NAME)
		{
			if (TREE_CODE(getFunctionAssignLHS) == VAR_DECL)
			{
				function_assign_array assign_array;
				vector<assign_type> assign_type_array;

				if (function_assign_collect->get(getFunctionAssignLHS) == NULL)
				{
					assign_array.assign_type_array = assign_type_array;
				}
				else
				{
					assign_array = *(function_assign_collect->get(getFunctionAssignLHS));
					assign_type_array = assign_array.assign_type_array;
				}
				tree function_return_lhstree = gimple_call_lhs(gc);
				struct assign_type assign_type;

				assign_type.stmt = gc;
				assign_type.assign_tree = gimple_assign_lhs(gc);
				// ret_type.reutnr_type_num = 0;
				assign_array.assign_type_array.push_back(assign_type);

				function_assign_collect->put(getFunctionAssignLHS, assign_array);
			}
		}

		if (TREE_CODE(getFunctionAssignLHS) == VAR_DECL)
		{
			if (!strcmp(get_tree_code_name(TREE_CODE(getFunctionAssignRHS)), "addr_expr"))
			{
				tree second = TREE_OPERAND(getFunctionAssignRHS, 0);
				if (TREE_CODE(second) == VAR_DECL)
				{
					/*
					執行緒

					pthread_mutex_lock(&mLock);
					int **ppData = malloc(10);
					int *pData = malloc(20);
					int data2 = 0;

					ppData = &pData;
					pData = &data2;
					*ppData[1] = 10;
					**ppData = 10; 
					pthread_mutex_unlock(&mLock);
					pthread_exit(NULL); // 離開子執行緒
					*/
					/*
						//pData = &data2;
					*/
					function_assign_array assign_array;
					vector<assign_type> assign_type_array;

					if (function_assign_collect->get(getFunctionAssignLHS) == NULL)
					{
						assign_array.assign_type_array = assign_type_array;
					}
					else
					{
						assign_array = *(function_assign_collect->get(getFunctionAssignLHS));
						assign_type_array = assign_array.assign_type_array;
					}
					tree function_return_lhstree = gimple_call_lhs(gc);
					struct assign_type assign_type;

					assign_type.stmt = gc;
					assign_type.assign_tree = gimple_assign_lhs(gc);
					// ret_type.reutnr_type_num = 0;
					assign_array.assign_type_array.push_back(assign_type);

					function_assign_collect->put(getFunctionAssignLHS, assign_array);
					fprintf(stderr, "--------------------xxxxxxxxxxxxxxxxxxxxxxx------------------\n");
				}
			}
		}

		if (TREE_CODE(getFunctionAssignLHS) == MEM_REF)
		{

			tree first = TREE_OPERAND(gimple_assign_lhs(gc), 0);
			if (!strcmp(get_tree_code_name(TREE_CODE(first)), "addr_expr"))
			// if (!strcmp( get_tree_code_name(first), "addr_expr"))
			// if (!strcmp(get_tree_code_name(first, "addr_expr")))
			{
				// debug_gimple_stmt(gc);
				// debug_tree(gimple_call_arg(gc, 0));
				fprintf(stderr, "addr_expraddr_expraddr_expraddr_expraddr_expr--------\n");
				tree second = TREE_OPERAND(first, 0);
				debug_tree(second);
				// debug_tree(gimple_assign_lhs(gc));

				function_assign_array assign_array;
				vector<assign_type> assign_type_array;

				if (function_assign_collect->get(second) == NULL)
				{
					// fprintf(stderr, "firstfirstfirstfirstfirstmappinggggggggggggggggggggggggggggggg-------\n");
					assign_array.assign_type_array = assign_type_array;
					// debug(gc);
				}
				else
				{
					fprintf(stderr, "mappinggggggggggggggggggggggggggggggg-------\n");
					debug(gc);
					assign_array = *(function_assign_collect->get(second));
					assign_type_array = assign_array.assign_type_array;
				}

				struct assign_type assign_type;

				assign_type.stmt = gc;
				assign_type.assign_tree = gimple_assign_lhs(gc);
				// ret_type.reutnr_type_num = 0;
				assign_array.assign_type_array.push_back(assign_type);
				function_assign_collect->put(second, assign_array);
				// function_assign_collect->put(second, gc);
			}
		}
		else if (TREE_CODE(getFunctionAssignLHS) == SSA_NAME)
		{
			/*
			執行緒

			pthread_mutex_lock(&mLock);
			int **ppData = malloc(10);
			int *pData = malloc(20);
			int data2 = 0;

			ppData = &pData;
			pData = &data2;
			*ppData[1] = 10;
			**ppData = 10; 
			pthread_mutex_unlock(&mLock);
			pthread_exit(NULL); // 離開子執行緒
			*/
			if (TREE_CODE(getFunctionAssignRHS) == MEM_REF)
			{
				tree first = TREE_OPERAND(getFunctionAssignRHS, 0);
				if (!strcmp(get_tree_code_name(TREE_CODE(first)), "addr_expr"))
				{
					// debug_gimple_stmt(gc);
					// debug_tree(gimple_call_arg(gc, 0));
					fprintf(stderr, "sssssssssssssssssaaaaaaaaa name addr_expraddr_expraddr_expraddr_expraddr_expr--------\n");
					tree second = TREE_OPERAND(first, 0);
					debug_tree(second);
					function_assign_array assign_array;
					vector<assign_type> assign_type_array;

					if (function_assign_collect->get(second) == NULL)
					{
						// fprintf(stderr, "firstfirstfirstfirstfirstmappinggggggggggggggggggggggggggggggg-------\n");
						assign_array.assign_type_array = assign_type_array;
						// debug(gc);
					}
					else
					{
						fprintf(stderr, "mappinggggggggggggggggggggggggggggggg-------\n");
						debug(gc);
						assign_array = *(function_assign_collect->get(second));
						assign_type_array = assign_array.assign_type_array;
					}

					struct assign_type assign_type;

					assign_type.stmt = gc;
					assign_type.assign_tree = gimple_assign_lhs(gc);
					// ret_type.reutnr_type_num = 0;
					assign_array.assign_type_array.push_back(assign_type);
					function_assign_collect->put(second, assign_array);
				}
			}

			else if (TREE_CODE(getFunctionAssignRHS) == VAR_DECL)
			{
				function_assign_array assign_array;
				vector<assign_type> assign_type_array;

				if (function_assign_collect->get(getFunctionAssignRHS) == NULL)
				{
					// fprintf(stderr, "firstfirstfirstfirstfirstmappinggggggggggggggggggggggggggggggg-------\n");
					assign_array.assign_type_array = assign_type_array;
					// debug(gc);
				}
				else
				{
					fprintf(stderr, "mappinggggggggggggggggggggggggggggggg-------\n");
					debug(gc);
					assign_array = *(function_assign_collect->get(getFunctionAssignRHS));
					assign_type_array = assign_array.assign_type_array;
				}

				struct assign_type assign_type;

				assign_type.stmt = gc;
				assign_type.assign_tree = gimple_assign_lhs(gc);
				// ret_type.reutnr_type_num = 0;
				assign_array.assign_type_array.push_back(assign_type);
				function_assign_collect->put(getFunctionAssignRHS, assign_array);
			}
		}
	}
}

function_assign_array ret_function_varstmt(tree function_tree)
{
	function_assign_array assign_array;
	if (function_assign_collect->get(function_tree) != NULL)
	{

		assign_array = *(function_assign_collect->get(function_tree));

		vector<assign_type> assign_type_array = assign_array.assign_type_array;
		//debug_tree(function_tree);
		//vector<pair<fdecl,location_t>> loc;
		fprintf(stderr, "=======print_function_var %d   %d========\n", function_tree, assign_type_array.size());
	}
	// fprintf(stderr, "[\n");
	// if (assign_type_array.size() != 0)
	return assign_array;
	// else
	// 	return ;
}

void get_function_varstmt(tree function_tree, tree vartree)
{
	if (function_assign_collect->get(function_tree) == NULL)
		return;
	function_assign_array assign_array = *(function_assign_collect->get(function_tree));

	vector<assign_type> assign_type_array = assign_array.assign_type_array;
	//debug_tree(function_tree);
	//vector<pair<fdecl,location_t>> loc;
	fprintf(stderr, "=======print_function_var %d   %d========\n", function_tree, assign_type_array.size());
	fprintf(stderr, "[\n");
	for (int i = 0; i < assign_type_array.size(); i++)
	{

		// for (int i = 0; i < (ret_type_array).size(); i++)
		// {

		debug((assign_type_array)[i].stmt);
		fprintf(stderr, ",\n");
		// }
		// fprintf(stderr, "]\n");
		// print_function_path(&ret_type_array);
	}
	fprintf(stderr, "GIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGN CALLLLLLLLLLLLLLLLLLLLLLL\n");
	fprintf(stderr, "]\n");
}

void get_function_specialstmt(tree function_tree, tree vartree)
{
	if (function_return_collect->get(function_tree) == NULL)
		return;
	function_return_array fun_array = *(function_return_collect->get(function_tree));

	vector<return_type> ret_type_array = fun_array.return_type_array;
	//debug_tree(function_tree);
	//vector<pair<fdecl,location_t>> loc;
	fprintf(stderr, "=======print_function_return %d   %d========\n", function_tree, ret_type_array.size());
	fprintf(stderr, "[\n");
	for (int i = 0; i < ret_type_array.size(); i++)
	{

		// for (int i = 0; i < (ret_type_array).size(); i++)
		// {

		debug((ret_type_array)[i].stmt);
		fprintf(stderr, ",\n");
		// }
		// fprintf(stderr, "]\n");
		// print_function_path(&ret_type_array);
	}
	fprintf(stderr, "GIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGN CALLLLLLLLLLLLLLLLLLLLLLL\n");
	fprintf(stderr, "]\n");
}

void collect_FunctionMapping_Ret(tree function_tree, gimple *u_stmt, gimple_array *user_tmp, ptb *table_temp, ptb *ptable)
{

	if (function_return_collect->get(function_tree) == NULL)
		return;
	function_return_array fun_array = *(function_return_collect->get(function_tree));

	vector<return_type> ret_type_array = fun_array.return_type_array;
	//debug_tree(function_tree);
	//vector<pair<fdecl,location_t>> loc;
	fprintf(stderr, "=======print_function_return %d   %d========\n", function_tree, ret_type_array.size());

	for (int i = 0; i < ret_type_array.size(); i++)
	{
		fprintf(stderr, "[\n");

		debug((ret_type_array)[i].stmt);
		debug(u_stmt);
		gimple *gc = (ret_type_array)[i].stmt;
		// _4=PHI<_>
		// return _4;
		if (gimple_code(u_stmt) == GIMPLE_PHI)
		{

			if ((ret_type_array)[i].return_tree == gimple_phi_result(u_stmt))
			{
				debug((ret_type_array)[i].stmt);
				debug(u_stmt);

				// tree fundecl = TREE_OPERAND(gimple_assign_lhs(u_stmt), 0);
				// gimple *def_stmt = SSA_NAME_DEF_STMT(u_stmt);
				// debug(def_stmt);
				set_gimple_array(user_tmp, table_temp->last_stmt, (ret_type_array)[i].return_tree, (ret_type_array)[i].return_tree, gc);
				return;
			}
		}
		else if (gimple_code(table_temp->last_stmt) == GIMPLE_CALL)
		{
			const char *name;
			name = get_name(gimple_call_fn(table_temp->last_stmt));
			// 		debug(u_stmt);
			// fprintf(stderr, "%s\n",name);
			if (!strcmp(name, "malloc") || !strcmp(name, "calloc") || !strcmp(name, "xmalloc") || !strcmp(name, "strdup"))
			{

				debug(table_temp->last_stmt);
				// debug(u_stmt);
				debug(gc);

				// gimple *test2 =   test;
				tree getmalloclhs = table_temp->target;
				// debug_tree (getmalloclhs);
				if ((ret_type_array)[i].return_tree != NULL && getmalloclhs != NULL)
				{

					if (gimple_code(gc) == GIMPLE_RETURN)
					{
						fprintf(stderr, "RETURN with possible malloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
						fprintf(stderr, "GIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGNGIMPLE_ASSIGN CALLLLLLLLLLLLLLLLLLLLLLL\n");
						//比對所有 ptable 有malloc 的stmt 假設有遇到 malloc 則可能為type2
						if (TREE_CODE((ret_type_array)[i].return_tree) == SSA_NAME)
							if (ptr_derefs_may_alias_p(table_temp->target, (ret_type_array)[i].return_tree))
							{
								fprintf(stderr, "GIMPLE_ASSIGNGIMPLE_successssssssssssssssss\n");
								// return value related malloc stmt
								// debug(test);
								// debug(u_stmt);
								// debug(gc);
								// set_ptb( table_temp->bb, ptable,  table_temp->target,  table_temp->loc, 0,  table_temp->last_stmt,  table_temp->node);
								fun_array.return_type_num = 2;
								function_return_collect->put(function_tree, fun_array);
								// return;
								return;
							}
					}
				}
				else
				{
					// fun_array.return_type_num = 1;
					// function_return_collect->put(function_tree, fun_array);
					// return ;
				}
				// 			if ((ret_type_array)[i].return_tree == NULL || getmalloclhs == NULL ){
				// 			debug_tree((ret_type_array)[i].return_tree );
				// 			debug_tree(getmalloclhs);
				// fprintf(stderr, "RETURN with malloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
				// 					fprintf(stderr, "RETURN with malloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
				// 					fprintf(stderr, "RETURN with malloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
				// 					fprintf(stderr, "RETURN with malloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
				// 					fprintf(stderr, "RETURN with malloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
				// 					fprintf(stderr, "RETURN with malloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
				// 					fprintf(stderr, "RETURN with malloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
				// 					fprintf(stderr, "RETURN with malloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
				// 					fprintf(stderr, "RETURN with malloc ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
				// return;

				// 			}

				// debug(getmalloclhs);
			}
			if (!strcmp(name, "free") || !strcmp(name, "xfree"))
			{
				// fun_array.return_type_num = 3;
				// fprintf(stderr, "RETURN with free ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
				// function_return_collect->put(function_tree, fun_array);
				// return;
			}
		}
	}
}

void printfPointerConstraint2(ptb *ftable, gimple_array *user_tmp)
{
	gimple *u_stmt;
	gimple_array start;
	start.stmt = NULL;
	gimple_array *used_stmt = &start;
	ptb *table_temp = ptable;
	tree t;
	//

	FOR_EACH_TABLE(table_temp, t)
	{
		fprintf(stderr, "\n ------------------------------------------\n");
		// debug_tree(table_temp->target);
		if (table_temp->target != NULL)
		{
			debug_tree(table_temp->target);
			user_tmp = treeGimpleArray->get(table_temp->target);
			start.stmt = NULL;
			used_stmt = &start;
			if (user_tmp != NULL)
			{
				fprintf(stderr, " \n Pointer to set  is size %d :[ \n", user_tmp->size);
				if (user_tmp->size > 0)
					FOR_EACH_USE_TABLE(user_tmp, u_stmt)
					{

						if (user_tmp->ret_stmt != NULL)
						{

							debug(user_tmp->ret_stmt);
						}
						else
						{
							debug(u_stmt);
							// if (gimple_code(u_stmt) == GIMPLE_RETURN)
							// {

							// }
						}
						// user_tmp->
					}
				fprintf(stderr, "] \n");
			}
			else
			{
				fprintf(stderr, " \n Pointer to set  is size 0 :[ \n");
				fprintf(stderr, "] \n");
			}
		}
	}
}

void FunctionStmtMappingRet(ptb *ptable, ptb *ftable, gimple_array *user_tmp)
{

	// set_gimple_array(used_stmt, use_stmt, fundecl, fundecl);
	gimple *u_stmt;
	gimple_array start;
	start.stmt = NULL;
	gimple_array *used_stmt = &start;
	ptb *table_temp = ptable;
	tree t;
	//

	FOR_EACH_TABLE(table_temp, t)
	{
		fprintf(stderr, "\n --------------------------ssssss----------------\n");
		if (table_temp->target != NULL)
		{
			debug_tree(table_temp->target);

			user_tmp = treeGimpleArray->get(table_temp->target);
			if (user_tmp != NULL)
			{
				fprintf(stderr, " \n Pointer to set  is size %d :[ \n", user_tmp->size);
				if (user_tmp->size > 0)
					FOR_EACH_USE_TABLE(user_tmp, u_stmt)
					{
						debug(u_stmt);
						struct cgraph_node *node;
						FOR_EACH_DEFINED_FUNCTION(node)
						{
							// if (!ipa)
							// 	init_table();
							push_cfun(node->get_fun());
							// if (strcmp(get_name(cfun->decl), "main") == 0)
							if (cfun == NULL)
								continue;

							debug_tree(cfun->decl);
							fprintf(stderr, "=======node_fun:%s=========\n", get_name(cfun->decl));
							calculate_dominance_info(CDI_DOMINATORS);
							fprintf(stderr, "oooooooooooooooooooooooooooooooooooooooo\n", get_name(cfun->decl));

							collect_FunctionMapping_Ret(cfun->decl, u_stmt, user_tmp, table_temp, ptable);
							pop_cfun();
						}

						// user_tmp->
					}
			}
			else
			{
				fprintf(stderr, " \n Pointer to set  is size 0 :[ \n");
				fprintf(stderr, "] \n");
			}
		}
	}
}

void FunctionStmtMappingAssign(ptb *ptable, gimple_array *user_tmp)
{

	// set_gimple_array(used_stmt, use_stmt, fundecl, fundecl);
	gimple *u_stmt;
	gimple_array start;
	start.stmt = NULL;
	gimple_array *used_stmt = &start;
	ptb *table_temp = ptable;
	tree t;
	basic_block bb;
	//
	struct cgraph_node *node;
	FOR_EACH_DEFINED_FUNCTION(node)
	{
		if (!ipa)
			init_table();
		push_cfun(node->get_fun());
		// if (strcmp(get_name(cfun->decl), "main") == 0)
		if (cfun == NULL)
			continue;
		fprintf(stderr, "=======Mapping node_fun:%s=========\n", get_name(cfun->decl));
		debug_tree(cfun->decl);
		calculate_dominance_info(CDI_DOMINATORS);
		FOR_EACH_BB_FN(bb, cfun)
		{
			// debug_bb(bb);

			for (gimple_stmt_iterator gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi))
			{
				gimple *gc = gsi_stmt(gsi);
				if (is_gimple_call(gc) || is_gimple_assign(gc))
				{
					/*collect malloc and free information*/
					// collect_function_call(gc, node, bb);
					collect_FunctionMapping_Assign(gc, node, bb);

					// fprintf(stderr, "add collect_function_call\n");
					// }else if (is_gimple_assign(gc)){
					// 	collect_FunctionMapping_Assign(gc, node, bb);
				}
				// }
			}
		}
		pop_cfun();
	}
}
void printfunctionCollect2(ptb *ptable, gimple_array *user_tmp)
{

	// set_gimple_array(used_stmt, use_stmt, fundecl, fundecl);
	gimple *u_stmt;
	gimple_array start;
	start.stmt = NULL;
	gimple_array *used_stmt = &start;
	ptb *table_temp = ptable;
	tree t;
	basic_block bb;
	//
	struct cgraph_node *node;
	FOR_EACH_DEFINED_FUNCTION(node)
	{
		if (!ipa)
			init_table();
		push_cfun(node->get_fun());
		// if (strcmp(get_name(cfun->decl), "main") == 0)
		fprintf(stderr, "=======node_fun:%s=========\n", get_name(cfun->decl));
		debug_tree(cfun->decl);
		if (cfun == NULL)
			continue;
		calculate_dominance_info(CDI_DOMINATORS);
		print_function_return2(node->get_fun()->decl);
		pop_cfun();
	}
}
void printfunctionCollect(ptb *ptable, gimple_array *user_tmp)
{

	// set_gimple_array(used_stmt, use_stmt, fundecl, fundecl);
	gimple *u_stmt;
	gimple_array start;
	start.stmt = NULL;
	gimple_array *used_stmt = &start;
	ptb *table_temp = ptable;
	tree t;
	basic_block bb;
	//
	struct cgraph_node *node;
	FOR_EACH_DEFINED_FUNCTION(node)
	{
		if (!ipa)
			init_table();
		push_cfun(node->get_fun());
		// if (strcmp(get_name(cfun->decl), "main") == 0)
		fprintf(stderr, "=======node_fun:%s=========\n", get_name(cfun->decl));
		debug_tree(cfun->decl);
		if (cfun == NULL)
			continue;
		calculate_dominance_info(CDI_DOMINATORS);
		print_function_return(node->get_fun()->decl);
		pop_cfun();
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

	gimple *u_stmt;
	gimple_array *used_stmt = NULL;
	gimple_array start;
	start.stmt = NULL;
	used_stmt = &start;

	table3 = ptable;
	//preproceess
	FunctionStmtMappingAssign(ptable, used_stmt);
	fprintf(stderr, "start collect similar stmtstart collect similar stmtstart collect similar stmtstart collect similar stmt\n");
	fprintf(stderr, "start collect similar stmtstart collect similar stmtstart collect similar stmtstart collect similar stmt\n");
	fprintf(stderr, "start collect similar stmtstart collect similar stmtstart collect similar stmtstart collect similar stmt\n");
	fprintf(stderr, "start collect similar stmtstart collect similar stmtstart collect similar stmtstart collect similar stmt\n");
	fprintf(stderr, "start collect similar stmtstart collect similar stmtstart collect similar stmtstart collect similar stmt\n");
	// printfunctionCollect(ptable, used_stmt);
	// return  ;
	tree t2;
	if (ptable->size >= 0)
	{
		FOR_EACH_TABLE(table1, t2)
		{
			if (TREE_CODE(TREE_TYPE(t2)) == METHOD_TYPE || TREE_CODE(TREE_TYPE(t2)) == FUNCTION_TYPE || TREE_CODE(TREE_TYPE(t2)) == RECORD_TYPE || !(TREE_CODE(t2) == SSA_NAME))
			{
				continue;
			}
			if (table1->removed || !TREE_CODE(t) == SSA_NAME)
				continue;
			used_stmt = NULL;
			gimple_array start;
			start.stmt = NULL;
			used_stmt = &start;
			new_search_imm_use(used_stmt, table1->target, table1->target);
			pi1 = SSA_NAME_PTR_INFO(table1->target);
			pt1 = &pi1->pt;
			if (pt1 == NULL)
				continue;
			if (pt1->vars == NULL)
				continue;

			if (!strcmp(get_tree_code_name(TREE_CODE(used_stmt->target)), "<invalid tree code>"))
				continue;
			FOR_EACH_USE_TABLE(used_stmt, u_stmt)
			{

				// pi2 = SSA_NAME_PTR_INFO(used_stmt->target);
				// pt2 = &pi2->pt;
				// debug_tree(used_stmt->target);
				// if()
				// if (pt2 == NULL)
				// 	continue;
				// if (pt2->vars == NULL)
				// 	continue;
				debug(used_stmt->target);
				fprintf(stderr, "%s\n", get_tree_code_name(TREE_CODE(used_stmt->target)));
				debug_tree(table1->target);

				// if (bitmap_intersect_p(pt1->vars, pt2->vars))
				// {

				// fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
				// fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
				// fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
				// fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
				// fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
				// fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
				// fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
				// fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
				// 	gimple_array *user_tmp2;
				// 	user_tmp2 = tvpt2->get(table1->target);
				// 	if (user_tmp2 != NULL)
				// 		break;

				// 	tvpt2->put(table1->target, *used_stmt);
				// 	break;
				// }

				if (ptr_derefs_may_alias_p(used_stmt->target, table1->target))
				{
					fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
					fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
					fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
					fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
					fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
					fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
					fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
					fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
					gimple_array *user_tmp2;
					user_tmp2 = treeGimpleArray->get(table1->target);
					if (user_tmp2 != NULL)
						break;

					treeGimpleArray->put(table1->target, *used_stmt);
					break;
				}
			}
		}
	}
	FunctionStmtMappingRet(ptable, ftable, used_stmt);

	fprintf(stderr, "\033[40;41mSTART CHECKSTART CHECKSTART CHECKSTART CHECKSTART CHECK\033[0m\n");
	fprintf(stderr, "\033[40;41mSTART CHECKSTART CHECKSTART CHECKSTART CHECKSTART CHECK\033[0m\n");
	fprintf(stderr, "    =()=\n");
	fprintf(stderr, " ,/'\_||_\n");
	fprintf(stderr, "  (___  `.\n");
	fprintf(stderr, " ./  `=='\n");
	fprintf(stderr, "      |||\n");
	fprintf(stderr, "      |||\n");
	fprintf(stderr, "      |||\n");
	fprintf(stderr, "      |||\n");
	fprintf(stderr, "\033[40;41mSTART CHECKSTART CHECKSTART CHECKSTART CHECKSTART CHECK\033[0m\n");
	fprintf(stderr, "\033[40;41mSTART CHECKSTART CHECKSTART CHECKSTART CHECKSTART CHECK\033[0m\n");

	fprintf(stderr, "=======================================================\n");
	// printfunctionCollect(ptable, used_stmt);
	//printfPointerConstraint(ptable, used_stmt);
	//printfBasicblock();
	//initMallocfunction

	// // printfBasicblock();
	// //printfunctionCollect2(ptable, used_stmt);
	// // printfPointerConstraint2(ptable, used_stmt);
	// // printfunctionCollect2(ptable, used_stmt);
	struct cgraph_node *node;
	record_fucntion(node);
	dump_fucntion(node, ptable, used_stmt);
	fprintf(stderr, "\033[40;32mSTART CHECKSTART CHECKSTART CHECKSTART CHECKSTART CHECK\033[0m\n");
	fprintf(stderr, "\033[40;32mSTART CHECKSTART CHECKSTART CHECKSTART CHECKSTART CHECK\033[0m\n");

	fprintf(stderr, "    =()=\n");
	fprintf(stderr, " ,/'\_||_\n");
	fprintf(stderr, "  (___  `.\n");
	fprintf(stderr, " ./  `=='\n");
	fprintf(stderr, "                   \n");
	fprintf(stderr, "    ~~~~~~~~~~~  \n");
	fprintf(stderr, " ~~~~~~\n");
	fprintf(stderr, "           ~~~~~~~\n");
	fprintf(stderr, "\033[40;32mSTART CHECKSTART CHECKSTART CHECKSTART CHECKSTART CHECK\033[0m\n");
	fprintf(stderr, "\033[40;32mSTART CHECKSTART CHECKSTART CHECKSTART CHECKSTART CHECK\033[0m\n");

	// // printfunctionCollect(ptable, used_stmt);

	// printfunctionCollect(ptable, used_stmt);
	// printfunctionCollect2(ptable, used_stmt);
	printfBasicblock();
	// printfPointerConstraint2(ptable, used_stmt);
}

void print_function_path(vector<return_type> *path)
{
	fprintf(stderr, "[\n");
	for (int i = 0; i < (*path).size(); i++)
	{
		debug((*path)[i].stmt);
		fprintf(stderr, ",\n");
	}
	fprintf(stderr, "]\n");
	//fprintf(stderr, "	function ->%s in loc %d \n", IDENTIFIER_POINTER(DECL_NAME((*path)[i].first.fndecl)), LOCATION_LINE((*path)[i].second));
}

void print_function_return2(tree function_tree)
{
	if (function_return_collect->get(function_tree) == NULL)
		return;
	function_return_array fun_array = *(function_return_collect->get(function_tree));

	vector<return_type> ret_type_array = fun_array.return_type_array;
	//debug_tree(function_tree);
	//vector<pair<fdecl,location_t>> loc;
	fprintf(stderr, "=======print_function_return %d   %d========\n", function_tree, ret_type_array.size());
	fprintf(stderr, "[\n");
	for (int i = 0; i < ret_type_array.size(); i++)
	{

		// for (int i = 0; i < (ret_type_array).size(); i++)
		// {
		debug((ret_type_array)[i].stmt);
		fprintf(stderr, ",\n");
		// }
		// fprintf(stderr, "]\n");
		// print_function_path(&ret_type_array);
	}
	fprintf(stderr, "]\n");
}
void print_function_return(tree function_tree)
{
	if (function_return_collect->get(function_tree) == NULL)
		return;
	function_return_array fun_array = *(function_return_collect->get(function_tree));

	vector<return_type> ret_type_array = fun_array.return_type_array;
	//debug_tree(function_tree);
	//vector<pair<fdecl,location_t>> loc;
	fprintf(stderr, "=======print_function_return %d   %d========\n", function_tree, ret_type_array.size());
	fprintf(stderr, "[\n");
	for (int i = 0; i < ret_type_array.size(); i++)
	{

		// for (int i = 0; i < (ret_type_array).size(); i++)
		// {
		debug((ret_type_array)[i].stmt);
		fprintf(stderr, ",\n");
		// }
		// fprintf(stderr, "]\n");
		// print_function_path(&ret_type_array);
	}
	fprintf(stderr, "]\n");
}
void collect_function_return(gimple *gc, cgraph_node *node, basic_block bb)
{

	fprintf(stderr, "GIMPLE_RETURN\n");
	function_return_array fun_array;
	tree get_function_return_tree = gimple_return_retval(as_a<greturn *>(gc));
	vector<return_type> ret_type_array;

	fun_array.return_type_array = ret_type_array;
	struct return_type ret_type;
	ret_type.stmt = gc;
	ret_type.return_tree = get_function_return_tree;
	// ret_type.reutnr_type_num = 0;
	fun_array.return_type_array.push_back(ret_type);
	function_return_collect->put(node->get_fun()->decl, fun_array);

	//debug_tree(get_function_return_tree);

	//tree test = cfun->decl;
	//var_points_to  get_function_return =*(function_return_collect->get(get_function_return_tree));
}
void printfBasicblock()
{

	// set_gimple_array(used_stmt, use_stmt, fundecl, fundecl);
	gimple *u_stmt;
	gimple_array start;
	start.stmt = NULL;
	gimple_array *used_stmt = &start;
	ptb *table_temp = ptable;
	tree t;
	basic_block bb;
	//
	struct cgraph_node *node;
	FOR_EACH_DEFINED_FUNCTION(node)
	{
		if (!ipa)
			init_table();
		push_cfun(node->get_fun());
		// if (strcmp(get_name(cfun->decl), "main") == 0)
		fprintf(stderr, "=======Mapping node_fun:%s=========\n", get_name(cfun->decl));
		debug_tree(cfun->decl);
		if (cfun == NULL)
			continue;
		calculate_dominance_info(CDI_DOMINATORS);
		FOR_EACH_BB_FN(bb, cfun)
		{
			debug_bb(bb);
		}
		pop_cfun();
	}
}

void checkPointerConstraint(tree function_tree, ptb *ptable, gimple_array *user_tmp, tree checkTeee)
{
	gimple *u_stmt;
	gimple_array start;
	start.stmt = NULL;
	gimple_array *used_stmt = &start;
	ptb *table_temp = ptable;
	tree t;
	//
	fprintf(stderr, "\033[40;42m =======pre_check_funciton:%s========= \033[0m\n", get_name(function_tree));
	FOR_EACH_TABLE(table_temp, t)
	{
		int ptable_type = 0;
		int childptable_type = 0;
		int find_phistmt = 0;
		int find_freestmt = 0;
		int find_mallocstmt = 0;

		// fprintf(stderr, "\n ------------------------------------------\n");
		// debug_tree(table_temp->target);
		// debug_tree(function_tree);
		// debug_tree(table_temp->node->get_fun()->decl);
		//  fprintf(stderr, "%d\n",int(table_temp->node->get_fun()->decl==function_tree));
		// debug_tree(function_tree);
		// fprintf(stderr, "\033[40;42m =======check _ addddddd:%s========= \033[0m\n",get_name(function_tree));
		//過濾所有 ptable 判斷 裡面的 node 是否為 比對的 fucntion tree
		if (table_temp->node->get_fun()->decl == function_tree)
			if (table_temp->target != NULL)
			{
				find_freestmt = find_mallocstmt = find_phistmt = 0;
				debug_tree(function_tree);
				fprintf(stderr, "\n======================================================================\n");
				// debug_tree(function_tree);
				const char *name;
				debug_tree(table_temp->target);
				gimple *def_stmt = SSA_NAME_DEF_STMT(table_temp->target);
				if (gimple_code(def_stmt) == GIMPLE_CALL)
				{
					name = get_name(gimple_call_fndecl(def_stmt));
					// fprintf(stderr, "GIMPLE CODE :addr_expr---%s-----\n", name);
				}
				if (
					!strcmp(name, "malloc") ||
					!strcmp(name, "xmalloc") ||
					!strcmp(name, "calloc") ||
					!strcmp(name, "xcalloc") ||
					!strcmp(name, "strdup"))
				{
					ptable_type = 1;
					debug(def_stmt);
					// find_mallocstmt=1;
					fprintf(stderr, "this Reserved word function ------%s-----\n", name);
					// fprintf(stderr, "GIMPLE CODE :addr_expr---%s-----\n", name);
				}
				else
				{
					ptable_type = 2;
					fprintf(stderr, "this other function ------%s-----\n", name);
				}

				fprintf(stderr, "\n======================================================================\n");
				user_tmp = treeGimpleArray->get(table_temp->target);
				tree userStart = NULL;
				start.stmt = NULL;
				used_stmt = &start;
				if (user_tmp != NULL)
				{

					fprintf(stderr, " \n start check Pointer Collect  \n");
					fprintf(stderr, "\n======================================================================\n");
					if (user_tmp->size > 0)
						FOR_EACH_USE_TABLE(user_tmp, u_stmt)
						{
							if (user_tmp->ret_stmt != NULL)
							{
								warning_at(gimple_location(u_stmt), 0, "use location");
								if (gimple_code(user_tmp->ret_stmt) == GIMPLE_RETURN)
								{
									find_mallocstmt = -1;
									continue;
								}
							}
							else
							{
								debug(u_stmt);

								warning_at(gimple_location(u_stmt), 0, "use location");

								gimple *use_stmt = SSA_NAME_DEF_STMT(user_tmp->target);

								if (gimple_code(u_stmt) == GIMPLE_CALL)
								{
									name = get_name(gimple_call_fndecl(u_stmt));
									fprintf(stderr, "this other function---%s-----\n", name);
								}
								if (gimple_code(u_stmt) == GIMPLE_PHI)
								{
									find_phistmt = 1;
									fprintf(stderr, "this stmt have mutiple branch ---%s-----\n", name);
								}
								if (name != NULL)
									if (
										!strcmp(name, "malloc") ||
										!strcmp(name, "xmalloc") ||
										!strcmp(name, "calloc") ||
										!strcmp(name, "xcalloc") ||
										!strcmp(name, "strdup"))
									{
										find_mallocstmt = 1;
									}
									else
									{

										find_mallocstmt = 2;
									}

								if (checkTeee != NULL && gimple_code(u_stmt) == GIMPLE_CALL && find_mallocstmt == 2)
								{

									tree gettreefucntionarg = TREE_OPERAND(gimple_call_fn(def_stmt), 0);
									name = get_name(gimple_call_fn(def_stmt));

									if (checkTeee == gettreefucntionarg)
									{
										find_mallocstmt = 1;

										fprintf(stderr, "\n ================== warring ================== \n");
										// sfprintf(stderr, "function return value related stmt \n");
										debug(checkTeee);
										fprintf(stderr, "\033[40;35m    function return value related stmt \033[0m\n");
										fprintf(stderr, "\033[40;35m    this stmt possible is heap-object 。 \033[0m\n");

										// fprintf(stderr, "this stmt possible is heap-object 。\n");
										fprintf(stderr, "\n ================== warring ================== \n");

										if (gimple_code(u_stmt) == GIMPLE_CALL)
										{
											name = get_name(gimple_call_fn(u_stmt));
											if (!strcmp(name, "free") || !strcmp(name, "xfree"))
											{
												find_freestmt++;

												fprintf(stderr, "\n ================== find ================== \n");
												debug(u_stmt);
												warning_at(gimple_location(u_stmt), 0, "use location");
												fprintf(stderr, "\033[40;32m    HAS FREE STMT count:%d name:%s \033[0m\n", find_freestmt, name);
												fprintf(stderr, "\n ================== find ================== \n");
											}
											else
											{
												fprintf(stderr, "\n ================== trace ================== \n");
												fprintf(stderr, "trace fucntion name:%s \n", name);
												fprintf(stderr, "\n ================== trace ================== \n");
											}
										}
									}
									else
									{
										find_mallocstmt = 0;
									}
								}
								else if ((checkTeee == NULL && gimple_code(u_stmt) == GIMPLE_CALL) && (find_mallocstmt == 1 || find_mallocstmt == 2))
								{

									find_mallocstmt = 1;

									// gimple *def_stmt = SSA_NAME_DEF_STMT(user_tmp->target);
									// fprintf(stderr, "NEWX FUCNTIONMWEQMEQWP: \n");
									gimple *def_stmt = SSA_NAME_DEF_STMT(user_tmp->target);
									debug(def_stmt);
									const char *name;
									name = get_name(gimple_call_fn(u_stmt));
									if (name != NULL)
										if (!strcmp(name, "free") || !strcmp(name, "xfree"))
										{
											find_freestmt++;
											fprintf(stderr, "\n ================== find ================== \n");
											debug(u_stmt);
											warning_at(gimple_location(u_stmt), 0, "use location");
											fprintf(stderr, "\033[40;32m    HAS FREE STMT count:%d name:%s \033[0m\n", find_freestmt, name);
											fprintf(stderr, "\n ================== find ================== \n");
										}
										else
										{
											fprintf(stderr, "\n ================== trace ================== \n");
											fprintf(stderr, "trace fucntion name:%s \n", name);
											fprintf(stderr, "\n ================== trace ================== \n");
										}
								}
							}
						}
				}
				else
				{
					find_mallocstmt = -2;
				}
				//you are Reserved word function so always check
				if (ptable_type == 1 && find_mallocstmt == 2)
					find_mallocstmt = 1;
				if (find_mallocstmt == 1)
				{

					if (find_freestmt == 0)
					{
						fprintf(stderr, "\n======================================================================\n");
						// fprintf(stderr, "	no free stmt possible memory leak\n");
						fprintf(stderr, "\033[40;31m    no free stmt possible memory leak \033[0m\n");
						fprintf(stderr, "\n======================================================================\n\n");
					}
					else if (find_freestmt == 1)
					{

						fprintf(stderr, "\n======================================================================\n");

						if (find_phistmt == 1)
							fprintf(stderr, "\033[40;31m   need check branch because multiple direction varible\033[0m\n");
						else
							fprintf(stderr, "\033[40;32m   May your no memory leak .... need more check \033[0m\n");
						// 	fprintf(stderr, "\033[40;32m    NO memory leak \033[0m\n");
						fprintf(stderr, "\n======================================================================\n\n");
					}
					else if (find_freestmt >= 2)
					{
						fprintf(stderr, "\n======================================================================\n");
						// fprintf(stderr, "	possible double free\n");
						fprintf(stderr, "\033[40;31m  	possible double free \033[0m\n");
						fprintf(stderr, "\n======================================================================\n\n");
					}
				}
				else if (find_mallocstmt == -1)
				{
					fprintf(stderr, "\n======================================================================\n");
					// fprintf(stderr, "	this fucntion return possible has heap-object\n");
					fprintf(stderr, "\033[40;31m 	this fucntion return possible is heap-object \033[0m\n");
					fprintf(stderr, "\n======================================================================\n\n");
				}
				else if (find_mallocstmt == -2)
				{
					fprintf(stderr, "\n======================================================================\n");
					// fprintf(stderr, "	this fucntion return possible has heap-object\n");
					fprintf(stderr, "\033[40;31m 	Can't find this fucntion in Pointer table \033[0m\n");
					debug_tree(table_temp->target);
					// if (checkTeee != NULL)
					// {
					// 	fprintf(stderr, "\033[40;31m 	Possible check from Callee \033[0m\n");
					// 	debug_tree(checkTeee);
					// }
					fprintf(stderr, "\n======================================================================\n\n");
				}
				else
				{
					fprintf(stderr, "\n======================================================================\n");
					// fprintf(stderr, "	this stmt need double check\n");

					fprintf(stderr, "\033[40;31m 	this stmt need double check \033[0m\n");
					fprintf(stderr, "\n======================================================================\n\n");
				}
			}
	}
}
void print_function_path(tree function_tree, int fucntion_level, ptb *ptable, gimple_array *user_tmp)
{
	if (function_path_collect->get(function_tree) == NULL)
		return;
	function_path_array fun_array = *(function_path_collect->get(function_tree));

	vector<function_path> function_path_array = fun_array.function_path_array;
	//debug_tree(function_tree);
	//vector<pair<fdecl,location_t>> loc;
	if (function_return_collect->get(function_tree) == NULL)
		return;
	function_return_array callerFunArray = *(function_return_collect->get(function_tree));
	vector<return_type> callerRetTypearray = callerFunArray.return_type_array;
	fprintf(stderr, "\033[40;44m =======print_function_path %s  function_call count: %d level :%d========  \033[0m\n", get_name(function_tree), function_path_array.size(), fucntion_level);
	fprintf(stderr, "\033[40;44m =======print_function_type %d  ========  \033[0m\n", callerFunArray.return_type_num);

	fucntion_level += 1;

	checkPointerConstraint(function_tree, ptable, user_tmp, NULL);
	fprintf(stderr, "\033[40;44m =======print_function_path %s  function_call count: %d level :%d========  \033[0m\n", get_name(function_tree), function_path_array.size(), fucntion_level);
	int find = 0;
	for (int i = 0; i < function_path_array.size(); i++)
	{
		find = 0;
		fprintf(stderr, "\033[40;42m =======pre add _ fucntion:%s========= \033[0m\n", get_name((function_path_array)[i].next));
		for (int o = 0; o < pathStack.size(); o++)
		{

			if (pathStack.c[o] == (function_path_array)[i].next)
			{
				find = 1;

				fprintf(stderr, "\033[40;41m =======recursive_fun:%s========= \033[0m\n", get_name(pathStack.c[o]));
				// pathStack.push((function_path_array)[i].next);
				break;
			}
		}
		// if(find ==1)
		// continue;
		if (find == 0)
		{
			fprintf(stderr, "\033[40;46m =======add node_fun stack:%s========= \033[0m\n", get_name((function_path_array)[i].next));

			pathStack.push((function_path_array)[i].next);
			// int find_type=0;
			if (function_return_collect->get((function_path_array)[i].next) != NULL)
			{

				function_return_array calleeFunArray = *(function_return_collect->get((function_path_array)[i].next));
				// vector<return_type> calleeRetTypearray = callerFunArray.return_type_array;
				fprintf(stderr, "\033[40;44m =======print_function_type %d  ========  \033[0m\n", calleeFunArray.return_type_num);

				// fprintf(stderr, "=======print_function_type %d  ========\n", calleeFunArray.return_type_num);
				if (calleeFunArray.return_type_num == 2)
				{
					for (int k = 0; k < callerRetTypearray.size(); k++)
					{
						if ((callerRetTypearray)[k].return_tree == (function_path_array)[i].next)
						{
							checkPointerConstraint(function_tree, ptable, user_tmp, (callerRetTypearray)[k].return_tree);
							break;
						}
					}
				}
			}
			print_function_path((function_path_array)[i].next, fucntion_level, ptable, user_tmp);
			fprintf(stderr, "\033[40;33m =======POP node_fun stack:%s========= \033[0m\n", get_name(pathStack.top()));
			pathStack.pop();
		}
	}
}

void dump_fucntion(cgraph_node *node, ptb *ptable, gimple_array *user_tmp)
{

	cgraph_edge *e;
	FOR_EACH_DEFINED_FUNCTION(node)
	{

		int fucntion_level = 0;

		push_cfun(node->get_fun());

		// fprintf(stderr, "=======node_fun:%s=========\n", get_name(cfun->decl));

		if (cfun == NULL)
			continue;
		if (!strcmp(get_name(cfun->decl), "main"))
		{
			fprintf(stderr, "\033[40;44m =======node_fun:%s========= \033[0m\n", get_name(cfun->decl));
			// fprintf(stderr, "=======node_fun:%s=========\n", get_name(cfun->decl));
			enum availability avail;
			fprintf(stderr, "\033[40;44m fucntion collect path  \033[0m\n");
			// fprintf(stderr, "fucntion collect path \n");
			pathStack.push(cfun->decl);

			print_function_path(cfun->decl, fucntion_level, ptable, user_tmp);
			fprintf(stderr, "\033[40;33m =======POP node_fun stack:%s========= \033[0m\n", get_name(pathStack.top()));
			pathStack.pop();
		}
		pop_cfun();
	}
	fprintf(stderr, "fucntion collect path finsh\n");
}
void print_function_path2(tree function_tree, int fucntion_level, ptb *ptable, gimple_array *user_tmp)
{
	if (function_path_collect->get(function_tree) == NULL)
		return;
	function_path_array fun_array = *(function_path_collect->get(function_tree));

	vector<function_path> function_path_array = fun_array.function_path_array;
	//debug_tree(function_tree);
	//vector<pair<fdecl,location_t>> loc;
	if (function_return_collect->get(function_tree) == NULL)
		return;
	function_return_array callerFunArray = *(function_return_collect->get(function_tree));
	vector<return_type> callerRetTypearray = callerFunArray.return_type_array;

	fprintf(stderr, "=======print_function_path %s  function_call count: %d level :%d========\n", get_name(function_tree), function_path_array.size(), fucntion_level);
	fprintf(stderr, "=======print_function_type %d  ========\n", callerFunArray.return_type_num);
	fucntion_level += 1;
	fprintf(stderr, "[\n");

	// checkPointerConstraint(function_tree,ptable,user_tmp);

	fprintf(stderr, "=======print_function_path %s  function_call count: %d level :%d========\n", get_name(function_tree), function_path_array.size(), fucntion_level);
	// pathStack.push(function_tree);
	for (int i = 0; i < function_path_array.size(); i++)
	{

		int find = 0;

		for (int o = 0; o < pathStack.size(); o++)
		{

			fprintf(stderr, "=======now node_fun stack:%s=========\n", get_name(pathStack.c[o]));
			if (pathStack.c[o] == (function_path_array)[i].next)
			{
				find = 1;
				fprintf(stderr, "				=======recursive_fun:%s=========\n", get_name(pathStack.c[o]));
			}
		}

		if (find == 0)
		{
			fprintf(stderr, "=======add node_fun stack:%s=========\n", get_name((function_path_array)[i].next));
			// debug_tree((function_path_array)[i].next);
			pathStack.push((function_path_array)[i].next);
			print_function_path((function_path_array)[i].next, fucntion_level, ptable, user_tmp);
			pathStack.pop();
		}
	}

	fprintf(stderr, "]\n");
}

void tracefree_fucntion(cgraph_node *node, ptb *ptable, gimple_array *user_tmp)
{

	cgraph_edge *e;
	FOR_EACH_DEFINED_FUNCTION(node)
	{

		int fucntion_level = 0;

		push_cfun(node->get_fun());

		fprintf(stderr, "=======node_fun:%s=========\n", get_name(cfun->decl));

		if (cfun == NULL)
			continue;
		enum availability avail;

		fprintf(stderr, "fucntion collect path \n");
		pathStack.push(cfun->decl);

		print_function_path(cfun->decl, fucntion_level, ptable, user_tmp);

		pathStack.pop();
		pop_cfun();
	}
	fprintf(stderr, "fucntion collect path finsh\n");
}
void record_fucntion(cgraph_node *node)
{

	cgraph_edge *e;
	FOR_EACH_DEFINED_FUNCTION(node)
	{
		// push_cfun(node->get_fun());
		// if (!ipa)
		// 	init_table();
		push_cfun(node->get_fun());
		// if (strcmp(get_name(cfun->decl), "main") == 0)
		// debug_tree(cfun->decl);
		//tree test=DECL_SAVED_TREE(cfun->decl);
		//analyze_func_body(DECL_SAVED_TREE(test), 0);
		if (cfun == NULL)
			continue;
		fprintf(stderr, "=======node_fun:%s=========\n", get_name(cfun->decl));
		enum availability avail;

		fprintf(stderr, "fucntion collect \n");
		function_path_array fun_array;
		//tree get_function_return_tree = gimple_return_retval(as_a<greturn *>(gc));
		vector<function_path> function_path_array;
		fun_array.function_path_array = function_path_array;

		for (e = node->callees; e; e = e->next_callee)
		{
			//funct_state l;
			cgraph_node *caller = e->caller->global.inlined_to ? e->caller->global.inlined_to : e->caller;
			cgraph_node *callee = e->callee->ultimate_alias_target(&avail, caller);

			fprintf(stderr, "=======child node_fun:%s=========\n", get_name(callee->decl));
			if (callee != NULL)
			{
				struct function_path path_type;
				path_type.cgnext = callee;
				path_type.next = callee->decl;
				fun_array.function_path_array.push_back(path_type);

				//pop_cfun();
				//		dump_fucntion(callee);
			}

			//analyze_function=	analyze_function (caller ,true);
		}
		function_path_collect->put(node->get_fun()->decl, fun_array);
		pop_cfun();
	}
}
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
	tree attr;
	function *ofun;
	function *fn;
	basic_block bb;
	ipa = true;
	cgraph_edge *e;
	tvpt = new hash_map<tree, var_points_to>;
	treeGimpleArray = new hash_map<tree, gimple_array>;
	function_return_collect = new hash_map<tree, function_return_array>;
	function_assign_collect = new hash_map<tree, function_assign_array>;
	function_path_collect = new hash_map<tree, function_path_array>;
	function_free_collect = new hash_map<tree, function_free_array>;
	function_graph_collect = new hash_map<tree , function_graph_array>;
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
	// fprintf(stderr,"%d\n",*tmp);
	//cout << "----------------------------------------" <<tmp<<endl;
	FOR_EACH_DEFINED_FUNCTION(node)
	{
		if (!ipa)
			init_table();
		push_cfun(node->get_fun());
		// if (strcmp(get_name(cfun->decl), "main") == 0)

		//tree test=DECL_SAVED_TREE(cfun->decl);
		//analyze_func_body(DECL_SAVED_TREE(test), 0);
		if (cfun == NULL)
			continue;
		fprintf(stderr, "=======node_fun:%s=========\n", get_name(cfun->decl));
		debug_tree(cfun->decl);
		enum availability avail;
		for (e = node->callees; e; e = e->next_callee)
		{
			//funct_state l;
			cgraph_node *caller = e->caller->global.inlined_to ? e->caller->global.inlined_to : e->caller;
			cgraph_node *callee = e->callee->ultimate_alias_target(&avail, caller);
			fprintf(stderr, "=======child node_fun:%s=========\n", get_name(callee->decl));
			//analyze_function=	analyze_function (caller ,true);
		}
		// if (strcmp(get_name(cfun->decl), "main") == 0)
		// 	main_fun = cfun;
		// fprintf(stderr,"=======node_fun:%s=========\n",get_name(cfun->decl));

		/*initialization DFS graph*/
		Graph DFS;
		DFS.init_Graph(cfun->cfg->x_last_basic_block);
		
		/*calculate dominator tree*/
		calculate_dominance_info(CDI_DOMINATORS);

		/*create DFS graph, Algorithm 1 and 2*/
		FOR_EACH_BB_FN(bb, cfun)
		{
			debug_bb(bb);
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
				if (gimple_code(gc) == GIMPLE_RETURN)
				{

					collect_function_return(gc, node, bb);
					//				//struct return_type type_struct ;
					//get_function_return.return_type_array.push_back(type_struct);

					//type_struct.stmt = gc;
					//	type_struct.return_tree = get_function_return_tree;
					// if( get_function_return.return_type_array.size() == 0){
					// 	fprintf(stderr,"hwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwas\n");
					// }
					// else{
					// 	fprintf(stderr,"no\n");
					// }
					//function_return_array get_function_return = *(function_return_collect->get(cfun->decl));

					//	get_function_return->return_type_array.push_back(type_struct);
					//
					//function_return_collect->put(cfun->decl,*get_function_return);
					//get_function_return->return_type_array->stmt=gc;
				}

				// debug_gimple_stmt(gc);
				if (is_gimple_call(gc))
				{

					/*collect malloc and free information*/
					collect_function_call(gc, node, bb);

					// fprintf(stderr, "add collect_function_call\n");
				}
				// print_function_return(cfun->decl);
			}
		}
		/*DFS of this function put set "fDfS" */
		// fDFS->put(node, DFS);

		function_graph_array fun_graph_array;
		fun_graph_array.graph_type_array = DFS; 
		fun_graph_array.graph_node = node ;
		function_graph_collect->put(node->get_fun()->decl, fun_graph_array);

		if (!ipa)
		{
			// new_memory_leak_analysis (ptable,ftable);
			// new_double_free_analysis(ptable,ftable);
			// new_use_after_free_analysis(ptable, ftable);
		}
		// fprintf(stderr, "=======node_fun:%s=========\n", get_name(cfun->decl));
		// print_function_return(cfun->decl);
		pop_cfun();
	}

	if (ipa)
	{

		// new_memory_leak_analysis(ptable, ftable);

		// new_double_free_analysis(ptable,ftable);
		// PointerConstraint(ptable, ftable);
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
		fprintf(stderr, "=======node_fun:%s=========\n", get_name(cfun->decl));
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
			always_inline = (DECL_DISREGARD_INLINE_LIMITS(callee->decl) && lookup_attribute("noinline", DECL_ATTRIBUTES(callee->decl)));
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
			//debug_bb(bb);
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
						//noinline
						//always_inline
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