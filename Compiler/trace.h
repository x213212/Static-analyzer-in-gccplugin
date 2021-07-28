static unsigned long now_fucntion = 0;
static gimple *now_laststmt;
static unsigned long now_laststmtid = 0;
static int fistconunt = 0;
int dump_points_to_solution2(FILE *file, struct pt_solution *pt)
{
	if (pt->anything)
		fprintf(file, ", points-to anything");
	if (pt->nonlocal)
		fprintf(file, ", points-to non-local");
	if (pt->escaped)
	{
		fprintf(file, ", points-to escaped");
	}
	if (pt->ipa_escaped)
	{
		fprintf(file, ", points-to unit escaped");
	}
	if (pt->null)
	{
		fprintf(file, ", points-to NULL");
	}
	if (pt->vars)
	{
		fprintf(file, ", points-to vars: ");
		// dump_decl_set(file, pt->vars);
		// if (pt->vars_contains_nonlocal || pt->vars_contains_escaped || pt->vars_contains_escaped_heap || pt->vars_contains_restrict)
		// {
		// 	const char *comma = "";
		// 	fprintf(file, " (");
		// 	if (pt->vars_contains_nonlocal)
		// 	{
		// 		fprintf(file, "nonlocal");
		// 		comma = ", ";
		// 	}
		// 	if (pt->vars_contains_escaped)
		// 	{
		// 		fprintf(file, "%sescaped", comma);
		// 		comma = ", ";
		// 	}
		// 	if (pt->vars_contains_escaped_heap)
		// 	{
		// 		fprintf(file, "%sescaped heap", comma);
		// 		comma = ", ";
		// 	}
		// 	if (pt->vars_contains_restrict)
		// 	{
		// 		fprintf(file, "%srestrict", comma);
		// 		comma = ", ";
		// 	}
		// 	if (pt->vars_contains_interposable)
		// 		fprintf(file, "%sinterposable", comma);
		// 	fprintf(file, ")");
		// }
	}

	return 1;
}
void trace_fucntion_relate_stmt(cgraph_node *node, tree function_tree, tree mallocStmt_tree)
{

	cgraph_edge *e;
	basic_block bb;
	tree t;
	if (node == NULL)
		return;
	// fprintf(stderr, "=======node_fun: =========\n");
	// debug(now_laststmt);
	FOR_EACH_DEFINED_FUNCTION(node)
	{
		//   if (!gimple_has_body_p (node->decl))
		//       continue;
		int fucntion_level = 0;

		push_cfun(node->get_fun());

		// fprintf(stderr, "=======node_fun:%s=========\n", get_name(cfun->decl));

		if (cfun == NULL)
		{
			pop_cfun();
			continue;
		}
		//mutlple entry point
		if (cfun->decl == function_tree)
		{
			/*calculate dominator tree*/
			calculate_dominance_info(CDI_DOMINATORS);

			/*create DFS graph, Algorithm 1 and 2*/
			fprintf(stderr, "\033[40;36m ======= node_fun:%s========= \033[0m\n", get_name(function_tree));
			fprintf(stderr, "\033[40;36m ======= find relate stmt with %s ========= \033[0m\n", get_name(mallocStmt_tree));

			fprintf(stderr, "\033[40;36m ======= relate stmt argument:%s ========= \033[0m\n", get_name(mallocStmt_tree));
			int seccount = 0;
			FOR_EACH_BB_FN(bb, cfun)
			{
				if (!(tracerelatestmt && freemod))
					fistconunt = 0;
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
					const char *name;
					if (is_gimple_call(gc))
					{
						name = get_name(gimple_call_fn(gc));
						if (name != NULL)
							if (!strcmp(name, "pthread_exit"))
							{
								// debug_tree("test");

								debug_gimple_stmt(gc);
							}
					}
					// debug_gimple_stmt(gc);
					if (is_gimple_assign(gc))
					{
						// debug_gimple_stmt(gc);
						// if (gimple_assign_lhs(gc) != NULL)
						// debug_tree(gimple_assign_lhs(gc));
						// tree parms, p;
						// debug_tree(DECL_ARGUMENTS(node->get_fun()->decl));
						// static_chain_decl
						// debug_tree(DECL_ARGUMENTS(node->get_fun()->decl));
						// debug_tree(DECL_CHAIN(DECL_ARGUMENTS(node->get_fun()->decl)));
						// p = DECL_ARGUMENTS(node->get_fun()->decl);
						/* Fix up the types of parms passed by invisible reference.  */
						// debug_tree(mallocStmt_tree);

						// debug_points_to_info_for( gimple_assign_lhs(gc));

						// if (DECL_ARGUMENTS(node->get_fun()->decl) != NULL)
						// 	// else if (ptr_derefs_may_alias_p(mallocStmt_tree, gimple_assign_lhs(gc)))
						// 	for (t = DECL_ARGUMENTS(node->get_fun()->decl); t; t = DECL_CHAIN(t))
						// 	{
						// 		// debug_tree(t);
						// 		if (gimple_assign_lhs(gc) != NULL)
						// 		{

						// 			// bool check_array_ref = false;
						// 			// debug_tree(gimple_assign_lhs(gc));
						// 			// debug_tree(t);
						// 			// debug_tree(mallocStmt_tree);
						// 			// if (TREE_CODE(gimple_assign_lhs(gc)) == ARRAY_REF)
						// 			// {
						// 			// 	fprintf(stderr, "\033[40;32m    qwdddddddddddddddddddddddd \033[0m\n");
						// 			// 	tree second = TREE_OPERAND(gimple_assign_lhs(gc), 0);
						// 			// 	debug_tree(second);
						// 			// }

						// 			// if (test != NULL)
						// 			// {
						// 			// fprintf(stderr, "\033[40;32m    FIND PTHREAD22222_CREATED STMT  \033[0m\n");
						// 			// 	debug_tree(test);
						// 			// }
						// 			// struct ptr_info_def *pi1, *pi2, *pi3;
						// 			// pi1 = SSA_NAME_PTR_INFO(gimple_assign_lhs(gc));
						// 			// pi2 = SSA_NAME_PTR_INFO(mallocStmt_tree);
						// 			// pi3 = SSA_NAME_PTR_INFO(t);
						// 			// // struct pt_solution pt1 = pi1->pt;
						// 			// // struct pt_solution pt2 = pi2->pt;
						// 			// if (&pi1->pt && &pi2->pt &&  &pi3->pt)
						// 			// {
						// 			// 	fprintf(stderr, "\033[40;32m    qwwwwwwwwwwwwwwwwwww  \033[0m\n");
						// 			// }
						// 			fprintf(stderr, "\033[40;32m    qwwwwwwwwwwwwwwwwwww  \033[0m\n");
						// 			debug_points_to_info_for(mallocStmt_tree);
						// 			// debug_points_to_info_for(t);
						// 			if(gimple_assign_rhs1(gc)!= NULL)
						// 			if(TREE_CODE(gimple_assign_rhs1(gc))== SSA_NAME)
						// 			debug_points_to_info_for(gimple_assign_rhs1(gc));
						// 			else if(TREE_CODE(gimple_assign_lhs(gc))== SSA_NAME)
						// 			debug_points_to_info_for(gimple_assign_lhs(gc));
						// if (ptr_derefs_may_alias_p(t, gimple_assign_lhs(gc)) && ptr_derefs_may_alias_p(mallocStmt_tree, t) && ptr_derefs_may_alias_p(mallocStmt_tree, gimple_assign_lhs(gc)))
						// // if (ptr_derefs_may_alias_p(mallocStmt_tree, gimple_assign_lhs(gc) ))
						// {

						// 	// tree test =gimple_assign_lhs(gc);
						// 	// debug_points_to_info_for(test);
						// 	fprintf(stderr, "\033[40;36m ======= relate node_fun argument2:%s========= \033[0m\n", get_name(t));
						// 	// debug_tree(t);

						// 	fprintf(stderr, "\033[40;36m ======= relate gimple_assign_lhs2:%s========= \033[0m\n", gimple_assign_lhs(gc));
						// 	debug_gimple_stmt(gc);
						// 	warning_at(gimple_location(gc), 0, "use location");

						// 	fprintf(stderr, "\n ================== warring ================== \n");
						// 	// sfprintf(stderr, "function return value related stmt \n");
						// 	fprintf(stderr, "\033[40;35m this pointer possible  reference other address \033[0m\n");
						// 	fprintf(stderr, "\033[40;35m or assign other value \033[0m\n");
						// 	// fprintf(stderr, "\033[40;35m    this stmt possible is heap-object 。 \033[0m\n");
						// 	// fprintf(stderr, "this stmt possible is heap-object 。\n");
						// 	fprintf(stderr, "\n ================== warring ================== \n");
						// }
						// 		}
						// 	}

						struct ptr_info_def *pi1, *pi2, *pi3, *pi4;
						pi1 = SSA_NAME_PTR_INFO(mallocStmt_tree);
						struct pt_solution *pt1 = &pi1->pt;
						// dump_points_to_solution2(stderr, &pi1->pt);
						// fprintf(stderr, "tx%d\n", dump_points_to_solution2(stderr, &pi1->pt));
						// pi1 = SSA_NAME_PTR_INFO(mallocStmt_tree);
						//  struct pt_solution *pt1 = &pi1->pt;
						//  fprintf(stderr, "tx%u\n",  pt1->null);

						if (pt1 && relatemod && debugmod)
							if (TREE_CODE(mallocStmt_tree) == SSA_NAME)
							{

								// debug_points_to_info_for(mallocStmt_tree);
								if (gimple_assign_lhs(gc) != NULL)
								{
									if (TREE_CODE(gimple_assign_lhs(gc)) == SSA_NAME || TREE_CODE(gimple_assign_lhs(gc)) == MEM_REF)
									{
										pi2 = SSA_NAME_PTR_INFO(gimple_assign_lhs(gc));

										struct pt_solution *pt2 = &pi2->pt;
										// // if (pt1->anything)
										// 	fprintf(stderr, "ptsoul%u", *pt1.anything);

										// fprintf(stderr, "ptsoul%u",pt1->anything )	;}
										if (pt2)
										{
											if (gimple_assign_rhs1(gc) != NULL)
											{
												// if (TREE_CODE(gimple_assign_rhs1(gc)) != INTEGER_CST)
												if (ptr_derefs_may_alias_p(mallocStmt_tree, gimple_assign_lhs(gc)))
												// if (ptr_derefs_may_alias_p(mallocStmt_tree, gimple_assign_lhs(gc) ))
												{

													if (gimple_assign_rhs1(gc) != NULL)
													{

														if (ptr_derefs_may_alias_p(mallocStmt_tree, gimple_assign_lhs(gc)))
														{
															// if (ptr_derefs_may_alias_p(mallocStmt_tree, gimple_assign_lhs(gc) ))

															if (TREE_CODE(gimple_assign_lhs(gc)) == ADDR_EXPR)
															{

																if (ptr_derefs_may_alias_p(mallocStmt_tree, gimple_assign_rhs1(gc)))
																{
																	debug_tree(gimple_assign_rhs1(gc));
																	fprintf(stderr, "\n ================== warring ================== \n");
																	// sfprintf(stderr, "function return value related stmt \n");
																	fprintf(stderr, "\033[40;35m this pointer possible  reference other address (rhs)\033[0m\n");
																	fprintf(stderr, "\033[40;35m or assign other value \033[0m\n");
																	// fprintf(stderr, "\033[40;35m    this stmt possible is heap-object 。 \033[0m\n");
																	// fprintf(stderr, "this stmt possible is heap-object 。\n");
																	fprintf(stderr, "\n ================== warring ================== \n");
																}
															}
														}
														// if (TREE_CODE(gimple_assign_rhs1(gc)) == VAR_DECL)
														// {
														// 	// debug_tree(gimple_assign_lhs(gc));
														// 	// debug_tree(gimple_assign_lhs(gc));
														// 	// pi3 = SSA_NAME_PTR_INFO(gimple_assign_lhs(gc));
														// 	// struct pt_solution *pt4 = &pi2->pt;
														// 	// if (dump_points_to_solution2(stderr, pt2) == 0)
														// 		break;
														// 	// break;

														// 	fprintf(stderr, "\n ================== warring ================== \n");
														// 	// sfprintf(stderr, "function return value related stmt \n");
														// 	fprintf(stderr, "\033[40;35m this pointer possible  reference other address (lhs)\033[0m\n");
														// 	fprintf(stderr, "\033[40;35m or assign other value \033[0m\n");
														// 	// fprintf(stderr, "\033[40;35m    this stmt possible is heap-object 。 \033[0m\n");
														// 	// fprintf(stderr, "this stmt possible is heap-object 。\n");
														// 	fprintf(stderr, "\n ================== warring ================== \n");
														// }
														if (TREE_CODE(gimple_assign_rhs1(gc)) == INTEGER_CST)
														{
															// debug_tree(gimple_assign_lhs(gc));
															// tree second = NULL_TREE;
															// second = TREE_OPERAND(gimple_assign_lhs(gc), 0);
															// if (ptr_derefs_may_alias_p(mallocStmt_tree, second))
															// {
															// 	if (is_global_var(second))
															// 	{

															// 	debug_tree(second);
															// 	fprintf(stderr, "\n ================== wasssssssssVVA33333=====%d========= \n", ptr_derefs_may_alias_p(mallocStmt_tree, second));
															// 	}
															// }

															// if (TREE_CODE(gimple_assign_lhs(gc)) == VAR_DECL)
															// 	break;
															// pi3 = SSA_NAME_PTR_INFO(gimple_assign_lhs(gc));
															// struct pt_solution *pt4 = &pi2->pt;
															// if (dump_points_to_solution2(stderr, pt2) == 0)
															// 	continue;
															continue;
															// break;

															// fprintf(stderr, "\n ================== warring ================== \n");
															// // sfprintf(stderr, "function return value related stmt \n");
															// fprintf(stderr, "\033[40;35m this pointer possible  reference other address (lhs)\033[0m\n");
															// fprintf(stderr, "\033[40;35m or assign other value \033[0m\n");
															// // fprintf(stderr, "\033[40;35m    this stmt possible is heap-object 。 \033[0m\n");
															// // fprintf(stderr, "this stmt possible is heap-object 。\n");
															// fprintf(stderr, "\n ================== warring ================== \n");
														}
														else if (TREE_CODE(gimple_assign_rhs1(gc)) == VAR_DECL)
														{
															// debug_points_to_info_for(gimple_assign_lhs(gc));
															// fprintf(stderr, "\n ================== warring ===VVVVVVVVVAR======%d========= \n",ptr_derefs_may_alias_p(mallocStmt_tree, gimple_assign_lhs(gc)));
															// debug_tree(gimple_assign_lhs(gc));
															// pi5 = SSA_NAME_PTR_INFO(gimple_assign_lhs(gc));

															// if (!pi5)
															// 	continue;
															// else
															// 	tree second = NULL_TREE;
															// 	second = TREE_OPERAND(gimple_assign_lhs(gc), 0);

															// 	fprintf(stderr, "\n ================== wasssssssssVVA332333=====%d========= \n", ptr_derefs_may_alias_p(mallocStmt_tree, second));
															// if (TREE_CODE(gimple_assign_lhs(gc)) == MEM_REF)
															// {
															// 	tree second = NULL_TREE;
															// 	second = TREE_OPERAND(gimple_assign_lhs(gc), 0);

															// 	fprintf(stderr, "\n ================== wasssssssssVVA332333=====%d========= \n", ptr_derefs_may_alias_p(mallocStmt_tree, second));
															// 	if (ptr_derefs_may_alias_p(mallocStmt_tree, second))
															// 	{
															// 		fprintf(stderr, "\n ================== wasssssssssVVA33333=====%d========= \n", ptr_derefs_may_alias_p(mallocStmt_tree, second));
															// 	}
															// }
															if (ptr_derefs_may_alias_p(mallocStmt_tree, gimple_assign_lhs(gc)))
															{
																// fprintf(stderr, "\n ================== wasssssssssVVA33333============= \n");

																// debug_tree(mallocStmt_tree);
																// debug_tree(gimple_assign_lhs(gc));
																// fprintf(stderr, "\n ================== ssa_name error============ \n");
																// // debug(gc);
																// debug_points_to_info_for(mallocStmt_tree);
																// debug_points_to_info_for(gimple_assign_lhs(gc));
																// gimple *def_stmt = SSA_NAME_DEF_STMT(gimple_assign_lhs(gc));
																// tree second = NULL_TREE;
																// second = TREE_OPERAND(gimple_assign_lhs(gc), 0);
																// if (ptr_derefs_may_alias_p(mallocStmt_tree, second))
																// {
																// 	debug_tree(second);
																// 	fprintf(stderr, "\n ================== wasssssssssVVA33333=====%d========= \n", ptr_derefs_may_alias_p(mallocStmt_tree, second));
																// }
																// if(is_gimple_call (def_stmt)){
																// 	debug_tree(gimple_call_fndecl(def_stmt));
																// if (!TREE_STATIC(gimple_assign_rhs1(gc)) == true)
																// {
																// 	fprintf(stderr, "FUCK ");
																// }
																// else
																// 	fprintf(stderr, "FUCK2 ");
																// fprintf(stderr, "\n ================== wasssssssssVVA33333=====%d========= \n", ptr_derefs_may_alias_p(mallocStmt_tree, gimple_assign_lhs(gc)));
																if (is_global_var(gimple_assign_rhs1(gc)))
																{
																	debug_tree(gimple_assign_rhs1(gc));
																	debug_tree(gimple_assign_lhs(gc));

																	debug_points_to_info_for(mallocStmt_tree);
																	debug_points_to_info_for(gimple_assign_lhs(gc));
																	fprintf(stderr, "\n ================== wasssssssssVVAR555======%d========= \n", ptr_derefs_may_alias_p(mallocStmt_tree, gimple_assign_lhs(gc)));
																	// debug_tree(gimple_assign_lhs(gc));
																	// gimple *def_stmt = SSA_NAME_DEF_STMT(gimple_assign_lhs(gc));
																	// debug_tree(gimple_assign_rhs1(def_stmt));

																	// fprintf(stderr, "\n ================== wasssssssssVVAR666======%d========= \n", ptr_derefs_may_alias_p(mallocStmt_tree, gimple_assign_lhs(gc)));
																	// imm_use_iterator imm_iter;
																	// gimple *use_stmt;
																	// debug(def_stmt);
																	// FOR_EACH_IMM_USE_STMT(use_stmt, imm_iter, gimple_assign_lhs(gc))
																	// {
																	// 	debug(use_stmt);
																	// }
																	// fprintf(stderr, "\n ================== wasssssssssVVAR22======%d========= \n", ptr_derefs_may_alias_p(mallocStmt_tree, gimple_assign_lhs(gc)));
																	continue;
																}
																// continue;
															}
															// if (TREE_CODE(gimple_assign_lhs(gc)) == VAR_DECL)
															// 	break;
															// pi3 = SSA_NAME_PTR_INFO(gimple_assign_lhs(gc));
															// struct pt_solution *pt4 = &pi2->pt;
															// if (dump_points_to_solution2(stderr, pt2) == 0)
															// 	continue;

															// break;

															// fprintf(stderr, "\n ================== warring ================== \n");
															// // sfprintf(stderr, "function return value related stmt \n");
															// fprintf(stderr, "\033[40;35m this pointer possible  reference other address (lhs)\033[0m\n");
															// fprintf(stderr, "\033[40;35m or assign other value \033[0m\n");
															// // fprintf(stderr, "\033[40;35m    this stmt possible is heap-object 。 \033[0m\n");
															// // fprintf(stderr, "this stmt possible is heap-object 。\n");
															// fprintf(stderr, "\n ================== warring ================== \n");
														}
														else if (TREE_CODE(gimple_assign_rhs1(gc)) == SSA_NAME)
														{
															// debug_tree(gimple_assign_lhs(gc));

															// if (TREE_CODE(gimple_assign_lhs(gc)) == VAR_DECL)
															// 	break;
															// pi3 = SSA_NAME_PTR_INFO(gimple_assign_lhs(gc));
															// struct pt_solution *pt4 = &pi2->pt;
															// if (dump_points_to_solution2(stderr, pt2) == 0)
															// 	continue;

															// fprintf(stderr, "\n ================== warring================== \n");
															//fix ssa_name = ssa_name (rhs ) no pt_solution
															// debug_tree(gimple_assign_lhs(gc));
															pi3 = SSA_NAME_PTR_INFO(gimple_assign_lhs(gc));
															pi4 = SSA_NAME_PTR_INFO(gimple_assign_rhs1(gc));
															struct pt_solution *pt3 = &pi3->pt;
															struct pt_solution *pt4 = &pi4->pt;
															if (!pt3 || !pt4)
																continue;
															else if (!ptr_derefs_may_alias_p(mallocStmt_tree, gimple_assign_rhs1(gc)))
																continue;
															// dump_points_to_solution2(stderr,pt3);
															// if(pt4)
															// 	dump_points_to_solution2(stderr,pt4 );

															// break;

															// // sfprintf(stderr, "function return value related stmt \n");
															// fprintf(stderr, "\033[40;35m this pointer possible  reference other address (lhs)\033[0m\n");
															// fprintf(stderr, "\033[40;35m or assign other value \033[0m\n");
															// // fprintf(stderr, "\033[40;35m    this stmt possible is heap-object 。 \033[0m\n");
															// // fprintf(stderr, "this stmt possible is heap-object 。\n");
															// fprintf(stderr, "\n ================== warring ================== \n");
														}
													}
													// tree test =gimple_assign_lhs(gc);
													// debug_points_to_info_for(test);

													// debug_tree(t);
													//ready add dot graph
													// debug_points_to_info_for(gimple_assign_lhs(gc));

													fprintf(stderr, "\033[40;36m ======= relate node_fun argument222:%s========= \033[0m\n", get_name(mallocStmt_tree));
													fprintf(stderr, "\033[40;36m ======= relate gimple_assign_lhs2:%s========= \033[0m\n", gimple_assign_lhs(gc));

													//ready add dot graph
													fprintf(stderr, "\n\n\n\n");
													unsigned long x = rand();
													if (freemod == true)
													{
														if (seccount == 0)
														{
															fprintf(stderr, "dot graph arrow");
														}
														else
														{
															if (now_laststmt != NULL)
															{
																fprintf(stderr, "dot graph relate stm2 start ID : %lu stmt(free) :", now_laststmtid);
																debug(now_laststmt);
																warning_at(gimple_location(now_laststmt), 0, "use location");
															}

															fprintf(stderr, "dot graph relate en1\n\n");
														}
														fprintf(stderr, "dot graph start relate for1");
													}
													else
													{

														if (fistconunt == 0)
														{
															fprintf(stderr, "dot graph start relate form");
															fistconunt++;
														}
														else
														{
															if (now_laststmt != NULL)
															{
																fprintf(stderr, "dot graph relate stm2 start ID : %lu stmt(free) :", now_laststmtid);
																debug(now_laststmt);
																warning_at(gimple_location(now_laststmt), 0, "use location");
																fprintf(stderr, "dot graph relate en1\n\n");

																now_laststmt = gc;
																now_laststmtid = x;
															}
															fprintf(stderr, "dot graph start relate for1");
														}
													}

													fistconunt++;
													fprintf(stderr, "ID : %lu\n", now_fucntion);
													fprintf(stderr, "from %s basic block %d", (char *)get_name(function_tree), gimple_bb(gc)->index);
													fprintf(stderr, "dot graph end relate end\n\n");
													// debug_tree(gimple_assign_lhs(gc));
													// if (gimple_assign_rhs1(gc))
													// 	debug_tree(gimple_assign_rhs1(gc));
													//print address error
													warning_at(gimple_location(gc), 0, "use location");
													fprintf(stderr, "dot graph relate stmt start ID : %lu stmt(LHS) :", x);
													debug(gc);
													warning_at(gimple_location(gc), 0, "use location");
													debug(gimple_assign_lhs(gc));
													fprintf(stderr, "dot graph relate end\n\n");
													//ready add dot graph

													// if (gimple_assign_rhs1(gc) != NULL)
													// {

													// 	if (TREE_CODE(gimple_assign_rhs1(gc)) != INTEGER_CST)
													// 	{

													// 		// debug_tree(gimple_assign_rhs1(gc));
													// 		// fprintf(stderr, "\n ================== warriwwwwwwwwwwwwwwwng ================== \n");
													// 	}
													// }
													fprintf(stderr, "\n ================== warring ================== \n");
													// sfprintf(stderr, "function return value related stmt \n");
													fprintf(stderr, "\033[40;35m this pointer possible  reference other address \033[0m\n");
													fprintf(stderr, "\033[40;35m or assign other value \033[0m\n");
													// fprintf(stderr, "\033[40;35m    this stmt possible is heap-object 。 \033[0m\n");
													// fprintf(stderr, "this stmt possible is heap-object 。\n");
													fprintf(stderr, "\n ================== warring ================== \n");
												}
											}
										}
									}
								}
							}

						// if (gimple_assign_rhs1(gc) != NULL)
						// 	if (TREE_CODE(gimple_assign_rhs1(gc)) == SSA_NAME || TREE_CODE(gimple_assign_rhs1(gc)) == MEM_REF)
						// 	{

						// 		pi3 = SSA_NAME_PTR_INFO(gimple_assign_rhs1(gc));
						// 		// fprintf(stderr, "ptsoul%d",&pi3->pt->anything )	;
						// 		// if (&pi3->pt != NULL)
						// 		// if (&pi3->pt )
						// 		struct pt_solution *pt3 = &pi3->pt;
						// 		if (pt3)

						// 			if (ptr_derefs_may_alias_p(mallocStmt_tree, gimple_assign_rhs1(gc)) && !refs_may_alias_p(mallocStmt_tree, gimple_assign_rhs1(gc)))
						// 			// if (ptr_derefs_may_alias_p(mallocStmt_tree, gimple_assign_lhs(gc) ))
						// 			{
						// 				// tree test =gimple_assign_lhs(gc);
						// 				// debug_points_to_info_for(test);

						// 				// debug_tree(t);
						// 				if (pt_solution_empty_p(pt3))
						// 				{
						// 					dump_points_to_solution2(stderr, pt3);
						// 					fprintf(stderr, "tx\n");
						// 				}
						// 				fprintf(stderr, "\033[40;36m ======= relate node_fun argument222:%s========= \033[0m\n", get_name(mallocStmt_tree));
						// 				fprintf(stderr, "\033[40;36m ======= relate gimple_assign_rhs3:%s========= \033[0m\n", gimple_assign_rhs1(gc));
						// 				//ready add dot graph

						// 				//ready add dot graph
						// 				fprintf(stderr, "dot graph start relate form ");

						// 				fprintf(stderr, "ID : %lu\n", now_fucntion);
						// 				fprintf(stderr, "from %s basic block %d", (char *)get_name(function_tree), gimple_bb(gc)->index);
						// 				fprintf(stderr, "dot graph end relate end\n\n");
						// 				//ready add dot graph
						// 				unsigned long x = rand();

						// 				//ready add dot graph
						// 				fprintf(stderr, "dot graph relate stmt start ID : %lu stmt(RHS) :", x);
						// 				debug(gc);
						// 				warning_at(gimple_location(gc), 0, "use location");
						// 				debug(gimple_assign_rhs1(gc));
						// 				fprintf(stderr, "dot graph relate end\n\n");
						// 				//ready add dot graph

						// 				fprintf(stderr, "\n ================== warring ================== \n");
						// 				// sfprintf(stderr, "function return value related stmt \n");
						// 				fprintf(stderr, "\033[40;35m this pointer possible  reference other address \033[0m\n");
						// 				fprintf(stderr, "\033[40;35m or assign other value \033[0m\n");
						// 				// fprintf(stderr, "\033[40;35m    this stmt possible is heap-object 。 \033[0m\n");
						// 				// fprintf(stderr, "this stmt possible is heap-object 。\n");
						// 				fprintf(stderr, "\n ================== warring ================== \n");
						// 			}
						// 	}
						// fprintf(stderr, "\033[40;36m ======= relate node_fun end:%s========= \033[0m\n", get_name(mallocStmt_tree));

						if (TREE_CODE(gimple_assign_lhs(gc)) == ARRAY_REF || TREE_CODE(mallocStmt_tree) == SSA_NAME)
						{
							tree second = NULL_TREE;
							second = TREE_OPERAND(gimple_assign_lhs(gc), 0);
							if (TREE_CODE(gimple_assign_lhs(gc)) == ARRAY_REF)
								second = TREE_OPERAND(gimple_assign_lhs(gc), 0);

							// debug_tree(second);
							// debug_tree(mallocStmt_tree);
							if (second != NULL_TREE)
								if ((TREE_CODE(mallocStmt_tree) == VAR_DECL) && TREE_CODE(second) == VAR_DECL)
									if ((second == mallocStmt_tree))
									{
										fprintf(stderr, "\033[40;36m ======= relate node_fun argument:%s========= \033[0m\n", get_name(function_tree));
										// debug_tree(t);

										fprintf(stderr, "\033[40;36m ======= relate gimple_assign_lhs:%s========= \033[0m\n", gimple_assign_lhs(gc));

										debug_gimple_stmt(gc);
										warning_at(gimple_location(gc), 0, "use location");

										fprintf(stderr, "\n ================== warring ================== \n");
										// sfprintf(stderr, "function return value related stmt \n");
										fprintf(stderr, "\033[40;35m this pointer possible  reference other address \033[0m\n");
										fprintf(stderr, "\033[40;35m or assign other value \033[0m\n");
										// fprintf(stderr, "\033[40;35m    this stmt possible is heap-object 。 \033[0m\n");
										// fprintf(stderr, "this stmt possible is heap-object 。\n");
										fprintf(stderr, "\n ================== warring ================== \n");
									}
							if ((second == NULL_TREE) && (TREE_CODE(mallocStmt_tree) == SSA_NAME))
							{
								if ((TREE_CODE(gimple_assign_rhs1(gc)) == SSA_NAME))
								{
									// debug_tree(gimple_assign_lhs(gc));
									// debug_tree(gimple_assign_rhs1(gc));
									gimple *def_stmt = SSA_NAME_DEF_STMT(gimple_assign_rhs1(gc));

									if (def_stmt != NULL)
										if ((gimple_code(def_stmt) != GIMPLE_PHI) && ptr_derefs_may_alias_p(mallocStmt_tree, gimple_assign_lhs(gc)) && ptr_derefs_may_alias_p(mallocStmt_tree, gimple_assign_rhs1(gc)))
										{
											fprintf(stderr, "\033[40;36m ======= relate node_fun argument:%s========= \033[0m\n", get_name(function_tree));
											// debug_tree(t);

											fprintf(stderr, "\033[40;36m ======= relate gimple_assign_lhs:%s========= \033[0m\n", gimple_assign_lhs(gc));

											debug_gimple_stmt(gc);

											warning_at(gimple_location(gc), 0, "use location");

											fprintf(stderr, "\n ================== warring ================== \n");
											// sfprintf(stderr, "function return value related stmt \n");
											fprintf(stderr, "\033[40;35m this pointer possible  reference other address \033[0m\n");
											fprintf(stderr, "\033[40;35m or assign other value \033[0m\n");
											// fprintf(stderr, "\033[40;35m    this stmt possible is heap-object 。 \033[0m\n");
											// fprintf(stderr, "this stmt possible is heap-object 。\n");
											fprintf(stderr, "\n ================== warring ================== \n");
										}
								}
							}
						}
					}
					else if (is_gimple_call(gc) != NULL)
					{
						struct ptr_info_def *pi1, *pi2, *pi3, *pi4;

						pi1 = SSA_NAME_PTR_INFO(mallocStmt_tree);

						struct pt_solution *pt1 = &pi1->pt;

						if (pt1 && relatemod && debugmod)
							if (gimple_call_num_args(gc) != 0)
							{
								const char *name;
								name = get_name(gimple_call_fn(gc));
								if (TREE_CODE(gimple_call_arg(gc, 0)) == SSA_NAME || ((!strcmp(get_tree_code_name(TREE_CODE(gimple_call_arg(gc, 0))), "addr_expr"))))
									if (name != NULL)
									{
										if (!strcmp(name, "pthread_mutex_lock") || !strcmp(name, "pthread_mutex_unlock") || !strcmp(name, "pthread_exit"))
										{
											continue;
										}
										pi2 = SSA_NAME_PTR_INFO(gimple_call_arg(gc, 0));

										struct pt_solution *pt2 = &pi2->pt;
										// // if (pt1->anything)
										// 	fprintf(stderr, "ptsoul%u", *pt1.anything);
										// debug_points_to_info_for(mallocStmt_tree);
										// debug_points_to_info_for(gimple_call_arg(gc, 0));
										// fprintf(stderr, "testttt");
										// debug_tree(gimple_call_arg(gc, 0));
										if (pt2)
										{
											if (ptr_derefs_may_alias_p(mallocStmt_tree, gimple_call_arg(gc, 0)))
											{
												unsigned long x = rand();
												if (freemod == true)
												{
													if (seccount == 0)
													{
														fprintf(stderr, "dot graph arrow");
													}
													else
													{
														if (now_laststmt != NULL)
														{
															fprintf(stderr, "dot graph relate stm2 start ID : %lu stmt(free) :", now_laststmtid);
															debug(now_laststmt);
															warning_at(gimple_location(now_laststmt), 0, "use location");
														}

														fprintf(stderr, "dot graph relate en1\n\n");
													}
													fprintf(stderr, "dot graph start relate for1");
												}
												else
												{

													if (fistconunt == 0)
													{
														fprintf(stderr, "dot graph start relate form");
														fistconunt++;
													}
													else
													{
														if (now_laststmt != NULL)
														{
															fprintf(stderr, "dot graph relate stm2 start ID : %lu stmt(free) :", now_laststmtid);
															debug(now_laststmt);
															warning_at(gimple_location(now_laststmt), 0, "use location");
															fprintf(stderr, "dot graph relate en1\n\n");

															now_laststmt = gc;
															now_laststmtid = x;
														}
														fprintf(stderr, "dot graph start relate for1");
													}
												}

												fistconunt++;
												fprintf(stderr, "ID : %lu\n", now_fucntion);
												fprintf(stderr, "from %s basic block %d", (char *)get_name(function_tree), gimple_bb(gc)->index);
												fprintf(stderr, "dot graph end relate end\n\n");
												// debug_tree(gimple_call_arg(gc, 0));

												fprintf(stderr, "dot graph relate stmt start ID : %lu stmt(call) :", x);
												debug(gc);
												warning_at(gimple_location(gc), 0, "use location");
												debug(gimple_call_arg(gc, 0));
												fprintf(stderr, "dot graph relate end\n\n");
											}
											// }
										}
									}
							}
					}
				}
			}
		}
		pop_cfun();
	}
	fprintf(stderr, "fucntion collect path finsh\n");
}

