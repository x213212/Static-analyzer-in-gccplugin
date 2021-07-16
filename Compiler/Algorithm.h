#include "system.h"
#include "config.h"
#include "DFS.h"
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

using namespace std;

static int totalsize; //宣告一個整數型態size變數，用來儲存x的位元組大小

int check_stmtStack(tree target)
{
	for (int o = 0; o < stmtStack.size(); o++)
	{

		if (stmtStack.c[o] == target)
		{
			// find = 1;
			// fprintf(stderr, "\033[40;41m =======recursive_stmt========= \033[0m\n");
			// debug_tree(stmtStack.c[o]);
			// fprintf(stderr, "\033[40;41m =======recursive_fun:%s========= \033[0m\n", get_name(stmtStack.c[o]));
			// pathStack.push((function_path_array)[i].next);
			return 1;
		}
	}
	int size = sizeof(tree);
	totalsize += size;
	stmtStack.push(target);
	return 0;
}
/*new_search_imm_use */
void new_search_imm_use(gimple_array *used_stmt, tree target, tree target2)
{
	imm_use_iterator imm_iter;
	gimple *use_stmt;
	gimple *gc;
	const char *name;
	gimple *def_stmt = SSA_NAME_DEF_STMT(target);
	gimple *def_stmt2 = SSA_NAME_DEF_STMT(target);
	int size = sizeof(used_stmt);
	// fprintf(stderr, "count size %d ", size);
	// // fprintf(stderr, "START NEW FOR_EACH_IMM_USE_STMT -------------------------------\n");
	// debug_tree(target);
	// fprintf(stderr, "TARGET NEW FOR_EACH_IMM_USE_STMT -------------------------------\n");
	if (!strcmp(get_tree_code_name(TREE_CODE(target)), "addr_expr"))
	{
		// fprintf(stderr, "GIMPLE CODE :addr_expr--------\n");
		return;
	}
	// if (target2 == NULL)
	// {
	// 	fprintf(stderr, "debug module\n");
	// 	debug_tree(target);
	// 	fprintf(stderr, "debug module\n");

	// 	// return ;
	// }
	// if(has_zero_uses (target2))
	// continue;
	// debug_tree(target);
	FOR_EACH_IMM_USE_STMT(use_stmt, imm_iter, target)
	{

		// fprintf(stderr, "			FOR_EACH_IMM_USE_STMT INSIDE-------------------------------\n");
		// debug(use_stmt);
		// if(target ==target2)

		if (target2 == NULL)
		{
			// debug_tree(target);
			// if(target == target2)
			// fprintf(stderr, "possible recivedebug module\n");
			// debug(use_stmt);

			// fprintf(stderr, "debug module2\n");
			// continue;
			// if (gimple_code(use_stmt) == GIMPLE_ASSIGN)
			// 	if (gimple_assign_lhs(use_stmt) && TREE_CODE(gimple_assign_lhs(use_stmt)) == VAR_DECL)
			// 	{
			// 		gimple *def_stmt = SSA_NAME_DEF_STMT(target2);
			// 		if (gimple_code(def_stmt) == GIMPLE_ASSIGN)
			// 		{

			// 			if (TREE_CODE(gimple_assign_rhs1(def_stmt)) == VAR_DECL)
			// 			{
			// 				fprintf(stderr, "debug mo\n");
			// 			}
			// 		}
			// 	}
			// continue;
		}
		gimple *def_stmt2 = SSA_NAME_DEF_STMT(target2);
		// if ( (target2 == gimple_assign_rhs1(use_stmt)))
		// 	continue;
		// if ( (target2 == gimple_assign_lhs(use_stmt)))
		// 	continue;
		// if(gimple_assign_lhs(def_stmt)==)
		// fprintf(stderr,"			FOR_EACH_IMM_USE_STMT INSIDE-------------------------------\n");

		if (gimple_code(use_stmt) == GIMPLE_RETURN)
		{
			// fprintf(stderr, "GIMPLE_RETURN\n");
			// fprintf(stderr, "------------------GIMPLE_RETURN : LHS------------------\n");
			tree return_tree = gimple_return_retval(as_a<greturn *>(use_stmt));
			if (!check_stmtStack(return_tree))
			{
				set_gimple_array(used_stmt, use_stmt, return_tree, return_tree, NULL);
			}
			// set_gimple_array(used_stmt, use_stmt, return_tree, return_tree, NULL);
			//可能產生無窮迴圈
			// if (TREE_CODE(return_tree) == SSA_NAME )
			// 	new_search_imm_use(used_stmt, return_tree, return_tree);
		}
		else if (gimple_code(use_stmt) == GIMPLE_ASSIGN)
		{
			// fprintf(stderr, "GIMPLE ASSIGN\n");
			// fprintf(stderr, "測試fuck %d\n", TREE_STATIC(gimple_assign_lhs(use_stmt)));

			//debug_tree(gimple_assign_lhs(use_stmt));
			// debug_gimple_stmt(use_stmt);
			// debug_tree(gimple_assign_lhs(use_stmt));
			// debug_tree(gimple_assign_lhs(use_stmt));
			// fprintf(stderr, "%s\n", get_tree_code_name(TREE_CODE(gimple_assign_lhs(use_stmt))));
			if (gimple_assign_lhs(use_stmt) && TREE_CODE(gimple_assign_lhs(use_stmt)) == SSA_NAME)
			{
				// debug_tree(gimple_assign_lhs(use_stmt));

				// debug(use_stmt);
				// fprintf(stderr, "------------------SSA_NAME : LHS------------------\n");
				// debug(use_stmt);
				// debug_tree(gimple_assign_lhs(use_stmt));
				// if (def_stmt2 == use_stmt)
				// 	continue;

				// if (TREE_CODE(gimple_assign_rhs1(use_stmt)) == COMPONENT_REF)
				// {

				// 	tree fundecl = TREE_OPERAND(gimple_assign_rhs1(use_stmt), 0);
				// 	tree fundecl2 = TREE_OPERAND(fundecl, 0);
				// 	debug(fundecl2);
				// 	if (target2 == fundecl2)
				// 		continue;
				// 	else
				// 	{
				// 		set_gimple_array(used_stmt, use_stmt, gimple_assign_rhs1(use_stmt), target, NULL);
				// 		// new_search_imm_use(used_stmt, gimple_assign_rhs1(use_stmt), gimple_assign_rhs1(use_stmt));
				// 		fprintf(stderr, "------------------SSA_NAME : fuck------------------\n");
				// 	}
				// }
				// fprintf(stderr, "------------------SSA_NAME : LHS------------------\n");
				// gimple *def_stmt = SSA_NAME_DEF_STMT( gimple_phi_result(use_stmt));
				// debug(def_stmt);
				// if (gimple_assign_lhs(use_stmt) != target2)
				// {
				// 	set_gimple_array(used_stmt, use_stmt, gimple_assign_lhs(use_stmt), target, NULL);
				// 	new_search_imm_use(used_stmt, gimple_assign_lhs(use_stmt), gimple_assign_lhs(use_stmt));
				// }
				if (!check_stmtStack(gimple_assign_lhs(use_stmt)))
				{
					set_gimple_array(used_stmt, use_stmt, gimple_assign_lhs(use_stmt), target, NULL);

					new_search_imm_use(used_stmt, gimple_assign_lhs(use_stmt), gimple_assign_lhs(use_stmt));
				}
				if (gimple_assign_rhs1(use_stmt) && TREE_CODE(gimple_assign_rhs1(use_stmt)) == SSA_NAME)
				{
					// fprintf(stderr, "------------------SSA_NAME : fuck------------------\n");
					// if (gimple_assign_rhs1(use_stmt) != target2)
					// 	new_search_imm_use(used_stmt, gimple_assign_rhs1(use_stmt), gimple_assign_rhs1(use_stmt));
					// else
					// 	fprintf(stderr, "------------------SSA_NAME : fuck2------------------\n");

					if (!check_stmtStack(gimple_assign_rhs1(use_stmt)))
					{
						// fprintf(stderr, "------------------SSA_NAME : rhs ssaname------------------\n");
						if (gimple_assign_rhs1(use_stmt) != target2)
							new_search_imm_use(used_stmt, gimple_assign_rhs1(use_stmt), gimple_assign_rhs1(use_stmt));
						// else
						// 	fprintf(stderr, "------------------SSA_NAME : rhs ssaname-------------------\n");
					}
				}
				// if (gimple_assign_rhs1(use_stmt) && TREE_CODE(gimple_assign_rhs1(use_stmt)) == ARRAY_REF)
				// {
				// 	fprintf(stderr, "------------------SSA_NAME : ARRAY_REF------------------\n");
				// 	// if (gimple_assign_rhs1(use_stmt) != target2)
				// 	// 	new_search_imm_use(used_stmt, gimple_assign_rhs1(use_stmt), gimple_assign_rhs1(use_stmt));
				// 	// else
				// 	// 	fprintf(stderr, "------------------SSA_NAME : fuck2------------------\n");

				// 	// if (!check_stmtStack(gimple_assign_rhs1(use_stmt)))
				// 	// {
				// 	// 	// fprintf(stderr, "------------------SSA_NAME : rhs ssaname------------------\n");
				// 	// 	if (gimple_assign_rhs1(use_stmt) != target2)
				// 	// 		new_search_imm_use(used_stmt, gimple_assign_rhs1(use_stmt), gimple_assign_rhs1(use_stmt));
				// 	// 	// else
				// 	// 	// 	fprintf(stderr, "------------------SSA_NAME : rhs ssaname-------------------\n");
				// 	// }
				// }
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
				// fprintf(stderr, "------------------VAR_DECL : LHS------------------\n");
				// debug_tree( gimple_assign_rhs1(use_stmt));
				// debug(use_stmt);
				// debug_tree (gimple_assign_lhs(use_stmt));
				//fprintf(stderr, "VAR_DECLVAR_DECLVAR_DECL------------------\n");
				// debug_gimple_stmt(use_stmt);
				// debug_tree(gimple_assign_lhs(use_stmt));
				//fprintf(stderr, "VAR_DECLVAR_DECLVAR_DECL------------------\n");
				// fprintf(stderr, "------------------VAR_DECL : LHS-----------------2-\n");
				if (!check_stmtStack(gimple_assign_lhs(use_stmt)))
				{
					set_gimple_array(used_stmt, use_stmt, gimple_assign_lhs(use_stmt), target, NULL);
					// new_search_imm_use(used_stmt, gimple_assign_lhs(use_stmt), gimple_assign_lhs(use_stmt));
				}
				// set_gimple_array(used_stmt, use_stmt, gimple_assign_lhs(use_stmt), target, NULL);

				// tree testttt = gimple_assign_lhs(use_stmt);
				// debug(use_stmt);
				tree getFunctionAssignVAR = gimple_assign_lhs(use_stmt);
				function_assign_array assign_array;
				if (function_assign_collect->get(getFunctionAssignVAR) != NULL)
				{

					// debug(use_stmt);
					// fprintf(stderr, "=======fist hit========\n");
					assign_array = ret_function_varstmt(getFunctionAssignVAR);
					// fprintf(stderr, "=======print_function_var fuck %d========\n", assign_array.pass);
					// fprintf(stderr, "=======print_function_var %d   %d========\n", getFunctionAssignVAR, assign_array.assign_type_array.size());
					for (int i = 0; i < assign_array.assign_type_array.size(); i++)
					{

						// if ((assign_array.assign_type_array)[i].stmt == use_stmt)
						// {
						// 	debug((assign_array.assign_type_array)[i].stmt);
						// 	continue;
						// }

						if (gimple_code((assign_array.assign_type_array)[i].stmt) == GIMPLE_ASSIGN)
						{

							if (TREE_CODE(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt)) == SSA_NAME)

								if (!check_stmtStack(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt)))
								{

									// debug(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt));
									set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, gimple_assign_lhs((assign_array.assign_type_array)[i].stmt), target, NULL);
									// used_stmt = used_stmt2;
									new_search_imm_use(used_stmt, gimple_assign_lhs((assign_array.assign_type_array)[i].stmt), gimple_assign_lhs((assign_array.assign_type_array)[i].stmt));
								}

								else if (TREE_CODE(gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt)) == SSA_NAME)

									if (!check_stmtStack(gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt)))
									{
										// debug((assign_array.assign_type_array)[i].stmt);
										set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt), target, NULL);
										new_search_imm_use(used_stmt, gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt), gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt));
									}
							// set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt), target, NULL);
							// new_search_imm_use(used_stmt, gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt), gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt));
						}

						if (gimple_code((assign_array.assign_type_array)[i].stmt) == GIMPLE_CALL)
						{

							// tree second = TREE_OPERAND(gimple_call_arg(gc, 0), 0);
							const char *name;
							name = get_tree_code_name(TREE_CODE(gimple_call_arg((assign_array.assign_type_array)[i].stmt, 0)));

							if (name != NULL)
								if (!strcmp(name, "addr_expr"))
									if (!check_stmtStack(gimple_call_arg((assign_array.assign_type_array)[i].stmt, 0)))
									{
										// debug(gimple_call_arg((assign_array.assign_type_array)[i].stmt, 0));
										set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, gimple_call_arg(gc, 0), target, NULL);
									}
						}

						// else if (TREE_CODE(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt)) == VAR_DECL)
						// {
						// gimple *def_stmt = SSA_NAME_DEF_STMT(used_stmt->target);
						// if (def_stmt != NULL)
						// 	if (gimple_code(def_stmt) == GIMPLE_ASSIGN)
						// 		if (TREE_CODE(gimple_assign_lhs(def_stmt)) == SSA_NAME)
						// 			if (gimple_assign_rhs1(def_stmt) == getFunctionAssignVAR)
						// 			{
						// 				find = 1;
						// 				fprintf(stderr, "wwwwwwwwwwwwwwww\n");
						// 				// return;
						// 			}
						// debug(gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt));
						// fprintf(stderr, "---------------RKO-------------2-\n");
						// if (TREE_CODE(gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt)) == SSA_NAME)
						// 	if (gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt) != getFunctionAssignVAR)
						// 		new_search_imm_use(used_stmt, gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt), NULL);
						// }
					}
					// fprintf(stderr, "=======VAR_DECL Unfold========\n");
					// 	}
					// }
					// else
					// {
					// 	debug(use_stmt);
					// 	fprintf(stderr, "=======double hit========\n");
					// }
				}
				// if (assign_array.assign_type_array.size() )
				// {

				// for (int i = 0; i < assign_array.assign_type_array.size(); i++)
				// {
				// 	// if (used_stmt->stmt == NULL){
				// 	// 	fprintf(stderr, "------------------1234-----------------2-\n");
				// 	// 	set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, gimple_assign_lhs((assign_array.assign_type_array)[i].stmt), target, NULL);
				// 	// }
				// 	// for (int i = 0; i < (ret_type_array).size(); i++)
				// 	// {
				// 	// assign_tree
				// 	// debug((assign_array.assign_type_array)[i].stmt);
				// 	// gimple *def_stmt2 = SSA_NAME_DEF_STMT((assign_array.assign_type_array)[i].assig);
				// 	// if (gimple_code((assign_array.assign_type_array)[i].stmt) == GIMPLE_ASSIGN)
				// 	// 	if (TREE_CODE(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt)) == SSA_NAME)
				// 	// 	{
				// 	fprintf(stderr, "------------------fuc222222222kkkkkkkker-----------------2-\n");
				// 	debug((assign_array.assign_type_array)[i].stmt);
				// 	debug(use_stmt);
				// 	// if((assign_array.assign_type_array)[i].stmt == use_stmt)
				// 	fprintf(stderr, "------------------fuc222222222kkkkkkkker-----------------2-\n");
				// 	// }
				// 	// if(used_stmt->stmt != NULL)
				// 	// if (gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt) == getFunctionAssignVAR)

				// 	if (gimple_code((assign_array.assign_type_array)[i].stmt) == GIMPLE_ASSIGN)
				// 		if (TREE_CODE(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt)) == SSA_NAME)
				// 		{
				// 			// gimple_array *used_stmt2 = used_stmt;
				// 			// gimple *u_stmt2;
				// 			// gimple_array start;
				// 			// gimple_array start2;
				// 			// start2.stmt = used_stmt->stmt;
				// 			// start.stmt = NULL;
				// 			// used_stmt2 = &start;
				// 			// int find = 0;
				// 			// // gimple *def_stmt = SSA_NAME_DEF_STMT(used_stmt2->target);
				// 			// // fprintf(stderr, "-----%s-------------2-\n", name);
				// 			// // if (def_stmt != NULL)
				// 			// // 	debug(def_stmt);
				// 			// fprintf(stderr, "------------------fuckkkkkkkkkkkkkk222222222kkkkkkkkkkkker-----------------2-\n");
				// 			// // if(used_stmt2->stmt == NULL)
				// 			// // 	continue;
				// 			// if (used_stmt->stmt != NULL)
				// 			// {
				// 			// 	debug(used_stmt->stmt);
				// 			// 	fprintf(stderr, "------------------123-----------------2-\n");
				// 			// 	// debug(u_stmt2);
				// 			// }
				// 			// if(u_stmt2 == NULL)
				// 			// 	break;
				// 			// FOR_EACH_USE_TABLE(used_stmt2, u_stmt2)
				// 			// {
				// 			// 	// if (used_stmt->stmt  == NULL)
				// 			// 	// 	continue;
				// 			// 	// if(u_stmt2 == NULL)
				// 			// 	// continue;
				// 			// 	if (used_stmt->target != NULL && used_stmt->stmt != NULL)
				// 			// 	{
				// 			// 		find = 2;
				// 			// 		if (used_stmt->stmt == NULL)
				// 			// 		{
				// 			// 			fprintf(stderr, "------------------廢物-----------------2-\n");
				// 			// 		}
				// 			// 		gimple *def_stmt3 = SSA_NAME_DEF_STMT(used_stmt->target);
				// 			// 		if (gimple_code(def_stmt3) == GIMPLE_PHI)
				// 			// 			// if (gimple_code(used_stmt->stmt) == GIMPLE_PHI)
				// 			// 			debug_tree(used_stmt->target);
				// 			// 		// else
				// 			// 		used_stmt->stmt = def_stmt3;

				// 			// 		//debug(def_stmt3);
				// 			// 		fprintf(stderr, "------------------qweqwe-----------------2-\n");
				// 			// 		fprintf(stderr, "------------------fuckkkkkkkkkkkkkk222222222kkkkkkkkkkkker-----------------2-\n");
				// 			// 		// }
				// 			// 		if (u_stmt2 != NULL)
				// 			// 			if (is_gimple_assign(u_stmt2))
				// 			// 			{

				// 			// 				debug_tree(used_stmt->target);
				// 			// 				if (TREE_CODE(used_stmt->target) == SSA_NAME)
				// 			// 				{
				// 			// 					name = get_tree_code_name(TREE_CODE(used_stmt->target));
				// 			// 					// if (name != NULL)
				// 			// 					// {
				// 			// 					debug_tree(used_stmt->target);
				// 			// 					//if(gimple_code(used_stmt->) == GIMPLE_PHI)
				// 			// 					if (name != NULL)
				// 			// 					{
				// 			// 						if (!strcmp(name, "ssa_name"))
				// 			// 						{
				// 			// 							gimple *def_stmt2 = SSA_NAME_DEF_STMT(used_stmt->target);
				// 			// 							if (gimple_code(def_stmt2) == GIMPLE_PHI)
				// 			// 							{
				// 			// 								if (TREE_CODE(gimple_assign_lhs(def_stmt2)) == SSA_NAME)
				// 			// 								{
				// 			// 									if (gimple_assign_rhs1(def_stmt2) == getFunctionAssignVAR)
				// 			// 									{
				// 			// 										find = 1;

				// 			// 										fprintf(stderr, "wwwwwwwwwwwwwwww%s\n", name);
				// 			// 									}
				// 			// 								}
				// 			// 							}

				// 			// 							else if (gimple_code(used_stmt->stmt) == GIMPLE_ASSIGN)
				// 			// 							{
				// 			// 								gimple *def_stmt = SSA_NAME_DEF_STMT(used_stmt->target);
				// 			// 								if (def_stmt != NULL)
				// 			// 									if (gimple_code(def_stmt) == GIMPLE_ASSIGN)
				// 			// 										if (TREE_CODE(gimple_assign_lhs(def_stmt)) == SSA_NAME)
				// 			// 											if (gimple_assign_rhs1(def_stmt) == getFunctionAssignVAR)
				// 			// 											{
				// 			// 												find = 1;
				// 			// 												fprintf(stderr, "wwwwwwwwwwwwwwww\n");
				// 			// 												// return;
				// 			// 											}
				// 			// 							}
				// 			// 						}
				// 			// 					}
				// 			// 				}

				// 			// 				// debug(def_stmt);
				// 			// 				// 		name = get_tree_code_name(TREE_CODE(used_stmt2->target));
				// 			// 				// 		// 			if(name != NULL)
				// 			// 				// 		// 			if (!strcmp(name, "<invalid tree code>") ||!strcmp(name, "union_type"))

				// 			// 				// {debug_tree(getFunctionAssignVAR);
				// 			// 				// debug_tree(gimple_assign_rhs1(def_stmt));
				// 			// 				// fprintf(stderr, "------------------fuckkkkkkkkkkkkk333kkkkkkkkkkkker-----------------2-\n");
				// 			// 				// 	}
				// 			// 				// if (def_stmt != NULL)
				// 			// 				// 	debug(def_stmt);
				// 			// 				// if (gimple_code(u_stmt2) == GIMPLE_ASSIGN)
				// 			// 				// {
				// 			// 				// 	tree tmp = gimple_assign_lhs(u_stmt2);
				// 			// 				// 	if (tmp != NULL)
				// 			// 				// 		if (TREE_CODE(tmp) == SSA_NAME)

				// 			// 				// }

				// 			// 				// if (u_stmt2 == (assign_array.assign_type_array)[i].stmt)
				// 			// 				// {

				// 			// 				// 	find = 1;
				// 			// 				// }
				// 			// 			}
				// 			// 		// if (TREE_CODE(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt)) == VAR_DECL)
				// 			// 		// {
				// 			// 		// 	fprintf(stderr, "---------------RKO-------------2-\n");
				// 			// 		// }
				// 			// 	}

				// 			// 	// else
				// 			// 	// 	used_stmt = used_stmt2;
				// 			// }

				// 			// if (find == 0)
				// 			// {
				// 			// used_stmt = used_stmt2;
				// 			// used_stmt = &start2;
				// 			if (gimple_assign_lhs((assign_array.assign_type_array)[i].stmt) !=getFunctionAssignVAR)
				// 			{
				// 				debug((assign_array.assign_type_array)[i].stmt);
				// 				// set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, gimple_assign_lhs((assign_array.assign_type_array)[i].stmt), target, NULL);
				// 				// used_stmt = used_stmt2;
				// 				new_search_imm_use(used_stmt, gimple_assign_lhs((assign_array.assign_type_array)[i].stmt), NULL);
				// 			}
				// 			// else
				// 			// break;
				// 			// }
				// 			// if (used_stmt->stmt == NULL)
				// 			// 	set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, gimple_assign_lhs((assign_array.assign_type_array)[i].stmt), target, NULL);
				// 			// if (gimple_code((assign_array.assign_type_array)[i].stmt) == GIMPLE_ASSIGN)
				// 			// new_search_imm_use(used_stmt, gimple_assign_lhs((assign_array.assign_type_array)[i].stmt), gimple_assign_lhs((assign_array.assign_type_array)[i].stmt));
				// 		}
				// 		else if (TREE_CODE(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt)) == VAR_DECL)
				// 		{
				// gimple *def_stmt = SSA_NAME_DEF_STMT(used_stmt->target);
				// if (def_stmt != NULL)
				// 	if (gimple_code(def_stmt) == GIMPLE_ASSIGN)
				// 		if (TREE_CODE(gimple_assign_lhs(def_stmt)) == SSA_NAME)
				// 			if (gimple_assign_rhs1(def_stmt) == getFunctionAssignVAR)
				// 			{
				// 				find = 1;
				// 				fprintf(stderr, "wwwwwwwwwwwwwwww\n");
				// 				// return;
				// 			}
				// debug(gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt));
				// fprintf(stderr, "---------------RKO-------------2-\n");
				// 	if (TREE_CODE(gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt)) == SSA_NAME)
				// 		if (gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt) != getFunctionAssignVAR)
				// 			new_search_imm_use(used_stmt, gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt), NULL);
				// }
				// if (gimple_code((assign_array.assign_type_array)[i].stmt) == GIMPLE_ASSIGN)
				// 	if (TREE_CODE(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt)) == SSA_NAME)
				// 	{
				// 		set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, gimple_call_arg((assign_array.assign_type_array)[i].stmt, 0), target, NULL);
				// 		// if (gimple_code((assign_array.assign_type_array)[i].stmt) == GIMPLE_ASSIGN)
				// new_search_imm_use(used_stmt, gimple_call_arg((assign_array.assign_type_array)[i].stmt, 0), gimple_call_arg((assign_array.assign_type_array)[i].stmt, 0));
				// 	}
				// if (gimple_code((assign_array.assign_type_array)[i].stmt) == GIMPLE_CALL)

				//{
				// debug_tree(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt));
				// // debug((assign_array.assign_type_array)[i].stmt);
				// fprintf(stderr, "------------------fuckkkkkkkkkkkkkkkkkkkkkkkkkker-----------------2-\n");
				// gimple_array *used_stmt2 = used_stmt;
				// gimple *u_stmt2;

				// gimple_array start;
				// gimple_array now;
				// // now.stmt = used_stmt->stmt;
				// start.stmt = NULL;
				// used_stmt2 = &start;
				// int find = 0;
				// int count = 0;
				// fprintf(stderr, "------------------dwq-----------------2-\n");
				// debug((assign_array.assign_type_array)[i].stmt);
				// 	// debug_tree(used_stmt->target);
				// if (used_stmt->size == 1)
				// {
				// 	fprintf(stderr, "------------------possible gimple_array Recursive-----------------2-\n");
				// 	FOR_EACH_USE_TABLE(used_stmt, u_stmt2)
				// 	{
				// 		if (u_stmt2== (assign_array.assign_type_array)[i].stmt){
				// 			find = 1;
				// 			fprintf(stderr, "------------------possible gimple_array Recursive-----------------2-\n");
				// 			debug(u_stmt2);
				// 			fprintf(stderr, "------------------possible gimple_array Recursive-----------------2-\n");

				// 			}
				// 		// if(used_stmt2->stmt)
				// 		// 		if(used_stmt2->target == NULL)
				// 		// 			return ;
				// 		// 			if(gimple_code (used_stmt2->stmt) ==GIMPLE_ASSIGN &&gimple_code (used_stmt2->stmt) ==GIMPLE_CALL){

				// 		// 		name = get_tree_code_name(TREE_CODE(used_stmt2->target));
				// 		// 			if(name != NULL)
				// 		// 			if (!strcmp(name, "<invalid tree code>") ||!strcmp(name, "union_type"))
				// 		// 			continue;
				// 		// 		gimple *def_stmt = SSA_NAME_DEF_STMT(used_stmt2->target);
				// 		// 			fprintf(stderr, "-----%s-------------2-\n",name);
				// 		// 		if(def_stmt != NULL)
				// 		// debug(def_stmt);
				// 		// count ++;
				// 		// if(used_stmt2->target == NULL)
				// 		// return ;
				// 		// if(TREE_CODE(used_stmt2->target)== GIMPLE_ASSIGN){
				// 		// name = get_tree_code_name(TREE_CODE(used_stmt2->target));
				// 		// fprintf(stderr, "-----%s-------------2-\n",name);
				// 		// if(name != NULL)

				// 		// if (!strcmp(name, "<invalid tree code>") ||!strcmp(name, "union_type"))
				// 		// continue;
				// 		// if(getFunctionAssignVAR!= NULL)
				// 		// if( getFunctionAssignVAR ==used_stmt2->target)
				// 		// find =1;
				// 		// debug_tree(used_stmt2->target);
				// 		// // else
				// 		// // debug_tree(used_stmt2->target);
				// 		// fprintf(stderr, "------------------fuckkkkkkkkkkkkkkkkkkkkkkkkkker-----------------2-\n");
				// 		// }
				// 	}
				// 	fprintf(stderr, "------------------fuckkkkkkkkkkkkkkkkkkkkkkkkkker-----------------2-\n");
				// 	// return ;
				// 	if (find == 0){
				// 		// used_stmt = used_stmt2;
				// 		set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, gimple_assign_lhs((assign_array.assign_type_array)[i].stmt), target, NULL);
				// 		new_search_imm_use(used_stmt, gimple_assign_lhs((assign_array.assign_type_array)[i].stmt), gimple_assign_lhs((assign_array.assign_type_array)[i].stmt));
				// 		}
				// }

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
				// }

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
				// fprintf(stderr, "------------------MEM_REF : LHS------------------\n");

				// debug_tree( gimple_assign_rhs1(use_stmt));
				// debug(use_stmt);
				// debug_tree (gimple_assign_lhs(use_stmt));
				//fprintf(stderr, "VAR_DECLVAR_DECLVAR_DECL------------------\n");
				// debug_gimple_stmt(use_stmt);
				// debug_tree(gimple_assign_lhs(use_stmt));
				tree fundecl = TREE_OPERAND(gimple_assign_lhs(use_stmt), 0);
				// debug_tree(fundecl);
				// debug(use_stmt);

				//tree fundecl = TREE_OPERAND(gc, 0);

				//fprintf(stderr, "VAR_DECLVAR_DECLVAR_DECL------------------\n");
				// fprintf(stderr, "------------------MEM_REF : LHS------------------\n");
				if (!check_stmtStack(gimple_assign_lhs(use_stmt)))
				{

					set_gimple_array(used_stmt, use_stmt, gimple_assign_lhs(use_stmt), gimple_assign_lhs(use_stmt), NULL);
					// fprintf(stderr, "-----------------rkoqwrkoqwrkoqwkr-----------------\n");
					// set_gimple_array(used_stmt, use_stmt, gimple_assign_rhs1(use_stmt), target, NULL);
					// if (TREE_CODE(gimple_assign_rhs1((use_stmt))) == SSA_NAME)
					// {
					// new_search_imm_use(used_stmt, gimple_assign_rhs1(use_stmt), gimple_assign_rhs1(use_stmt));
					// }
				}
				// set_gimple_array(used_stmt, use_stmt, fundecl, fundecl, NULL);
				// if (TREE_CODE(fundecl) == SSA_NAME)

				// 	new_search_imm_use(used_stmt, fundecl, fundecl);
				// new_search_imm_use(used_stmt,  gimple_assign_rhs1(use_stmt),   gimple_assign_lhs(use_stmt));
			}

			else if (gimple_assign_lhs(use_stmt) && TREE_CODE(gimple_assign_lhs(use_stmt)) == COMPONENT_REF)
			{
				// fprintf(stderr, "------------------COMPONENT_REF : LHS------------------\n");
				// debug_tree(gimple_assign_lhs(use_stmt));

				// debug(use_stmt);
				// debug_tree(use_stmt);
				// set_gimple_array(used_stmt, use_stmt, gimple_assign_lhs(use_stmt), target, NULL);
				// fprintf(stderr, "------------------COMPONENT_REF : LHS------------------\n");
				// debug(use_stmt);
				// debug_tree(gimple_assign_lhs(use_stmt));
				// fprintf(stderr, "------------------COMPONENT_REF : LHS------------------\n");
				// new_search_imm_use(used_stmt, gimple_assign_lhs(use_stmt), gimple_assign_lhs(use_stmt));
				// if (gimple_assign_lhs(use_stmt) != target2 && gimple_assign_rhs1(use_stmt) != target2)
				// {
				// if (!check_stmtStack(use_stmt))
				// {

				// 	set_gimple_array(used_stmt, use_stmt, gimple_assign_lhs(use_stmt), target, NULL);
				// }
				// set_gimple_array(used_stmt, use_stmt, gimple_assign_lhs(use_stmt), target, NULL);
				// if (!check_stmtStack(use_stmt))
				// {
				// if ((gimple_assign_lhs(def_stmt2) == gimple_assign_rhs1(use_stmt)) || (target2 == gimple_assign_rhs1(use_stmt)))
				// 	continue;
				// if ((gimple_assign_rhs1(def_stmt2) == gimple_assign_lhs(use_stmt)) || (target2 == gimple_assign_lhs(use_stmt)))
				// 	continue;
				if (TREE_CODE(gimple_assign_lhs(use_stmt)) == SSA_NAME)
				{
					if (!check_stmtStack(gimple_assign_lhs(use_stmt)))
					{
						// gimple_assign_lhs(def_stmt2) != gimple_assign_lhs(use_stmt)
						// fprintf(stderr, "------------------COMPONENT_REF : LHS-2-----------------\n");
						set_gimple_array(used_stmt, use_stmt, gimple_assign_lhs(use_stmt), target, NULL);

						// if (gimple_assign_lhs(use_stmt) != target2)
						new_search_imm_use(used_stmt, gimple_assign_lhs(use_stmt), gimple_assign_lhs(use_stmt));
					}
				}
				else
				{
					if (!check_stmtStack(gimple_assign_lhs(use_stmt)))
					{
						// gimple_assign_lhs(def_stmt2) != gimple_assign_lhs(use_stmt)
						// fprintf(stderr, "------------------COMPONENT_REF : LHS-3-----------------\n");
						set_gimple_array(used_stmt, use_stmt, gimple_assign_lhs(use_stmt), target, NULL);

						// if (gimple_assign_lhs(use_stmt) != target2)
						// new_search_imm_use(used_stmt, gimple_assign_lhs(use_stmt), gimple_assign_lhs(use_stmt));
					}
				}
				if (TREE_CODE(gimple_assign_rhs1(use_stmt)) == SSA_NAME)
				{

					if (!check_stmtStack(gimple_assign_rhs1(use_stmt)))
					{
						// debug_tree(gimple_assign_lhs(use_stmt));
						// fprintf(stderr, "-----------------GIMPLE_qweRT------------------\n");
						// debug_tree(gimple_assign_rhs1(use_stmt));
						// debug_tree(gimple_assign_rhs1(def_stmt2));
						// debug_tree(gimple_assign_lhs(def_stmt2));
						set_gimple_array(used_stmt, use_stmt, gimple_assign_rhs1(use_stmt), target, NULL);
						// 					if (gimple_assign_lhs(def_stmt2) == gimple_assign_rhs1(use_stmt))
						// 						continue;
						// 					if (gimple_assign_rhs1(def_stmt2) == gimple_assign_lhs(use_stmt))
						// 						continue;
						// 					if (gimple_assign_rhs1(use_stmt)==gimple_assign_rhs1(def_stmt2))
						// 						continue;
						// if (gimple_assign_lhs(use_stmt)==gimple_assign_lhs(def_stmt2))
						// 						continue;
						// if ((gimple_assign_rhs1(def_stmt2) != gimple_assign_rhs1(use_stmt)) && (gimple_assign_rhs1(def_stmt2) != gimple_assign_lhs(use_stmt)))
						// {
						// debug_tree(gimple_assign_lhs(use_stmt));

						// debug_tree(gimple_assign_lhs(use_stmt));
						// debug_tree(gimple_assign_rhs1(use_stmt));
						// if(gimple_assign_rhs1(use_stmt) != target2)
						// continue;
						// debug(def_stmt2);
						// debug_tree(gimple_assign_rhs1(def_stmt2));
						// debug_tree(gimple_assign_lhs(def_stmt2));
						// if(gimple_assign_lhs(def_stmt2))
						// fprintf(stderr, "------------------SSA_NAME : fuck------------------\n");
						if (gimple_assign_rhs1(use_stmt) != target2)
							new_search_imm_use(used_stmt, gimple_assign_rhs1(use_stmt), gimple_assign_rhs1(use_stmt));
						// else
						// fprintf(stderr, "------------------SSA_NAME : fuck2------------------\n");
					}
				}
				// if(has_single_use( gimple_assign_rhs1(use_stmt)))
				// 	set_gimple_array(used_stmt, use_stmt, gimple_assign_rhs1(use_stmt), target, NULL);

				// fprintf(stderr, "------------------SSA_NAME : fuck------------------\n");
				// if (TREE_CODE(gimple_assign_lhs(use_stmt)) == COMPONENT_REF)
				// {

				// 	tree fundecl = TREE_OPERAND(gimple_assign_lhs(use_stmt), 0);
				// 	debug_tree(fundecl);
				// 	tree fundecl2 = TREE_OPERAND(fundecl, 0);
				// 	// debug(fundecl2);
				// 	if (fundecl2 != NULL)
				// 		if (target2 == fundecl2)
				// 			set_gimple_array(used_stmt, use_stmt, gimple_assign_rhs1(use_stmt), target, NULL);
				// 		else
				// 		{
				// 			set_gimple_array(used_stmt, use_stmt, gimple_assign_rhs1(use_stmt), target, NULL);
				// 			if (gimple_assign_rhs1(use_stmt) != target2)
				// 			{
				// 				new_search_imm_use(used_stmt, gimple_assign_rhs1(use_stmt), gimple_assign_rhs1(use_stmt));
				// 				fprintf(stderr, "------------------SSA_NAME : fuck------------------\n");
				// 			}
				// 		}
				// }
				// else if (gimple_assign_rhs1(use_stmt) != target2)
				// {
				// 	fprintf(stderr, "-----------------GIMPLE_qwe123123-----------------\n");
				// 	set_gimple_array(used_stmt, use_stmt, gimple_assign_rhs1(use_stmt), target, NULL);
				// 	new_search_imm_use(used_stmt, gimple_assign_rhs1(use_stmt), gimple_assign_rhs1(use_stmt));
				// }
				// debug(use_stmt)

				// fprintf(stderr, "------------------COMPONENT_REF : RHS------------------\n");
				// continue;
				// }
				// else
				// {
				// 	if (!check_stmtStack(use_stmt))
				// 	{
				// 		debug_tree(gimple_assign_lhs(use_stmt));
				// 		fprintf(stderr, "-----------------GIMPLE_qweRT------------------\n");
				// 		set_gimple_array(used_stmt, use_stmt, gimple_assign_lhs(use_stmt), target, NULL);

				// 		new_search_imm_use(used_stmt, gimple_assign_lhs(use_stmt), gimple_assign_lhs(use_stmt));
				// 		fprintf(stderr, "------------------SSA_NAME : fuck------------------\n");
				// 	}
				// }
				// W}
			}
		}
		else if (gimple_code(use_stmt) == GIMPLE_PHI)
		{
			// fprintf(stderr, "GIMPLE_PHI\n");
			// fprintf(stderr, "-----------------GIMPLE_PHI : LOOP------------------\n");
			if (gimple_phi_result(use_stmt) && TREE_CODE(gimple_phi_result(use_stmt)) == SSA_NAME)
			{
				//debug_gimple_stmt(use_stmt);
				// debug_gimple_stmt(use_stmt);
				// //debug_tree(gimple_phi_result(use_stmt));
				// bool exist = false;
				// FOR_EACH_USE_TABLE(used2, gc)
				// {

				// 	//debug_gimple_stmt(gc);
				// 	if (gc == use_stmt)
				// 	{
				// 		exist = true;
				// 		// fprintf(stderr, "----------------what r u loolking for------------------\n");
				// 		debug_tree(gimple_phi_result(use_stmt));
				// 	}
				// }
				// if (!exist)
				// {
				if (!check_stmtStack(gimple_phi_result(use_stmt)))
				{

					// debug(use_stmt);
					// debug_tree(gimple_phi_result(use_stmt));
					set_gimple_array(used_stmt, use_stmt, gimple_phi_result(use_stmt), gimple_phi_result(use_stmt), NULL);
					// fprintf(stderr, "-----------------GIMPLE_PHI : INSERT------------------\n");
					// gimple *def_stmt = SSA_NAME_DEF_STMT( gimple_phi_result(use_stmt));
					// debug(def_stmt);
					// debug_tree(gimple_assign_lhs(def_stmt));
					// fprintf(stderr, "-----------------GIMPLE_PHI : INSERT------------------\n");
					if (TREE_CODE(gimple_phi_result(use_stmt)) == SSA_NAME)
					{
						new_search_imm_use(used_stmt, gimple_phi_result(use_stmt), gimple_phi_result(use_stmt));
					}
				}
				// }
			}
		}
		else if (gimple_code(use_stmt) == GIMPLE_CALL)
		{
			// fprintf(stderr, "GIMPLE_CALL2\n");

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
			if (name != NULL)
				if (!strcmp(name, "free") || !strcmp(name, "xfree"))
				{

					// debug(use_stmt);
					// debug_tree(target);
					// debug_tree(gimple_call_fn(use_stmt));
					// fprintf(stderr, "-----------------GIMPLE_CALL : FIN22D------------------\n");

					// fprintf(stderr, "-------always in therealways in therealways in there--------------------------\n");
					if (!check_stmtStack(gimple_call_fn(use_stmt)))
					{

						set_gimple_array(used_stmt, use_stmt, gimple_call_fn(use_stmt), target, NULL);
						if (TREE_CODE(gimple_call_fn(use_stmt)) == SSA_NAME)
							new_search_imm_use(used_stmt, gimple_call_fn(use_stmt), gimple_call_fn(use_stmt));
					}
					// set_gimple_array(used_stmt, use_stmt, gimple_call_fn(use_stmt), target, NULL);
					// if (TREE_CODE(gimple_call_fn(use_stmt)) == SSA_NAME && gimple_call_fn(use_stmt) != target2)
					// 	new_search_imm_use(used_stmt, gimple_call_fn(use_stmt), gimple_call_fn(use_stmt));
				}
				else if (!strcmp(name, "pthread_create") || !strcmp(name, "pthread_join"))
				{

					if (!check_stmtStack(gimple_call_fn(use_stmt)))
					{
						// fprintf(stderr, "-----------------GIMPLE_CALL : FIN22D------------------\n");
						// debug(target);

						tree second = NULL;
						int isVardecl = 0;
						if (!strcmp(get_tree_code_name(TREE_CODE(gimple_assign_rhs1(def_stmt))), "addr_expr"))
						{
							second = TREE_OPERAND(gimple_assign_rhs1(def_stmt), 0);
							second = TREE_OPERAND(second, 0);
							// debug_tree(second);
							isVardecl = 1;
						}
						else if (TREE_CODE(gimple_assign_rhs1(def_stmt)) == VAR_DECL)
						{
							second = gimple_assign_rhs1(def_stmt);
							// debug_tree(second);
							isVardecl = 1;
						}

						if (second != NULL)
							if (TREE_CODE(second) == VAR_DECL)
							{
								// fprintf(stderr, "-----------------GIMPLE_CALL : FIN22D2------------------\n");
								// if (TREE_CODE(gimple_assign_rhs1(def_stmt)) == VAR_DECL)
								// {
								// 	second = gimple_assign_rhs1(def_stmt);
								// }

								// debug_tree(second);
								tree getFunctionAssignVAR = second;
								function_assign_array assign_array;
								if (function_assign_collect->get(getFunctionAssignVAR) != NULL)
								{

									// debug(use_stmt);
									// fprintf(stderr, "=======fist hit========\n");
									assign_array = ret_function_varstmt(getFunctionAssignVAR);
									// fprintf(stderr, "=======print_function_var fuck %d========\n", assign_array.pass);
									// fprintf(stderr, "=======print_function_var %d   %d========\n", getFunctionAssignVAR, assign_array.assign_type_array.size());
									for (int i = 0; i < assign_array.assign_type_array.size(); i++)
									{

										// if ((assign_array.assign_type_array)[i].stmt == use_stmt)
										// {
										// 	debug((assign_array.assign_type_array)[i].stmt);
										// 	continue;
										// }

										if (gimple_code((assign_array.assign_type_array)[i].stmt) == GIMPLE_ASSIGN)
										{

											if (gimple_assign_lhs((assign_array.assign_type_array)[i].stmt) != target && TREE_CODE(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt)) == SSA_NAME)

												if (!check_stmtStack(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt)))
												{

													// debug(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt));

													set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, gimple_assign_lhs((assign_array.assign_type_array)[i].stmt), target, NULL);
													// used_stmt = used_stmt2;
													new_search_imm_use(used_stmt, gimple_assign_lhs((assign_array.assign_type_array)[i].stmt), gimple_assign_lhs((assign_array.assign_type_array)[i].stmt));
												}

												else if (gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt) != target && TREE_CODE(gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt)) == SSA_NAME)

													if (!check_stmtStack(gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt)))
													{
														// debug((assign_array.assign_type_array)[i].stmt);
														set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt), target, NULL);
														new_search_imm_use(used_stmt, gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt), gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt));
													}
											// set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt), target, NULL);
											// new_search_imm_use(used_stmt, gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt), gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt));
										}

										if (gimple_code((assign_array.assign_type_array)[i].stmt) == GIMPLE_CALL)
										{

											// tree second = TREE_OPERAND(gimple_call_arg(gc, 0), 0);
											const char *name;
											name = get_tree_code_name(TREE_CODE(gimple_call_arg((assign_array.assign_type_array)[i].stmt, 0)));

											if (name != NULL)
												if (!strcmp(name, "addr_expr"))
													if (!check_stmtStack(gimple_call_arg((assign_array.assign_type_array)[i].stmt, 0)))
													{
														// debug(gimple_call_arg((assign_array.assign_type_array)[i].stmt, 0));
														set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, gimple_call_arg(gc, 0), target, NULL);
													}
										}

										// else if (TREE_CODE(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt)) == VAR_DECL)
										// {
										// gimple *def_stmt = SSA_NAME_DEF_STMT(used_stmt->target);
										// if (def_stmt != NULL)
										// 	if (gimple_code(def_stmt) == GIMPLE_ASSIGN)
										// 		if (TREE_CODE(gimple_assign_lhs(def_stmt)) == SSA_NAME)
										// 			if (gimple_assign_rhs1(def_stmt) == getFunctionAssignVAR)
										// 			{
										// 				find = 1;
										// 				fprintf(stderr, "wwwwwwwwwwwwwwww\n");
										// 				// return;
										// 			}
										// debug(gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt));
										// fprintf(stderr, "---------------RKO-------------2-\n");
										// if (TREE_CODE(gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt)) == SSA_NAME)
										// 	if (gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt) != getFunctionAssignVAR)
										// 		new_search_imm_use(used_stmt, gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt), NULL);
										// }
									}
									// fprintf(stderr, "=======VAR_DECL Unfold========\n");
									// 	}
									// }
									// else
									// {
									// 	debug(use_stmt);
									// 	fprintf(stderr, "=======double hit========\n");
									// }
								}
							}
						// debug(def_stmt2);
						// debug_tree( gimple_call_fndecl(use_stmt));
						// debug(use_stmt);
						// debug_tree(gimple_assign_rhs1(use_stmt));
						// fprintf(stderr, "-----------------GIMPLE_CALL : FIN22D------------------\n");
						// debug_tree(gimple_call_fn(use_stmt));
						// set_gimple_array(used_stmt, use_stmt, gimple_phi_result(use_stmt), gimple_phi_result(use_stmt), NULL);
						set_gimple_array(used_stmt, use_stmt, gimple_call_fn(use_stmt), target, NULL);
						if (TREE_CODE(gimple_call_fn(use_stmt)) == SSA_NAME)
							new_search_imm_use(used_stmt, gimple_call_fn(use_stmt), gimple_call_fn(use_stmt));
						// if (TREE_CODE(gimple_call_fn(use_stmt)) == SSA_NAME)
						// 	new_search_imm_use(used_stmt, gimple_call_fn(use_stmt), gimple_call_fn(use_stmt));
					}
				}
				else
				{
					if (!check_stmtStack(gimple_call_fn(use_stmt)))
					{

						// fprintf(stderr, "-------always in therealways in therealways in there--------------------------\n");
						set_gimple_array(used_stmt, use_stmt, gimple_call_fn(use_stmt), target, NULL);

						if (TREE_CODE(gimple_call_fn(use_stmt)) == SSA_NAME)
							new_search_imm_use(used_stmt, gimple_call_fn(use_stmt), gimple_call_fn(use_stmt));
					}
				}
		}
		// else
		// {
		// 	// if (!check_stmtStack(use_stmt))
		// 	// {

		// 	// fprintf(stderr, "-------always in therealways in therealways in there--------------------------\n");
		// 	debug_gimple_stmt(use_stmt);

		// 	//set_gimple_array(used_stmt, use_stmt, target, target2, NULL);
		// 	// }
		// }
	}
}

