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
int check_stmtStack2(gimple *stmt)
{
	for (int o = 0; o < stmtStack.size(); o++)
	{

		if (stmtStack2.c[o] == stmt)
		{
			// find = 1;
			fprintf(stderr, "\033[40;41m =======recursive_stmt========= \033[0m\n");
			// debug_tree(stmtStack.c[o]);
			// fprintf(stderr, "\033[40;41m =======recursive_fun:%s========= \033[0m\n", get_name(stmtStack.c[o]));
			// pathStack.push((function_path_array)[i].next);
			return 1;
		}
	}
	int size = sizeof(stmt);
	totalsize += size;
	stmtStack2.push(stmt);
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
	// gimple *def_stmt2 = SSA_NAME_DEF_STMT(target);
	int size = sizeof(used_stmt);
	// fprintf(stderr, "count size %d ", size);
	// // fprintf(stderr, "START NEW FOR_EACH_IMM_USE_STMT -------------------------------\n");
	// debug_tree(target);
	// fprintf(stderr, "TARGET NEW FOR_EACH_IMM_USE_STMT -------------------------------\n");
	if (TREE_CODE(target) == ADDR_EXPR)
	{
		// fprintf(stderr, "GIMPLE CODE :addr_expr--------\n");
		return;
	}

	FOR_EACH_IMM_USE_STMT(use_stmt, imm_iter, target)
	{

		if (gimple_cond_code(use_stmt))
		{
			if (!is_gimple_assign(use_stmt))
				if (gimple_cond_lhs(use_stmt))
				{
					debug_gimple_stmt(use_stmt);
					// fprintf(stderr, "--------GIMPLE Cond -------\n");

					const gcond *gc = GIMPLE_CHECK2<const gcond *>(use_stmt);
					// fprintf(stderr, "		true-------------------------------\n");
					// if (folded_cond)
					// {
					// basic_block bb = gimple_bb(use_stmt);
					// edge *taken_edge_p = find_taken_edge(bb, folded_cond);
					// if (*taken_edge_p)
					// fprintf(stderr, "		true--%d\n",taken_edge_p->dest->index);
					// taken_edge_p->dest->index
					// }
					// // gimple_bb(user_tmp->ret_stmt)->index
					// if (gimple_cond_true_label(gc))
					// 	// debug_tree( gimple_cond_true_label  (gc));
					// 	debug_tree(gimple_goto_dest(gc));
					// fprintf(stderr, "		false-------------------------------\n");
					// if (gimple_cond_false_label(gc))
					// 	debug_tree(gimple_cond_false_label(gc));
					if (gimple_cond_rhs(gc) && gimple_cond_lhs(gc))
						if (!check_stmtStack(gimple_cond_rhs(gc)))
						{
							// if (check_stmtStack(gimple_cond_lhs(gc)))
							// {
							// fprintf(stderr, "--------GIMPLE true-------\n");
							set_gimple_array(used_stmt, use_stmt, gimple_cond_lhs(gc), target, NULL);
							// }
							// if (TREE_CODE(gimple_cond_lhs(gc)) == SSA_NAME)
							// 	new_search_imm_use(used_stmt, gimple_cond_lhs(gc), gimple_cond_lhs(gc));
						}
					// fprintf(stderr, "--------GIMPLE Cond222 -------\n");
				}
		}

		// fprintf(stderr, "--------GIMPLE goto -------\n");
		// debug_gimple_stmt(use_stmt);
		// if(gimple_goto_dest (use_stmt)){
		// 	fprintf(stderr, "--------GIMPLE goto -------\n");
		// 	debug_gimple_stmt(use_stmt);

		// }
		// if(gimple_code (use_stmt) == GIMPLE_LABEL){
		// 	fprintf(stderr, "--------GIMPLE LABEL -------\n");
		// 	debug_gimple_stmt(use_stmt);
		// }
		// if(target ==target2)

		// if (target2 == NULL)
		// {
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
		// }
		// gimple *def_stmt2 = SSA_NAME_DEF_STMT(target2);
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

			if (gimple_assign_lhs(use_stmt) && TREE_CODE(gimple_assign_lhs(use_stmt)) == SSA_NAME)
			{

				// debug_gimple_stmt(use_stmt);
				// debug_tree(gimple_assign_lhs(use_stmt));
				if (!check_stmtStack(gimple_assign_lhs(use_stmt)))
				{
					set_gimple_array(used_stmt, use_stmt, gimple_assign_lhs(use_stmt), target, NULL);

					new_search_imm_use(used_stmt, gimple_assign_lhs(use_stmt), gimple_assign_lhs(use_stmt));
				}
				if (gimple_assign_rhs1(use_stmt) && TREE_CODE(gimple_assign_rhs1(use_stmt)) == SSA_NAME)
				{

					if (!check_stmtStack(gimple_assign_rhs1(use_stmt)))
					{
						// fprintf(stderr, "------------------SSA_NAME : rhs ssaname------------------\n");
						if (gimple_assign_rhs1(use_stmt) != target2)
							new_search_imm_use(used_stmt, gimple_assign_rhs1(use_stmt), gimple_assign_rhs1(use_stmt));
						// else
						// 	fprintf(stderr, "------------------SSA_NAME : rhs ssaname-------------------\n");
					}
				}
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

				if (!check_stmtStack(gimple_assign_lhs(use_stmt)))
				{
					// debug(use_stmt);
					// debug_tree(target);
					set_gimple_array(used_stmt, use_stmt, gimple_assign_lhs(use_stmt), target, NULL);
					// new_search_imm_use(used_stmt, gimple_assign_lhs(use_stmt), gimple_assign_lhs(use_stmt));
				}

				tree getFunctionAssignVAR = gimple_assign_lhs(use_stmt);

				function_assign_array assign_array;
				if (function_assign_collect->get(getFunctionAssignVAR) != NULL)
				{

					assign_array = ret_function_varstmt(getFunctionAssignVAR);

					for (int i = 0; i < assign_array.assign_type_array.size(); i++)
					{

						if (gimple_code((assign_array.assign_type_array)[i].stmt) == GIMPLE_ASSIGN)
						{

							if (TREE_CODE(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt)) == MEM_REF)
							{
								// fprintf(stderr, "=======fist hit2========\n");
								// fprintf(stderr, "=======fist hit3=======\n");
								// debug_gimple_stmt((assign_array.assign_type_array)[i].stmt);
								// debug_tree(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt));
								// debug(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt));
								// NULL_TREE
								if (!check_stmtStack(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt)))
								// fprintf(stderr, "=======fist hit4=======\n");
								{
									if ((assign_array.assign_type_array)[i].form_tree != NULL)
									{

										// fprintf(stderr, "=======fist hit3=======\n");
										// debug_gimple_stmt((assign_array.assign_type_array)[i].stmt);
										// debug_tree((assign_array.assign_type_array)[i].form_tree);

										set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, (assign_array.assign_type_array)[i].form_tree, target, NULL);
									}
									else
										set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, gimple_assign_lhs((assign_array.assign_type_array)[i].stmt), target, NULL);
								}
							}
							else if (TREE_CODE(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt)) == SSA_NAME)
							{
								if (!check_stmtStack(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt)))
								{
									if ((assign_array.assign_type_array)[i].form_tree != NULL)
									{
										// 			fprintf(stderr, "=======fist hit2========\n");
										// debug_gimple_stmt((assign_array.assign_type_array)[i].stmt);
										// debug_tree(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt));
										// debug_tree((assign_array.assign_type_array)[i].form_tree);
										// fprintf(stderr, "=======fist hit2========\n");
										// debug_gimple_stmt((assign_array.assign_type_array)[i].stmt);
										set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, (assign_array.assign_type_array)[i].form_tree, target, NULL);
									}
									else
										set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, gimple_assign_lhs((assign_array.assign_type_array)[i].stmt), target, NULL);
									// debug(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt));
									// used_stmt = used_stmt2;
									new_search_imm_use(used_stmt, gimple_assign_lhs((assign_array.assign_type_array)[i].stmt), gimple_assign_lhs((assign_array.assign_type_array)[i].stmt));
								}
							}
						}

						else if (gimple_code((assign_array.assign_type_array)[i].stmt) == GIMPLE_CALL)
						{

							// tree second = TREE_OPERAND(gimple_call_arg(gc, 0), 0);
							// const char *name;
							// name = get_tree_code_name(TREE_CODE(gimple_call_arg((assign_array.assign_type_array)[i].stmt, 0)));
							// ADDR_EXPR
							// if (name != NULL)
							// {
							if (TREE_CODE(gimple_call_arg((assign_array.assign_type_array)[i].stmt, 0)) == ADDR_EXPR)
								if (!check_stmtStack(gimple_call_arg((assign_array.assign_type_array)[i].stmt, 0)))
								{

									set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, gimple_call_arg((assign_array.assign_type_array)[i].stmt, 0), target, NULL);
								}
							// }
							if (!check_stmtStack(gimple_call_arg((assign_array.assign_type_array)[i].stmt, 0)))
							{
								if (TREE_CODE(gimple_call_arg((assign_array.assign_type_array)[i].stmt, 0)) == SSA_NAME)
								{

									if ((assign_array.assign_type_array)[i].form_tree != NULL)
									{

										set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, (assign_array.assign_type_array)[i].form_tree, target, NULL);
									}
									else
										set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, gimple_call_arg((assign_array.assign_type_array)[i].stmt, 0), target, NULL);
								}
							}
						}
					}
				}
			}
			else if (gimple_assign_lhs(use_stmt) && TREE_CODE(gimple_assign_lhs(use_stmt)) == MEM_REF)
			{

				tree fundecl = TREE_OPERAND(gimple_assign_lhs(use_stmt), 0);

				if (!check_stmtStack(gimple_assign_lhs(use_stmt)))
				{
					// fprintf(stderr, "ewwwwwwwwwwwwwwwwwwwwww");
					// debug_gimple_stmt(use_stmt);
					set_gimple_array(used_stmt, use_stmt, gimple_assign_lhs(use_stmt), target, NULL);
				}
			}

			else if (gimple_assign_lhs(use_stmt) && TREE_CODE(gimple_assign_lhs(use_stmt)) == COMPONENT_REF)
			{
				if (TREE_CODE(gimple_assign_rhs1(use_stmt)) == SSA_NAME)
				{

					if (!check_stmtStack(gimple_assign_rhs1(use_stmt)))
					{

						set_gimple_array(used_stmt, use_stmt, gimple_assign_rhs1(use_stmt), target, NULL);

						if (gimple_assign_rhs1(use_stmt) != target2)
							new_search_imm_use(used_stmt, gimple_assign_rhs1(use_stmt), gimple_assign_rhs1(use_stmt));
					}
				}
				// debug_tree(gimple_assign_lhs(use_stmt));
	// 			tree second = TREE_OPERAND(gimple_assign_lhs(use_stmt), 0);
	// 			if(second)
	// 			if (TREE_CODE(second) == MEM_REF)
	// 			{
	// 				tree three = TREE_OPERAND(second, 0);
	// 				if(three)
	// 				if (TREE_CODE(three) == SSA_NAME)
	// 				{
	// 					if (!check_stmtStack(three))
	// 					{
	// fprintf(stderr, "-----------------GIMPLE_CALL : FIN22D------------------\n");
	// 						// set_gimple_array(used_stmt, use_stmt, gimple_assign_lhs(use_stmt), target, NULL);
	// 						debug_tree(three);
	// 						// if (gimple_assign_lhs(use_stmt) != target2)
	// 						// new_search_imm_use(used_stmt, three, three);
	// 					}
	// 				}
	// 			}

			}
			else if (gimple_assign_rhs1(use_stmt) && TREE_CODE(gimple_assign_rhs1(use_stmt)) == COMPONENT_REF)
			{
				if (TREE_CODE(gimple_assign_lhs(use_stmt)) == SSA_NAME)
				{

					if (!check_stmtStack(gimple_assign_lhs(use_stmt)))
					{

						set_gimple_array(used_stmt, use_stmt, gimple_assign_lhs(use_stmt), target, NULL);

						if (gimple_assign_lhs(use_stmt) != target2)
							new_search_imm_use(used_stmt, gimple_assign_lhs(use_stmt), gimple_assign_lhs(use_stmt));
					}
				}
	// 			tree second = TREE_OPERAND(gimple_assign_rhs1(use_stmt), 0);
	// 			if(second)
	// 			if (TREE_CODE(second) == MEM_REF)
	// 			{
	// 				tree three = TREE_OPERAND(second, 0);
	// 				if(three)
	// 				if (TREE_CODE(three) == SSA_NAME)
	// 				{
	// 					if (!check_stmtStack(three))
	// 					{
	// fprintf(stderr, "-----------------GIMPLE_CALL : FIN22D------------------\n");
	// 						// set_gimple_array(used_stmt, use_stmt, gimple_assign_lhs(use_stmt), target, NULL);
	// 						debug_tree(three);
	// 						// if (gimple_assign_lhs(use_stmt) != target2)
	// 						// new_search_imm_use(used_stmt, three, three);
	// 					}
	// 				}
	// 			}
			}
		}
		else if (gimple_code(use_stmt) == GIMPLE_PHI)
		{

			if (gimple_phi_result(use_stmt) && TREE_CODE(gimple_phi_result(use_stmt)) == SSA_NAME)
			{

				if (TREE_CODE(gimple_phi_result(use_stmt)) == SSA_NAME)
				{
					if (!check_stmtStack(gimple_phi_result(use_stmt)) && !check_stmtStack2(use_stmt))
					{
						set_gimple_array(used_stmt, use_stmt, gimple_phi_result(use_stmt), target, NULL);
						// debug_tree(gimple_phi_result(use_stmt));
						if (gimple_phi_result(use_stmt) != target2)
						{
							fprintf(stderr, "tewtw\n");
							new_search_imm_use(used_stmt, gimple_phi_result(use_stmt), gimple_phi_result(use_stmt));
						}
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

			name = get_name(gimple_call_fn(use_stmt));
			if (name != NULL)
				if (!strcmp(name, "free") || !strcmp(name, "xfree"))
				{

					if (!check_stmtStack(gimple_call_fn(use_stmt)))
					{

						set_gimple_array(used_stmt, use_stmt, gimple_call_fn(use_stmt), target, NULL);
						if (TREE_CODE(gimple_call_fn(use_stmt)) == SSA_NAME)
							new_search_imm_use(used_stmt, gimple_call_fn(use_stmt), gimple_call_fn(use_stmt));
					}
				}
				else if (!strcmp(name, "pthread_create") || !strcmp(name, "pthread_join"))
				{

					if (!check_stmtStack(gimple_call_fn(use_stmt)))
					{
						// fprintf(stderr, "-----------------GIMPLE_CALL : FIN22D------------------\n");
						// debug(target);

						tree second = NULL;
						int isVardecl = 0;
						if (TREE_CODE(gimple_assign_rhs1(def_stmt)) == ADDR_EXPR)
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
											{

												if (!check_stmtStack(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt)))
												{

													// debug(gimple_assign_lhs((assign_array.assign_type_array)[i].stmt));
													if ((assign_array.assign_type_array)[i].form_tree != NULL)
													{
														// fprintf(stderr, "=======fist hit2========\n");
														// debug_tree((assign_array.assign_type_array)[i].form_tree);
														set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, (assign_array.assign_type_array)[i].form_tree, target, NULL);
													}
													else
														set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, gimple_assign_lhs((assign_array.assign_type_array)[i].stmt), target, NULL);

													// set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, gimple_assign_lhs((assign_array.assign_type_array)[i].stmt), target, NULL);
													// used_stmt = used_stmt2;
													new_search_imm_use(used_stmt, gimple_assign_lhs((assign_array.assign_type_array)[i].stmt), gimple_assign_lhs((assign_array.assign_type_array)[i].stmt));
												}
											}
											else if (gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt) != target && TREE_CODE(gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt)) == SSA_NAME)
											{

												if (!check_stmtStack(gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt)))
												{
													// debug((assign_array.assign_type_array)[i].stmt);
													if ((assign_array.assign_type_array)[i].form_tree != NULL)
													{
														// fprintf(stderr, "=======fist hit2========\n");
														debug_tree((assign_array.assign_type_array)[i].form_tree);
														// set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, (assign_array.assign_type_array)[i].form_tree, target, NULL);
														set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, (assign_array.assign_type_array)[i].form_tree, target, NULL);
													}
													else
														set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt), target, NULL);
													new_search_imm_use(used_stmt, gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt), gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt));
												}
											}
											// set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt), target, NULL);
											// new_search_imm_use(used_stmt, gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt), gimple_assign_rhs1((assign_array.assign_type_array)[i].stmt));
										}

										else if (gimple_code((assign_array.assign_type_array)[i].stmt) == GIMPLE_CALL)
										{

											// tree second = TREE_OPERAND(gimple_call_arg(gc, 0), 0);
											// 											const char *name;
											// 											name = get_tree_code_name(TREE_CODE(gimple_call_arg((assign_array.assign_type_array)[i].stmt, 0)));
											// ADDR_EXPR
											// 											if (name != NULL)
											if (TREE_CODE(gimple_call_arg((assign_array.assign_type_array)[i].stmt, 0)) == ADDR_EXPR)
												if (!check_stmtStack(gimple_call_arg((assign_array.assign_type_array)[i].stmt, 0)))
												{
													// debug(gimple_call_arg((assign_array.assign_type_array)[i].stmt, 0));
													set_gimple_array(used_stmt, (assign_array.assign_type_array)[i].stmt, gimple_call_arg((assign_array.assign_type_array)[i].stmt, 0), target, NULL);
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
						// debug_gimple_stmt(use_stmt);
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
	struct timespec astart, aend;
	clock_gettime(CLOCK_MONOTONIC, &astart);
	// printfunctionCollect(ptable, used_stmt);
	// return  ;
	tree t2 = NULL_TREE;
	// printfBasicblock();
	if (ptable->size >= 0)
	{
		FOR_EACH_TABLE(table1, t2)
		{
			// debug_tree(table1->target);
			// if(t2 != NULL_TREE){
			// debug(t2);
			// fprintf(stderr, "start collect similar stmtstart collect similar stmtstart collect similar stmtstart collect similar stmt\n");}
			if (TREE_CODE(TREE_TYPE(t2)) == METHOD_TYPE || TREE_CODE(TREE_TYPE(t2)) == FUNCTION_TYPE || TREE_CODE(TREE_TYPE(t2)) == RECORD_TYPE || !(TREE_CODE(t2) == SSA_NAME))
			{
				continue;
			}
			if (table1->removed || !TREE_CODE(t) == SSA_NAME)
				continue;
			int colectCount = 0;
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
				colectCount++;
				// fprintf(stderr, "check stmt\n");
				// debug(stmtStack.top());
				stmtStack.pop();
			}
			while (stmtStack2.size())
			{
				// colectCount++;
				// fprintf(stderr, "check stmt\n");
				// debug(stmtStack.top());
				stmtStack2.pop();
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
			// 		// if (gimple_location_safe(table1->last_stmt))
			// 		// {
			// 		// 	location_t loc = gimple_location_safe(table1->last_stmt);
			// 		// 	loc = gimple_location(table1->last_stmt);
			// 		// 	debug(table1->last_stmt);
			// 		// 	warning_at(gimple_location_safe(table1->last_stmt), 0, "use location");
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
				break;
			// fprintf(stderr, "check stmt\n");
			// debug_tree(used_stmt->target);
			// debug_tree(table1->target);
			treeGimpleArray->put(table1->target, *used_stmt);
			Entrypoint++;
			// if(table1->next == NULL)
			// 	fprintf(stderr, "weritjroti;ejhoerjhio;rtjhojert\n");
			// break;
			// debug(table1->next->target);
		}
	}
	// FunctionStmtMappingAssign(ptable, used_stmt);
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
	clock_gettime(CLOCK_MONOTONIC, &aend);
	struct timespec temp = diff(astart, aend);
	double time_used;
	time_used = temp.tv_sec + (double)temp.tv_nsec / 1000000000.0;
	fprintf(stderr, "algorithm time: %f s\n", time_used);
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

void checkPointerConstraint(tree function_tree, ptb *ptable, gimple_array *user_tmp, tree checkTree, int calleetype)
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

	fprintf(stderr, "\033[40;42m =======pre_check_funciton:%s========= \033[0m\n", get_name(function_tree));
	if (calleetype == FUNCITON_THREAD && threadmod == true)
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
		int find_retheapstmt = -2;
		int find_pthread_detched = 0;
		int find_pthread_join = 0;
		int is_pthread_detched = 0;
		int find_mallocstmt = 0;

		// debug_tree(table_temp->target);
		// debug_tree(table_temp->node->get_fun()->decl);
		// fprintf(stderr, "\n ------------------------------------------\n");
		// debug_tree(function_tree);
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
				debug_tree(table_temp->target);

				//ready add dot graph
				fprintf(stderr, "\ndot graph START\n");
				// fprintf(stderr, "dot graph entry %s\n", (char *)get_name(function_tree));
				// fprintf(stderr, "dot graph target entry end\n\n");
				//ready add dot graph

				fprintf(stderr, "\n======================================================================\n");
				// vector<relate_type> relate_type_array;
				// function_relate_array fun_array;
				// if (function_relate_collect->get(mallocStmt_tree) == NULL)
				// {
				// 	// fprintf(stderr, "%s\n",get_name (getFucntionDecl));
				// 	fun_array.relate_type_array = relate_type_array;
				// }
				// else
				// {
				// 	fun_array = *(function_relate_collect->get(mallocStmt_tree));
				// 	relate_type_array = fun_array.relate_type_array;
				// }

				// debug_tree(function_tree);
				const char *name = "";
				//fprintf(stderr ,"dot graph target %s\n", (char *)get_name(table_temp->target));
				// //ready add dot graph
				// fprintf(stderr, "dot graph target loc start ");
				// warning_at(gimple_location(table_temp->last_stmt), 0, "use location");
				// fprintf(stderr, "dot graph target loc end\n\n");
				// //ready add dot graph

				// fprintf(stderr, "\n====qwd==================================================================\n");
				// if (TREE_CODE(table_temp->target) == INTEGER_CST)
				// 	continue;
				// if (!strcmp(get_tree_code_name(TREE_CODE(table_temp->target)), "addr_expr"))
				// 	continue;
				// if (!strcmp(get_tree_code_name(TREE_CODE(gimple_assign_rhs1(def_stmt))), "addr_expr"))
				// 	continue;
				// debug_tree(table_temp->target);
				gimple *def_stmt = SSA_NAME_DEF_STMT(table_temp->target);
				if (TREE_CODE(table_temp->target) == INTEGER_CST)
				{
					debug_tree(table_temp->target);
					continue;
				}
				// continue;
				else if (gimple_code(def_stmt) == GIMPLE_CALL)
				{
					name = get_name(gimple_call_fndecl(def_stmt));
					// fprintf(stderr, "GIMPLE CODE :addr_expr---%s-----\n", name);
				}
				// debug_gimple_stmt(def_stmt);
				fprintf(stderr, "\n ================== trace ptable================== \n");
				if (is_gimple_call(def_stmt))
					if (gimple_call_fn(def_stmt))
					{
						const char *name;
						name = get_name(gimple_call_fn(def_stmt));
						fprintf(stderr, "trace fucntion name:%s \n", name);
						// debug_tree(gimple_call_fndecl(u_stmt));
						trace_function_path(gimple_call_fndecl(def_stmt), -100, table_temp->target, &find_retheapstmt);
						if (find_retheapstmt > 0)
							fprintf(stderr, "this is fucntion return value is heap-object \n");

						// while (traceStack.size())
						// {
						// 	// fprintf(stderr, "check stmt\n");
						// 	// debug(stmtStack.top());
						// 	traceStack.pop();
						// }
					}
				fprintf(stderr, "\n ================== trace ptable ================== \n");
				// if (gimple_code(def_stmt) == GIMPLE_NOP)
				// 	continue;

				// debug(def_stmt);
				// if(def_stmt == NULL)
				// fprintf(stderr, "\n====qwd==================================================================\n");
				// debug(def_stmt);
				// debug_tree(gimple_assign_rhs1(def_stmt));
				// 	debug(def_stmt);
				fprintf(stderr, "GIMPLE CODE :addr_expr---%s-----\n", name);
				// if(def_stmt != NULL)
				// debug_tree(table_temp->target);
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
					hash_map<tree, function_relate_array> *function_maxbb_collect;
					function_maxbb_collect = new hash_map<tree, function_relate_array>;
					tree lastfucntion = NULL;
					gimple *laststmt = NULL;
					vector<relate_type> maxbb_type_array;
					function_relate_array maxbb_array;
					if (function_maxbb_collect->get(table_temp->target) == NULL)
					{
						// fprintf(stderr, "%s\n",get_name (getFucntionDecl));
						maxbb_array.relate_type_array = maxbb_type_array;
					}
					else
					{
						maxbb_array = *(function_maxbb_collect->get(table_temp->target));
						maxbb_type_array = maxbb_array.relate_type_array;
					}
					int lastbasicblock = -1;
					if (table_temp->swap_type == FUNCITON_THREAD)
						fprintf(stderr, " \n Start is Pthread Job Collect  \n");
					else
						fprintf(stderr, " \n Start check Pointer Collect  \n");
					fprintf(stderr, "\n======================================================================\n");
					if (user_tmp->size > 0)
					{
						FOR_EACH_USE_TABLE(user_tmp, u_stmt)
						{

							struct free_type free_type;

							if (user_tmp->ret_stmt != NULL)
							{

								if (debugmod)
								{
									fprintf(stderr, "dot graph entry %s\n", (char *)get_name(function_tree));
									fprintf(stderr, "dot graph target entry end\n\n");
								}
								if (gimple_code(user_tmp->ret_stmt) == GIMPLE_RETURN)
								{
									debug(user_tmp->ret_stmt);
									warning_at(gimple_location(user_tmp->ret_stmt), 0, "use location");

									now_stmt = user_tmp->ret_stmt;
									if (debugmod)
									{
										//ready add dot graph
										fprintf(stderr, "dot graph target loc start ");
										debug_gimple_stmt(user_tmp->ret_stmt);
										warning_at(gimple_location(user_tmp->ret_stmt), 0, "use location");
										fprintf(stderr, "dot graph target loc en1\n\n");

										fprintf(stderr, "dot graph target line desc form basic block %d: ", gimple_bb(user_tmp->ret_stmt)->index);
										debug_gimple_stmt(user_tmp->ret_stmt);
										warning_at(gimple_location(user_tmp->ret_stmt), 0, "use location");
										fprintf(stderr, "dot graph target line desend\n\n");

										fprintf(stderr, "dot graph target loc start ");
										debug_gimple_stmt(user_tmp->ret_stmt);
										warning_at(gimple_location(user_tmp->ret_stmt), 0, "use location");
										fprintf(stderr, "dot graph target loc end\n\n");

										fprintf(stderr, "dot graph target basicblock start ");
										fprintf(stderr, "from %s basic block %d", (char *)get_name(function_tree), gimple_bb(user_tmp->ret_stmt)->index);
										fprintf(stderr, "dot graph target basicblock end\n\n");
										// if(gimple_block (u_stmt))
										// debug_tree(BLOCK_SUPERCONTEXT (gimple_block (u_stmt)));
										// lastbasicblock = gimple_bb(user_tmp->ret_stmt)->index;
										// lastfucntion = function_tree;
										// laststmt = user_tmp->ret_stmt;
										if (gimple_block(user_tmp->ret_stmt))
											if (TREE_CODE(BLOCK_SUPERCONTEXT(gimple_block(user_tmp->ret_stmt))) == FUNCTION_DECL && (function_tree != BLOCK_SUPERCONTEXT(gimple_block(user_tmp->ret_stmt))))
											{
												// fprintf(stderr, "測試222%d\n\n", LOCATION_LINE(loc));
												// fprintf(stderr, "測試%s\n\n", LOCATION_FILE(loc));
												fprintf(stderr, "dot graph target basicblock star1 ");
												fprintf(stderr, "from %s basic block %d", (char *)get_name(BLOCK_SUPERCONTEXT(gimple_block(user_tmp->ret_stmt))), gimple_bb(user_tmp->ret_stmt)->index);
												fprintf(stderr, "dot graph target basicblock end\n\n");
												// lastfucntion = BLOCK_SUPERCONTEXT(gimple_block(user_tmp->ret_stmt));
												// debug(user_tmp->aptr);
											}
										fprintf(stderr, "dot graph stmt start ");
										debug(user_tmp->ret_stmt);
										warning_at(gimple_location(user_tmp->ret_stmt), 0, "use location");
										fprintf(stderr, "dot graph stmt end\n\n");
										//ready add dot graph
									}
									// find_mallocstmt = IS_HEAP_FUCNTION;
									// continue;
								}
							}
							else
							{

								gimple *finalstmt;
								if (user_tmp->target != NULL)
								{
									if (debugmod)
									{
										fprintf(stderr, "dot graph entry %s\n", (char *)get_name(function_tree));
										fprintf(stderr, "dot graph target entry end\n\n");
									}

									finalstmt = u_stmt;

									debug(u_stmt);
									warning_at(gimple_location(u_stmt), 0, "use location");
									now_stmt = u_stmt;
									//ready add dot graph
									if (debugmod)
									{

										fprintf(stderr, "dot graph target loc start ");
										debug_gimple_stmt(table_temp->last_stmt);
										warning_at(gimple_location(table_temp->last_stmt), 0, "use location");
										fprintf(stderr, "dot graph target loc en1\n\n");

										fprintf(stderr, "dot graph target line desc form basic block %d: ", gimple_bb(u_stmt)->index);
										debug(u_stmt);
										warning_at(gimple_location(u_stmt), 0, "use location");
										fprintf(stderr, "dot graph target line desend\n\n");

										fprintf(stderr, "dot graph target loc start ");
										debug_gimple_stmt(table_temp->last_stmt);
										warning_at(gimple_location(table_temp->last_stmt), 0, "use location");
										fprintf(stderr, "dot graph target loc end\n\n");

										fprintf(stderr, "dot graph target basicblock start ");
										fprintf(stderr, "from %s basic block %d", (char *)get_name(function_tree), gimple_bb(u_stmt)->index);
										fprintf(stderr, "dot graph target basicblock en1\n\n");

										// lastbasicblock = gimple_bb(user_tmp->ret_stmt)->index;
										// lastfucntion = function_tree;
										// laststmt = u_stmt;
										struct relate_type relate_type;
										// vector<relate_type>::iterator it_i;
										// for (it_i = fun_array2.relate_type_array.begin(); it_i != fun_array2.relate_type_array.end(); ++it_i)
										// {
										// 	fprintf(stderr, "\n===================================wwwwwwwwwwwww==========================\n");
										// 	if(it_i->relate_funtree == function_tree)
										// 	// if (it_i->stmt == callee->decl)
										// 	// 	find = 1;
										// }
										// if ((gimple_bb(u_stmt)->index > fun_array2.relate_type_array.now_basicblock) && function_tree == fun_array2.relate_type_array.relate_funtree)
										// {
										relate_type.relate_funtree = function_tree;
										// }
										if (gimple_block(u_stmt))
											if (TREE_CODE(BLOCK_SUPERCONTEXT(gimple_block(u_stmt))) == FUNCTION_DECL && (function_tree != BLOCK_SUPERCONTEXT(gimple_block(u_stmt))))
											{

												fprintf(stderr, "dot graph target basicblock star1 ");
												fprintf(stderr, "from %s basic block %d", (char *)get_name(BLOCK_SUPERCONTEXT(gimple_block(u_stmt))), gimple_bb(u_stmt)->index);
												fprintf(stderr, "dot graph target basicblock end\n\n");
												// if ((gimple_bb(u_stmt)->index > fun_array2.relate_type_array.now_basicblock) && BLOCK_SUPERCONTEXT(gimple_block(u_stmt)) == fun_array2.relate_type_array.relate_funtree)
												// {
												// fprintf(stderr, "\n===================================wwwwwwwwwwwww=================%d===\n", gimple_bb(u_stmt)->index);
												// debug_gimple_stmt(u_stmt);
												relate_type.relate_funtree = BLOCK_SUPERCONTEXT(gimple_block(u_stmt));
												// }
												// relate_type.relate_funtree = BLOCK_SUPERCONTEXT(gimple_block(u_stmt));
											}
											else
											{
												// fprintf(stderr, "\n==================================zzzzzzzzz================%d===\n", gimple_bb(u_stmt)->index);
												// debug_gimple_stmt(u_stmt);
												// // debug_tree( BLOCK_SUPERCONTEXT(gimple_block(u_stmt)));
												// tree TEST = BLOCK_SUPERCONTEXT(gimple_block(u_stmt));

												// while(TREE_CODE(TEST)!=FUNCTION_DECL){
												// 	TEST = BLOCK_SUPERCONTEXT(TEST);
												// }
												// debug_tree( TEST);
												// debug_tree(function_tree);
												relate_type.relate_funtree = function_tree;
											}
										// if (gimple_bb(u_stmt)->index  > fun_array2.relate_type_array.now_basicblock && ())
										// {
										// relate_type.relate_funtree = BLOCK_SUPERCONTEXT(gimple_block(u_stmt));
										relate_type.laststmt = table_temp->last_stmt;
										relate_type.stmt = u_stmt;
										relate_type.relate_tree = user_tmp->target;
										// relate_type.relate_funtree = function_tree;
										relate_type.now_basicblock = gimple_bb(u_stmt)->index;
										maxbb_array.relate_type_array.push_back(relate_type);
										function_maxbb_collect->put(table_temp->target, maxbb_array);
										// int find = 0;
										// for (int i = 0; i < relate_type_array2.size(); i++)
										// {
										// 	// debug_tree((relate_type_array2)[i].relate_funtree );
										// 	if ((relate_type_array2)[i].relate_funtree == relate_type.relate_funtree)
										// 	{
										// 		if (relate_type.now_basicblock >= (relate_type_array2)[i].now_basicblock)
										// 		{
										// 			lastbasicblock = relate_type.now_basicblock;
										// 			fprintf(stderr, "\n==================================asaa=wwwwwwwwwwwww========================%d===\n", relate_type.now_basicblock);
										// 			(relate_type_array2)[i].stmt = relate_type.stmt;
										// 			(relate_type_array2)[i].relate_tree = relate_type.relate_tree;
										// 			(relate_type_array2)[i].now_basicblock = relate_type.now_basicblock;
										// 			(relate_type_array2)[i].relate_funtree = relate_type.relate_funtree;
										// 			(relate_type_array2)[i].laststmt = relate_type.laststmt;
										// 			find = 1;
										// 		}
										// 	}
										// }
										// if (find == 0)
										// {
										// fprintf(stderr, "\n================================xxxx===================\n");
										// if (gimple_bb(u_stmt)->index >= lastbasicblock)
										// {
										// debug_tree(relate_type.relate_funtree);
										// debug(relate_type.stmt);
										// lastbasicblock = gimple_bb(u_stmt)->index;

										// }
										// }
										// for (it_i = fun_array2.relate_type_array.begin(); it_i != fun_array2.relate_type_array.end(); ++it_i)
										// {
										// 	fprintf(stderr, "\n===================================wwwwwwwwwwwww==========================\n");
										// 	if (it_i->relate_funtree == relate_type.relate_funtree)
										// 	{
										// 		if (gimple_bb(u_stmt)->index > it_i->now_basicblock)
										// 		{
										// 			it_i->u_stmt = relate_type.stmt;
										// 			it_i->relate_tree = user_tmp->target;
										// 			it_i->now_basicblock =gimple_bb(u_stmt)->index ;
										// 			it_i->relate_funtree = relate_type.relate_funtree ;
										// 			find =1 ;
										// 		}
										// 	}
										// 	// if (it_i->stmt == callee->decl)
										// 	// 	find = 1;
										// }
										// if (find == 0)
										// {
										// 	fun_array2.relate_type_array.push_back(relate_type);
										// 	function_relate_collect2->put(user_tmp->target, fun_array2);
										// }
										// }
										if (gimple_code(u_stmt) == GIMPLE_CALL)
										{
											// if(is_gimple_call (u_stmt) &&!gimple_call_lhs (u_stmt)){
											// 						fprintf(stderr, "dot graph stmt start ");
											// 					debug(finalstmt);
											// 					debug(table_temp->last_stmt);
											// 					// warning_at(gimple_location(table_temp->last_stmt), 0, "use location");
											// 					fprintf(stderr, "dot graph stmt end\n\n");
											// 					// fprintf(stderr, "---------ss3s---");
											// 					}

											// if (gimple_block(u_stmt))
											// 	if (TREE_CODE(BLOCK_SUPERCONTEXT(gimple_block(u_stmt))) == FUNCTION_DECL && (function_tree != BLOCK_SUPERCONTEXT(gimple_block(u_stmt))))
											// 	{
											// 		// fprintf(stderr, "測試222%d\n\n", LOCATION_LINE(loc));
											// 		// fprintf(stderr, "測試%s\n\n", LOCATION_FILE(loc));
											// 		fprintf(stderr, "dot graph target basicblock star1 ");
											// 		fprintf(stderr, "from %s basic block %d", (char *)get_name(BLOCK_SUPERCONTEXT(gimple_block(u_stmt))), gimple_bb(u_stmt)->index);
											// 		fprintf(stderr, "dot graph target basicblock end\n\n");
											// 		// debug(user_tmp->aptr);
											// 	}
											// if (is_gimple_call(u_stmt) && !gimple_call_lhs(u_stmt))
											// {
											fprintf(stderr, "dot graph stmt start ");
											debug(finalstmt);
											debug(table_temp->last_stmt);
											// warning_at(gimple_location(table_temp->last_stmt), 0, "use location");
											fprintf(stderr, "dot graph stmt end\n\n");
											// fprintf(stderr, "---------ss3s---");
											// }

											name = get_name(gimple_call_fn(u_stmt));
											if (name != NULL)
												if (!strcmp(name, "free") || !strcmp(name, "xfree"))
												{
													fprintf(stderr, "dot graph stmt start ");
													debug(u_stmt);
													warning_at(gimple_location(u_stmt), 0, "use location");

													fprintf(stderr, "dot graph stmt end\n\n");

													if (freemod)
													{

														fprintf(stderr, "dot graph target color desc");
														fprintf(stderr, "green");
														fprintf(stderr, "dot graph target color desend\n\n");
													}
												}
												else
												{

													fprintf(stderr, "dot graph stmt start ");
													debug(u_stmt);
													warning_at(gimple_location(u_stmt), 0, "use location");

													fprintf(stderr, "dot graph stmt end\n\n");
												}
										}
										else
										{
											// if (gimple_block(u_stmt))
											// 	if (TREE_CODE(BLOCK_SUPERCONTEXT(gimple_block(u_stmt))) == FUNCTION_DECL && (function_tree != BLOCK_SUPERCONTEXT(gimple_block(u_stmt))))
											// 	{
											// fprintf(stderr, "測試222%d\n\n", LOCATION_LINE(loc));
											// 		// fprintf(stderr, "測試%s\n\n", LOCATION_FILE(loc));
											// fprintf(stderr, "--------------------------------------");
											// 		fprintf(stderr, "from %s basic block %d", (char *)get_name(BLOCK_SUPERCONTEXT(gimple_block(u_stmt))), gimple_bb(u_stmt)->index);
											// 		fprintf(stderr, "dot graph target basicblock end\n\n");
											// 		// debug(user_tmp->aptr);
											// 	}
											fprintf(stderr, "dot graph stmt start ");
											debug(u_stmt);
											debug(table_temp->last_stmt);
											// warning_at(gimple_location(table_temp->last_stmt), 0, "use location");
											fprintf(stderr, "dot graph stmt end\n\n");

											fprintf(stderr, "dot graph stmt start ");
											debug(u_stmt);
											warning_at(gimple_location(u_stmt), 0, "use location");
											fprintf(stderr, "dot graph stmt end\n\n");
										}
									}
									// }
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

									if (calleetype == FUNCITON_HEAP)
									{

										debug_tree(function_tree);
									}
									else if (calleetype == FUNCITON_THREAD && threadmod == true)
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
									// fprintf(stderr, "\n ================== function ================ \n");
									// debug_tree(gimple_call_builtin_p (u_stmt));
									// debug_tree(gimple_block (u_stmt));
									// tree supercontext = BLOCK_SUPERCONTEXT (gimple_block (u_stmt));
									// if(gimple_block (u_stmt))
									// debug_tree(BLOCK_SUPERCONTEXT (gimple_block (u_stmt)));
									if (gimple_cond_code(u_stmt))
									{
										if (!is_gimple_assign(u_stmt))
											if (gimple_cond_lhs(u_stmt))
											{
												debug_gimple_stmt(u_stmt);
												fprintf(stderr, "\n ================== find cond branch ================= \n");
												debug(u_stmt);
												// warning_at(gimple_location(u_stmt), 0, "use location");
												// fprintf(stderr, "\033[40;32m    HAS FREE STMT count:%d name:%s \033[0m\n", find_freestmt, name);

												//
												// table_temp->node;
												basic_block bb;
												FOR_EACH_BB_FN(bb, table_temp->node->get_fun())
												{
													// debug_tree( table_temp->node->get_fun()->decl);
													if (bb->index == gimple_bb(u_stmt)->index)
													{
														if (bb != table_temp->node->get_fun()->cfg->x_exit_block_ptr->prev_bb)
														{
															edge e;
															edge_iterator ei;

															// fprintf(stderr, "node:= %d\n", bb->index);
															fprintf(stderr, "Cond in fucntion %s basic block %d", (char *)get_name(table_temp->node->get_fun()->decl), bb->index);
															FOR_EACH_EDGE(e, ei, bb->succs)
															{
																// DFS.addEdge(bb->index, e->dest->index);
																fprintf(stderr, "\n ================== possiable direct basic block ================= \n");
																fprintf(stderr, "from %s basic block %d", (char *)get_name(table_temp->node->get_fun()->decl), e->dest->index);
																// fprintf(stderr, "succs:= %d\n", e->dest->index);
															}
															fprintf(stderr, "\n ================== warring ================== \n");

															// debug(checkTree);
															fprintf(stderr, "\033[40;35m    need check this branch possiable have return or exit stmt \033[0m\n");
															// fprintf(stderr, "\033[40;35m    this stmt possible is heap-object 。 \033[0m\n");

															// fprintf(stderr, "this stmt possible is heap-object 。\n");
															fprintf(stderr, "\n ================== warring ================== \n");
														}
													}
												}
												fprintf(stderr, "\n ================== find cond branch ================= \n");
												// if (bb != cfun->cfg->x_exit_block_ptr->prev_bb)
												// {
												// 	edge e;
												// 	edge_iterator ei;
												// 	fprintf(stderr, "node:= %d\n ", bb->index);
												// 	FOR_EACH_EDGE(e, ei, bb->succs)
												// 	{
												// 		// DFS.addEdge(bb->index, e->dest->index);
												// 		fprintf(stderr, "succs:= %d\n", e->dest->index);
												// 	}
												// }
												// fprintf(stderr, "---------GIMPLE333333 Cond  -------\n");
											}
									}

									if ((gimple_code(u_stmt) == GIMPLE_CALL) && (find_mallocstmt == IS_MALLOC_FUCNTION || find_mallocstmt == IS_OTHRER_FUCNTION))
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
													if (gimple_call_num_args(table_temp->last_stmt) != 0)
													{
														if (is_gimple_assign(table_temp->last_stmt))
															// debug_tree(TREE_OPERAND(gimple_call_arg(table_temp->last_stmt, 2), 0));
															trace_function_path(gimple_assign_lhs(table_temp->last_stmt), -1, NULL_TREE, &find_pthread_detched);
														else
															trace_function_path(TREE_OPERAND(gimple_call_arg(table_temp->last_stmt, 2), 0), -1, NULL_TREE, &find_pthread_detched);
													}
													fprintf(stderr, "\n ================== pre_pthread_detched end ================== \n");
													// fprintf(stderr, "\033[40;31m  wqeeeeeeeeeee %d  \033[0m\n", find_pthread_detched);
													tree findtree = gimple_call_arg(table_temp->last_stmt, 3);

													// debug_gimple_stmt(table_temp->last_stmt);
													// debug_gimple_stmt(u_stmt);
													if (is_gimple_call(u_stmt))
													{
														tree findtree;
														if (gimple_call_num_args(u_stmt) != 0)
														{
															// fprintf(stderr, "\033[40;32m    FIND PTHREAD222_CREATED STMT  \033[0m\n");
															// debug_gimple_stmt(u_stmt);
															findtree = gimple_call_arg(u_stmt, 3);

															if (TREE_CODE(findtree) == ADDR_EXPR)
															{

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
													// debug_gimple_stmt(table_temp->last_stmt);

													if (findtree != NULL)
														if (gimple_call_num_args(table_temp->last_stmt) != 0 && is_gimple_call(table_temp->last_stmt))
															trace_function_path(TREE_OPERAND(gimple_call_arg(table_temp->last_stmt, 2), 0), 0, findtree, &find_freestmt);
													if (table_temp->swap_stmt != NULL && gimple_call_num_args(table_temp->swap_stmt) != 0)
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
												debug_tree(table_temp->target);
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
							// fprintf(stderr, "\n========================================================================\n");
							// if (laststmt)
						}
						if (debugmod)
						{
							lastbasicblock = -1;
							maxbb_array = *(function_maxbb_collect->get(table_temp->target));
							maxbb_type_array = maxbb_array.relate_type_array;
							hash_map<tree, int> *function_maxbasicblock_collect = new hash_map<tree, int>;
							for (int i = 0; i < maxbb_type_array.size(); i++)
							{
								function_maxbasicblock_collect->put(maxbb_type_array[i].relate_funtree, maxbb_type_array[i].now_basicblock);
								for (int j = 0; j < maxbb_type_array.size(); j++)
								{
									// if (i != j)
									// {
									if (maxbb_type_array[i].relate_funtree == maxbb_type_array[j].relate_funtree)
									{
										int max_bb;
										// debug_tree(it_i->relate_funtree);
										max_bb = *(function_maxbasicblock_collect)->get(maxbb_type_array[i].relate_funtree);
										if (maxbb_type_array[j].now_basicblock >= max_bb)
											function_maxbasicblock_collect->put(maxbb_type_array[j].relate_funtree, maxbb_type_array[j].now_basicblock);
									}
									// }
								}
							}
							vector<relate_type>::iterator it_i;
							for (it_i = maxbb_type_array.begin(); it_i != maxbb_type_array.end(); ++it_i)
							{
								int max_bb;
								// debug_tree(it_i->relate_funtree);
								max_bb = *(function_maxbasicblock_collect)->get(it_i->relate_funtree);
								// fprintf(stderr, "\n==============================hhhhhhhhhhhhhhhh===%d==%s===============\n", it_i->now_basicblock, get_name(it_i->relate_funtree));
								// fprintf(stderr, "\n==============================hhhhhhhhhhhhhhhh===%d==%s===============\n", max_bb, get_name(it_i->relate_funtree));
								if (max_bb == (it_i->now_basicblock))
								{
									// fprintf(stderr, "\n==============================hhhhhhhhhhhhhhhh===%d==%s===============\n", max_bb, get_name(it_i->relate_funtree));
									// fprintf(stderr, "\n==============================hhhhhhhhhhhhhhhh===============%d=====\n", lastbasicblock);
									gimple *def_stmt = SSA_NAME_DEF_STMT(it_i->relate_tree);
									fprintf(stderr, "dot graph target basicblock start ");
									fprintf(stderr, "from %s basic block %d", (char *)get_name(it_i->relate_funtree), gimple_bb(it_i->stmt)->index);
									fprintf(stderr, "dot graph target basicblock en1\n\n");
									unsigned long x = rand();
									fprintf(stderr, "dot graph arrow");
									fprintf(stderr, "subgraph cluster_%lu dot graph subgraph  ", x);
									debug(it_i->stmt);
									debug(def_stmt);

									fprintf(stderr, "dot graph subgrapend\n\n");
								}
							}
						}
					}
					else
					{
						find_mallocstmt = PTABLE_IS_NULL;
					}
					// fprintf(stderr, "\n===================================wwwwwwwwwwwww==========================\n");
					// fprintf(stderr, "\n====================================zzz=================================\n");
					if (debugmod)
					{
						vector<relate_type> relate_type_array;
						function_relate_array fun_array;
						debug_tree(table_temp->target);
						if (function_relate_collect->get(table_temp->target) != NULL)
						{
							// fprintf(stderr, "%s\n",get_name (getFucntionDecl));

							fun_array = *(function_relate_collect->get(table_temp->target));
							relate_type_array = fun_array.relate_type_array;

							// struct relate_type relate_type;
							// relate_type.stmt = now_relatelaststmt;
							// relate_type.relate_tree = mallocStmt_tree;
							// relate_type.relate_funtree = function_tree;
							for (int k = 0; k < relate_type_array.size(); k++)
							{

								// debug((relate_type_array)[k].stmt);
								// debug_tree((relate_type_array)[k].relate_funtree);
								// fprintf(stderr, "dot graph subgraph\n");
								// fprintf(stderr, "dot graph target loc start ");
								// 				debug_gimple_stmt(table_temp->last_stmt);
								// 				warning_at(gimple_location(table_temp->last_stmt), 0, "use location");
								// 				fprintf(stderr, "dot graph target loc end\n\n");
								// fprintf(stderr, "dot graph start relate for1");
								// fprintf(stderr, "ID : %lu\n", (relate_type_array)[k].now_fucntion);
								// fprintf(stderr, "from %s basic block %d", (char *)get_name((relate_type_array)[k].relate_funtree), gimple_bb((relate_type_array)[k].stmt)->index);
								// fprintf(stderr, "dot graph end relate end\n\n");
								unsigned long x = rand();
								fprintf(stderr, "dot graph start relate for1");
								fprintf(stderr, "ID : %lu\n", (relate_type_array)[k].now_fucntion);
								fprintf(stderr, "from %s basic block %d", (char *)get_name((relate_type_array)[k].relate_funtree), gimple_bb((relate_type_array)[k].stmt)->index);
								fprintf(stderr, "dot graph end relate end\n\n");
								fprintf(stderr, "subgraph cluster_%lu dot graph subgraph  start ID : %lu stmt(relate) ， Tree ID : %lu : ", x, (relate_type_array)[k].now_stmt, (relate_type_array)[k].now_fucntion);
								debug((relate_type_array)[k].stmt);
								warning_at(gimple_location((relate_type_array)[k].stmt), 0, "use location");

								fprintf(stderr, "dot graph subgrapend\n\n");
								// warning_at(gimple_location(now_relatelaststmt), 0, "use location");
							}
							// fun_array.relate_type_array.push_back(relate_type);
							// function_relate_collect->put(mallocStmt_tree, fun_array);
						}
					}
					// fprintf(stderr, "\n====================================ffff=================================\n");
					// vector<return_type> callerRetTypearray = callerFunArray.return_type_array;
					// for (int k = 0; k < callerRetTypearray.size(); k++)
					// {
					// 	// if (ptr_derefs_may_alias_p((callerRetTypearray)[k].free_tree, mallocStmt_tree))
					// 	// {
					// 	// fprintf(stderr, "\033[40;31m  find return stmt  \033[0m\n");
					// 	// fprintf(stderr, "find wwwwwwwwwwwwwww ointer\n");
					// 	// debug_tree((callerRetTypearray)[k].return_tree);

					// 	// debug_tree(mallocStmt_tree);
					// 	// debug_gimple_stmt((callerRetTypearray)[k].stmt);
					// 	// tree get_function_return_tree = gimple_return_retval(as_a<greturn *>((callerRetTypearray)[k].stmt));
					// 	// if(gimple_return_retval(as_a<greturn *>((callerRetTypearray)[k].stmt)))

					// 	// if((callerRetTypearray)[k].return_tree )
					// 	// fprintf(stderr ,"%s\n", get_tree_code_name(TREE_CODE((callerRetTypearray)[k].return_tree)));
					// 	if ((callerRetTypearray)[k].return_tree)
					// 	{

					// 	}
					// 	}
					// if (now_relatelaststmt != NULL)
					// {
					// 	// fprintf(stderr, "dot graph subgraph\n");
					// 	fprintf(stderr, "dot graph subgraph start ID : %lu stmt(relate) :", now_relatelaststmtid);
					// 	debug(now_relatelaststmt);
					// 	warning_at(gimple_location(now_relatelaststmt), 0, "use location");
					// 	fprintf(stderr, "dot graph subgrapend\n\n");
					// }
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

								if (u_stmt != free_array.at(i).stmt)
								{

									if (Location_b2(free_array.at(i).stmt, u_stmt, function_tree))
									{
										debug_gimple_stmt(free_array.at(i).stmt);
										warning_at(gimple_location(free_array.at(i).stmt), 0, "Use after free error!: free location ");
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
					fprintf(stderr, "\n ================== Start Use after free Check ================== \n");
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
							fprintf(stderr, "\033[40;32m   Maybe you don't have memory leak.... need more checks  \033[0m\n");
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
		if (!gimple_has_body_p(node->decl))
			continue;
		push_cfun(node->get_fun());
		// if (strcmp(get_name(cfun->decl), "main") == 0)
		// debug_tree(cfun->decl);
		//tree test=DECL_SAVED_TREE(cfun->decl);
		//analyze_func_body(DECL_SAVED_TREE(test), 0);
		if (cfun == NULL)
		{
			pop_cfun();
			continue;
		}
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

				//pop_cfun();
				//		dump_fucntion(callee);
			}

			//analyze_function=	analyze_function (caller ,true);
		}
		//  DECL_STRUCT_FUNCTION (cfun->decl);
		// debug_tree(node->get_fun()->decl);
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
	function_relate_collect = new hash_map<tree, function_relate_array>;

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

		//tree test=DECL_SAVED_TREE(cfun->decl);
		//analyze_func_body(DECL_SAVED_TREE(test), 0);
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
					// debug_gimple_stmt(gc);
					// debug_tree(gimple_assign_lhs(gc));
					// warning_at(gimple_location(gc), 0, "use location");
					// fprintf(stderr, "add collect_function_call\n");
				}
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

		if (!gimple_has_body_p(node->decl))
			continue;
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