int trace_function_path(tree function_tree, int fucntion_level, tree mallocStmt_tree, int *freecount)
{
	// srand((unsigned) time(NULL) + getpid());
	/* 產生亂數 */

	// string t = "qweqwe";

	if (function_path_collect->get(function_tree) == NULL)
	{
		// debug_tree(function_tree);
		// fprintf(stderr, "\033[40;31m  find fwwwwwwwwwwwww \033[0m\n");
		return 0;
	}
	now_fucntion = rand();
	// now_laststmtid = 0;

	function_path_array fun_array = *(function_path_collect->get(function_tree));

	vector<function_path> function_path_array = fun_array.function_path_array;
	const char *name;

	// if (is_gimple_call(now_stmt))
	// {
	// 	if (gimple_call_fn(now_stmt) != NULL)
	// 	{
	// 		name = get_name(gimple_call_fn(now_stmt));
	// 		// if (!strcmp(name, "free") || !strcmp(name, "xfree"))
	// 		// {
	// 		// fprintf(stderr, "dot graph stmt start ");
	// 		// debug(now_stmt);
	// 		// warning_at(gimple_location(now_stmt), 0, "use location");

	// 		// fprintf(stderr, "dot graph stmt en1\n\n");
	// 		// fprintf(stderr, "dot graph arrow\n\n");
	// 		// }
	// 		// if (!strcmp(name, "pthread_create"))
	// 		// 	// debug();
	// 		// 	fprintf(stderr, "\033[40;44m =======qwdwqdqwd %s\n", get_name(gimple_call_fn(now_stmt)));
	// 	}
	// }
	//debug_tree(function_tree);
	//vector<pair<fdecl,location_t>> loc;
	fistconunt = 0;
	// function_return_array callerFunArray = *(function_free_collect->get(function_tree));
	// vector<return_type> callerRetTypearray = callerFunArray.return_type_array;
	// fprintf(stderr, "=======trace_function_path %s  function_call count: %d level :%d========\n", get_name(function_tree), function_path_array.size(), fucntion_level);
	fprintf(stderr, "\033[40;44m =======trace_function_path %s  function_call count: %d level :%d========  \033[0m\n", get_name(function_tree), function_path_array.size(), fucntion_level);
	if (mallocStmt_tree != NULL_TREE && fucntion_level != -100)
	{
		if (function_free_collect->get(function_tree) != NULL)
		{
			function_free_array callerFunArray = *(function_free_collect->get(function_tree));
			vector<free_type> callerRetTypearray = callerFunArray.free_type_array;
			for (int k = 0; k < callerRetTypearray.size(); k++)
			{
				debug_gimple_stmt((callerRetTypearray)[k].stmt);
				if (ptr_derefs_may_alias_p((callerRetTypearray)[k].free_tree, mallocStmt_tree))
				{

					fprintf(stderr, "\033[40;31m  find free stmt free same pointer \033[0m\n");
					// fprintf(stderr, "find free stmt free same pointer\n");
					// debug_tree(mallocStmt_tree);
					debug_gimple_stmt((callerRetTypearray)[k].stmt);
					warning_at(gimple_location((callerRetTypearray)[k].stmt), 0, "use location");
					++(*freecount);

					if (freemod && debugmod)
					{

						// if (fistconunt == 0)
						// {
						fprintf(stderr, "dot graph start relate form ");
						// 	fistconunt++;
						// }
						// else
						// 	fprintf(stderr, "dot graph start relate for1 ");

						fprintf(stderr, "ID : %lu\n", now_fucntion);
						fprintf(stderr, "from %s basic block %d", (char *)get_name(function_tree), gimple_bb((callerRetTypearray)[k].stmt)->index);
						fprintf(stderr, "dot graph end relate end\n\n");
						now_laststmt = (callerRetTypearray)[k].stmt;
						//ready add dot graph
						unsigned long x = rand();
						now_laststmtid = x;

						//ready add dot graph
						fprintf(stderr, "dot graph relate stmt start ID : %lu stmt(free) :", x);
						debug((callerRetTypearray)[k].stmt);
						warning_at(gimple_location((callerRetTypearray)[k].stmt), 0, "use location");
						// debug(gimple_assign_lhs((callerRetTypearray)[k].stmt));
						fprintf(stderr, "dot graph relate end\n\n");

						//ready add dot graph
						fprintf(stderr, "dot graph target color desc");
						fprintf(stderr, "green");
						fprintf(stderr, "dot graph target color desend\n\n");

						//ready add dot graph
						fprintf(stderr, "dot graph relate stmt start ID : %lu stmt(free) :", x);
						debug((callerRetTypearray)[k].stmt);
						warning_at(gimple_location((callerRetTypearray)[k].stmt), 0, "use location");
						// debug(gimple_assign_lhs((callerRetTypearray)[k].stmt));
						fprintf(stderr, "dot graph relate end\n\n");
					}
					// debug_tree((callerRetTypearray)[k].free_tree);
				}
				// debug_tree((callerRetTypearray)[k].free_tree);
				// debug_tree (mallocStmt_tree);
			}
			if (function_return_collect->get(function_tree) != NULL && fucntion_level != -100)
			{
				// function_pthread_exit_array callerFunArray = *(function_pthread_exit_collect->get(function_tree));
				// vector<pthread_exit_type> callerRetTypearray = callerFunArray.pthread_exit_array;
				function_return_array callerFunArray = *(function_return_collect->get(function_tree));

				vector<return_type> callerRetTypearray = callerFunArray.return_type_array;
				for (int k = 0; k < callerRetTypearray.size(); k++)
				{
					// if (ptr_derefs_may_alias_p((callerRetTypearray)[k].free_tree, mallocStmt_tree))
					// {
					// fprintf(stderr, "\033[40;31m  find return stmt  \033[0m\n");
					// fprintf(stderr, "find wwwwwwwwwwwwwww ointer\n");
					// debug_tree((callerRetTypearray)[k].return_tree);
					// debug_tree(mallocStmt_tree);
					// debug_gimple_stmt((callerRetTypearray)[k].stmt);
					// tree get_function_return_tree = gimple_return_retval(as_a<greturn *>((callerRetTypearray)[k].stmt));
					// if(gimple_return_retval(as_a<greturn *>((callerRetTypearray)[k].stmt)))

					// if((callerRetTypearray)[k].return_tree )
					// fprintf(stderr ,"%s\n", get_tree_code_name(TREE_CODE((callerRetTypearray)[k].return_tree)));
					if ((callerRetTypearray)[k].return_tree)
					{
						if (TREE_CODE(mallocStmt_tree) != INTEGER_CST && TREE_CODE(mallocStmt_tree) != FUNCTION_DECL && TREE_CODE((callerRetTypearray)[k].return_tree) != FUNCTION_DECL)
							if (TREE_CODE((callerRetTypearray)[k].return_tree) != INTEGER_CST)
							{
								// fprintf(stderr, "\033[40;31m  find return stmt  \033[0m\n");
								// // debug_tree(mallocStmt_tree);
								// debug_gimple_stmt((callerRetTypearray)[k].stmt);
								// debug_points_to_info_for((callerRetTypearray)[k].return_tree);
								struct ptr_info_def *pi;
								pi = SSA_NAME_PTR_INFO(mallocStmt_tree);

								struct pt_solution *pt = &pi->pt;
								if (pi)
									if (ptr_derefs_may_alias_p((callerRetTypearray)[k].return_tree, mallocStmt_tree))
									{
										fprintf(stderr, "\033[40;31m  find return stmt  \033[0m\n");
										// debug_tree(mallocStmt_tree);
										// debug_tree((callerRetTypearray)[k].return_tree);
										// debug_points_to_info_for(mallocStmt_tree);
										// debug_points_to_info_for((callerRetTypearray)[k].return_tree);
										debug_gimple_stmt((callerRetTypearray)[k].stmt);
										warning_at(gimple_location((callerRetTypearray)[k].stmt), 0, "use location");
										if (retmod && debugmod)
										{
											// if (fistconunt == 0)
											// {
											fprintf(stderr, "dot graph start relate form ");
											// 	fistconunt++;
											// }
											// else
											// 	fprintf(stderr, "dot graph start relate for1 ");
											fprintf(stderr, "ID : %lu\n", now_fucntion);
											fprintf(stderr, "from %s basic block %d", (char *)get_name(function_tree), gimple_bb((callerRetTypearray)[k].stmt)->index);
											fprintf(stderr, "dot graph end relate end\n\n");
											//ready add dot graph
											// unsigned long x = rand();
											now_laststmt = (callerRetTypearray)[k].stmt;
											//ready add dot graph
											unsigned long x = rand();
											now_laststmtid = x;
											//ready add dot graph
											fprintf(stderr, "dot graph relate stmt start ID : %lu stmt(return) :", x);
											debug((callerRetTypearray)[k].stmt);
											warning_at(gimple_location((callerRetTypearray)[k].stmt), 0, "use location");
											// debug(gimple_assign_lhs((callerRetTypearray)[k].stmt));
											fprintf(stderr, "dot graph relate end\n\n");
										}
									}
							}
					}
					// ++(*freecount);

					// 	++(*freecount);

					// 	// debug_tree((callerRetTypearray)[k].free_tree);
					// }
					// debug_tree((callerRetTypearray)[k].free_tree);
					// debug_tree (mallocStmt_tree);
				}
			}
		}
	}
	if (function_pthread_detched_collect->get(function_tree) != NULL && fucntion_level == -1 && fucntion_level != -100)
	{
		function_pthread_detched_array callerFunArray = *(function_pthread_detched_collect->get(function_tree));
		vector<pthread_detched_type> callerRetTypearray = callerFunArray.pthread_detched_array;
		for (int k = 0; k < callerRetTypearray.size(); k++)
		{
			// if (ptr_derefs_may_alias_p((callerRetTypearray)[k].free_tree, mallocStmt_tree))
			// {
			fprintf(stderr, "\033[40;31m  find pthread_detched stmt  \033[0m\n");
			// fprintf(stderr, "find free stmt free same pointer\n");
			debug_gimple_stmt((callerRetTypearray)[k].stmt);
			warning_at(gimple_location((callerRetTypearray)[k].stmt), 0, "use location");
			*freecount = *freecount + 1;
			if (pthread_detachedmod)
			{
				fprintf(stderr, "dot graph start relate form ");

				fprintf(stderr, "ID : %lu\n", now_fucntion);
				fprintf(stderr, "from %s basic block %d", (char *)get_name(function_tree), gimple_bb((callerRetTypearray)[k].stmt)->index);
				fprintf(stderr, "dot graph end relate end\n\n");
				//ready add dot graph
				unsigned long x = rand();

				//ready add dot graph
				fprintf(stderr, "dot graph relate stmt start ID : %lu stmt(pthread_detched)) :", x);
				debug((callerRetTypearray)[k].stmt);
				warning_at(gimple_location((callerRetTypearray)[k].stmt), 0, "use location");
				// debug(gimple_assign_lhs((callerRetTypearray)[k].stmt));
				fprintf(stderr, "dot graph relate end\n\n");
			}
			// (*freecount)+=1;
			// 		fprintf(stderr, "\033[40;31m  wqeeeeeeeeeee %d  \033[0m\n",*freecount);
			// 	++(*freecount);

			// 	// debug_tree((callerRetTypearray)[k].free_tree);
			// }
			// debug_tree((callerRetTypearray)[k].free_tree);
			// debug_tree (mallocStmt_tree);
		}
	}

	if (function_pthread_exit_collect->get(function_tree) != NULL && fucntion_level == -1 && fucntion_level != -100)
	{
		function_pthread_exit_array callerFunArray = *(function_pthread_exit_collect->get(function_tree));
		vector<pthread_exit_type> callerRetTypearray = callerFunArray.pthread_exit_array;
		for (int k = 0; k < callerRetTypearray.size(); k++)
		{
			// if (ptr_derefs_may_alias_p((callerRetTypearray)[k].free_tree, mallocStmt_tree))
			// {
			fprintf(stderr, "\033[40;31m  find pthread_exit stmt  \033[0m\n");
			// fprintf(stderr, "find free stmt free same pointer\n");
			debug_gimple_stmt((callerRetTypearray)[k].stmt);
			warning_at(gimple_location((callerRetTypearray)[k].stmt), 0, "use location");
			if (pthread_exitmod)
			{
				fprintf(stderr, "dot graph start relate form ");

				fprintf(stderr, "ID : %lu\n", now_fucntion);
				fprintf(stderr, "from %s basic block %d", (char *)get_name(function_tree), gimple_bb((callerRetTypearray)[k].stmt)->index);
				fprintf(stderr, "dot graph end relate end\n\n");
				//ready add dot graph
				unsigned long x = rand();

				//ready add dot graph
				fprintf(stderr, "dot graph relate stmt start ID : %lu stmt(pthread_exit) :", x);
				debug((callerRetTypearray)[k].stmt);
				warning_at(gimple_location((callerRetTypearray)[k].stmt), 0, "use location");
				// debug(gimple_assign_lhs((callerRetTypearray)[k].stmt));
				fprintf(stderr, "dot graph relate end\n\n");
			}
			// ++(*freecount);

			// 	++(*freecount);

			// 	// debug_tree((callerRetTypearray)[k].free_tree);
			// }
			// debug_tree((callerRetTypearray)[k].free_tree);
			// debug_tree (mallocStmt_tree);
		}
	}
	// if (mallocStmt_tree != NULL_TREE)
	// 	if (function_return_collect->get(function_tree) != NULL && fucntion_level == -1)
	// 	{
	// 		// function_pthread_exit_array callerFunArray = *(function_pthread_exit_collect->get(function_tree));
	// 		// vector<pthread_exit_type> callerRetTypearray = callerFunArray.pthread_exit_array;
	// 		function_return_array callerFunArray = *(function_return_collect->get(function_tree));

	// 		vector<return_type> callerRetTypearray = callerFunArray.return_type_array;
	// 		for (int k = 0; k < callerRetTypearray.size(); k++)
	// 		{
	// 			// if (ptr_derefs_may_alias_p((callerRetTypearray)[k].free_tree, mallocStmt_tree))
	// 			// {
	// 			// fprintf(stderr, "\033[40;31m  find return stmt  \033[0m\n");
	// 			// fprintf(stderr, "find free stmt free same pointer\n");
	// 			// debug_tree((callerRetTypearray)[k].return_tree);
	// 			// debug_gimple_stmt((callerRetTypearray)[k].return_tree);
	// 			// tree get_function_return_tree = gimple_return_retval(as_a<greturn *>((callerRetTypearray)[k].stmt));
	// 			// if(gimple_return_retval(as_a<greturn *>((callerRetTypearray)[k].stmt)))

	// 			// if((callerRetTypearray)[k].return_tree )
	// 			// fprintf(stderr ,"%s\n", get_tree_code_name(TREE_CODE((callerRetTypearray)[k].return_tree)));
	// 			if ((callerRetTypearray)[k].return_tree)
	// 			{

	// 				if (TREE_CODE((callerRetTypearray)[k].return_tree) != INTEGER_CST)
	// 				{
	// 						fprintf(stderr, "\033[40;31m  find return stmt  \033[0m\n");
	// 						debug_tree(mallocStmt_tree);
	// 					// debug_gimple_stmt((callerRetTypearray)[k].stmt);
	// 					// debug_points_to_info_for((callerRetTypearray)[k].return_tree);
	// 					struct ptr_info_def *pi;
	// 					pi = SSA_NAME_PTR_INFO((callerRetTypearray)[k].return_tree);

	// 					struct pt_solution *pt = &pi->pt;
	// 					if (pi)
	// 						if (ptr_derefs_may_alias_p((callerRetTypearray)[k].return_tree, mallocStmt_tree))
	// 						{
	// 							fprintf(stderr, "\033[40;31m  find return stmt  \033[0m\n");
	// 							warning_at(gimple_location((callerRetTypearray)[k].stmt), 0, "use location");
	// 							if (retmod)
	// 							{
	// 								fprintf(stderr, "dot graph start relate form ");

	// 								fprintf(stderr, "ID : %lu\n", now_fucntion);
	// 								fprintf(stderr, "from %s basic block %d", (char *)get_name(function_tree), gimple_bb((callerRetTypearray)[k].stmt)->index);
	// 								fprintf(stderr, "dot graph end relate end\n\n");
	// 								//ready add dot graph
	// 								unsigned long x = rand();

	// 								//ready add dot graph
	// 								fprintf(stderr, "dot graph relate stmt start ID : %lu stmt(return) :", x);
	// 								debug((callerRetTypearray)[k].stmt);
	// 								warning_at(gimple_location((callerRetTypearray)[k].stmt), 0, "use location");
	// 								// debug(gimple_assign_lhs((callerRetTypearray)[k].stmt));
	// 								fprintf(stderr, "dot graph relate end\n\n");
	// 							}
	// 						}
	// 				}
	// 			}
	// 			// ++(*freecount);

	// 			// 	++(*freecount);

	// 			// 	// debug_tree((callerRetTypearray)[k].free_tree);
	// 			// }
	// 			// debug_tree((callerRetTypearray)[k].free_tree);
	// 			// debug_tree (mallocStmt_tree);
	// 		}
	// 	}
	//避開 pthread
	if (tracerelatestmt == true && fucntion_level != -1 && fucntion_level != -100)
	{
		struct cgraph_node *node;
		if (mallocStmt_tree != NULL)
		{

			// deubg_tree(mallocStmt_tree);
			struct ptr_info_def *pi1, *pi2, *pi3;
			pi1 = SSA_NAME_PTR_INFO(mallocStmt_tree);
			struct pt_solution *pt1 = &pi1->pt;
			// fprintf(stderr, "tx%u\n", pt1->null);

			if (pt1 && pt1->null)
				trace_fucntion_relate_stmt(node, function_tree, mallocStmt_tree);
		}
	}
	if (fucntion_level != -1 && fucntion_level != -100)
		fucntion_level += 1;

	// checkPointerConstraint(function_tree,ptable,user_tmp);

	// fprintf(stderr, "\033[40;44m =======print_function_path %s  function_call count: %d level :%d========  \033[0m\n", get_name(function_tree), function_path_array.size(), fucntion_level);

	// pathStack.push(function_tree);
	if (function_path_array.size() == 0)
	{
		if (function_return_collect->get(function_tree) != NULL)
		{
			function_return_array callerFunArray = *(function_return_collect->get(function_tree));
			if (function_return_collect->get(function_tree) != NULL && callerFunArray.return_type_num == 2)
				fprintf(stderr, "%s this is fucntion return value is heap-object \n", get_name(function_tree));
		}
	}

	for (int i = 0; i < function_path_array.size(); i++)
	{

		int find = 0;
		fprintf(stderr, "\033[40;42m =======pre add _ fucntion:%s========= \033[0m\n", get_name((function_path_array)[i].next));

		for (int o = 0; o < traceStack.size(); o++)
		{
			if (traceStack.c[o] == (function_path_array)[i].next)
			{
				find = 1;
				fprintf(stderr, "\033[40;41m =======recursive_fun2:%s========= \033[0m\n", get_name(traceStack.c[o]));
				break;
				//	fprintf(stderr, "				=======recursive_fun:%s=========\n", get_name(traceStack.c[o]));
			}
		}
		// fprintf(stderr, "%s this is fucntion return value is heap-object \n", get_name((function_path_array)[i].next));
		if (function_return_collect->get((function_path_array)[i].next) != NULL)
		{
			function_return_array callerFunArray = *(function_return_collect->get(function_tree));

			vector<return_type> callerRetTypearray = callerFunArray.return_type_array;
			function_return_array calleeFunArray = *(function_return_collect->get((function_path_array)[i].next));

			// fprintf(stderr, "\033[40;44m =======print_function_type2 %d  ========  \033[0m\n", calleeFunArray.return_type_num);

			//thread lock set
			function_return_array find_fun_array;
			vector<return_type> ret_type_find_fun_array;
			tree getFucntionDecl = ((function_path_array)[i].next);

			if (function_return_collect->get(getFucntionDecl) == NULL)
			{
				find_fun_array.return_type_array = ret_type_find_fun_array;
			}
			else
			{
				find_fun_array = *(function_return_collect->get(getFucntionDecl));
			}
			// vector<return_type> funcalleeRetTypearray = find_fun_array.return_type_array;
			// fprintf(stderr, "\033[40;44m =======print_function_typ123123 %d  ========  \033[0m\n", calleeFunArray.return_type_num);
			for (int k = 0; k < callerRetTypearray.size(); k++)
			{
				// fprintf(stderr, "\033[40;44m =======print_function_path %s  function_call count: %d level :%d========  \033[0m\n", get_name(function_tree), function_path_array.size(), fucntion_level);

				if (calleeFunArray.return_type_num == 2)
					if ((callerRetTypearray)[k].return_tree == (function_path_array)[i].next)
					{
						fprintf(stderr, "%s this is fucntion return value is heap-object \n", get_name((function_path_array)[i].next));
						fprintf(stderr, "%s this is fucntion return value is heap-object %s\n", get_name(function_tree), get_name((callerRetTypearray)[k].return_tree));
						if (fucntion_level == -100)
							(*freecount) = 100;
						// break;
					}
			}
		}
		if (find == 0)
		{
			fprintf(stderr, "\033[40;46m =======add node_fun stack:%s========= \033[0m\n", get_name((function_path_array)[i].next));

			//fprintf(stderr, "=======add node_fun trace stack:%s=========\n", get_name((function_path_array)[i].next));
			// debug_tree((function_path_array)[i].next);
			traceStack.push((function_path_array)[i].next);
			// 	function_return_array callerFunArray = *(function_return_collect->get(function_tree));

			// vector<return_type> callerRetTypearray = callerFunArray.return_type_array;

			trace_function_path((function_path_array)[i].next, fucntion_level, mallocStmt_tree, freecount);
			traceStack.pop();
		}
	}

	// fprintf(stderr, "\033[40;31m  find free stmt free same pointer count:%d \033[0m\n", freecount);
	// return freecount;
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
		{
			pop_cfun();
			continue;
		}
		enum availability avail;

		fprintf(stderr, "fucntion collect path \n");
		pathStack.push(cfun->decl);

		walk_function_path(cfun->decl, fucntion_level, ptable, user_tmp);

		pathStack.pop();
		pop_cfun();
	}
	fprintf(stderr, "fucntion collect path finsh\n");
}