/*Pointer Constraint  5*/
void PointerConstraint(ptb *ptable, ptb *ftable)
{
	gimple *u_stmt;
	tree t;
	struct ptr_info_def *pi1;
	struct pt_solution *pt1;
	struct ptr_info_def *pi2;
	struct pt_solution *pt2;

	ptb *table1 = ptable;
	ptb *table3 = ftable;
	gimple_array *used_stmt = NULL;
	gimple_array start;
	start.stmt = NULL;
	used_stmt = &start;
	bool to_remove = false;
	int Entrypoint = 0;

	fprintf(stderr, "start PointerConstraint\n");
	fprintf(stderr, "pointer ftable is %d \n", ftable->size);
	fprintf(stderr, "pointer ptable is %d \n", ptable->size);

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
	tree t2 = NULL_TREE;
	// printfBasicblock();
	if (ptable->size >= 0)
	{
		FOR_EACH_TABLE(table1, t2)
		{
			// if(t2 != NULL_TREE){
			// debug(t2);
			// fprintf(stderr, "start collect similar stmtstart collect similar stmtstart collect similar stmtstart collect similar stmt\n");}
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
			// pi1 = SSA_NAME_PTR_INFO(table1->target);
			// pt1 = &pi1->pt;
			// if (pt1 == NULL)
			// 	continue;
			// if (pt1->vars == NULL)
			// 	continue;

			if (!strcmp(get_tree_code_name(TREE_CODE(used_stmt->target)), "<invalid tree code>"))
				continue;
			// fprintf(stderr, "check stmt\n");
			while (stmtStack.size())
			{
				// fprintf(stderr, "check stmt\n");
				// debug(stmtStack.top());
				stmtStack.pop();
			}

			// tree teewtetqq;
			// size = sizeof(teewtetqq);
			// fprintf(stderr, " used_stmt array size of : %d\n", totalsize);
			// totalsize =0 ;
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
				// debug(used_stmt->target);
				// fprintf(stderr, "%s\n", get_tree_code_name(TREE_CODE(used_stmt->target)));
				// debug_tree(table1->target);

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
					// fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
					// fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
					// fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
					// fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
					// fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
					// fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
					// fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");
					// fprintf(stderr, "----------------------------rkooooooooooooooooooooooooooooooooooooooooooooooooooooo\n");

					if (treeGimpleArray->get(table1->target) != NULL)
						break;
					// fprintf(stderr, "check stmt\n");
					// debug_tree(used_stmt->target);
					// debug_tree(table1->target);
					treeGimpleArray->put(table1->target, *used_stmt);
					Entrypoint++;
				}
			}
			// if(table1->next == NULL)
			// 	fprintf(stderr, "weritjroti;ejhoerjhio;rtjhojert\n");
			// break;
			// debug(table1->next->target);
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
	fprintf(stderr, "    =()=\n");
	fprintf(stderr, " ,/'\_||_\n");
	fprintf(stderr, "  (___  `.\n");
	fprintf(stderr, " ./  `=='\n");
	fprintf(stderr, "                   \n");
	fprintf(stderr, "    ~~~~~~~~~~~  \n");
	fprintf(stderr, " ~~~~~~\n");
	fprintf(stderr, "           ~~~~~~~\n");
	fprintf(stderr, "\033[40;34m    find Entry point : %d\n \033[0m\n", Entrypoint);
	fprintf(stderr, "\033[40;34m    used_stmt array stack totalsize of : %d\n \033[0m\n", totalsize);
	fprintf(stderr, "\033[40;32mSTART CHECKSTART CHECKSTART CHECKSTART CHECKSTART CHECK\033[0m\n");

	totalsize = 0;
	// // printfunctionCollect(ptable, used_stmt);

	// printfunctionCollect(ptable, used_stmt);
	// printfunctionCollect2(ptable, used_stmt);
	// printfPointerConstraint2(ptable, used_stmt);
	printfBasicblock();
	// printfunctionCollect(ptable, used_stmt);
	// fprintf(stderr, " used_stmt array size of : %d\n", totalsize);

	// free(table1);
	// free(table3);
	// free(used_stmt);
}

