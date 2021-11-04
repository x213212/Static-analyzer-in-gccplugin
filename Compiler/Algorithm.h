#include "system.h"
#include "config.h"
#include "DFS.h"
#include <list>
#include "cfgloop.h"
#include <vector>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fstream>
#include <iostream>
#include <stack>
#include <string.h>
#include <math.h>
#include "define.h"
#include "unit.h"
#include "debug.h"
#include "gimplearray.h"
#include "getinfo.h"
#include "trace.h"
#include "collect.h"
#include "mapping.h"
#include "performance.h"
#include "tree-cfg.h"
#include "stmt.h"
#include "pretty-print.h"
#include "dumpfile.h"
#include "new_imm_use.h"
#include "check.h"
#include "analyze.h"
using namespace std;

// static int totalsize; //宣告一個整數型態size變數，用來儲存x的位元組大小
// static int levelsize = 0;
// static tree now_tree;

// bool bb_in_branch_p(tree a, tree b)
// {
// 	return dominated_by_p(CDI_DOMINATORS, a, a);
// }


/*Pointer Constraint  5*/
void PointerConstraint(ptb *ptable, ptb *ftable)
{
	gimple *u_stmt;
	tree t;
	struct ptr_info_def *pi1;
	struct pt_solution *pt1;
	struct ptr_info_def *pi2;
	struct pt_solution *pt2;

	gimple_array *used_stmt = NULL;
	gimple_array start;
	start.stmt = NULL;
	used_stmt = &start;
	bool to_remove = false;
	int Entrypoint = 0;

	fprintf(stderr, "start PointerConstraint\n");
	fprintf(stderr, "pointer ftable is %d \n", ftable->size);
	fprintf(stderr, "pointer ptable is %d \n", ptable->size);
	fprintf(stderr, "===============The second stage : Mapping stmt=================\n");
	FunctionStmtMappingAssign(ptable, used_stmt);

	ptb *table1 = ptable;

	// ptb *table3 = ftable;
	// table3 = ptable;
	// preproceess

	fprintf(stderr, "start collect similar stmtstart collect similar stmtstart collect similar stmtstart collect similar stmt\n");
	fprintf(stderr, "start collect similar stmtstart collect similar stmtstart collect similar stmtstart collect similar stmt\n");
	fprintf(stderr, "start collect similar stmtstart collect similar stmtstart collect similar stmtstart collect similar stmt\n");
	fprintf(stderr, "start collect similar stmtstart collect similar stmtstart collect similar stmtstart collect similar stmt\n");
	fprintf(stderr, "start collect similar stmtstart collect similar stmtstart collect similar stmtstart collect similar stmt\n");
	struct timespec astart, aend;
	clock_gettime(CLOCK_MONOTONIC, &astart);
	// printfunctionCollect(ptable, used_stmt);
	// return  ;
	tree t2 = NULL_TREE;
	// printfBasicblock();
	if (table1->size > 0)
	{
		// debug_tree(table1->next->target);
		int count = 0;

		while (table1->next != NULL)
		{
			// if (table1->next == NULL)
			// 	break;
			// count ++;
			// debug_tree(table1->target);

			// debug_tree(table1->target);
			// debug_tree(table1->target);
			// debug_gimple_stmt(table1->last_stmt);
			// if(t2 != NULL_TREE)
			// if( SSA_NAME_CHECK   (table1->target) )
			// continue;
			// fprintf(stderr, "start collect similar stmtstart collect similar stmtstart collect similar stmtstart collect similar stmt\n");}
			if (TREE_CODE(TREE_TYPE(table1->target)) == METHOD_TYPE || TREE_CODE(TREE_TYPE(table1->target)) == FUNCTION_TYPE || TREE_CODE(TREE_TYPE(table1->target)) == RECORD_TYPE || !(TREE_CODE(table1->target) == SSA_NAME))
			{
				table1 = table1->next;
				continue;
			}
			// if (table1->removed || !TREE_CODE(t) == SSA_NAME)
			// 	continue;
			if (TREE_CODE(table1->target) == INTEGER_CST)
			{
				table1 = table1->next;
				continue;
			}
			// debug_tree(t2);

			int colectCount = 0;
			used_stmt = NULL;
			gimple_array start;
			start.stmt = NULL;
			used_stmt = &start;

			const char *name;

			gimple *def_stmt = SSA_NAME_DEF_STMT(table1->target);
			levelsize = 0;
			// debug_gimple_stmt(def_stmt);
			if (gimple_code(def_stmt) != GIMPLE_NOP || gimple_code(def_stmt) != ADDR_EXPR)
			{
				int pass = 0;
				if (gimple_code(def_stmt) == GIMPLE_CALL)
					if (TREE_CODE(gimple_call_fn(def_stmt)) != MEM_REF)
						pass = 1;

				if (gimple_code(def_stmt) == GIMPLE_ASSIGN)
					if (TREE_CODE(gimple_assign_rhs1(def_stmt)) != MEM_REF && TREE_CODE(gimple_assign_rhs1(def_stmt)) != VAR_DECL)
						// debug_tree(gimple_assign_rhs1(def_stmt));
						pass = 1;

				if (TREE_CODE(table1->target) != ADDR_EXPR && pass == 1)
					if (def_stmt)
					{
						// 	debug_gimple_stmt(def_stmt);
						// if(gimple_assign_rhs1(def_stmt))
						// 	debug_tree(gimple_assign_rhs1(def_stmt));

						// fprintf(stderr, "qwdqwdqwdqwdqwdqwdqwdqwdqwdqwdqwdqwdqwdqwdqwdqwdqwdqwdqwdqwdqwdqwdqwdqw%ddqwdqwdqwdqwd\n");
						if (gimple_call_fn(def_stmt) && gimple_call_fndecl(def_stmt))
						{
							// debug_tree(gimple_call_fn(def_stmt));
							name = get_name(gimple_call_fn(def_stmt));

							if (name != NULL)

								if (
									strcmp(name, "malloc"))

								{
									// fprintf(stderr, "qwdqwdqwdqwdqwdqwdqwdqwdqwdqwdqwdqwdqwdqwdqwdqwdqwdqwdqwdqwdqwdqwdqwdqw%ddqwdqwdqwdqwd\n");
									Prenew_search_imm_use(used_stmt, table1->target, table1->target);
								}
						}
					}
			}
			now_tree = table1->target;
			new_search_imm_use(used_stmt, table1->target, table1->target);
			set_gimple_array(used_stmt, table1->last_stmt, table1->target, table1->target, NULL);
			// pi1 = SSA_NAME_PTR_INFO(table1->target);
			// pt1 = &pi1->pt;
			// if (pt1 == NULL)
			// 	continue;
			// if (pt1->vars == NULL)
			// 	continue;

			if (!strcmp(get_tree_code_name(TREE_CODE(used_stmt->target)), "<invalid tree code>"))
			{
				table1 = table1->next;
				continue;
			}
			// fprintf(stderr, "check stmt\n");

			while (new_gimple_array.size())
			{
				colectCount++;
				// fprintf(stderr, "check stmt\n");
				// debug(stmtStack.top());
				new_gimple_array.pop_back();
			}
			while (new_gimpletree_array.size())
			{
				// colectCount++;
				// fprintf(stderr, "check stmt\n");
				// debug(stmtStack.top());
				new_gimpletree_array.pop_back();
			}
			// table1->size
			// debug_tree(used_stmt->target);
			// table1->size=colectCount ;
			// fprintf(stderr, "---------------------------%d-rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n",table1->size);
			// debug_tree(table1->target);
			// tree teewtetqq;
			// size = sizeof(teewtetqq);
			// fprintf(stderr, " used_stmt array size of : %d\n", totalsize);
			// totalsize =0 ;
			// FOR_EACH_USE_TABLE(used_stmt, u_stmt)
			// {

			// 	// pi2 = SSA_NAME_PTR_INFO(used_stmt->target);
			// 	// pt2 = &pi2->pt;
			// 	// debug_tree(used_stmt->target);
			// 	// if()
			// 	// if (pt2 == NULL)
			// 	// 	continue;
			// 	// if (pt2->vars == NULL)
			// 	// 	continue;
			// 	// debug(used_stmt->target);
			// 	// fprintf(stderr, "%s\n", get_tree_code_name(TREE_CODE(used_stmt->target)));
			// 	// debug_tree(table1->target);

			// 	// if (bitmap_intersect_p(pt1->vars, pt2->vars))
			// 	// {

			// 	// fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
			// 	// fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
			// 	// fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
			// 	// fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
			// 	// fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
			// 	// fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
			// 	// fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
			// 	// fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
			// 	// 	gimple_array *user_tmp2;
			// 	// 	user_tmp2 = tvpt2->get(table1->target);
			// 	// 	if (user_tmp2 != NULL)
			// 	// 		break;

			// 	// 	tvpt2->put(table1->target, *used_stmt);
			// 	// 	break;
			// 	// }
			// 	// debug_tree(table1->target);
			// 	// debug_gimple_stmt(table1->last_stmt);
			// 	if (ptr_derefs_may_alias_p(used_stmt->target, table1->target))
			// 	{
			// 		debug_tree(used_stmt->target);
			// 		debug_tree(table1->target);
			// 		debug_gimple_stmt(table1->last_stmt);
			// 		fprintf(stderr, "filterstart\n\n");
			// 		// if (gimple_location_safe_safe(table1->last_stmt))
			// 		// {
			// 		// 	location_t loc = gimple_location_safe_safe(table1->last_stmt);
			// 		// 	loc = gimple_location_safe(table1->last_stmt);
			// 		// 	debug(table1->last_stmt);
			// 		// 	warning_at(gimple_location_safe_safe(table1->last_stmt), 0, "use location");
			// 		// 	// if (LOCATION_LINE(loc) != 171)
			// 		// 	// {

			// 		// 	// 	continue;
			// 		// 	// }
			// 		// }
			// 		// else
			// 			continue;
			// 		fprintf(stderr, "filter\n\n");

			// 		// fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
			// 		// fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
			// 		// fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
			// 		// fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
			// 		// fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
			// 		// fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
			// 		// fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
			// 		// fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");

			// 		if (treeGimpleArray->get(table1->target) != NULL)
			// 			break;
			// 		// fprintf(stderr, "check stmt\n");
			// 		// debug_tree(used_stmt->target);
			// 		// debug_tree(table1->target);
			// 		treeGimpleArray->put(table1->target, *used_stmt);
			// 		Entrypoint++;
			// 	}
			// }

			if (treeGimpleArray->get(table1->target) != NULL)
			{
				table1 = table1->next;
				continue;
			}
			// fprintf(stderr, "check stmt\n");
			// debug_tree(used_stmt->target);
			// debug_tree(table1->target);
			treeGimpleArray->put(table1->target, *used_stmt);
			Entrypoint++;
			// if(table1->next == NULL)
			// 	fprintf(stderr, "weritjroti;ejhoerjhio;rtjhojert\n");
			// break;
			// debug(table1->next->target);
			if (table1->next->target == NULL)
				break;
			else
			{
				// debug_tree(table1->next->target );
				table1 = table1->next;
				// debug_tree(table1->next->target );
			}
		}
		// FOR_EACH_TABLE(table1, t2)
		// {
		// }
	}
	// FunctionStmtMappingAssign(ptable, used_stmt);
	FunctionStmtMappingRet(ptable, ftable, used_stmt);
	fprintf(stderr, "===============The second stage : build fucntion type=================\n");
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

	fprintf(stderr, "===============The second stage : record fucntion =================\n");
	// printfunctionCollect(ptable, used_stmt);
	// printfPointerConstraint(ptable, used_stmt);
	// printfBasicblock();
	// initMallocfunction

	// // printfBasicblock();
	// //printfunctionCollect2(ptable, used_stmt);
	// // printfPointerConstraint2(ptable, used_stmt);
	// // printfunctionCollect2(ptable, used_stmt);
	struct cgraph_node *node;
	record_fucntion(node);
	clock_gettime(CLOCK_MONOTONIC, &aend);
	struct timespec temp = diff(astart, aend);
	double time_used;
	time_used = temp.tv_sec + (double)temp.tv_nsec / 1000000000.0;
	fprintf(stderr, "\n=============== The third stage : Start detection  =================\n");
	dump_fucntion(node, ptable, used_stmt);
	fprintf(stderr, "\n=============== The third stage : detection  End=================\n");
	clock_gettime(CLOCK_MONOTONIC, &aend);
	temp = diff(astart, aend);
	double time_used2;
	time_used2 = temp.tv_sec + (double)temp.tv_nsec / 1000000000.0;

	printfBasicblock();

	fprintf(stderr, "\033[40;32mSTART CHECKSTART CHECKSTART CHECKSTART CHECKSTART CHECK\033[0m\n");
	fprintf(stderr, "    =()=\n");
	fprintf(stderr, " ,/'\_||_\n");
	fprintf(stderr, "  (___  `.\n");
	fprintf(stderr, " ./  `=='\n");
	fprintf(stderr, "                   \n");
	fprintf(stderr, "    ~~~~~~~~~~~  \n");
	fprintf(stderr, " ~~~~~~\n");
	fprintf(stderr, "           ~~~~~~~\n");
	fprintf(stderr, "\033[40;34m    find Entry point : %d \033[0m\n", Entrypoint);
	fprintf(stderr, "\033[40;34m    used_stmt array stack totalsize of : %d \033[0m\n", totalsize);
	fprintf(stderr, "\033[40;34m    collect time: : %f s \033[0m\n", time_used);
	fprintf(stderr, "\033[40;34m    algorithm time: %f s \033[0m\n", time_used2);
	fprintf(stderr, "\033[40;34m    gimple stmt count : : %d \033[0m\n", gimplestmt_count);
	fprintf(stderr, "\033[40;32mSTART CHECKSTART CHECKSTART CHECKSTART CHECKSTART CHECK\033[0m\n");
	// totalsize = 0;
	// // printfunctionCollect(ptable, used_stmt);
	// fprintf(stderr, "algorithm time: %f s\n", time_used2);

	// printfunctionCollect(ptable, used_stmt);
	// printfunctionCollect2(ptable, used_stmt);
	// printfPointerConstraint2(ptable, used_stmt);
	// printfBasicblock();
	// fprintf(stderr, "collect time: %f s\n", time_used);
	// fprintf(stderr, "algorithm time: %f s\n", time_used2);
	// fprintf(stderr, "gimple stmt count : %d \n", gimplestmt_count);
	// printfunctionCollect(ptable, used_stmt);
	// fprintf(stderr, " used_stmt array size of : %d\n", totalsize);

	// free(table1);
	// free(table3);
	// free(used_stmt);
}