void walk_function_path(tree function_tree, int fucntion_level, ptb *ptable, gimple_array *user_tmp)
{
	// debug_tree(function_tree);
	// fprintf(stderr, "\033[40;33m =======POP 222ode_fun stack:%s========= \033[0m\n", get_name(function_tree));
	// debug_tree(function_tree);
	if (function_path_collect->get(function_tree) == NULL)
		return;
	// fprintf(stderr, "\033[40;33m =======POP 224ode_fun stack:%s========= \033[0m\n", get_name(function_tree));
	function_path_array fun_array = *(function_path_collect->get(function_tree));

	vector<function_path> function_path_array = fun_array.function_path_array;
	function_return_array callerFunArray;
	vector<return_type> callerRetTypearray;
	//debug_tree(function_tree);
	//vector<pair<fdecl,location_t>> loc;
	fprintf(stderr, "\033[40;44m =======print_function_path %s  function_call count: %d level :%d========  \033[0m\n", get_name(function_tree), function_path_array.size(), fucntion_level);
	if (function_return_collect->get(function_tree) != NULL)
	{

		callerFunArray = *(function_return_collect->get(function_tree));
		callerRetTypearray = callerFunArray.return_type_array;
		fprintf(stderr, "\033[40;44m =======print_function_type %d  ========  \033[0m\n", callerFunArray.return_type_num);
		// fprintf(stderr, "rrrrrr%d-------\n", callerFunArray.pthread_type_num );
		if (callerFunArray.pthread_type_num == FUNCITON_THREAD)
			fprintf(stderr, "\033[40;44m =======print_pthread_type_is thread_fucntion  ========  \033[0m\n");
	}

	fucntion_level += 1;

	int find = 0;
	int find_thread = 0;
	// if( function_path_array.size())
	// checkPointerConstraint((function_path_array)[i].next, ptable, user_tmp, NULL, 0);
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
			// fprintf(stderr, "\033[40;46m =======add node_fun stack2:%s========= \033[0m\n", get_name((function_path_array)[i].next));

			pathStack.push((function_path_array)[i].next);
			// checkPointerConstraint((function_path_array)[i].next, ptable, user_tmp, NULL, 0);

			// int find_type=0;

			if (function_return_collect->get((function_path_array)[i].next) != NULL)
			{
				function_return_array calleeFunArray = *(function_return_collect->get((function_path_array)[i].next));

				fprintf(stderr, "\033[40;44m =======print_function_type %d  ========  \033[0m\n", calleeFunArray.return_type_num);

				//thread lock set
				function_return_array find_fun_array;
				vector<return_type> ret_type_find_fun_array;
				tree getFucntionDecl = ((function_path_array)[i].next);

				if (function_return_collect->get(getFucntionDecl) == NULL)
				{
					find_fun_array.return_type_array = ret_type_find_fun_array;
				}
				else
				{
					find_fun_array = *(function_return_collect->get(getFucntionDecl));
				}
				vector<return_type> funcalleeRetTypearray = find_fun_array.return_type_array;

				for (int k = 0; k < callerRetTypearray.size(); k++)
				{
					if ((callerRetTypearray)[k].return_type_stmt_num == FUNCITON_THREAD)
					{
						int find_diff = 0;
						for (int k2 = 0; k2 < funcalleeRetTypearray.size(); k2++)
						{
							if ((callerRetTypearray)[k].return_tree == (funcalleeRetTypearray)[k2].return_tree)
								find_diff = 1;
						}
						if (find_diff == 0)
						{
							// fprintf(stderr, "\033[40;44m ======= print pthread lock stmt %d ========  \033[0m\n" ,(callerRetTypearray)[k].return_type_stmt_num);
							// debug_gimple_stmt((callerRetTypearray)[k].stmt);
							// debug_tree((callerRetTypearray)[k].return_tree);
							struct return_type ret_type;
							ret_type.stmt = (callerRetTypearray)[k].stmt;
							ret_type.return_tree = (callerRetTypearray)[k].return_tree;
							ret_type.return_type_stmt_num = FUNCITON_THREAD;

							find_fun_array.return_type_array.push_back(ret_type);
							function_return_collect->put(getFucntionDecl, find_fun_array);
							find_thread = FUNCITON_THREAD;

							//fprintf(stderr, "\033[40;44m ============================================  \033[0m\n" ,(callerRetTypearray)[k].return_type_stmt_num);
						}
					}

					// if (calleeFunArray.return_type_num == 2)
					// 	if ((callerRetTypearray)[k].return_tree == (function_path_array)[i].next)
					// 	{
					// 		checkPointerConstraint(function_tree, ptable, user_tmp, (callerRetTypearray)[k].return_tree, FUNCITON_HEAP);
					// 		// break;
					// 	}
				}

				// fprintf(stderr, "=======print_function_type %d  ========\n", calleeFunArray.return_type_num);

				// for (int k = 0; k < callerRetTypearray.size(); k++)
				// {

				// 	if (calleeFunArray.return_type_num == 2)
				// 		if ((callerRetTypearray)[k].return_tree == (function_path_array)[i].next)
				// 		{
				// 			// fprintf(stderr, "asdddddddd");
				// 			// debug_tree((function_path_array)[i].next);
				// 			checkPointerConstraint(function_tree, ptable, user_tmp, (callerRetTypearray)[k].return_tree, FUNCITON_HEAP);

				// 			// break;
				// 		}
				// }
			}
			walk_function_path((function_path_array)[i].next, fucntion_level, ptable, user_tmp);
			fprintf(stderr, "\033[40;33m =======POP node_fun stack:%s========= \033[0m\n", get_name(pathStack.top()));
			pathStack.pop();
		}
	}
	// fprintf(stderr, "\033[40;33m =======POP 222ode_fun stack:%s========= \033[0m\n", get_name(pathStack.top()));
	if (find_thread == FUNCITON_THREAD)
	{
		checkPointerConstraint(function_tree, ptable, user_tmp, NULL, FUNCITON_THREAD);
	}
	else
	{
		// debug_tree(function_tree);

		checkPointerConstraint(function_tree, ptable, user_tmp, NULL, 0);
	}
	// if (check == 0)
	//

	// fprintf(stderr, "\033[40;44m =======print_function_path %s  function_call count: %d level :%d========  \033[0m\n", get_name(function_tree), function_path_array.size(), fucntion_level);
}