void checkPointerConstraint(tree function_tree, ptb *ptable, gimple_array *user_tmp, tree checkTree, int threadcheck)
{

	gimple *u_stmt;
	gimple *u_stmt2;
	gimple_array start;
	gimple_array start2;

	start.stmt = NULL;
	start2.stmt = NULL;
	gimple_array *used_stmt = &start;
	gimple_array *used_stmt2 = &start;
	gimple_array *user_tmp2 = user_tmp;
	ptb *table_temp = ptable;

	tree t;
	while (traceStack.size())
	{
		// fprintf(stderr, "check stmt\n");
		// debug(stmtStack.top());
		traceStack.pop();
	}

	// //
	// 		//ready add dot graph

	// 			fprintf(stderr, "dot graph entry %s\n", (char *)get_name(function_tree));
	// 			fprintf(stderr, "dot graph target entry end\n\n");
	// 			//ready add dot graph
	fprintf(stderr, "\033[40;42m =======pre_check_funciton:%s========= \033[0m\n", get_name(function_tree));
	if (threadcheck == FUNCITON_THREAD && threadmod == true)
	{
		function_return_array find_fun_array;

		fprintf(stderr, "\033[40;42m =======thread job funciton:%s========= \033[0m\n", get_name(function_tree));
		find_fun_array = *(function_return_collect->get(function_tree));
		vector<return_type> funcalleeRetTypearray = find_fun_array.return_type_array;

		int find_thread = 0;
		int find_thread_count = 0;
		for (int k = 0; k < funcalleeRetTypearray.size(); k++)
		{
			if ((funcalleeRetTypearray)[k].return_type_stmt_num == FUNCITON_THREAD)
			{
				fprintf(stderr, "\033[40;44m ======= print pthread lock stmt %d ========  \033[0m\n", (funcalleeRetTypearray)[k].return_type_stmt_num);
				debug_gimple_stmt((funcalleeRetTypearray)[k].stmt);
				warning_at(gimple_location((funcalleeRetTypearray)[k].stmt), 0, "use location");
				find_thread_count++;
				// debug_tree((funcalleeRetTypearray)[k].return_tree);
			}
		}
		fprintf(stderr, "\033[40;42m =======thread job funciton lock count :%d========= \033[0m\n", find_thread_count);
	}
	fprintf(stderr, "\033[40;42m =======start_check_funciton:%s========= \033[0m\n", get_name(function_tree));
	FOR_EACH_TABLE(table_temp, t)
	{
		int ptable_type = 0;
		int childptable_type = 0;
		int find_phistmt = 0;
		int find_freestmt = 0;
		int find_pthread_detched = 0;
		int find_pthread_join = 0;
		int is_pthread_detched = 0;
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
				vector<free_type> free_array;
				find_freestmt = find_mallocstmt = find_phistmt = 0;
				debug_tree(function_tree);
				//ready add dot graph
				fprintf(stderr, "\ndot graph START\n");
				// fprintf(stderr, "dot graph entry %s\n", (char *)get_name(function_tree));
				// fprintf(stderr, "dot graph target entry end\n\n");
				//ready add dot graph

				fprintf(stderr, "\n======================================================================\n");
				// debug_tree(function_tree);
				const char *name;
				//fprintf(stderr ,"dot graph target %s\n", (char *)get_name(table_temp->target));
				// //ready add dot graph
				// fprintf(stderr, "dot graph target loc start ");
				// warning_at(gimple_location(table_temp->last_stmt), 0, "use location");
				// fprintf(stderr, "dot graph target loc end\n\n");
				// //ready add dot graph
				debug_tree(table_temp->target);
				// fprintf(stderr, "\n====qwd==================================================================\n");
				// if (TREE_CODE(table_temp->target) == INTEGER_CST)
				// 	continue;
				// if (!strcmp(get_tree_code_name(TREE_CODE(table_temp->target)), "addr_expr"))
				// 	continue;
				gimple *def_stmt = SSA_NAME_DEF_STMT(table_temp->target);
				// if (!strcmp(get_tree_code_name(TREE_CODE(gimple_assign_rhs1(def_stmt))), "addr_expr"))
				// 	continue;
				// debug_tree(table_temp->target);
				if (TREE_CODE(table_temp->target) == INTEGER_CST)
				{
					debug_tree(table_temp->target);
				}
				// continue;
				else if (gimple_code(def_stmt) == GIMPLE_CALL)
				{
					name = get_name(gimple_call_fndecl(def_stmt));
					// fprintf(stderr, "GIMPLE CODE :addr_expr---%s-----\n", name);
				}
				// if (gimple_code(def_stmt) == GIMPLE_NOP)
				// 	continue;

				// debug(def_stmt);
				// if(def_stmt == NULL)
				// fprintf(stderr, "\n====qwd==================================================================\n");
				// debug(def_stmt);
				// debug_tree(gimple_assign_rhs1(def_stmt));
				// 	debug(def_stmt);
				// fprintf(stderr, "GIMPLE CODE :addr_expr---%s-----\n", name);
				// warning_at(gimple_location(def_stmt), 0, "use location");
				if (name != NULL)

					if (!strcmp(name, "realloc") ||
						!strcmp(name, "malloc") ||
						!strcmp(name, "xmalloc") ||
						!strcmp(name, "calloc") ||
						!strcmp(name, "xcalloc") ||
						!strcmp(name, "strdup"))
					{

						ptable_type = IS_MALLOC_FUCNTION;
						// debug(def_stmt);
						// find_mallocstmt=1;
						fprintf(stderr, "this Reserved word function ------%s-----\n", name);
						// fprintf(stderr, "GIMPLE CODE :addr_expr---%s-----\n", name);
					}
					else
					{
						ptable_type = IS_OTHRER_FUCNTION;
						fprintf(stderr, "this other function ------%s-----\n", name);
					}

				// fprintf(stderr, "\n======================================================================\n");
				user_tmp = treeGimpleArray->get(table_temp->target);
				// debug_tree(table_temp->target);
				// fprintf(stderr, "\n====================================ffff=================================\n");

				start.stmt = NULL;
				used_stmt = &start;
				if (user_tmp != NULL)
				{

					if (table_temp->swap_type == FUNCITON_THREAD)
						fprintf(stderr, " \n Start is Pthread Job Collect  \n");
					else
						fprintf(stderr, " \n Start check Pointer Collect  \n");
					fprintf(stderr, "\n======================================================================\n");
					if (user_tmp->size > 0)
						FOR_EACH_USE_TABLE(user_tmp, u_stmt)
						{

							struct free_type free_type;

							if (user_tmp->ret_stmt != NULL)
							{
								fprintf(stderr, "dot graph entry %s\n", (char *)get_name(function_tree));
								fprintf(stderr, "dot graph target entry end\n\n");

								if (gimple_code(user_tmp->ret_stmt) == GIMPLE_RETURN)
								{
									debug(user_tmp->ret_stmt);
									warning_at(gimple_location(user_tmp->ret_stmt), 0, "use location");
									if (checkTree != NULL)
									{
										fprintf(stderr, "\n ================== warring ================== \n");
										// sfprintf(stderr, "function return value related stmt \n");
										debug(checkTree);
										fprintf(stderr, "\033[40;35m    function return value related stmt \033[0m\n");
										fprintf(stderr, "\033[40;35m    this stmt possible is heap-object 。 \033[0m\n");

										// fprintf(stderr, "this stmt possible is heap-object 。\n");
										fprintf(stderr, "\n ================== warring ================== \n");
									}
									//ready add dot graph
									fprintf(stderr, "dot graph target loc start ");
									debug_gimple_stmt(table_temp->last_stmt);
									warning_at(gimple_location(table_temp->last_stmt), 0, "use location");
									fprintf(stderr, "dot graph target loc end\n\n");
									//ready add dot graph

									//ready add dot graph
									fprintf(stderr, "dot graph target basicblock start ");
									fprintf(stderr, "from %s basic block %d", (char *)get_name(function_tree), gimple_bb(user_tmp->ret_stmt)->index);
									now_stmt = user_tmp->ret_stmt;
									fprintf(stderr, "dot graph target basicblock end\n\n");
									//ready add dot graph

									//ready add dot graph
									fprintf(stderr, "dot graph stmt start ");
									debug(user_tmp->ret_stmt);
									warning_at(gimple_location(user_tmp->ret_stmt), 0, "use location");
									fprintf(stderr, "dot graph stmt end\n\n");
									//ready add dot graph
									find_mallocstmt = IS_HEAP_FUCNTION;
									continue;
								}
							}
							else
							{

								if (user_tmp->target != NULL)
								{
									fprintf(stderr, "dot graph entry %s\n", (char *)get_name(function_tree));
									fprintf(stderr, "dot graph target entry end\n\n");

									gimple *finalstmt;

									if (!strcmp(get_tree_code_name(TREE_CODE(user_tmp->target)), "addr_expr"))
									{
										// debug_ee(user_tmp->target);
										tree second = TREE_OPERAND(user_tmp->target, 0);
										if (TREE_CODE(second) != VAR_DECL)
										{

											//ready add dot graph
											fprintf(stderr, "dot graph target loc start ");
											debug_gimple_stmt(table_temp->last_stmt);
											warning_at(gimple_location(table_temp->last_stmt), 0, "use location");
											fprintf(stderr, "dot graph target loc end\n\n");
											//ready add dot graph

											gimple *def_stmt = SSA_NAME_DEF_STMT(user_tmp->target);

											debug(def_stmt);
											finalstmt = def_stmt;
											warning_at(gimple_location(def_stmt), 0, "use location");

											//ready add dot graph
											fprintf(stderr, "dot graph target basicblock start ");
											fprintf(stderr, "from %s basic block %d", (char *)get_name(function_tree), gimple_bb(def_stmt)->index);
											now_stmt = def_stmt;
											fprintf(stderr, "dot graph target basicblock end\n\n");
											//ready add dot graph

											//ready add dot graph
											fprintf(stderr, "dot graph target stmt start [addr_expr]");
											debug(def_stmt);
											warning_at(gimple_location(def_stmt), 0, "use location");
											fprintf(stderr, "dot graph target stmt end\n\n");
											//ready add dot graph
										}
										else
										{
											//ready add dot graph
											fprintf(stderr, "dot graph target loc start ");
											debug_gimple_stmt(table_temp->last_stmt);
											warning_at(gimple_location(table_temp->last_stmt), 0, "use location");
											fprintf(stderr, "dot graph target loc end\n\n");
											//ready add dot graph

											gimple *def_stmt = SSA_NAME_DEF_STMT(second);
											debug(second);
											debug_tree(second);
											finalstmt = def_stmt;
											fprintf(stderr, "addr_expr------vardecl onlay tree-------\n");

											//ready add dot graph
											fprintf(stderr, "dot graph target basicblock start ");
											fprintf(stderr, "from %s basic block %d", (char *)get_name(function_tree), gimple_bb(def_stmt)->index);
											now_stmt = def_stmt;
											fprintf(stderr, "dot graph target basicblock end\n\n");
											//ready add dot graph

											//ready add dot graph
											fprintf(stderr, "dot graph target stmt start [nomal]");
											debug(def_stmt);
											warning_at(gimple_location(def_stmt), 0, "use location");
											fprintf(stderr, "dot graph target stmt end\n\n");
											//ready add dot graph

											// warning_at(gimple_location(u_stmt), 0, "use location");
											// debug(u_stmt);
											// warning_at(gimple_location(u_stmt), 0, "use location");
										}
										// user_tmp->target = second;
										// user_tmp->last_stmt = def_stmt;

										// fprintf(stderr, "addr_expraddr_expraddr_expraddr_expraddr_expr--------\n");
									}
									else
									{

										//ready add dot graph
										fprintf(stderr, "dot graph target loc start ");
										debug_gimple_stmt(table_temp->last_stmt);
										warning_at(gimple_location(table_temp->last_stmt), 0, "use location");
										fprintf(stderr, "dot graph target loc end\n\n");
										//ready add dot graph

										finalstmt = u_stmt;
										debug(u_stmt);
										warning_at(gimple_location(u_stmt), 0, "use location");

										//ready add dot graph
										fprintf(stderr, "dot graph target basicblock start ");
										fprintf(stderr, "from %s basic block %d", (char *)get_name(function_tree), gimple_bb(u_stmt)->index);
										now_stmt = u_stmt;
										fprintf(stderr, "dot graph target basicblock end\n\n");
										//ready add dot graph

										//ready add dot graph
										fprintf(stderr, "dot graph stmt start ");
										debug(u_stmt);
										warning_at(gimple_location(u_stmt), 0, "use location");
										//ready add dot graph
										fprintf(stderr, "dot graph stmt end\n\n");

										// fprintf(stderr, "addr_expraddr_expraddr_expraddr_expraddr_expr--------\n");
									}
									if (threadcheck == FUNCITON_THREAD && threadmod == true)
									{
										if (gimple_code(finalstmt) == GIMPLE_ASSIGN)
										{
											//debug_tree(gimple_assign_lhs(use_stmt));

											//global variable
											if (!TREE_STATIC(gimple_assign_lhs(finalstmt)) == true)
											{

												fprintf(stderr, " \n LOCAL VARIBALE  \n");
												debug_gimple_stmt(finalstmt);
												warning_at(gimple_location(finalstmt), 0, "use location");
												fprintf(stderr, " \n LOCAL VARIBALE  \n");
												continue;
											}
											else
											{
												fprintf(stderr, "\n ================== warring  ================== \n");

												fprintf(stderr, "\033[40;35m warring thread fucntion detction is on \033[0m\n");
												// sfprintf(stderr, "function return value related stmt \n");
												fprintf(stderr, "\033[40;35m Detect GLOBAL VARIBALE \033[0m\n");
												debug_gimple_stmt(finalstmt);
												warning_at(gimple_location(finalstmt), 0, "use location");
												fprintf(stderr, "\033[40;35m thread job function \033[0m\n");
												fprintf(stderr, "\033[40;35m this stmt possible have Race Condition 。 \033[0m\n");
												fprintf(stderr, "\n ================== warring  ================== \n");
											}
										}
									}
								}
								// debug(u_stmt);
								if (user_tmp->target != NULL)
								{

									gimple *use_stmt = SSA_NAME_DEF_STMT(user_tmp->target);

									if (gimple_code(u_stmt) == GIMPLE_CALL)
									{
										name = get_name(gimple_call_fndecl(u_stmt));
										if (name != NULL)
											fprintf(stderr, "this stmt is child function---%s-----\n", name);
										else
											fprintf(stderr, "this other child function---i null-----\n", name);
									}
									if (gimple_code(u_stmt) == GIMPLE_PHI)
									{
										find_phistmt = 1;
										fprintf(stderr, "this stmt have mutiple branch ---%s-----\n", name);
									}
									if (name != NULL)
										if (
											!strcmp(name, "realloc") ||
											!strcmp(name, "malloc") ||
											!strcmp(name, "xmalloc") ||
											!strcmp(name, "calloc") ||
											!strcmp(name, "xcalloc") ||
											!strcmp(name, "strdup"))
										{

											find_mallocstmt = IS_MALLOC_FUCNTION;
										}
										else
										{

											find_mallocstmt = IS_OTHRER_FUCNTION;
										}
								}

								if (checkTree != NULL && gimple_code(def_stmt) == GIMPLE_CALL && gimple_code(u_stmt) == GIMPLE_CALL && find_mallocstmt == IS_OTHRER_FUCNTION)
								{

									tree gettreefucntionarg = TREE_OPERAND(gimple_call_fn(def_stmt), 0);
									name = get_name(gimple_call_fn(def_stmt));
									if (name != NULL)
										if (checkTree == gettreefucntionarg)
										{
											find_mallocstmt = IS_MALLOC_FUCNTION;

											fprintf(stderr, "\n ================== warring ================== \n");
											// sfprintf(stderr, "function return value related stmt \n");
											debug(checkTree);
											fprintf(stderr, "\033[40;35m    function return value related stmt \033[0m\n");
											fprintf(stderr, "\033[40;35m    this stmt possible is heap-object 。 \033[0m\n");

											// fprintf(stderr, "this stmt possible is heap-object 。\n");
											fprintf(stderr, "\n ================== warring ================== \n");

											if (gimple_code(u_stmt) == GIMPLE_CALL)
											{
												name = get_name(gimple_call_fn(u_stmt));
												if (name != NULL)
													if (!strcmp(name, "free") || !strcmp(name, "xfree"))
													{
														find_freestmt++;
														free_type.stmt = u_stmt;
														free_array.push_back(free_type);
														fprintf(stderr, "\n ================== find ================== \n");
														debug(u_stmt);
														warning_at(gimple_location(u_stmt), 0, "use location");
														fprintf(stderr, "\033[40;32m    HAS FREE STMT count:%d name:%s \033[0m\n", find_freestmt, name);
														fprintf(stderr, "\n ================== find ================== \n");
													}
													else if (!strcmp(name, "pthread_create"))
													{
														if (table_temp->swap_type == FUNCITON_THREAD)
														{

															fprintf(stderr, "\n ================== find ================== \n");
															debug(u_stmt);
															warning_at(gimple_location(u_stmt), 0, "use location");
															fprintf(stderr, "\033[40;32m    FIND PTHREAD_CREATED STMT  \033[0m\n");
															//  callerFunArray.pthread_type_num== 0?"CREATE_JOINABLE" : "CREATE_DETACHED");
															fprintf(stderr, "\n ================== pre_pthread_detched ================== \n");
															trace_function_path(TREE_OPERAND(gimple_call_arg(table_temp->last_stmt, 2), 0), -1, NULL_TREE, &find_pthread_detched);
															fprintf(stderr, "\n ================== pre_pthread_detched end ================== \n");
															tree findtree = gimple_call_arg(table_temp->last_stmt, 3);
															// debug_gimple_stmt(table_temp->last_stmt);
															// debug_gimple_stmt(u_stmt);
															if (is_gimple_call(u_stmt))
															{
																tree findtree;
																if (gimple_call_num_args(u_stmt) != 0)
																{
																	findtree = gimple_call_arg(u_stmt, 3);
																	if (!strcmp(get_tree_code_name(TREE_CODE(findtree)), "addr_expr"))
																	{
																		// fprintf(stderr, "\033[40;32m    FIND PTHREAD222_CREATED STMT  \033[0m\n");
																		// tree findtree = gimple_call_arg(u_stmt, 3);
																		// debug_tree(TREE_OPERAND(findtree, 0));
																		trace_function_path(function_tree, -1, TREE_OPERAND(findtree, 0), &find_freestmt);
																	}
																	else if (TREE_CODE(findtree) == SSA_NAME)
																	{
																		// fprintf(stderr, "\033[40;32m    FIND PTHREAD222_CREATED STMT  \033[0m\n");
																		// tree findtree = gimple_call_arg(u_stmt, 3);
																		// debug_tree(TREE_OPERAND(findtree, 0));
																		trace_function_path(function_tree, -1, findtree, &find_freestmt);
																	}
																}
																// debug_tree(findtree);
																// debug_tree(TREE_OPERAND(TREE_OPERAND(findtree, 0), 0));
															}
															if (findtree != NULL)
																trace_function_path(TREE_OPERAND(gimple_call_arg(table_temp->last_stmt, 2), 0), 0, findtree, &find_freestmt);
															if (table_temp->swap_stmt != NULL)
															{
																fprintf(stderr, "\n ================== pre_pthread_detched ================== \n");
																trace_function_path(TREE_OPERAND(gimple_call_arg(table_temp->swap_stmt, 2), 0), -1, NULL_TREE, &find_pthread_detched);
																fprintf(stderr, "\n ================== pre_pthread_detched end ================== \n");
																findtree = gimple_call_arg(table_temp->swap_stmt, 3);
																if (findtree != NULL)
																	trace_function_path(TREE_OPERAND(gimple_call_arg(table_temp->swap_stmt, 2), 0), 0, findtree, &find_freestmt);
															}
															if (find_pthread_detched == 0)
															{
																fprintf(stderr, "\033[40;32m    FIND PTHREAD_CREATED TYPE is %s  \033[0m\n", table_temp->pthread_type == 0 ? "CREATE_JOINABLE" : "CREATE_DETACHED");
																if (table_temp->pthread_type == 1)
																	is_pthread_detched = 1;
															}
															else
															{
																fprintf(stderr, "\033[40;32m    FIND PTHREAD_CREATED TYPE is CREATE_DETACHED  \033[0m\n");
																is_pthread_detched = 1;
															}
															fprintf(stderr, "\n ================== find ================== \n");
														}
													}
													else if (!strcmp(name, "pthread_join"))
													{
														if (table_temp->swap_type == FUNCITON_THREAD)
														{
															fprintf(stderr, "\033[40;32m    FIND PTHREAD_JOIN \033[0m\n");
															find_pthread_join++;
														}
													}
													else
													{
														int freecount = find_freestmt;
														fprintf(stderr, "\n ================== trace ================== \n");
														fprintf(stderr, "trace fucntion name:%s \n", name);
														trace_function_path(gimple_call_fndecl(u_stmt), 0, table_temp->target, &find_freestmt);
														fprintf(stderr, "\n ================== trace ================== \n");
														if (find_freestmt > freecount)
														{
															free_type.stmt = u_stmt;
															free_array.push_back(free_type);
														}
													}
											}
										}
										else
										{
											if (gimple_code(u_stmt) == GIMPLE_CALL)
											{
												// name = get_name(gimple_call_fndecl(u_stmt));
												name = get_name(gimple_call_fn(u_stmt));
												if (name != NULL)
												{
													int freecount = find_freestmt;
													fprintf(stderr, "\n ================== trace ================== \n");
													fprintf(stderr, "trace fucntion name:%s \n", name);
													trace_function_path(gimple_call_fndecl(u_stmt), 0, table_temp->target, &find_freestmt);
													fprintf(stderr, "\n ================== trace ================== \n");
													if (find_freestmt > freecount)
													{
														free_type.stmt = u_stmt;
														free_array.push_back(free_type);
													}
												}
												else if (!strcmp(name, "pthread_create"))
												{
													if (table_temp->swap_type == FUNCITON_THREAD)
													{
														fprintf(stderr, "\n ================== find ================== \n");
														debug(u_stmt);
														warning_at(gimple_location(u_stmt), 0, "use location");
														fprintf(stderr, "\033[40;32m    FIND PTHREAD_CREATED STMT  \033[0m\n");

														//  callerFunArray.pthread_type_num== 0?"CREATE_JOINABLE" : "CREATE_DETACHED");
														fprintf(stderr, "\n ================== pre_pthread_detched ================== \n");
														trace_function_path(TREE_OPERAND(gimple_call_arg(table_temp->last_stmt, 2), 0), -1, NULL_TREE, &find_pthread_detched);
														fprintf(stderr, "\n ================== pre_pthread_detched end ================== \n");
														tree findtree = gimple_call_arg(table_temp->last_stmt, 3);

														// debug_gimple_stmt(table_temp->last_stmt);
														// debug_gimple_stmt(u_stmt);
														if (is_gimple_call(u_stmt))
														{
															tree findtree;
															if (gimple_call_num_args(u_stmt) != 0)
															{
																findtree = gimple_call_arg(u_stmt, 3);
																if (!strcmp(get_tree_code_name(TREE_CODE(findtree)), "addr_expr"))
																{
																	// fprintf(stderr, "\033[40;32m    FIND PTHREAD222_CREATED STMT  \033[0m\n");
																	// tree findtree = gimple_call_arg(u_stmt, 3);
																	// debug_tree(TREE_OPERAND(findtree, 0));
																	trace_function_path(function_tree, -1, TREE_OPERAND(findtree, 0), &find_freestmt);
																}
																else if (TREE_CODE(findtree) == SSA_NAME)
																{
																	// fprintf(stderr, "\033[40;32m    FIND PTHREAD222_CREATED STMT  \033[0m\n");
																	// tree findtree = gimple_call_arg(u_stmt, 3);
																	// debug_tree(TREE_OPERAND(findtree, 0));
																	trace_function_path(function_tree, -1, findtree, &find_freestmt);
																}
															}
															// debug_tree(findtree);
															// debug_tree(TREE_OPERAND(TREE_OPERAND(findtree, 0), 0));
														}

														if (findtree != NULL)
															trace_function_path(TREE_OPERAND(gimple_call_arg(table_temp->last_stmt, 2), 0), 0, findtree, &find_freestmt);
														if (table_temp->swap_stmt != NULL)
														{
															fprintf(stderr, "\n ================== pre_pthread_detched ================== \n");
															trace_function_path(TREE_OPERAND(gimple_call_arg(table_temp->swap_stmt, 2), 0), -1, NULL_TREE, &find_pthread_detched);
															findtree = gimple_call_arg(table_temp->swap_stmt, 3);
															fprintf(stderr, "\n ================== pre_pthread_detched end ================== \n");
															if (findtree != NULL)
																trace_function_path(TREE_OPERAND(gimple_call_arg(table_temp->swap_stmt, 2), 0), 0, findtree, &find_freestmt);
														}

														if (find_pthread_detched == 0)
														{
															fprintf(stderr, "\033[40;32m    FIND PTHREAD_CREATED TYPE is %s  \033[0m\n", table_temp->pthread_type == 0 ? "CREATE_JOINABLE" : "CREATE_DETACHED");
															if (table_temp->pthread_type == 1)
																is_pthread_detched = 1;
														}
														else
														{
															fprintf(stderr, "\033[40;32m    FIND PTHREAD_CREATED TYPE is CREATE_DETACHED  \033[0m\n");
															is_pthread_detched = 1;
														}
														fprintf(stderr, "\n ================== find ================== \n");
													}
												}
												else if (!strcmp(name, "pthread_join"))
												{
													if (table_temp->swap_type == FUNCITON_THREAD)
													{
														fprintf(stderr, "\033[40;32m    FIND PTHREAD_JOIN \033[0m\n");
														find_pthread_join++;
													}
												}
												// else
												// 	fprintf(stderr, "this other child function---i null-----\n", name);
											}

											find_mallocstmt = 0;
										}
								}
								else if (checkTree != NULL && gimple_code(u_stmt) == GIMPLE_RETURN && find_mallocstmt == IS_OTHRER_FUCNTION)
								{
									fprintf(stderr, "\n ================== warring ================== \n");
									// sfprintf(stderr, "function return value related stmt \n");
									debug(checkTree);
									fprintf(stderr, "\033[40;35m    function return value related stmt \033[0m\n");
									fprintf(stderr, "\033[40;35m    this stmt possible is heap-object 。 \033[0m\n");

									// fprintf(stderr, "this stmt possible is heap-object 。\n");
									fprintf(stderr, "\n ================== warring ================== \n");
								}
								else if ((checkTree == NULL && gimple_code(u_stmt) == GIMPLE_CALL) && (find_mallocstmt == IS_MALLOC_FUCNTION || find_mallocstmt == IS_OTHRER_FUCNTION))
								{

									find_mallocstmt = IS_MALLOC_FUCNTION;

									// gimple *def_stmt = SSA_NAME_DEF_STMT(user_tmp->target);
									// fprintf(stderr, "NEWX FUCNTIONMWEQMEQWP: \n");
									gimple *def_stmt = SSA_NAME_DEF_STMT(user_tmp->target);
									// debug(def_stmt);
									const char *name;
									name = get_name(gimple_call_fn(u_stmt));
									if (name != NULL)
										if (!strcmp(name, "free") || !strcmp(name, "xfree"))
										{
											find_freestmt++;
											free_type.stmt = u_stmt;
											free_array.push_back(free_type);
											fprintf(stderr, "\n ================== find ================== \n");
											debug(u_stmt);
											warning_at(gimple_location(u_stmt), 0, "use location");
											fprintf(stderr, "\033[40;32m    HAS FREE STMT count:%d name:%s \033[0m\n", find_freestmt, name);
											fprintf(stderr, "\n ================== find ================== \n");
										}
										else if (!strcmp(name, "pthread_create"))
										{
											if (table_temp->swap_type == FUNCITON_THREAD)
											{
												fprintf(stderr, "\n ================== find ================== \n");
												debug(u_stmt);
												warning_at(gimple_location(u_stmt), 0, "use location");
												fprintf(stderr, "\033[40;32m    FIND PTHREAD_CREATED STMT  \033[0m\n");
												//  callerFunArray.pthread_type_num== 0?"CREATE_JOINABLE" : "CREATE_DETACHED");
												// debug_gimple_stmt(table_temp->last_stmt);
												fprintf(stderr, "\n ================== pre_pthread_detched ================== \n");
												trace_function_path(TREE_OPERAND(gimple_call_arg(table_temp->last_stmt, 2), 0), -1, NULL_TREE, &find_pthread_detched);
												fprintf(stderr, "\n ================== pre_pthread_detched end ================== \n");
												fprintf(stderr, "\033[40;31m  wqeeeeeeeeeee %d  \033[0m\n", find_pthread_detched);
												tree findtree = gimple_call_arg(table_temp->last_stmt, 3);

												// debug_gimple_stmt(table_temp->last_stmt);
												// debug_gimple_stmt(u_stmt);
												if (is_gimple_call(u_stmt))
												{
													tree findtree;
													if (gimple_call_num_args(u_stmt) != 0)
													{
														findtree = gimple_call_arg(u_stmt, 3);
														if (!strcmp(get_tree_code_name(TREE_CODE(findtree)), "addr_expr"))
														{
															// fprintf(stderr, "\033[40;32m    FIND PTHREAD222_CREATED STMT  \033[0m\n");
															// tree findtree = gimple_call_arg(u_stmt, 3);
															// debug_tree(TREE_OPERAND(findtree, 0));
															trace_function_path(function_tree, -1, TREE_OPERAND(findtree, 0), &find_freestmt);
														}
														else if (TREE_CODE(findtree) == SSA_NAME)
														{
															// fprintf(stderr, "\033[40;32m    FIND PTHREAD222_CREATED STMT  \033[0m\n");
															// tree findtree = gimple_call_arg(u_stmt, 3);
															// debug_tree(findtree);
															trace_function_path(function_tree, -1, findtree, &find_freestmt);
														}
													}
													// debug_tree(findtree);
													// debug_tree(TREE_OPERAND(TREE_OPERAND(findtree, 0), 0));
												}
												// tree findtree2 = gimple_call_arg(u_stmt, 3);
												// if(findtree2 != NULL_TREE)
												// debug_tree(findtree2);
												// debug_tree(findtree);
												// tree findtree2 = gimple_call_arg(u_stmt, 3);
												// if(findtree2 != NULL)
												// debug_tree(findtree2);

												if (findtree != NULL)
													trace_function_path(TREE_OPERAND(gimple_call_arg(table_temp->last_stmt, 2), 0), 0, findtree, &find_freestmt);
												if (table_temp->swap_stmt != NULL)
												{
													fprintf(stderr, "\n ================== pre_pthread_detched ================== \n");
													trace_function_path(TREE_OPERAND(gimple_call_arg(table_temp->swap_stmt, 2), 0), -1, NULL_TREE, &find_pthread_detched);
													fprintf(stderr, "\n ================== pre_pthread_detched end ================== \n");
													findtree = gimple_call_arg(table_temp->swap_stmt, 3);
													if (findtree != NULL)
														trace_function_path(TREE_OPERAND(gimple_call_arg(table_temp->swap_stmt, 2), 0), 0, findtree, &find_freestmt);
												}

												if (find_pthread_detched == 0)
												{

													fprintf(stderr, "\033[40;32m    FIND PTHREAD_CREATED TYPE is %s  \033[0m\n", table_temp->pthread_type == 0 ? "CREATE_JOINABLE" : "CREATE_DETACHED");
													is_pthread_detched = 0;
													if (table_temp->pthread_type == 1)
														is_pthread_detched = 1;
												}
												else
												{

													fprintf(stderr, "\033[40;32m    FIND PTHREAD_CREATED TYPE is CREATE_DETACHED  \033[0m\n");
													is_pthread_detched = 1;
												}
												fprintf(stderr, "\n ================== find ================== \n");
											}
										}
										else if (!strcmp(name, "pthread_join"))
										{
											if (table_temp->swap_type == FUNCITON_THREAD)
											{
												fprintf(stderr, "\033[40;32m    FIND PTHREAD_JOIN \033[0m\n");
												find_pthread_join++;
											}
										}
										else
										{
											int freecount = find_freestmt;
											fprintf(stderr, "\n ================== trace ================== \n");
											fprintf(stderr, "trace fucntion name:%s \n", name);
											// debug_tree(gimple_call_fndecl(u_stmt));
											trace_function_path(gimple_call_fndecl(u_stmt), 0, table_temp->target, &find_freestmt);
											fprintf(stderr, "\n ================== trace ================== \n");
											if (find_freestmt > freecount)
											{
												free_type.stmt = u_stmt;
												free_array.push_back(free_type);
											}
										}
								}
							}
						}
					user_tmp = treeGimpleArray->get(table_temp->target);
					// debug_tree(table_temp->target);
					// fprintf(stderr, "\n====================================ffff=================================\n");
					gimple_array start;
					start.stmt = NULL;
					used_stmt = &start;
					fprintf(stderr, "\n ================== Start Use after free Check ================== \n");
					if (user_tmp->size > 0)
						FOR_EACH_USE_TABLE(user_tmp, u_stmt)
						{

							// debug_gimple_stmt(u_stmt);

							for (int i = 0; i < free_array.size(); i++)
							{
								//fprintf(stderr, "\n====================================ffff2=================================\n");
								if (u_stmt != free_array.at(i).stmt)
									if (gimple_bb(u_stmt)->index == gimple_bb(free_array.at(i).stmt)->index)
									{
										if (Location_b(free_array.at(i).stmt, u_stmt, gimple_bb(u_stmt)))
										{
											debug_gimple_stmt(free_array.at(i).stmt);
											warning_at(gimple_location(free_array.at(i).stmt), 0, "Use after free error!: free location");
											debug_gimple_stmt(u_stmt);
											warning_at(gimple_location(u_stmt), 0, "use location");
											fprintf(stderr, "\n ================== warring ================== \n");

											// debug(checkTree);
											fprintf(stderr, "\033[40;35m    Use after free error! \033[0m\n");
											// fprintf(stderr, "\033[40;35m    this stmt possible is heap-object 。 \033[0m\n");

											// fprintf(stderr, "this stmt possible is heap-object 。\n");
											fprintf(stderr, "\n ================== warring ================== \n");
										}
									}
							}
						}
					fprintf(stderr, "\ndot graph END\n");
				}
				else
				{
					find_mallocstmt = PTABLE_IS_NULL;
				}
				//you are Reserved word function so always check
				if (table_temp->swap_type == FUNCITON_THREAD)
				{
					if (is_pthread_detched == 0)
					{
						if (find_pthread_join == 0)
						{
							fprintf(stderr, "\n======================================================================\n");
							// fprintf(stderr, "	no free stmt possible memory leak\n");
							fprintf(stderr, "\033[40;31m    pthread is JOINABLE but no pthread_join stmt \033[0m\n");
							fprintf(stderr, "\n======================================================================\n");
						}
						else
						{
							fprintf(stderr, "\n======================================================================\n");
							// fprintf(stderr, "	no free stmt possible memory leak\n");
							// fprintf(stderr, "\033[40;31m    pthread is JOINABLE and no memory leak \033[0m\n");
							fprintf(stderr, "\033[40;32m    pthread is JOINABLE and no memory leak   \033[0m\n");
							fprintf(stderr, "\n======================================================================\n");
						}
					}
					else
					{
						fprintf(stderr, "\n======================================================================\n");
						// fprintf(stderr, "	no free stmt possible memory leak\n");
						fprintf(stderr, "\033[40;32m    pthread is DETACHED and no memory leak  \033[0m\n");
						// fprintf(stderr, "\033[40;31m    pthread is DETACHED and no memory leak  \033[0m\n");
						fprintf(stderr, "\n======================================================================\n");
					}
				}
				if (ptable_type == IS_MALLOC_FUCNTION && find_mallocstmt == IS_OTHRER_FUCNTION)
					find_mallocstmt = IS_MALLOC_FUCNTION;
				if (find_mallocstmt == IS_MALLOC_FUCNTION)
				{

					if (find_freestmt == 0)
					{

						fprintf(stderr, "\n======================================================================\n");
						// fprintf(stderr, "	no free stmt possible memory leak\n");
						fprintf(stderr, "\033[40;31m    no free stmt possible memory leak \033[0m\n");
						fprintf(stderr, "\n======================================================================\n");
					}
					else if (find_freestmt == 1)
					{

						fprintf(stderr, "\n======================================================================\n");

						if (find_phistmt == 1)
							fprintf(stderr, "\033[40;31m   need check branch because multiple direction varible\033[0m\n");
						else
							fprintf(stderr, "\033[40;32m   Maybe you don't have a memory leak.... more checks  \033[0m\n");
						// 	fprintf(stderr, "\033[40;32m    NO memory leak \033[0m\n");
						fprintf(stderr, "\n======================================================================\n\n");
					}
					else if (find_freestmt >= 2)
					{
						fprintf(stderr, "\n======================================================================\n");
						// fprintf(stderr, "	possible double free\n");
						fprintf(stderr, "\033[40;31m  	possible double free :%d \033[0m\n", find_freestmt);
						fprintf(stderr, "\n======================================================================\n");
					}
				}
				else if (find_mallocstmt == IS_HEAP_FUCNTION)
				{
					fprintf(stderr, "\n======================================================================\n");
					// fprintf(stderr, "	this fucntion return possible has heap-object\n");
					fprintf(stderr, "\033[40;31m 	this fucntion return possible is heap-object \033[0m\n");
					fprintf(stderr, "\n======================================================================\n");
				}
				else if (find_mallocstmt == PTABLE_IS_NULL)
				{
					fprintf(stderr, "\n======================================================================\n");
					// fprintf(stderr, "	this fucntion return possible has heap-object\n");
					fprintf(stderr, "\033[40;31m 	Can't find this fucntion in Pointer table \033[0m\n");
					fprintf(stderr, "\033[40;31m 	Possiable no other stmt relate with this stmt \033[0m\n");
					warning_at(gimple_location(table_temp->last_stmt), 0, "use location");
					// debug_tree(table_temp->target);
					// if (checkTree != NULL)
					// {
					// 	fprintf(stderr, "\033[40;31m 	Possible check from Callee \033[0m\n");
					// 	debug_tree(checkTree);
					// }
					fprintf(stderr, "\n======================================================================\n");
				}
				else
				{
					fprintf(stderr, "\n======================================================================\n");
					// fprintf(stderr, "	this stmt need double check\n");

					fprintf(stderr, "\033[40;31m 	this stmt need double check \033[0m\n");
					fprintf(stderr, "\n======================================================================\n");
				}
			}
	}
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
		push_cfun(node->get_fun());
		// if (strcmp(get_name(cfun->decl), "main") == 0)
		// debug_tree(cfun->decl);
		//tree test=DECL_SAVED_TREE(cfun->decl);
		//analyze_func_body(DECL_SAVED_TREE(test), 0);
		if (cfun == NULL)
			continue;
		// fprintf(stderr, "=======node_fun:%s=========\n", get_name(cfun->decl));
		enum availability avail;

		// fprintf(stderr, "fucntion collect \n");
		function_path_array fun_array;
		//tree get_function_return_tree = gimple_return_retval(as_a<greturn *>(gc));
		vector<function_path> function_path_array;
		fun_array.function_path_array = function_path_array;

		for (e = node->callees; e; e = e->next_callee)
		{
			//funct_state l;
			cgraph_node *caller = e->caller->global.inlined_to ? e->caller->global.inlined_to : e->caller;
			cgraph_node *callee = e->callee->ultimate_alias_target(&avail, caller);

			// fprintf(stderr, "=======child node_fun:%s=========\n", get_name(callee->decl));
			if (callee != NULL)
			{
				if (!gimple_has_body_p(callee->decl))
					continue;
				//
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
	pthread_attr_setdetachstates = new hash_map<tree, pthread_attr_array>;
	function_pthread_detched_collect = new hash_map<tree, function_pthread_detched_array>;
	function_pthread_exit_collect = new hash_map<tree, function_pthread_exit_array>;
	function_path_collect = new hash_map<tree, function_path_array>;
	function_free_collect = new hash_map<tree, function_free_array>;
	function_graph_collect = new hash_map<tree, function_graph_array>;

	srand((unsigned)time(NULL) + getpid());
	// fDFS = new hash_map<cgraph_node *, Graph>;
	// fnode = new hash_map<tree, cgraph_node *>;
	fprintf(stderr, "=======ipa_pta=========\n");
	//ompfucntio();
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
	//cout << "----------------------------------------" <<tmp<<endl;
	// debug_tree(gimple_call_fndecl(node->decl));
	FOR_EACH_DEFINED_FUNCTION(node)
	{
		if (!ipa)
			init_table();
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

		//tree test=DECL_SAVED_TREE(cfun->decl);
		//analyze_func_body(DECL_SAVED_TREE(test), 0);
		if (cfun == NULL)
			continue;
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
			// 	//fprintf(stderr,"node:= %d \n succs:=",bb->index);
			// 	FOR_EACH_EDGE(e, ei, bb->succs)
			// 	{
			// 		DFS.addEdge(bb->index, e->dest->index);
			// 		//fprintf(stderr,"%d",e->dest->index);
			// 	}
			// }
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
				if (is_gimple_assign(gc))
				{

					/*collect malloc and free information*/
					collect_function_call(gc, node, bb);

					// fprintf(stderr, "add collect_function_call\n");
				}
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
	//printf("time: %f s\n", ((double)(end - start)) / CLOCKS_PER_SEC / N); 	// fprintf(stderr,"time: %f s\n",((double)(end - start)) / CLOCKS_PER_SEC / N);
	struct timespec temp = diff(start, end);
	double time_used;
	time_used = temp.tv_sec + (double)temp.tv_nsec / 1000000000.0;
	fprintf(stderr, "time: %f s\n", time_used);

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
						!strcmp(name, "realloc") ||
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
};