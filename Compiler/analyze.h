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
		traceStack.pop_back();
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
				warning_at(gimple_location_safe((funcalleeRetTypearray)[k].stmt), 0, "use location");
				find_thread_count++;
				// debug_tree((funcalleeRetTypearray)[k].return_tree);
			}
		}
		fprintf(stderr, "\033[40;42m =======thread job funciton lock count :%d========= \033[0m\n", find_thread_count);
	}
	fprintf(stderr, "\033[40;42m =======start_check_funciton:%s========= \033[0m\n", get_name(function_tree));

	FOR_EACH_TABLE(table_temp, t)
	{
		int find_Looserulesfreestmt = 0;
		int ptable_type = 0;
		int childptable_type = 0;
		int find_phistmt = 0;
		int find_freestmt = 0;
		int find_retheapstmt = -2;
		int find_pthread_detched = 0;
		int find_pthread_join = 0;
		int is_pthread_detched = 0;
		int find_mallocstmt = 0;
		int init_check_table_stmt = 0;

		// debug_tree(table_temp->target);
		// debug_tree(table_temp->node->get_fun()->decl);
		// fprintf(stderr, "\n ------------------------------------------\n");
		// debug_tree(function_tree);
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
				// debug_tree(table_temp->target);
				vector<free_type> free_array;
				find_freestmt = find_mallocstmt = find_phistmt = 0;
				// debug_tree(function_tree);

				fprintf(stderr, "\ndot graph START\n");

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
				// fprintf(stderr ,"dot graph target %s\n", (char *)get_name(table_temp->target));
				//  //ready add dot graph
				//  fprintf(stderr, "dot graph target loc start ");
				//  warning_at(gimple_location_safe(table_temp->last_stmt), 0, "use location");
				//  fprintf(stderr, "dot graph target loc end\n\n");
				//  //ready add dot graph

				// fprintf(stderr, "\n====qwd==================================================================\n");
				// if (TREE_CODE(table_temp->target) == INTEGER_CST)
				// 	continue;
				// if (!strcmp(get_tree_code_name(TREE_CODE(table_temp->target)), "addr_expr"))
				// 	continue;
				// if (!strcmp(get_tree_code_name(TREE_CODE(gimple_assign_rhs1(def_stmt))), "addr_expr"))
				// 	continue;
				// debug_tree(table_temp->target);
				const char *name = "";

				if (TREE_CODE(table_temp->target) == INTEGER_CST || TREE_CODE(table_temp->target) == STRING_CST)
				{
					fprintf(stderr, "\n ================== collect possiable invalid ================== \n");
					debug_tree(table_temp->target);
					continue;
				}
				// debug_tree(table_temp->target);
				gimple *def_stmt = SSA_NAME_DEF_STMT(table_temp->target);
				// debug_tree(table_temp->target);

				if (def_stmt)
					if (TREE_CODE(table_temp->target) != VAR_DECL)
					{
						// debug_gimple_stmt(def_stmt);
						if (def_stmt)
						{
							debug_tree(table_temp->target);
							// debug(def_stmt);
							if (TREE_CODE(table_temp->target) == FUNCTION_DECL)
								name = get_name(table_temp->target);
							if (TREE_CODE(table_temp->target) != ADDR_EXPR && TREE_CODE(table_temp->target))
								if (gimple_code(def_stmt))
								{
									debug_gimple_stmt(def_stmt);
									if (gimple_code(def_stmt) == GIMPLE_CALL)
										name = get_name(gimple_call_fndecl(def_stmt));
									// fprintf(stderr, "GIMPLE CODE :addr_expr---%s-----\n", name);
								}
						}
						// debug_gimple_stmt(def_stmt);
						fprintf(stderr, "\n ================== trace ptable================== \n");
						if (def_stmt)
							if (TREE_CODE(table_temp->target) == FUNCTION_DECL)
							{
								const char *name;
								name = get_name(table_temp->target);
								// debug_tree(gimple_call_fndecl(u_stmt));
								fprintf(stderr, "trace fucntion name:%s \n", name);
								trace_function_path(table_temp->target, RET_HEAP_OBJECT, table_temp->target, &find_retheapstmt);
								if (find_retheapstmt > 0)
									fprintf(stderr, "some fucntion return value is heap-object and with Collection SSA_NAME alias relation\n");
							}
							else if (TREE_CODE(table_temp->target) != ADDR_EXPR && TREE_CODE(table_temp->target) != PARM_DECL)
							{
								if (is_gimple_call(def_stmt))
									if (gimple_call_fn(def_stmt))
									{
										const char *name;
										name = get_name(gimple_call_fn(def_stmt));
										fprintf(stderr, "trace fucntion name:%s \n", name);

										// debug_tree(gimple_call_fndecl(u_stmt));
										trace_function_path(gimple_call_fndecl(def_stmt), RET_HEAP_OBJECT, table_temp->target, &find_retheapstmt);
										if (find_retheapstmt > 0)
											fprintf(stderr, "some fucntion return value is heap-object and with Collection SSA_NAME alias relation\n");

										// while (traceStack.size())
										// {
										// 	// fprintf(stderr, "check stmt\n");
										// 	// debug(stmtStack.top());
										// 	traceStack.pop();
										// }
									}
							}
					}
				fprintf(stderr, "\n ================== trace ptable ================== \n");
				// if (gimple_code(def_stmt) == GIMPLE_NOP)
				// 	continue;
				// debug(def_stmt);
				// if(def_stmt == NULL)
				// debug(def_stmt);
				// debug_tree(gimple_assign_rhs1(def_stmt));
				// debug(def_stmt);
				fprintf(stderr, "GIMPLE CODE :addr_expr---%s-----\n", name);
				// if(def_stmt != NULL)
				// debug_tree(table_temp->target);
				// warning_at(gimple_location_safe(def_stmt), 0, "use location");

				// filter fucntion name preprocess
				if (name != NULL)

					if (!strcmp(name, "realloc") ||
						!strcmp(name, "malloc") ||
						!strcmp(name, "xmalloc") ||
						!strcmp(name, "calloc") ||
						!strcmp(name, "xcalloc") ||
						!strcmp(name, "strdup") ||
						!strcmp(name, "xstrdup"))

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
						// if(find_retheapstmt <0)
						// 	continue;
					}

				// fprintf(stderr, "\n======================================================================\n");
				user_tmp = treeGimpleArray->get(table_temp->target);
				// debug_tree(table_temp->target);
				// fprintf(stderr, "\n======================================================================\n");
				vector<basic_block> earlyend;
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

							// walk return table input fucntion_tree
							push_cfun(table_temp->node->get_fun());
							calculate_dominance_info(CDI_DOMINATORS);
							for (int k = 0; k < global_ret_type_array.size(); k++)
							{

								// fprintf(stderr ,"\global_ret_type_array index %d\n" , gimple_bb((global_ret_type_array)[k].stmt)->index);
								// fprintf(stderr, "\ntest %d\n", global_ret_type_array.size());
								// warning_at(gimple_location_safe((global_ret_type_array)[k].stmt), 0, "use location");
								// debug_gimple_stmt(u_stmt);
								// debug_gimple_stmt((callerRetTypearray)[k].stmt);
								// debug(findedge);
								basic_block bb;
								FOR_EACH_BB_FN(bb, table_temp->node->get_fun())
								{

									edge e;
									edge_iterator ei;
									FOR_EACH_EDGE(e, ei, bb->succs)
									{
										name = "";

										if (!gimple_call_lhs((global_ret_type_array)[k].stmt))
										{
											if (gimple_call_fn((global_ret_type_array)[k].stmt))
												name = get_name(gimple_call_fn((global_ret_type_array)[k].stmt));
										}
										// else
										// name = "";
										// debug_tree(gimple_call_lhs((callerRetTypearray)[k].stmt) );
										// fprintf(stderr, "\033[40;31m    branch possiable hawwwwve return   \033[0m\n");
										// if (gimple_code((callerRetTypearray)[k].stmt) == GIMPLE_RETURN)
										// debug_gimple_stmt((callerRetTypearray)[k].stmt);
										// fprintf(stderr, "bb index:= %s\n", name);
										if (name || gimple_code((global_ret_type_array)[k].stmt) == GIMPLE_RETURN)
										{
											int check = 0;
											if (name)
											{
												if (!strcmp(name, "exit"))
													check = 1;
											}
											if (gimple_code((global_ret_type_array)[k].stmt) == GIMPLE_RETURN)
											{

												check = 1;
											}

											if (check == 1)
											{
												// debug_gimple_stmt((callerRetTypearray)[k].stmt);
												if ((bb->index == gimple_bb(u_stmt)->index || e->dest->index == gimple_bb(u_stmt)->index))
												// if ( ( bb->index == gimple_bb(table_temp->last_stmt)->index || e->dest->index == gimple_bb(table_temp->last_stmt)->index))
												{
													// debug_gimple_stmt(u_stmt);
													// debug_gimple_stmt((callerRetTypearray)[k].stmt);
													// fprintf(stderr, "\nnow %d\n", bb->index);
													// fprintf(stderr, "\ntest %d\n", gimple_bb(u_stmt)->index);
													// fprintf(stderr, "\ntest %d\n", gimple_bb((global_ret_type_array)[k].stmt)->index);
													// fprintf(stderr, "bb index:= %d\n", bb->index);
													// fprintf(stderr, " succs:= %d\n", e->dest->index);
													// fprintf(stderr, " succs2:= %d\n", e2->dest->index);
													edge e2;
													edge_iterator ei2;
													gimple *tmp = (global_ret_type_array)[k].stmt;
													FOR_EACH_EDGE(e2, ei2, bb->succs)
													{
														// fprintf(stderr, "bb index:= %d\n", bb->index);
														// if( e->dest->index != e2->dest->index ){
														// fprintf(stderr, " succs2:= %d\n", bb->index);
														// fprintf(stderr, " succs3:= %d\n", gimple_bb((callerRetTypearray)[k].stmt)->index);
														// fprintf(stderr, " succs4:= %d\n", e2->dest->index);

														// if (!gimple_bb(tmp) && !e2->dest)
														if ((e->dest->index != e2->dest->index))
															if (dominated_by_p(CDI_DOMINATORS, gimple_bb(tmp), e2->dest))

															{

																// debug_gimple_stmt((callerRetTypearray)[k].stmt);
																fprintf(stderr, "\n======================================================================\n");
																// fprintf(stderr, "	no free stmt possible memory leak\n");
																fprintf(stderr, "\033[40;31m    branch possiable have return or exit  \033[0m\n");

																debug_gimple_stmt(u_stmt);
																fprintf(stderr, "bb index := %d\n", gimple_bb(u_stmt)->index);
																debug_gimple_stmt(tmp);
																warning_at(gimple_location_safe(tmp), 0, "use location");
																fprintf(stderr, "beacuse in succ := %d have return or exit\n", gimple_bb(tmp)->index);
																debug_gimple_stmt((global_ret_type_array)[k].stmt);
																warning_at(gimple_location_safe((global_ret_type_array)[k].stmt), 0, "use location");
																fprintf(stderr, "gimple stmt in succ := %d ,possiable got to succ := %d\n", gimple_bb(u_stmt)->index, gimple_bb(tmp)->index);
																// debug_tree((callerRetTypearray)[k].return_tree);
																// debug_gimple_stmt(u_stmt);
																// debug_gimple_stmt((callerRetTypearray)[k].stmt);
																// warning_at(gimple_location((callerRetTypearray)[k].stmt), 0, "use location");
																// check_bbinfo(gimple_bb((callerRetTypearray)[k].stmt));
																fprintf(stderr, "\n======================================================================\n");
															}
													}
												}
											}
										}
									}
								}
							}
							pop_cfun();

							if (bb_in_loop_p(gimple_bb(u_stmt)))
							{
								// debug_gimple_stmt(u_stmt);
								// warning_at(gimple_location_safe(u_stmt), 0, "use location");
								fprintf(stderr, "\n======================================================================\n");
								// fprintf(stderr, "	no free stmt possible memory leak\n");
								fprintf(stderr, "\033[40;31m    collect Stmt in loop \033[0m\n");
								fprintf(stderr, "\n======================================================================\n");
								// fprintf(stderr, "=======is loop:%d=========\n", bb_in_loop_p(gimple_bb(def_stmt)));
							}

							// fprintf(stderr, "\n======================================================================\n");
							// debug_gimple_stmt(u_stmt);
							// check_bbinfo2(gimple_bb(u_stmt));
							if (gimple_code(table_temp->last_stmt) == GIMPLE_CALL)
							{

								if (gimple_code(u_stmt) == GIMPLE_CALL)
									if (gimple_call_fn(table_temp->last_stmt) != NULL)
										if (gimple_call_fn(u_stmt) != NULL)
										{

											// debug_tree(gimple_call_fn(table_temp->last_stmt));
											// debug_tree(gimple_call_fn(u_stmt));
											if (gimple_call_fn(table_temp->last_stmt) == gimple_call_fn(u_stmt))
											{
												if (u_stmt == table_temp->last_stmt)
												{

													debug_gimple_stmt(u_stmt);
												}
											}
											else
											{
												name = get_name(gimple_call_fn(u_stmt));
												if (name != NULL)
												{
													if (!strcmp(name, get_name(gimple_call_fn(table_temp->last_stmt))))
													{
														fprintf(stderr, "\n Expand conflict \n", name);
														fprintf(stderr, " The same function may have multiple branches :%s\n", name);
														debug_gimple_stmt(table_temp->last_stmt);
														warning_at(gimple_location_safe(table_temp->last_stmt), 0, "use location");

														debug_gimple_stmt(u_stmt);
														warning_at(gimple_location_safe(u_stmt), 0, "use location");
														// fprintf(stderr, "\n Expand conflict %d \n", gimple_bb(u_stmt)->index);

														// check_bbinfo(gimple_bb(u_stmt));
														check_bbinfo(table_temp->node,gimple_bb(u_stmt));
															// fprintf(stderr, "succs:= %d\n", gimple_bb(u_stmt)->index);

														while (new_path_array.size())
														{

															new_path_array.pop_back();
														}
														continue;
													}
												}
											}
										}
							}

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
									warning_at(gimple_location_safe(user_tmp->ret_stmt), 0, "use location");

									debug_gimple_stmt(user_tmp->ret_stmt);
									now_stmt = user_tmp->ret_stmt;
									if (debugmod)
									{
										// ready add dot graph
										fprintf(stderr, "dot graph target loc start ");
										debug_gimple_stmt(user_tmp->ret_stmt);
										warning_at(gimple_location_safe(user_tmp->ret_stmt), 0, "use location");
										fprintf(stderr, "dot graph target loc en1\n\n");

										fprintf(stderr, "dot graph target line desc form basic block %d: ", gimple_bb(user_tmp->ret_stmt)->index);
										debug_gimple_stmt(user_tmp->ret_stmt);
										warning_at(gimple_location_safe(user_tmp->ret_stmt), 0, "use location");
										fprintf(stderr, "dot graph target line desend\n\n");

										fprintf(stderr, "dot graph target loc start ");
										debug_gimple_stmt(user_tmp->ret_stmt);
										warning_at(gimple_location_safe(user_tmp->ret_stmt), 0, "use location");
										fprintf(stderr, "dot graph target loc end\n\n");

										fprintf(stderr, "dot graph target basicblock start ");
										fprintf(stderr, "from %s basic block %d", (char *)get_name(function_tree), gimple_bb(user_tmp->ret_stmt)->index);
										fprintf(stderr, "dot graph target basicblock end\n\n");

										if (gimple_block(user_tmp->ret_stmt))
											if (TREE_CODE(BLOCK_SUPERCONTEXT(gimple_block(user_tmp->ret_stmt))) == FUNCTION_DECL && (function_tree != BLOCK_SUPERCONTEXT(gimple_block(user_tmp->ret_stmt))))
											{

												fprintf(stderr, "dot graph target basicblock star1 ");
												fprintf(stderr, "from %s basic block %d", (char *)get_name(BLOCK_SUPERCONTEXT(gimple_block(user_tmp->ret_stmt))), gimple_bb(user_tmp->ret_stmt)->index);
												fprintf(stderr, "dot graph target basicblock end\n\n");
												// lastfucntion = BLOCK_SUPERCONTEXT(gimple_block(user_tmp->ret_stmt));
												// debug(user_tmp->aptr);
											}
										fprintf(stderr, "dot graph stmt start ");
										debug(user_tmp->ret_stmt);
										warning_at(gimple_location_safe(user_tmp->ret_stmt), 0, "use location");
										fprintf(stderr, "dot graph stmt end\n\n");
										// ready add dot graph
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
									warning_at(gimple_location_safe(u_stmt), 0, "use location");
									now_stmt = u_stmt;
									// ready add dot graph
									if (debugmod)
									{

										fprintf(stderr, "dot graph target loc start ");
										debug_gimple_stmt(table_temp->last_stmt);
										warning_at(gimple_location_safe(table_temp->last_stmt), 0, "use location");
										fprintf(stderr, "dot graph target loc en1\n\n");

										fprintf(stderr, "dot graph target line desc form basic block %d: ", gimple_bb(u_stmt)->index);
										debug(u_stmt);
										warning_at(gimple_location_safe(u_stmt), 0, "use location");
										fprintf(stderr, "dot graph target line desend\n\n");

										fprintf(stderr, "dot graph target loc start ");
										debug_gimple_stmt(table_temp->last_stmt);
										warning_at(gimple_location_safe(table_temp->last_stmt), 0, "use location");
										fprintf(stderr, "dot graph target loc end\n\n");

										fprintf(stderr, "dot graph target basicblock start ");
										fprintf(stderr, "from %s basic block %d", (char *)get_name(function_tree), gimple_bb(u_stmt)->index);
										fprintf(stderr, "dot graph target basicblock en1\n\n");

										struct relate_type relate_type;

										relate_type.relate_funtree = function_tree;

										if (gimple_block(u_stmt))
											if (TREE_CODE(BLOCK_SUPERCONTEXT(gimple_block(u_stmt))) == FUNCTION_DECL && (function_tree != BLOCK_SUPERCONTEXT(gimple_block(u_stmt))))
											{

												fprintf(stderr, "dot graph target basicblock star1 ");
												fprintf(stderr, "from %s basic block %d", (char *)get_name(BLOCK_SUPERCONTEXT(gimple_block(u_stmt))), gimple_bb(u_stmt)->index);
												fprintf(stderr, "dot graph target basicblock end\n\n");

												relate_type.relate_funtree = BLOCK_SUPERCONTEXT(gimple_block(u_stmt));
											}
											else
											{

												relate_type.relate_funtree = function_tree;
											}

										relate_type.laststmt = table_temp->last_stmt;
										relate_type.stmt = u_stmt;
										relate_type.relate_tree = user_tmp->target;
										// relate_type.relate_funtree = function_tree;
										relate_type.now_basicblock = gimple_bb(u_stmt)->index;
										maxbb_array.relate_type_array.push_back(relate_type);
										function_maxbb_collect->put(table_temp->target, maxbb_array);

										if (gimple_code(u_stmt) == GIMPLE_CALL)
										{

											if (debugmod)
											{
												fprintf(stderr, "dot graph stmt start ");
												debug(finalstmt);
												debug(table_temp->last_stmt);
												// warning_at(gimple_location_safe(table_temp->last_stmt), 0, "use location");
												fprintf(stderr, "dot graph stmt end\n\n");
											}

											name = get_name(gimple_call_fn(u_stmt));
											if (name != NULL)
												if (!strcmp(name, "free") || !strcmp(name, "xfree") || !strcmp(name, "realloc"))
												{
													fprintf(stderr, "dot graph stmt start ");
													debug(u_stmt);
													warning_at(gimple_location_safe(u_stmt), 0, "use location");

													fprintf(stderr, "dot graph stmt end\n\n");

													if (freemod)
													{

														fprintf(stderr, "dot graph target color desc");
														if (!strcmp(name, "realloc"))
														{
															fprintf(stderr, "red");
														}
														else
															fprintf(stderr, "green");
														fprintf(stderr, "dot graph target color desend\n\n");
													}
													if (!strcmp(name, "realloc"))
													{
														fprintf(stderr, "If realloc() fails, the initial memory block will not be freed() ");
													}
												}
												else
												{

													fprintf(stderr, "dot graph stmt start ");
													debug(u_stmt);
													warning_at(gimple_location_safe(u_stmt), 0, "use location");

													fprintf(stderr, "dot graph stmt end\n\n");
												}
										}
										else
										{

											if (debugmod)
											{
												fprintf(stderr, "dot graph stmt start ");
												debug(u_stmt);
												debug(table_temp->last_stmt);

												fprintf(stderr, "dot graph stmt end\n\n");
											}
											fprintf(stderr, "dot graph stmt start ");
											debug(u_stmt);
											warning_at(gimple_location_safe(u_stmt), 0, "use location");
											fprintf(stderr, "dot graph stmt end\n\n");
										}
									}
								}

								if (user_tmp->target != NULL)
								{

									gimple *use_stmt = SSA_NAME_DEF_STMT(user_tmp->target);

									if (gimple_code(u_stmt) == GIMPLE_CALL)
									{

										if (gimple_call_fndecl(u_stmt))
										{
											name = get_name(gimple_call_fndecl(u_stmt));

											if (name != NULL)
												fprintf(stderr, "this stmt is child function---%s-----\n", name);
											else
												fprintf(stderr, "this other child function---i null-----\n", name);
										}
									}
									if (gimple_code(u_stmt) == GIMPLE_PHI)
									{
										find_phistmt = 1;
										fprintf(stderr, "this stmt have mutiple branch ---%s-----\n", name);
									}
									if (name != NULL)
										if (!strcmp(name, "realloc") ||
											!strcmp(name, "malloc") ||
											!strcmp(name, "xmalloc") ||
											!strcmp(name, "calloc") ||
											!strcmp(name, "xcalloc") ||
											!strcmp(name, "strdup") ||
											!strcmp(name, "xstrdup"))
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
											// debug_tree(gimple_assign_lhs(use_stmt));

											// global variable
											if (!TREE_STATIC(gimple_assign_lhs(finalstmt)) == true)
											{

												fprintf(stderr, " \n LOCAL VARIBALE  \n");
												debug_gimple_stmt(finalstmt);
												warning_at(gimple_location_safe(finalstmt), 0, "use location");
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
												warning_at(gimple_location_safe(finalstmt), 0, "use location");
												fprintf(stderr, "\033[40;35m thread job function \033[0m\n");
												fprintf(stderr, "\033[40;35m this stmt possible have Race Condition 。 \033[0m\n");
												fprintf(stderr, "\n ================== warring  ================== \n");
											}
										}
									}

									if (gimple_cond_code(u_stmt))
									{
										if (!is_gimple_assign(u_stmt))
											if (gimple_cond_lhs(u_stmt))
											{
												debug_gimple_stmt(u_stmt);
												fprintf(stderr, "\n ================== find cond branch ================= \n");
												// debug(u_stmt);
												warning_at(gimple_location_safe(u_stmt), 0, "use location");
												// fprintf(stderr, "\033[40;32m    HAS FREE STMT count:%d name:%s \033[0m\n", find_freestmt, name);

												//

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
																fprintf(stderr, "\n ================== possible direct basic block ================= \n");
																fprintf(stderr, "from %s basic block %d", (char *)get_name(table_temp->node->get_fun()->decl), e->dest->index);
																// fprintf(stderr, "succs:= %d\n", e->dest->index);
															}
															fprintf(stderr, "\n ================== warring ================== \n");

															// debug(checkTree);
															fprintf(stderr, "\033[40;35m    need check this branch possible have return or exit stmt \033[0m\n");
															// fprintf(stderr, "\033[40;35m    this stmt possible is heap-object 。 \033[0m\n");

															// fprintf(stderr, "this stmt possible is heap-object 。\n");
															fprintf(stderr, "\n ================== warring ================== \n");
														}
													}
												}
												fprintf(stderr, "\n ================== find cond branch ================= \n");
											}
									}

									if ((gimple_code(u_stmt) == GIMPLE_CALL) && (find_mallocstmt == IS_MALLOC_FUCNTION || find_mallocstmt == IS_OTHRER_FUCNTION))
									{
										// fprintf(stderr, "\n ================== find ================== \n");
										// debug(u_stmt);

										find_mallocstmt = IS_MALLOC_FUCNTION;

										// gimple *def_stmt = SSA_NAME_DEF_STMT(user_tmp->target);
										// fprintf(stderr, "NEWX FUCNTIONMWEQMEQWP: \n");
										gimple *def_stmt = SSA_NAME_DEF_STMT(user_tmp->target);

										const char *name;
										name = get_name(gimple_call_fn(u_stmt));

										if (name != NULL)
											if (!strcmp(name, "free") || !strcmp(name, "xfree") || !strcmp(name, "realloc"))
											{

												free_type.stmt = u_stmt;
												free_array.push_back(free_type);
												fprintf(stderr, "\n ================== find ================== \n");
												debug(u_stmt);
												warning_at(gimple_location_safe(u_stmt), 0, "use location");
												if (!strcmp(name, "realloc"))
												{
													find_freestmt++;
													fprintf(stderr, "\033[40;32m    FIND REALLOC STMT count:%d name:%s \033[0m\n", find_freestmt, name);
													fprintf(stderr, "\033[40;32m    this stmt possiable free memory \033[0m\n", find_freestmt, name);
												}
												else
												{
													find_freestmt++;
													fprintf(stderr, "\033[40;32m    HAS FREE STMT count:%d name:%s \033[0m\n", find_freestmt, name);
												}
												fprintf(stderr, "\n ================== find ================== \n");
											}
											else if (!strcmp(name, "pthread_create"))
											{

												table_temp->last_stmt = u_stmt;
												table_temp->swap_type = FUNCITON_THREAD;

												if (table_temp->swap_type == FUNCITON_THREAD)
												{
													fprintf(stderr, "\n ================== find ================== \n");
													debug(u_stmt);
													warning_at(gimple_location_safe(u_stmt), 0, "use location");
													fprintf(stderr, "\033[40;32m    FIND PTHREAD_CREATED STMT  \033[0m\n");
													// callerFunArray.pthread_type_num== 0?"CREATE_JOINABLE" : "CREATE_DETACHED");
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

													tree findtree = gimple_call_arg(table_temp->last_stmt, 3);

													if (is_gimple_call(u_stmt))
													{
														tree findtree;
														if (gimple_call_num_args(u_stmt) != 0)
														{
															findtree = gimple_call_arg(u_stmt, 3);
															if (TREE_CODE(findtree) == ADDR_EXPR)
															{

																trace_function_path(function_tree, -1, TREE_OPERAND(findtree, 0), &find_freestmt);
															}
															else if (TREE_CODE(findtree) == SSA_NAME)
															{

																trace_function_path(function_tree, -1, findtree, &find_freestmt);
															}
														}
													}

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
												if (name != NULL)
													if (!strcmp(name, "realloc") ||
														!strcmp(name, "malloc") ||
														!strcmp(name, "xmalloc") ||
														!strcmp(name, "calloc") ||
														!strcmp(name, "xcalloc") ||
														!strcmp(name, "strdup") ||
														!strcmp(name, "xstrdup"))
													{
													}
													else
													{
														find_mallocstmt = IS_MALLOC_FUCNTION;

														// debug_tree(table_temp->target);

														fprintf(stderr, "trace fucntion name:%s \n", name);
														trace_function_path(gimple_call_fndecl(u_stmt), 0, table_temp->target, &find_freestmt);
														fprintf(stderr, "\n ================== trace ================== \n");
														fprintf(stderr, "trace fucntion free:%d \n", find_freestmt);
														if (find_freestmt > freecount)
														{
															free_type.stmt = u_stmt;
															free_array.push_back(free_type);
														}
													}
											}
									}
								}
							}
							// fprintf(stderr, "\n========================================================================\n");
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

									if (maxbb_type_array[i].relate_funtree == maxbb_type_array[j].relate_funtree)
									{
										int max_bb;
										// debug_tree(it_i->relate_funtree);
										max_bb = *(function_maxbasicblock_collect)->get(maxbb_type_array[i].relate_funtree);
										if (maxbb_type_array[j].now_basicblock >= max_bb)
											function_maxbasicblock_collect->put(maxbb_type_array[j].relate_funtree, maxbb_type_array[j].now_basicblock);
									}
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
						// debug_tree(table_temp->target);
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
								// debug_gimple_stmt(table_temp->last_stmt);
								// warning_at(gimple_location_safe(table_temp->last_stmt), 0, "use location");
								// fprintf(stderr, "dot graph target loc end\n\n");
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
								warning_at(gimple_location_safe((relate_type_array)[k].stmt), 0, "use location");

								fprintf(stderr, "dot graph subgrapend\n\n");
							}
						}
					}

					fprintf(stderr, "\n ================== Start Looserules free Check ================== \n");
					fprintf(stderr, "\n ================== warring ================== \n");
					fprintf(stderr, "\033[40;35m   this module possiable false positives \033[0m\n");
					fprintf(stderr, "\n ================== warring ================== \n");
					if (Looserulesfree)
					{

						if (TREE_CODE(table_temp->target) == SSA_NAME || TREE_CODE(table_temp->target) == MEM_REF)
							if (table_temp->target != NULL_TREE)
							{
								debug_tree(table_temp->target);
								if (function_free_collect->get(function_tree) != NULL)
								{
									function_free_array callerFunArray = *(function_free_collect->get(function_tree));
									vector<free_type> callerRetTypearray = callerFunArray.free_type_array;

									for (int k = 0; k < callerRetTypearray.size(); k++)
									{
										// fprintf(stderr, "\n ================== free stmt ================== \n");
										// debug_gimple_stmt((callerRetTypearray)[k].stmt);
										if (ptr_derefs_may_alias_p((callerRetTypearray)[k].free_tree, table_temp->target))
										{

											if (is_gimple_call((callerRetTypearray)[k].stmt))
												name = get_name(gimple_call_fn((callerRetTypearray)[k].stmt));

											if (!strcmp(name, "realloc"))
											{
												// fprintf(stderr, "\033[40;35m  realloc Use after free error! \033[0m\n");
												fprintf(stderr, "\033[40;35m <Looserules> find realloc stmt free same pointer \033[0m\n");
												fprintf(stderr, "*/If realloc() fails, the initial memory block will not be freed() */");
											}
											else
												fprintf(stderr, "\033[40;35m <Looserules> find free stmt free same pointer \033[0m\n");

											debug_gimple_stmt((callerRetTypearray)[k].stmt);
											warning_at(gimple_location((callerRetTypearray)[k].stmt), 0, "use location");
											struct free_type free_type;
											free_type.stmt = (callerRetTypearray)[k].stmt;
											free_type.Looserulesfree = 1;
											free_array.push_back(free_type);
											find_Looserulesfreestmt++;
										}
									}
								}
							}
					}

					if (useafterfree)
					{
						user_tmp = treeGimpleArray->get(table_temp->target);
						// debug_tree(table_temp->target);

						gimple_array start;
						start.stmt = NULL;
						used_stmt = &start;

						fprintf(stderr, "\n ================== Start Use after free Check ================== \n");
						if (user_tmp->size > 0)
							FOR_EACH_USE_TABLE(user_tmp, u_stmt)
							{
								// debug_gimple_stmt(u_stmt);
								// debug_tree(user_tmp->aptr);
								// fprintf(stderr, "\n ================== warring ==%d================ \n",free_array.size());
								for (int i = 0; i < free_array.size(); i++)
								{

									if (gimple_call_fndecl(free_array.at(i).stmt))
										if (u_stmt != free_array.at(i).stmt)
										{
											if (is_gimple_call(u_stmt))
												if (gimple_call_fndecl(free_array.at(i).stmt) == gimple_call_fndecl(u_stmt))
													continue;
											if (Location_b2(free_array.at(i).stmt, u_stmt, function_tree))
											{
												fprintf(stderr, "\n============================================================\n");
												name = get_name(gimple_call_fn(free_array.at(i).stmt));
												if (free_array.at(i).Looserulesfree)
													fprintf(stderr, "\033[40;35m <Looserules> find free stmt free same pointer \033[0m\n");
												// free_type.Looserulesfree = 1;
												debug_gimple_stmt(free_array.at(i).stmt);
												warning_at(gimple_location_safe(free_array.at(i).stmt), 0, "Use after free error!: free location ");
												debug_gimple_stmt(u_stmt);
												warning_at(gimple_location_safe(u_stmt), 0, "use location");
												fprintf(stderr, "\n ================== warring ================== \n");

												if (!strcmp(name, "realloc"))
												{
													fprintf(stderr, "\033[40;35m  realloc Use after free error! \033[0m\n");
												}
												else
													fprintf(stderr, "\033[40;35m    Use after free error! \033[0m\n");

												// check_bbinfo(gimple_bb(free_array.at(i).stmt));
												check_bbinfo(table_temp->node,gimple_bb(free_array.at(i).stmt));
												// debug(checkTree);
												// fprintf(stderr, "\033[40;35m    Use after free error! \033[0m\n");
												// fprintf(stderr, "\033[40;35m    this stmt possible is heap-object 。 \033[0m\n");

												// fprintf(stderr, "this stmt possible is heap-object 。\n");
												fprintf(stderr, "\n ================== warring ================== \n");
											}
										}
								}
								// }
							}
						fprintf(stderr, "\ndot graph END\n");
						// fprintf(stderr, "\n ================== Start Use after free Check ================== \n");
					}
				}
				else
				{
					find_mallocstmt = PTABLE_IS_NULL;
				}
				// you are Reserved word function so always check
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
				if (Looserulesfree)
				{
					fprintf(stderr, "\n======================================================================\n");
					// fprintf(stderr, "	possible double free\n");

					fprintf(stderr, "\033[40;35m  	Looserules free count:%d \033[0m\n", find_Looserulesfreestmt);
					fprintf(stderr, "\033[40;31m  	free count:%d \033[0m\n", find_freestmt);
					if (find_Looserulesfreestmt >= 2)
					{
						fprintf(stderr, "\033[40;31m  	possible double free [Looserules]:%d \033[0m\n", find_Looserulesfreestmt);
					}
					if (find_freestmt >= 2)
					{
						fprintf(stderr, "\033[40;31m  	possible double free [rigorous]:%d \033[0m\n", find_freestmt);
					}
					fprintf(stderr, "\n======================================================================\n");
				}
				if ((find_mallocstmt == IS_MALLOC_FUCNTION))
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

						fprintf(stderr, "\033[40;31m  	possible double free [rigorous]:%d \033[0m\n", find_freestmt);
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
					// debug_gimple_stmt(table_temp->last_stmt);
					if (empty_body_p(table_temp->last_stmt))
						warning_at(gimple_location_safe(table_temp->last_stmt), 0, "use location");

					fprintf(stderr, "\n======================================================================\n");
				}
				else
				{

					fprintf(stderr, "\n======================================================================\n");
					// fprintf(stderr, "	this stmt need double check\n");

					fprintf(stderr, "\033[40;31m 	this stmt need double check \033[0m\n");
					if (find_retheapstmt > 0)
					{
						if (find_freestmt > 2)
						{
							fprintf(stderr, "\n======================================================================\n");
							// fprintf(stderr, "	possible double free\n");
							fprintf(stderr, "\033[40;31m    this function with other function alias that other function return value is heap-object \033[0m\n");
							fprintf(stderr, "\033[40;31m  	possible double free :%d \033[0m\n", find_freestmt);
							fprintf(stderr, "\n======================================================================\n");
						}
						else if (find_freestmt == 1)
						{

							fprintf(stderr, "\n======================================================================\n");
							fprintf(stderr, "\033[40;31m    this function with other function alias that other function return value is heap-object \033[0m\n");
							if (find_phistmt == 1)
								fprintf(stderr, "\033[40;31m   need check branch because multiple direction varible\033[0m\n");
							else
								fprintf(stderr, "\033[40;32m   Maybe you don't have memory leak.... need more checks  \033[0m\n");
							// 	fprintf(stderr, "\033[40;32m    NO memory leak \033[0m\n");
							fprintf(stderr, "\n======================================================================\n\n");
						}
						else if (find_freestmt == 0)
						{
							fprintf(stderr, "\n======================================================================\n");
							fprintf(stderr, "\033[40;31m    this function with other function alias that other function return value is heap-object \033[0m\n");
							fprintf(stderr, "\033[40;31m    no free stmt possible memory leak \033[0m\n");
							fprintf(stderr, "\n======================================================================\n");
						}
					}
					fprintf(stderr, "\n======================================================================\n");
				}
			}
	}
}