void dump_fucntion(cgraph_node *node, ptb *ptable, gimple_array *user_tmp)
{

	cgraph_edge *e;
	if (node == NULL)

		fprintf(stderr, "=======node_fun: =========\n");

	FOR_EACH_DEFINED_FUNCTION(node)
	{
		if (!gimple_has_body_p(node->decl))
			continue;
		int fucntion_level = 0;

		push_cfun(node->get_fun());

		// fprintf(stderr, "=======node_fu222n:=========\n");
		// debug_tree(node->get_fun()->decl);
		if (node->get_fun()->decl == NULL)
		{
			pop_cfun();
			continue;
		}
		//mutlple entry point
		// fprintf(stderr, "filter\n\n");
		if (!strcmp(get_name(cfun->decl), "main"))
		{
			fprintf(stderr, "\033[40;44m =======node_fun:%s========= \033[0m\n", get_name(cfun->decl));
			// fprintf(stderr, "=======node_fun:%s=========\n", get_name(cfun->decl));

			fprintf(stderr, "\033[40;44m fucntion collect path  \033[0m\n");
			// fprintf(stderr, "fucntion collect path \n");
			pathStack.push(cfun->decl);
			// debug_tree(node->get_fun()->decl );
			walk_function_path(cfun->decl, fucntion_level, ptable, user_tmp);
			fprintf(stderr, "\033[40;33m =======POP node_fun stack:%s========= \033[0m\n", get_name(pathStack.top()));
			pathStack.pop();
		}
		pop_cfun();
	}
	fprintf(stderr, "fucntion collect path finsh\n");
}