// void print_function_path2(tree function_tree, int fucntion_level, ptb *ptable, gimple_array *user_tmp)
// {
// 	if (function_path_collect->get(function_tree) == NULL)
// 		return;
// 	function_path_array fun_array = *(function_path_collect->get(function_tree));

// 	vector<function_path> function_path_array = fun_array.function_path_array;
// 	//debug_tree(function_tree);
// 	//vector<pair<fdecl,location_t>> loc;
// 	if (function_return_collect->get(function_tree) == NULL)
// 		return;
// 	function_return_array callerFunArray = *(function_return_collect->get(function_tree));
// 	vector<return_type> callerRetTypearray = callerFunArray.return_type_array;

// 	fprintf(stderr, "=======print_function_path %s  function_call count: %d level :%d========\n", get_name(function_tree), function_path_array.size(), fucntion_level);
// 	fprintf(stderr, "=======print_function_type %d  ========\n", callerFunArray.return_type_num);
// 	fucntion_level += 1;
// 	fprintf(stderr, "[\n");

// 	// checkPointerConstraint(function_tree,ptable,user_tmp);

// 	fprintf(stderr, "=======print_function_path %s  function_call count: %d level :%d========\n", get_name(function_tree), function_path_array.size(), fucntion_level);
// 	// pathStack.push(function_tree);
// 	for (int i = 0; i < function_path_array.size(); i++)
// 	{

// 		int find = 0;

// 		for (int o = 0; o < pathStack.size(); o++)
// 		{

// 			fprintf(stderr, "=======now node_fun stack:%s=========\n", get_name(pathStack.c[o]));
// 			if (pathStack.c[o] == (function_path_array)[i].next)
// 			{
// 				find = 1;
// 				fprintf(stderr, "				=======recursive_fun:%s=========\n", get_name(pathStack.c[o]));
// 			}
// 		}

// 		if (find == 0)
// 		{
// 			fprintf(stderr, "=======add node_fun stack:%s=========\n", get_name((function_path_array)[i].next));
// 			// debug_tree((function_path_array)[i].next);
// 			pathStack.push((function_path_array)[i].next);
// 			print_function_path((function_path_array)[i].next, fucntion_level, ptable, user_tmp);
// 			pathStack.pop();
// 		}
// 	}

// 	fprintf(stderr, "]\n");
// }

void record_fucntion(cgraph_node *node)
{

	cgraph_edge *e;
	FOR_EACH_DEFINED_FUNCTION(node)
	{
		// push_cfun(node->get_fun());
		// if (!ipa)
		// 	init_table();
		if (!gimple_has_body_p(node->decl))
			continue;
		push_cfun(node->get_fun());
		// if (strcmp(get_name(cfun->decl), "main") == 0)
		// debug_tree(cfun->decl);
		// tree test=DECL_SAVED_TREE(cfun->decl);
		// analyze_func_body(DECL_SAVED_TREE(test), 0);
		if (cfun == NULL)
		{
			pop_cfun();
			continue;
		}
		// fprintf(stderr, "=======node_fun:%s=========\n", get_name(cfun->decl));
		enum availability avail;

		// fprintf(stderr, "fucntion collect \n");
		function_path_array fun_array;
		// tree get_function_return_tree = gimple_return_retval(as_a<greturn *>(gc));
		vector<function_path> function_path_array;
		fun_array.function_path_array = function_path_array;

		for (e = node->callees; e; e = e->next_callee)
		{
			// funct_state l;
			cgraph_node *caller = e->caller->global.inlined_to ? e->caller->global.inlined_to : e->caller;
			cgraph_node *callee = e->callee->ultimate_alias_target(&avail, caller);

			// fprintf(stderr, "=======child node_fun:%s=========\n", get_name(callee->decl));
			if (callee != NULL)
			{
				// if (!gimple_has_body_p(callee->decl))
				// 	continue;
				//
				int find = 0;
				vector<function_path>::iterator it_i;
				for (it_i = fun_array.function_path_array.begin(); it_i != fun_array.function_path_array.end(); ++it_i)
				{
					if (it_i->next == callee->decl)
						find = 1;
				}
				// cout << *it_i << " "
				if (find == 0)
				{
					// fprintf(stderr, "=======child node_fun:%s=========\n", get_name(callee->decl));
					struct function_path path_type;
					path_type.cgnext = callee;
					path_type.next = callee->decl;
					fun_array.function_path_array.push_back(path_type);
				}

				// pop_cfun();
				//		dump_fucntion(callee);
			}

			// analyze_function=	analyze_function (caller ,true);
		}
		//  DECL_STRUCT_FUNCTION (cfun->decl);
		// debug_tree(node->get_fun()->decl);
		function_path_collect->put(node->get_fun()->decl, fun_array);
		pop_cfun();
	}
}

void printf_bbinfo(basic_block bb, int flag)
{

	if (syminfo->get(bb) != NULL)
	{

		struct symbolicinfo *symbolicinfotmp = syminfo->get(bb);
		// syminfo->get(syminfo->get(bb)-
		// debug_gimple_stmt(syminfo->get(syminfo->get(bb)->symbolicExecutionPathConstraint[o])->cond_stmt);
		// struct symbolicinfo *symbolicinfo3 = syminfo->get(bb);
		fprintf(stderr, "=======================Path Constaint===========================\n\n");
		if (flag == 1)
			for (int o = 0; o < symbolicinfotmp->symbolicExecutionPathConstraintarrayTrue.size(); o++)
			{
				// debug_tree()

				// fprintf(stderr, "-----------\n");
				fprintf(stderr, "succs:= %d\n", symbolicinfotmp->symbolicExecutionPathConstraintarrayTrue[o].bb->index);
				// // debug_gimple_stmt(syminfo->get(bb)->cond_stmt);
				if (gimple_location_safe(syminfo->get(symbolicinfotmp->symbolicExecutionPathConstraintarrayTrue[o].bb)->cond_stmt), 0, "use location")
					debug_gimple_stmt(syminfo->get(symbolicinfotmp->symbolicExecutionPathConstraintarrayTrue[o].bb)->cond_stmt);

				warning_at(gimple_location_safe(syminfo->get(symbolicinfotmp->symbolicExecutionPathConstraintarrayTrue[o].bb)->cond_stmt), 0, "use location");
				// //syminfo->get(syminfo->get(bb)->symbolicExecutionPathConstraint[o])->prevlogic
				fprintf(stderr, "	relate logic:= %d\n", symbolicinfotmp->symbolicExecutionPathConstraintarrayTrue[o].boolt);
			}
		else
			for (int o = 0; o < symbolicinfotmp->symbolicExecutionPathConstraintarrayFalse.size(); o++)
			{
				fprintf(stderr, "=succs:= %d\n", symbolicinfotmp->symbolicExecutionPathConstraintarrayFalse[o].bb->index);
				// fprintf(stderr, "-----------\n");
				// fprintf(stderr, "succs:=	 %d\n",symbolicinfotmp->symbolicExecutionPathConstraint[o]->index);
				// // debug_gimple_stmt(syminfo->get(bb)->cond_stmt);
				debug_gimple_stmt(syminfo->get(symbolicinfotmp->symbolicExecutionPathConstraintarrayFalse[o].bb)->cond_stmt);
				if (gimple_location_safe(syminfo->get(symbolicinfotmp->symbolicExecutionPathConstraintarrayFalse[o].bb)->cond_stmt), 0, "use location")
					warning_at(gimple_location_safe(syminfo->get(symbolicinfotmp->symbolicExecutionPathConstraintarrayFalse[o].bb)->cond_stmt), 0, "use location");
				// //syminfo->get(syminfo->get(bb)->symbolicExecutionPathConstraint[o])->prevlogic
				fprintf(stderr, "	relate logic:= %d\n", symbolicinfotmp->symbolicExecutionPathConstraintarrayFalse[o].boolt);
			}
		fprintf(stderr, "=======================Path Constaint===========================\n\n");
	}
}
void set_PathConstraintarray(basic_block bb, int flag)
{
	if (syminfo->get(bb) != NULL)
	{
		// push_self if bb
		struct symbolicinfo *symbolicinfotmp = syminfo->get(bb);
		//    symbolicinfotmp->symbolicExecutionPathConstraintarray.clear();
		//    symbolicinfotmp->symbolicExecutionPathConstraintarray2.clear();
		// fprintf(stderr, "path constaint-----------\n");
		// fprintf(stderr, "succs:= %d\n", bb->index);

		// // debug_gimple_stmt(syminfo->get(bb)->cond_stmt);
		// // warning_at(gimple_location_safe(syminfo->get(bb)->cond_stmt), 0, "use location");
		// fprintf(stderr, " relate logic:= %d\n", flag);
		struct symbolicinfoPatharray test;
		test.bb = bb;
		test.boolt = flag;
		int find = 0;
		if (flag == 1)
		{
			for (int o = 0; o < symbolicinfotmp->symbolicExecutionPathConstraintarrayTrue.size(); o++)
			{
				if (symbolicinfotmp->symbolicExecutionPathConstraintarrayTrue[o].bb == bb)
				{
					find = 1;
					break;
				}
			}
		}
		else
		{
			for (int o = 0; o < symbolicinfotmp->symbolicExecutionPathConstraintarrayFalse.size(); o++)
			{
				if (symbolicinfotmp->symbolicExecutionPathConstraintarrayFalse[o].bb == bb)
				{
					find = 1;
					break;
				}
			}
		}
		if (find == 0)
		{
			// fprintf(stderr, "path constaint-----------\n");
			fprintf(stderr, "succs:= %d\n", bb->index);
			//
			// debug_gimple_stmt(syminfo->get(bb)->cond_stmt);
			// warning_at(gimple_location_safe(syminfo->get(bb)->cond_stmt), 0, "use location");
			// fprintf(stderr, " relate logic:= %d\n", flag);
			// fprintf(stderr, "3index %d\n", test.boolt);
			if (flag == 1)
			{
				// fprintf(stderr,"index %d\n",symbolicinfotmp->symbolicExecutionPathConstraintarray.size());
				symbolicinfotmp->symbolicExecutionPathConstraintarrayTrue.push_back(test);
			}
			else
				symbolicinfotmp->symbolicExecutionPathConstraintarrayFalse.push_back(test);
		}
		// syminfo->get(syminfo->get(bb)->prevlogic;
		// if (syminfo->get(bb) != NULL)
		// {

		// struct symbolicinfo *symbolicinfo3 = syminfo->get(bb);
		for (int o = 0; o < symbolicinfotmp->symbolicExecutionPathConstraint.size(); o++)
		{

			// fprintf(stderr, "-----------\n");
			// fprintf(stderr, "succs:= %d\n", symbolicinfotmp->symbolicExecutionPathConstraint[o]->index);
			// debug_gimple_stmt(syminfo->get(bb)->cond_stmt);
			// debug_gimple_stmt(syminfo->get(symbolicinfotmp->symbolicExecutionPathConstraint[o])->cond_stmt);
			// syminfo->get(syminfo->get(bb)->symbolicExecutionPathConstraint[o])->prevlogic
			// fprintf(stderr, " relate logic:= %d\n", syminfo->get(symbolicinfotmp->symbolicExecutionPathConstraint[o])->prevlogic);
			int find = 0;
			// symbolicinfotmp->symbolicExecutionPathConstraintarray=NULL;
			// symbolicinfotmp->symbolicExecutionPathConstraintarray2=NULL;
			if (flag == 1)
			{

				for (int i = 0; i < symbolicinfotmp->symbolicExecutionPathConstraintarrayTrue.size(); i++)
				{
					// fprintf(stderr, "succs:= %d\n", syminfo->get(bb)->symbolicExecutionPathConstraint[o]->index);
					if (symbolicinfotmp->symbolicExecutionPathConstraintarrayTrue[i].bb == symbolicinfotmp->symbolicExecutionPathConstraint[o])
					{
						// fprintf(stderr, "qweeeeeeeeee\n");
						// symbolicinfotmp->symbolicExecutionPathConstraintarray[i].boolt = syminfo->get(syminfo->get(bb)->symbolicExecutionPathConstraint[o])->prevlogic;
						find = 1;
						break;
					}
				}
			}
			else
			{
				for (int i = 0; i < symbolicinfotmp->symbolicExecutionPathConstraintarrayFalse.size(); i++)
				{
					// fprintf(stderr,"2index %d\n",symbolicinfotmp->symbolicExecutionPathConstraintarray.size());
					if (symbolicinfotmp->symbolicExecutionPathConstraintarrayFalse[i].bb == symbolicinfotmp->symbolicExecutionPathConstraint[o])
					{
						// fprintf(stderr, "qweeeeeeeeee\n");
						// symbolicinfotmp->symbolicExecutionPathConstraintarray2[i].boolt = syminfo->get(syminfo->get(bb)->symbolicExecutionPathConstraint[o])->prevlogic;
						find = 1;
						break;
					}
				}
			}
			if (find == 0)
			{
				// fprintf(stderr, "succs:= %d\n", bb->index);
				// fprintf(stderr,"2index %d\n",symbolicinfotmp->symbolicExecutionPathConstraintarray.size());
				// push_self prev bb index
				test.bb = symbolicinfotmp->symbolicExecutionPathConstraint[o];

				test.boolt = syminfo->get(symbolicinfotmp->symbolicExecutionPathConstraint[o])->prevlogic;
				// fprintf(stderr, "3index %d\n", test.boolt);
				if (flag == 1)
				{
					symbolicinfotmp->symbolicExecutionPathConstraintarrayTrue.push_back(test);
				}
				else
					symbolicinfotmp->symbolicExecutionPathConstraintarrayFalse.push_back(test);
			}
		}
		// }
	}
}
void set_bbinfo(basic_block bb)
{
	// fprintf(stderr, "path constaint-----------\n");
	// fprintf(stderr, "succs:= %d\n", bb->index);
	// debug_gimple_stmt(syminfo->get(bb)->cond_stmt);
	// fprintf(stderr, " relate logic:= %d\n", syminfo->get(bb)->prevlogic);
	// syminfo->get(syminfo->get(bb)->prevlogic;
	for (int o = 0; o < symbolicExecution.size(); o++)
	{

		if (syminfo->get(symbolicExecution[o]) != NULL)
		{
			struct symbolicinfo *symbolicinfotmp = syminfo->get(symbolicExecution[o]);

			if (symbolicinfotmp->cond_truebranch == bb || dominated_by_p(CDI_DOMINATORS, bb, symbolicinfotmp->cond_truebranch))
			{

				set_PathConstraintarray(symbolicExecution[o], 1);
			}
			else if (symbolicinfotmp->cond_falsebranch == bb || dominated_by_p(CDI_DOMINATORS, bb, symbolicinfotmp->cond_falsebranch))
			{
				set_PathConstraintarray(symbolicExecution[o], 0);
			}
		}
	}
	// if (syminfo->get(bb) != NULL)
	// {
	// 	// struct symbolicinfo *symbolicinfo3 = syminfo->get(bb);
	// 	for (int o = 0; o < syminfo->get(bb)->symbolicExecutionPathConstraint.size(); o++)
	// 	{

	// 		fprintf(stderr, "-----------\n");
	// 		fprintf(stderr, "succs:= %d\n", syminfo->get(bb)->symbolicExecutionPathConstraint[o]->index);
	// 		// debug_gimple_stmt(syminfo->get(bb)->cond_stmt);
	// 		debug_gimple_stmt(syminfo->get(syminfo->get(bb)->symbolicExecutionPathConstraint[o])->cond_stmt);
	// 		//syminfo->get(syminfo->get(bb)->symbolicExecutionPathConstraint[o])->prevlogic
	// 		fprintf(stderr, " relate logic:= %d\n", syminfo->get(syminfo->get(bb)->symbolicExecutionPathConstraint[o])->prevlogic);
	// 	}
	// }
}
void check_bbinfo(basic_block bb)
{

	// fprintf(stderr, "succs:= %d\n", bb->index);
	// debug_gimple_stmt(syminfo->get(bb)->cond_stmt);
	// fprintf(stderr, " relate logic:= %d\n", syminfo->get(bb)->prevlogic);
	// syminfo->get(syminfo->get(bb)->prevlogic;
	// if(symbolicExecution.size())
	// fprintf(stderr, "prev:= %d\n", bb->index);
	for (int o = 0; o < symbolicExecution.size(); o++)
	{

		if (syminfo->get(symbolicExecution[o]) != NULL)
		{
			struct symbolicinfo *symbolicinfotmp = syminfo->get(symbolicExecution[o]);

			if (symbolicinfotmp->cond_truebranch == bb)
			{
				// fprintf(stderr, "succs:= %d\n", symbolicExecution[o]->index);
				printf_bbinfo(symbolicExecution[o], 1);
				// break;
			}
			else if (symbolicinfotmp->cond_falsebranch == bb)
			{
				// fprintf(stderr, "succs:= %d\n",symbolicExecution[o]->index);
				printf_bbinfo(symbolicExecution[o], 0);
			}
		}
	}
	// for (int o = 0; o < symbolicExecutionswitch.size(); o++)
	// {
	// 	if (syminfo->get(symbolicExecutionswitch[o]) != NULL)
	// 	{
	// 		struct symbolicinfo *symbolicinfotmp = syminfo->get(symbolicExecutionswitch[o]);

	// 		for (int o2 = 0; o2 < symbolicinfotmp->switchs.size(); o2++)
	// 		{
	// 			if (symbolicinfotmp->switchs[o2] == bb)
	// 			{
	// 				debug_gimple_stmt(symbolicinfotmp->switch_stmt);
	// 				if (gimple_location_safe(symbolicinfotmp->switch_stmt))
	// 					warning_at(gimple_location_safe(symbolicinfotmp->switch_stmt), 0, "use location");

	// 				gswitch *swtch = as_a<gswitch *>(symbolicinfotmp->switch_stmt);
	// 				int lsize1 = gimple_switch_num_labels(swtch);
	// 				fprintf(stderr, "in switch succs:= %d\n", symbolicinfotmp->switchs[o2]->index);
	// 				for (int i = 0; i < lsize1; i++)
	// 				{

	// 					// if (i == symbolicinfotmp->switchs[o2]->index)
	// 					// {
	// 					// fprintf(stderr, "--------GIMPLE_SWITCHss %d -------\n",i);
	// 					tree label1 = gimple_switch_label(swtch, i);
	// 					tree label = CASE_LABEL(label1);
	// 					basic_block dest = label_to_block(label);
	// 					if (dest->index == symbolicinfotmp->switchs[o2]->index)
	// 					{
	// 						debug(label1);
	// 						// debug_tree(label1);
	// 					}
	// 					// }
	// 					// debug_tree(label1);
	// 				}
	// 				// printf_bbinfo2(symbolicExecution[o], 1);
	// 			}
	// 		}
	// 	}
	// }
	// else{
	// 	fprintf(stderr, "this fucntion no branch or cond\n");
	// }
	// if (syminfo->get(bb) != NULL)
	// {
	// 	// struct symbolicinfo *symbolicinfo3 = syminfo->get(bb);
	// 	for (int o = 0; o < syminfo->get(bb)->symbolicExecutionPathConstraint.size(); o++)
	// 	{

	// 		fprintf(stderr, "-----------\n");
	// 		fprintf(stderr, "succs:= %d\n", syminfo->get(bb)->symbolicExecutionPathConstraint[o]->index);
	// 		// debug_gimple_stmt(syminfo->get(bb)->cond_stmt);
	// 		debug_gimple_stmt(syminfo->get(syminfo->get(bb)->symbolicExecutionPathConstraint[o])->cond_stmt);
	// 		//syminfo->get(syminfo->get(bb)->symbolicExecutionPathConstraint[o])->prevlogic
	// 		fprintf(stderr, " relate logic:= %d\n", syminfo->get(syminfo->get(bb)->symbolicExecutionPathConstraint[o])->prevlogic);
	// 	}
	// }
}
// static void
// dump_edge_probability (pretty_printer *buffer, edge e)
// {
//   pp_scalar (buffer, " %s", dump_probability (e->probability));
// }
static void
dump_gimple_fmt(pretty_printer *buffer, int spc,
				const char *fmt, ...)
{
	va_list args;
	const char *c;
	const char *tmp;
	va_start(args, fmt);
	for (c = fmt; *c; c++)
	{
		if (*c == '%')
		{
			gimple_seq seq;
			tree t;
			gimple *g;
			//    g = va_arg (args, gimple *);
			//     tmp = gimple_code_name[gimple_code (g)];
			// debug_gimple_stmt(g);
			switch (*++c)
			{
			case 'G':
				g = va_arg(args, gimple *);
				tmp = gimple_code_name[gimple_code(g)];
				// pp_string (buffer, tmp);
				// if(g)
				// debug(g);
				fprintf(stderr, "=======ipa_pta= %s========\n", tmp);
				break;
				//   case 'S':
				// debug_gimple_stmt(g);
				// fprintf(stderr, "=======ipa_pta=========\n");
				//         seq = va_arg (args, gimple_seq);
				//         pp_newline (buffer);
				//         dump_gimple_seq (buffer, seq, spc + 2, flags);
				//         newline_and_indent (buffer, spc);
				// break;
			}
			//       case 'T':
			//         t = va_arg (args, tree);
			//         if (t == NULL_TREE)
			//           pp_string (buffer, "NULL");
			//         else
			//           dump_generic_node (buffer, t, spc, flags, false);
			//         break;
			//       case 'd':
			//         pp_decimal_int (buffer, va_arg (args, int));
			//         break;
			//       case 's':
			//         pp_string (buffer, va_arg (args, char *));
			//         break;
			//       case 'n':
			//         newline_and_indent (buffer, spc);
			//         break;
			//       case 'x':
			//         pp_scalar (buffer, "%x", va_arg (args, int));
			//         break;
			//       case '+':
			//         spc += 2;
			//         newline_and_indent (buffer, spc);
			//         break;
			//       case '-':
			//         spc -= 2;
			//         newline_and_indent (buffer, spc);
			//         break;
			//       default:
			//         gcc_unreachable ();
			//     }
		}
		//   else
		//     pp_character (buffer, *c);
	}
	va_end(args);
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
	syminfo = new hash_map<basic_block, symbolicinfo>;

	function_return_collect = new hash_map<tree, function_return_array>;
	function_assign_collect = new hash_map<tree, function_assign_array>;
	pthread_attr_setdetachstates = new hash_map<tree, pthread_attr_array>;
	function_pthread_detched_collect = new hash_map<tree, function_pthread_detched_array>;
	function_pthread_exit_collect = new hash_map<tree, function_pthread_exit_array>;
	function_path_collect = new hash_map<tree, function_path_array>;
	function_free_collect = new hash_map<tree, function_free_array>;
	function_graph_collect = new hash_map<tree, function_graph_array>;
	function_relate_collect = new hash_map<tree, function_relate_array>;

	// vector<breakpoint> vbreakpoint;
	breakpoint getbp;
	// vscode extension path
	if (vscode_extensionmod)
	{
		std::ifstream ifs("/root/.vscode-server/data/User/globalStorage/vscode-samples.helloworld-sample/breakpoint.txt", std::ios::in);
		fprintf(stderr, "==============breakpoint=========\n");
		if (!ifs.is_open())
		{
			cout << "Failed to open file.\n";
			return;
		}
		else
		{
			std::string name;
			int score;
			while (ifs >> name >> score)
			{
				getbp.name = name;
				getbp.line = score;
				fprintf(stderr, "%s %d\n", getbp.name.c_str(), getbp.line);
				// cout << name << " " << score << "\n";
				vbreakpoint.push_back(getbp);
				// scores.push_back(score);
			}
			ifs.close();
		}
		ifs.close();
	}
	srand((unsigned)time(NULL) + getpid());
	// fDFS = new hash_map<cgraph_node *, Graph>;
	// fnode = new hash_map<tree, cgraph_node *>;
	fprintf(stderr, "=======ipa_pta=========\n");
	// ompfucntio();
	/*initialization table*/
	init_table();
	// omp_set_num_threads(4);
	double s = 0.0;
	struct timespec start, end;

	// start = clock();
	clock_gettime(CLOCK_MONOTONIC, &start);
	getrusage(RUSAGE_SELF, &ru);
	utime = ru.ru_utime;
	stime = ru.ru_stime;

	// fprintf(stderr,"%d\n",*tmp);
	// cout << "----------------------------------------" <<tmp<<endl;
	// debug_tree(gimple_call_fndecl(node->decl));
	fprintf(stderr, "===============The first stage : Point of interest stmt collect=================\n");
	FOR_EACH_DEFINED_FUNCTION(node)
	{
		if (!ipa)
			init_table();
		if (!gimple_has_body_p(node->decl))
			continue;
		push_cfun(node->get_fun());
		//  ipa_ref *ref;
		// FOR_EACH_ALIAS(node,ref){
		// 		cout << "--------------ref--------------------------" <<endl;
		// }
		// function_args_iterator it;

		// function_args_iter_init(&it, TREE_TYPE(node->get_fun()->decl));
		// for (unsigned argno = 0;; ++argno, function_args_iter_next(&it))
		// {
		// 	// tree argtype = function_args_iter_cond(&it);
		// 	cout << "--------------ref--------------------------" << endl;
		// }
		// unsigned int nargs = 1;
		// function_args_iterator iter;
		// tree t;
		// int argct = 0;
		// fprintf(stderr, "=======node_fun:%s=========\n", get_name(cfun->decl));
		// // debug_tree(node->get_fun()->decl);
		// unsigned int i;
		// tree parms, p;
		// // debug_tree(DECL_ARGUMENTS(node->get_fun()->decl));
		// // static_chain_decl
		// // debug_tree(DECL_ARGUMENTS(node->get_fun()->decl));
		// // debug_tree(DECL_CHAIN(DECL_ARGUMENTS(node->get_fun()->decl)));
		// p = DECL_ARGUMENTS(node->get_fun()->decl);
		// /* Fix up the types of parms passed by invisible reference.  */
		// for (t = DECL_ARGUMENTS(node->get_fun()->decl); t; t = DECL_CHAIN(t)){
		// 	debug_tree(t);
		// }

		// for (i = 0, p = DECL_ARGUMENTS(node->get_fun()->decl);
		// 	DECL_CHAIN(p) != NULL;
		// 	 i++, p = DECL_CHAIN(p))
		// {
		// 	tree arg;
		// 	/* We cannot distinguish a varargs function from the case
		//      of excess parameters, still deferring the inlining decision
		//      to the callee is possible.  */
		// 	// if (!p)
		// 	// 	break;
		// 	// arg = gimple_call_arg(stmt, i);

		// 	debug_tree(p);
		// 	// if( DECL_CHAIN(p) == NULL)
		// 	// break;
		// 	// if (p == error_mark_node || DECL_ARG_TYPE(p) == error_mark_node || arg == error_mark_node || (!types_compatible_p(DECL_ARG_TYPE(p), TREE_TYPE(arg)) && !fold_convertible_p(DECL_ARG_TYPE(p), arg)))
		// 	// 	return false;
		// }
		// tree attr = lookup_attribute("sentinel", TYPE_ATTRIBUTES(TREE_TYPE(node->get_fun()->decl)));
		// FOREACH_FUNCTION_ARGS(TREE_TYPE(node->get_fun()->decl), t, iter)
		// {
		// 	debug_tree(t);
		// 	// if (t == void_type_node)
		// 	// 	break;

		// 	// gimple *def_stmt = SSA_NAME_DEF_STMT(t);
		// 	// debug_tree(t);
		// 	// debug_gimple_stmt(def_stmt);
		// 	// tree_stmt_iterator it;
		// 	// for (i = 0, it = tsi_start(t); !tsi_end_p(it); tsi_next(&it), i++)
		// 	// {
		// 	// 	char buffer[32];
		// 	// 	sprintf(buffer, "%u", i);
		// 	// 	dump_child(buffer, tsi_stmt(it));
		// 	// }
		// 	// if (TREE_VALUE(attr))
		// 	// {
		// 	// 	tree p = TREE_VALUE(TREE_VALUE(attr));
		// 	// 	debug_tree(p);
		// 	// 	// pos = TREE_INT_CST_LOW(p);
		// 	// }
		// 	argct++;
		// 	// fprintf(stderr, "=======node_fun:%d=========\n", argct);
		// }
		// fprintf(stderr, "=======node_fun:%d=========\n", argct);
		// if (strcmp(get_name(cfun->decl), "main") == 0)

		// tree test=DECL_SAVED_TREE(cfun->decl);
		// analyze_func_body(DECL_SAVED_TREE(test), 0);
		if (cfun == NULL)
		{
			pop_cfun();
			continue;
		}
		// debug_tree(cfun->decl);
		// enum availability avail;
		// for (e = node->callees; e; e = e->next_callee)
		// {
		// 	//funct_state l;
		// 	cgraph_node *caller = e->caller->global.inlined_to ? e->caller->global.inlined_to : e->caller;
		// 	cgraph_node *callee = e->callee->ultimate_alias_target(&avail, caller);
		// 	fprintf(stderr, "=======child node_fun:%s=========\n", get_name(callee->decl));
		// 	//analyze_function=	analyze_function (caller ,true);
		// }
		// if (strcmp(get_name(cfun->decl), "main") == 0)
		// 	main_fun = cfun;
		// fprintf(stderr,"=======node_fun:%s=========\n",get_name(cfun->decl));

		/*initialization DFS graph*/
		// Graph DFS;
		// DFS.init_Graph(cfun->cfg->x_last_basic_block);

		/*calculate dominator tree*/
		calculate_dominance_info(CDI_DOMINATORS);

		/*create DFS graph, Algorithm 1 and 2*/
		FOR_EACH_BB_FN(bb, cfun)
		{
			// debug_bb(bb);
			// if (bb != cfun->cfg->x_exit_block_ptr->prev_bb)
			// {
			// 	edge e;
			// 	edge_iterator ei;
			// 	fprintf(stderr,"node:= %d \n",bb->index);
			// 	FOR_EACH_EDGE(e, ei, bb->succs)
			// 	{
			// 		// DFS.addEdge(bb->index, e->dest->index);
			// 		fprintf(stderr," succs:= %d\n",e->dest->index);
			// 	}
			// }
			for (gimple_stmt_iterator gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi))
			{
				gimple *gc = gsi_stmt(gsi);

				// debug_gimple_stmt(gc);
				// debug_tree(gimple_block_label(gimple_bb(gc)));
				// fprintf(stderr, " succs:= %d\n", gimple_bb(gc)->index);
				if (gimple_code(gc) == GIMPLE_SWITCH)
				{
					fprintf(stderr, "--------GIMPLE_SWITCH -------\n");
					// debug_gimple_stmt(gc);
					gswitch *swtch = as_a<gswitch *>(gc);

					unsigned lsize1, lsize2, i;
					lsize1 = gimple_switch_num_labels(swtch);
					pretty_printer buffer;
					buffer.buffer->stream = stderr;
					// debug_tree(gimple_switch_index(swtch));
					// dump_gimple_fmt (&buffer, 0, "%G <%T, ", swtch,gimple_switch_index (swtch));
					// pp_gimple_stmt_1(&pp, gc, 1 /* spc */, 0 /* flags */);
					// for (i = 0; i < lsize1; i++)
					// {
					// 	// fprintf(stderr, "--------GIMPLE_SWITCHss %d -------\n",i);
					// 	tree label1 = gimple_switch_label(swtch, i);
					// 	tree label = CASE_LABEL(label1);
					// 	// debug_tree(label);
					// 	// debug_tree(label1);

					// 	// dump_gimple_switch(buffer, label, spc, flags, false);
					// 	//    pp_gimple_stmt_1  (&buffer,swtch, 0, 0);

					// 	// if (cfun && cfun->cfg)
					// 	// {
					// 	// 	basic_block dest = label_to_block(label);
					// 	// 	if (dest)
					// 	// 	{
					// 	// 		edge label_edge = find_edge(gimple_bb(swtch), dest);
					// 	// 		debug( label_edge);
					// 	// 		// if (label_edge && !(flags & TDF_GIMPLE))
					// 	// 		// 	dump_edge_probability(buffer, label_edge);
					// 	// 	}
					// 	// }
					// 	// debug_tree(label1);
					// }

					// fprintf(stderr, "--------GIMPLE_SWITCHss -------\n");
					symbolicExecutionswitch.push_back(bb);
					symbolicinfo symbolicinfo;
					symbolicinfo.switch_stmt = gc;

					if (bb != cfun->cfg->x_exit_block_ptr->prev_bb)
					{
						edge e;
						edge_iterator ei;
						// fprintf(stderr, "node:= %d \n", bb->index);
						// 	//BLOCK_SUPERCONTEXT(gimple_block(u_stmt)
						// 	int init = 0;
						FOR_EACH_EDGE(e, ei, bb->succs)
						{
							// 		// DFS.addEdge(bb->index, e->dest->index);

							// 		// debug_tree(test);
							// 		// if (init == 0)
							// 		// {
							// 		// 	symbolicinfo.cond_truebranch = e->dest;
							// 		// 	fprintf(stderr, " true succs:= %d\n", e->dest->index);
							// 		// }

							// 		// else
							// 		// {
							// 		// 	symbolicinfo.cond_falsebranch = e->dest;
							// 		// 	fprintf(stderr, " false succs:= %d\n", e->dest->index);
							// 		// }
							// 		// init++;
							symbolicinfo.switchs.push_back(e->dest);
							// fprintf(stderr, " succs:= %d\n", e->dest->index);
						}
						syminfo->put(bb, symbolicinfo);
					}
				}
				// if (gimple_code(gc) == GIMPLE_GOTO)
				// {
				// 	fprintf(stderr, "--------GIMPLE gc -------\n");
				// 	debug_gimple_stmt(gc);
				// }
				// if (gimple_code(gc) == GIMPLE_LABEL)
				// {
				// 	fprintf(stderr, "--------GIMPLE gc -------\n");
				// 	debug_gimple_stmt(gc);
				// }
				// if (gimple_code(gc) == EQ_EXPR)
				// {
				// 	fprintf(stderr, "--------GIMPLE EQ_EXPR -------\n");
				// 	debug_gimple_stmt(gc);
				// }
				// debug_gimple_stmt(gc);
				if (gimple_cond_code(gc))
				{
					enum tree_code code = gimple_cond_code(gc);
					// if (code == LT_EXPR || code == GT_EXPR || code == LE_EXPR || code == GE_EXPR || code == EQ_EXPR || code == NE_EXPR)

					// 	fprintf(stderr, "--------GIMPLE ok -------\n");
					if (!is_gimple_assign(gc))
					{
						// if (gimple_cond_lhs(use_stmt))
						// debug_gimple_stmt(gc);
						fprintf(stderr, "--------GIMPLE Cond -------\n");

						symbolicExecution.push_back(bb);
						symbolicinfo symbolicinfo;
						symbolicinfo.cond_stmt = gc;
						symbolicinfo.cond_lhs = gimple_cond_lhs(gc);
						symbolicinfo.cond_rhs = gimple_cond_rhs(gc);

						// symbolicinfo.cond_stmt= gc;

						if (bb != cfun->cfg->x_exit_block_ptr->prev_bb)
						{
							edge e;
							edge_iterator ei;
							// fprintf(stderr, "node:= %d \n", bb->index);
							// BLOCK_SUPERCONTEXT(gimple_block(u_stmt)
							int init = 0;
							FOR_EACH_EDGE(e, ei, bb->succs)
							{
								// DFS.addEdge(bb->index, e->dest->index);

								// debug_tree(test);
								if (init == 0)
								{
									symbolicinfo.cond_truebranch = e->dest;
									// fprintf(stderr, " true succs:= %d\n", e->dest->index);
								}

								else
								{
									symbolicinfo.cond_falsebranch = e->dest;
									// fprintf(stderr, " false succs:= %d\n", e->dest->index);
								}
								init++;
								// fprintf(stderr, " succs:= %d\n", e->dest->index);
							}
						}
						int find = 0;
						vector<basic_block>::iterator it_i;
						int max_path = 0;
						basic_block max_bb;
						for (int o = 0; o < symbolicExecution.size(); o++)
						{

							if (syminfo->get(symbolicExecution[o]) != NULL)
							{
								struct symbolicinfo *symbolicinfotmp = syminfo->get(symbolicExecution[o]);

								// return dominated_by_p(CDI_DOMINATORS, a, a);
								if (symbolicinfotmp->cond_truebranch == bb || dominated_by_p(CDI_DOMINATORS, bb, symbolicinfotmp->cond_truebranch))
								{
									if (symbolicinfotmp->symbolicExecutionPathConstraint.size() >= max_path && max_bb != symbolicExecution[o])
									{
										// fprintf(stderr, "max_path := %d\n", max_path);
										symbolicinfo.symbolicExecutionPathConstraint.clear();
										max_path = symbolicinfotmp->symbolicExecutionPathConstraint.size();
										max_bb = symbolicExecution[o];
										// branch_boolean = 1;
										symbolicinfo.prevlogic = 1;
										symbolicinfo.symbolicExecutionPathConstraint.push_back(symbolicExecution[o]);
										// symbolicinfotmp->prevlogic = symbolicinfo.prevlogic;
										for (int o2 = 0; o2 < symbolicinfotmp->symbolicExecutionPathConstraint.size(); o2++)
										{
											// fprintf(stderr, "size 2:= %d\n", symbolicinfotmp->symbolicExecutionPathConstraint.size());
											// debug_gimple_stmt(syminfo->get(bb)->cond_stmt);
											// if(syminfo->get(symbolicExecution[o]))
											// if (symbolicinfotmp->symbolicExecutionPathConstraint[o2] != bb)
											// {
											// fprintf(stderr, " prevlogic:= %d\n", syminfo->get(symbolicExecution[o])->prevlogic);
											// fprintf(stderr, "sub prevlogic:= %d\n", syminfo->get(symbolicExecution[o])->prevlogic);
											symbolicinfo.symbolicExecutionPathConstraint.push_back(symbolicinfotmp->symbolicExecutionPathConstraint[o2]);
											// }
											// fprintf(stderr, " 22succs:= %d\n", symbolicinfo2->symbolicExecutionPathConstraint[o]->index);
										}
									}
									// else
									// continue;
									// if (dominated_by_p(CDI_DOMINATORS, bb, symbolicinfotmp->cond_truebranch))
									// 	fprintf(stderr, "CDI_DOMINATORS prev succs:= %d\n", symbolicExecution[o]->index);
									// fprintf(stderr, "prev succs:= %d\n", symbolicExecution[o]->index);
									// struct symbolicinfoPatharray test;
									// test.bb = symbolicExecution[o];
									// test.boolt = 1;
									// symbolicinfo.symbolicExecutionPathConstraint.
									// fprintf(stderr, "size 1:= %d\n", symbolicinfotmp->symbolicExecutionPathConstraint.size());
									// if (symbolicinfotmp->symbolicExecutionPathConstraint.size() == max_path)
									// {
									// 	// fprintf(stderr, " prev logic:= %d\n", symbolicinfotmp->prevlogic);
									// 	// fprintf(stderr, " now logic:= %d\n", symbolicinfo.prevlogic);
									// }
									// fprintf(stderr, " test\n");
								}
								else if (symbolicinfotmp->cond_falsebranch == bb || dominated_by_p(CDI_DOMINATORS, bb, symbolicinfotmp->cond_falsebranch))
								{
									if (symbolicinfotmp->symbolicExecutionPathConstraint.size() >= max_path && max_bb != symbolicExecution[o])
									{
										// fprintf(stderr, "max_path := %d\n", max_path);
										// fprintf(stderr, " test\n");
										symbolicinfo.symbolicExecutionPathConstraint.clear();
										max_path = symbolicinfotmp->symbolicExecutionPathConstraint.size();
										max_bb = symbolicExecution[o];
										symbolicinfo.prevlogic = 0;
										// struct free_type free_type;
										symbolicinfo.symbolicExecutionPathConstraint.push_back(symbolicExecution[o]);
										// symbolicinfotmp->prevlogic = symbolicinfo.prevlogic;
										for (int o2 = 0; o2 < symbolicinfotmp->symbolicExecutionPathConstraint.size(); o2++)
										{
											// fprintf(stderr, "size 2:= %d\n", symbolicinfotmp->symbolicExecutionPathConstraint.size());
											// debug_gimple_stmt(syminfo->get(bb)->cond_stmt);
											// if(syminfo->get(symbolicExecution[o]))
											// if (symbolicinfotmp->symbolicExecutionPathConstraint[o2] != bb)
											// {
											// fprintf(stderr, "sub prevlogic:= %d\n", syminfo->get(symbolicExecution[o])->prevlogic);
											// fprintf(stderr, " prevlogic:= %d\n", syminfo->get(symbolicExecution[o])->prevlogic);

											symbolicinfo.symbolicExecutionPathConstraint.push_back(symbolicinfotmp->symbolicExecutionPathConstraint[o2]);
											// }
											// fprintf(stderr, " 22succs:= %d\n", symbolicinfo2->symbolicExecutionPathConstraint[o]->index);
										}
										// branch_boolean = 1;
									}
									// if (dominated_by_p(CDI_DOMINATORS, bb, symbolicinfotmp->cond_falsebranch))
									// 	fprintf(stderr, "CDI_DOMINATORS prev succs:= %d\n", symbolicExecution[o]->index);
									// fprintf(stderr, "prev succs:= %d\n", symbolicExecution[o]->index);
									// fprintf(stderr, "size 1:= %d\n", symbolicinfotmp->symbolicExecutionPathConstraint.size());
									// if (symbolicinfotmp->symbolicExecutionPathConstraint.size() == max_path)
									// {
									// 	// fprintf(stderr, " prev logic:= %d\n", symbolicinfotmp->prevlogic);
									// 	// fprintf(stderr, " now logic:= %d\n", symbolicinfo.prevlogic);
									// 	// fprintf(stderr, " test\n");
									// }
								}
							}
						}
						syminfo->put(bb, symbolicinfo);
					}
				}
				// set_basic_block info
				set_bbinfo(gimple_bb(gc));
				if (is_gimple_call(gc))
				{

					// fprintf(stderr, "--------------------wwwwwwwwwwwwSSSSSSSSSSSSSSSSSwwwwwwwwwwwwwwwww------------------\n");
					// 			debug_gimple_stmt(gc);
					/*collect malloc and free information*/
					collect_function_call(gc, node, bb);

					// fprintf(stderr, "add collect_function_call\n");
				}
				if (is_gimple_assign(gc))
				{

					// if (!check_stmtStack(gimple_call_arg(def_stmt, i)))
					// {
					// if (gimple_call_num_args(gc) > 1)
					// {
					// debug_gimple_stmt(gc);
					// 	debug_tree(gimple_call_arg(gc, 0));
					// }
					/*collect malloc and free information*/
					collect_function_call(gc, node, bb);

					// debug_gimple_stmt(gc);
					// debug_tree(gimple_assign_lhs(gc));
					// debug_tree(gimple_assign_rhs1(gc));
					// if (TREE_CODE(gimple_assign_rhs1(gc)) == GIMPLE_PHI)
					// {
					// 	gimple *def_stmt = SSA_NAME_DEF_STMT(gimple_assign_rhs1(gc));
					// 	if (def_stmt)
					// 	{
					// 		debug_gimple_stmt(def_stmt);
					// 		debug_tree(node->get_fun()->decl);
					// 		warning_at(gimple_location_safe(def_stmt), 0, "use location");
					// 	}
					// }
					// warning_at(gimple_location_safe(gc), 0, "use location");
					// fprintf(stderr, "add collect_function_call\n");
				}
				if (gimple_code(gc) == GIMPLE_RETURN)
				{

					collect_function_return(gc, node, bb);
					//				//struct return_type type_struct ;
					// get_function_return.return_type_array.push_back(type_struct);

					// type_struct.stmt = gc;
					//	type_struct.return_tree = get_function_return_tree;
					//  if( get_function_return.return_type_array.size() == 0){
					//  	fprintf(stderr,"hwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwas\n");
					//  }
					//  else{
					//  	fprintf(stderr,"no\n");
					//  }
					// function_return_array get_function_return = *(function_return_collect->get(cfun->decl));

					//	get_function_return->return_type_array.push_back(type_struct);
					//
					// function_return_collect->put(cfun->decl,*get_function_return);
					// get_function_return->return_type_array->stmt=gc;
				}
				// fprintf(stderr, "hwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwas\n");
				// debug_gimple_stmt(gc);

				// fprintf(stderr, "kkkkwwwwwwwwwwwwwwwwwwwwwwwwwwwwas\n");
				// check_bbinfo2(gimple_bb(gc));
				// fprintf(stderr, "bb index %d\n", gimple_bb(gc)->index);
				// fprintf(stderr, "hwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwas\n");
				// if (gimple_code(gc) == GIMPLE_GOTO)
				// {
				// 		fprintf(stderr,"hwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwas\n");
				// }
				// debug_gimple_stmt(gc);
				// if (is_gimple_assign(gc))
				// {
				// 	// debug_gimple_stmt(gc);
				// 	// if (gimple_assign_lhs(gc) != NULL)
				// 	// 	debug_tree(gimple_assign_lhs(gc));
				// 	tree parms, p;
				// 	// debug_tree(DECL_ARGUMENTS(node->get_fun()->decl));
				// 	// static_chain_decl
				// 	// debug_tree(DECL_ARGUMENTS(node->get_fun()->decl));
				// 	// debug_tree(DECL_CHAIN(DECL_ARGUMENTS(node->get_fun()->decl)));
				// 	p = DECL_ARGUMENTS(node->get_fun()->decl);
				// 	/* Fix up the types of parms passed by invisible reference.  */
				// 	for (t = DECL_ARGUMENTS(node->get_fun()->decl); t; t = DECL_CHAIN(t))
				// 	{
				// 		// debug_tree(t);
				// 		if (gimple_assign_lhs(gc) != NULL)
				// 			if (ptr_derefs_may_alias_p(t, gimple_assign_lhs(gc)))
				// 			{
				// 				debug_gimple_stmt(gc);
				// 			}
				// 	}
				// }
				// print_function_return(cfun->decl);
			}
		}
		/*DFS of this function put set "fDfS" */
		// fDFS->put(node, DFS);

		// function_graph_array fun_graph_array;
		// fun_graph_array.graph_type_array = DFS;
		// fun_graph_array.graph_node = node;
		// function_graph_collect->put(node->get_fun()->decl, fun_graph_array);

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
	fprintf(stderr, "===============The first stage : Point of interest stmt collect=================\n");
	if (ipa)
	{

		// new_memory_leak_analysis(ptable, ftable);

		// new_double_free_analysis(ptable,ftable);

		PointerConstraint(ptable, ftable);

		// new_double_free_analysis(ptable,ftable);
		// new_use_after_free_analysis(ptable, ftable);
	}
	clock_gettime(CLOCK_MONOTONIC, &end);
	// end = clock();
	// printf("time: %f s\n", ((double)(end - start)) / CLOCKS_PER_SEC / N); 	// fprintf(stderr,"time: %f s\n",((double)(end - start)) / CLOCKS_PER_SEC / N);
	struct timespec temp = diff(start, end);
	double time_used;
	time_used = temp.tv_sec + (double)temp.tv_nsec / 1000000000.0;
	// fprintf(stderr, "time: %f s\n", time_used);

	ofstream myfile("time.txt");
	if (myfile.is_open())
	{ //
		myfile << "utime_used: " << time_used << " s;\n";
		myfile << "stime_used: " << time_used << " s;\n";

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
	// fprintf(stderr,"=======inline=========\n");

	FOR_EACH_DEFINED_FUNCTION(node)
	{
		basic_block bb;
		cgraph_edge *e;

		if (!gimple_has_body_p(node->decl))
			continue;
		fprintf(stderr, "=======node_fun:%s=========\n", get_name(cfun->decl));
		// fprintf(stderr,"=======node_fun:%s %d=========\n",get_name(node->decl),node->decl);
		// fprintf(stderr,"attribute:%s \n",IDENTIFIER_POINTER (node->decl));

		// fprintf(stderr,"attribute:%s \n",IDENTIFIER_POINTER (DECL_ASSEMBLER_NAME (node->decl)));
		// node->debug();
		// fprintf(stderr,"=======fun:%d=========\n",node->decl);
		tree attr;
		enum availability avail;
		for (e = node->callees; e; e = e->next_callee)
		{

			cgraph_node *caller = e->caller->global.inlined_to ? e->caller->global.inlined_to : e->caller;
			cgraph_node *callee = e->callee->ultimate_alias_target(&avail, caller);
			tree callee_tree = callee ? DECL_FUNCTION_SPECIFIC_OPTIMIZATION(callee->decl) : NULL;
			// DECL_DISREGARD_INLINE_LIMITS (callee->decl)=1;
			if (DECL_ATTRIBUTES(callee->decl) != NULL)
			{
				attr = get_attribute_name(DECL_ATTRIBUTES(callee->decl));
				// debug_tree(attr);
			}
			else
			{
				// if(callee->decl)
				// DECL_ATTRIBUTES (callee->decl) = tree_cons (get_identifier ("always_inline"),
				// NULL, DECL_ATTRIBUTES (callee->decl));
			}
			always_inline = (DECL_DISREGARD_INLINE_LIMITS(callee->decl) && lookup_attribute("noinline", DECL_ATTRIBUTES(callee->decl)));
			// fprintf(stderr,"=======%s 's address:%d %d=========\n",get_name(callee->decl),callee->decl,always_inline);
		}

		// node->debug();
		push_cfun(DECL_STRUCT_FUNCTION(node->decl));
		if (cfun == NULL)
		{
			// fprintf(stderr,"=======NULL=========\n");
		}

		FOR_EACH_BB_FN(bb, cfun)
		{
			// debug_bb(bb);
			for (gimple_stmt_iterator gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi))
			{
				gimple *gc = gsi_stmt(gsi);
				// debug_gimple_stmt(gc);
				if (is_gimple_call(gc))
				{

					if (gimple_call_fn(gc) == NULL)
						continue;
					name = get_name(gimple_call_fn(gc));
					if (name == NULL)
						continue;
					// fprintf(stderr, "functionname %s\n",name);
					if (!strcmp(name, "free") ||
						!strcmp(name, "xfree") ||
						!strcmp(name, "malloc") ||
						!strcmp(name, "realloc") ||
						!strcmp(name, "xmalloc") ||
						!strcmp(name, "calloc") ||
						!strcmp(name, "xcalloc") ||
						!strcmp(name, "strdup"))
					{

						// fprintf(stderr,"=======add_always:%d=========\n",node->decl);
						// noinline
						// always_inline
						always_inline = (DECL_DISREGARD_INLINE_LIMITS(node->decl) && lookup_attribute("always_inline", DECL_ATTRIBUTES(node->decl)));
						if (!always_inline && !MAIN_NAME_P(DECL_NAME(node->decl)))
						{
							DECL_ATTRIBUTES(node->decl) = tree_cons(get_identifier("always_inline"), NULL, DECL_ATTRIBUTES(node->decl));
							DECL_DISREGARD_INLINE_LIMITS(node->decl) = 1;
							// printf("always_inline\n");
						}
					}
				}
			}
		}
		pop_cfun();
	}
};