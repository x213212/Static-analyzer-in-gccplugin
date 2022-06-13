#include <set>

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
		// fprintf2(stderr, "check stmt\n");
		// debug2(stmtStack.top());
		traceStack.pop_back();
	}

	fprintf2(stderr, "\033[40;42m =======pre_check_funciton:%s========= \033[0m\n", get_name(function_tree));
	if (calleetype == FUNCITON_THREAD && threadmod == true)
	{
		function_return_array find_fun_array;

		fprintf2(stderr, "\033[40;42m =======thread job funciton:%s========= \033[0m\n", get_name(function_tree));
		find_fun_array = *(function_return_collect->get(function_tree));
		vector<return_type> funcalleeRetTypearray = find_fun_array.return_type_array;

		int find_thread = 0;
		int find_thread_count = 0;
		for (int k = 0; k < funcalleeRetTypearray.size(); k++)
		{
			if ((funcalleeRetTypearray)[k].return_type_stmt_num == FUNCITON_THREAD)
			{
				fprintf2(stderr, "\033[40;44m ======= print pthread lock stmt %d ========  \033[0m\n", (funcalleeRetTypearray)[k].return_type_stmt_num);
				debug_gimple_stmt2((funcalleeRetTypearray)[k].stmt);
				warning_at2(gimple_location_safe((funcalleeRetTypearray)[k].stmt), 0, "use location");
				find_thread_count++;
				// debug_tree2((funcalleeRetTypearray)[k].return_tree);
			}
		}
		fprintf2(stderr, "\033[40;42m =======thread job funciton lock count :%d========= \033[0m\n", find_thread_count);
	}
	fprintf2(stderr, "\033[40;42m =======start_check_funciton:%s========= \033[0m\n", get_name(function_tree));

	vector<defmalloc_type> defmalloc_array;

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

		if (table_temp->node->get_fun()->decl == function_tree)
			if (table_temp->target != NULL)
			{
				vector<free_type> free_array;
				find_freestmt = find_mallocstmt = find_phistmt = 0;

				// if (debugmod)
				fprintf2(stderr, "\ndot graph START\n");
				fprintf2(stderr, "\n======================================================================\n");

				const char *name = "";
				if (TREE_CODE(table_temp->target) == INTEGER_CST || TREE_CODE(table_temp->target) == STRING_CST)
				{
					fprintf2(stderr, "\n ================== collect possiable invalid ================== \n");
					debug_tree2(table_temp->target);
					continue;
				}
				// debug2(table_temp->last_stmt);
				gimple *def_stmt = SSA_NAME_DEF_STMT(table_temp->target);

				if (def_stmt)
					// parm_decl
					if (TREE_CODE(table_temp->target) != VAR_DECL && TREE_CODE(table_temp->target) != PARM_DECL)
					{
						// debug_gimple_stmt2(def_stmt);
						debug_tree2(table_temp->target);
						if (def_stmt)
						{
							debug_tree2(table_temp->target);
							// debug2(def_stmt);
							if (TREE_CODE(table_temp->target) == FUNCTION_DECL)
								name = get_name(table_temp->target);
							else if (TREE_CODE(table_temp->target) != ADDR_EXPR && TREE_CODE(table_temp->target))
								if (is_gimple_call(def_stmt))
								{
									// debug_gimple_stmt2(def_stmt);

									if (gimple_call_fndecl(def_stmt))
										name = get_name(gimple_call_fndecl(def_stmt));
								}
						}

						fprintf2(stderr, "\n ================== pre trace ptable ================== \n");
						if (def_stmt)
							if (TREE_CODE(table_temp->target) == FUNCTION_DECL)
							{
								const char *name;
								name = get_name(table_temp->target);

								fprintf2(stderr, "trace fucntion name:%s \n", name);
								trace_function_path(table_temp->target, RET_HEAP_OBJECT, table_temp->target, &find_retheapstmt);
								if (find_retheapstmt > 0)
									fprintf2(stderr, "some fucntion return value is heap-object and with Collection SSA_NAME alias relation\n");
							}
							else if (TREE_CODE(table_temp->target) != ADDR_EXPR && TREE_CODE(table_temp->target) != PARM_DECL)
							{
								if (is_gimple_call(def_stmt))
									if (gimple_call_fn(def_stmt))
									{
										const char *name;
										name = get_name(gimple_call_fn(def_stmt));
										fprintf2(stderr, "trace fucntion name:%s \n", name);

										trace_function_path(gimple_call_fndecl(def_stmt), RET_HEAP_OBJECT, table_temp->target, &find_retheapstmt);
										if (find_retheapstmt > 0)
											fprintf2(stderr, "some fucntion return value is heap-object and with Collection SSA_NAME alias relation\n");
									}
							}
						fprintf2(stderr, "\n ================== pre trace ptable finish================== \n");
					}
					else
					{
						debug_tree2(table_temp->target);
						// debug2(table_temp->last_stmt);
						// debug2(table_temp->last_stmt);
						fprintf2(stderr, "\n ================== this stmt hava call fucntion ================== \n");
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
						ptable_type = IS_MALLOC_FUCNTION;

						fprintf2(stderr, "is Reserved word function :%s\n", name);
					}
					else
					{
						ptable_type = IS_OTHRER_FUCNTION;
						debug_tree2(table_temp->target);
						fprintf2(stderr, "is Other function %s\n", name);
						// continue;
					}
				user_tmp = treeGimpleArray->get(table_temp->target);
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
						// fprintf2(stderr, "%s\n",get_name (getFucntionDecl));
						maxbb_array.relate_type_array = maxbb_type_array;
					}
					else
					{
						maxbb_array = *(function_maxbb_collect->get(table_temp->target));
						maxbb_type_array = maxbb_array.relate_type_array;
					}
					int lastbasicblock = -1;
					if (table_temp->swap_type == FUNCITON_THREAD)
						fprintf2(stderr, " \n Start is Pthread Job Collect  \n");
					else
						fprintf2(stderr, " \n Start check Pointer Collect  \n");
					fprintf2(stderr, "\n======================================================================\n");
					if (user_tmp->size > 0)
					{
						// set<gimple *> myset;
						// gimple stmt = u_stmt;
						// if(!myset.insert(*u_stmt))
						// continue;
						// debug_gimple_stmt2(u_stmt);
						// fprintf2(stderr, "\n=====================hello================\n");

						FOR_EACH_USE_TABLE(user_tmp, u_stmt)
						{

							// r

							if (stmtloopcheck)
								if (bb_in_loop_p(gimple_bb(u_stmt)))
								{
									fprintf2(stderr, "\n======================================================================\n");
									// fprintf2(stderr, "	no free stmt possible memory leak\n");
									fprintf2(stderr, "\033[40;31m    collect Stmt in loop \033[0m\n");
									fprintf2(stderr, "\n======================================================================\n");
								}
							if (samefucntioncheck)
							{
								if (gimple_code(table_temp->last_stmt) == GIMPLE_CALL)
								{
									if (gimple_code(u_stmt) == GIMPLE_CALL)
										if (gimple_call_fn(table_temp->last_stmt) != NULL)
											if (gimple_call_fn(u_stmt) != NULL)
											{

												// debug_tree2(gimple_call_fn(table_temp->last_stmt));
												// debug_tree2(gimple_call_fn(u_stmt));
												if (gimple_call_fn(table_temp->last_stmt) == gimple_call_fn(u_stmt))
												{
													if (u_stmt == table_temp->last_stmt)
													{

														debug_gimple_stmt2(u_stmt);
													}
												}
												else
												{
													name = get_name(gimple_call_fn(u_stmt));
													if (name != NULL)
													{
														if (!strcmp(name, get_name(gimple_call_fn(table_temp->last_stmt))))
														{
															fprintf2(stderr, "\n======================================================================\n");
															fprintf2(stderr, "\n Expand conflict \n", name);
															fprintf2(stderr, " The same function may have multiple branches :%s\n", name);
															debug_gimple_stmt2(table_temp->last_stmt);
															warning_at2(gimple_location_safe(table_temp->last_stmt), 0, "use location");

															debug_gimple_stmt2(u_stmt);
															warning_at2(gimple_location_safe(u_stmt), 0, "use location");
															check_bbinfo(table_temp->node, gimple_bb(u_stmt));

															continue;
														}
													}
												}
											}
								}
							}

							struct free_type free_type;
							if (memoryleakcheck)
							{
								if (user_tmp->ret_stmt != NULL)
								{

									if (debugmod)
									{
										fprintf2(stderr, "dot graph entry %s\n", (char *)get_name(function_tree));
										fprintf2(stderr, "dot graph target entry end\n\n");
									}
									if (gimple_code(user_tmp->ret_stmt) == GIMPLE_RETURN)
									{
										debug2(user_tmp->ret_stmt);
										warning_at2(gimple_location_safe(user_tmp->ret_stmt), 0, "use location");

										debug_gimple_stmt2(user_tmp->ret_stmt);
										now_stmt = user_tmp->ret_stmt;
										if (debugmod)
										{
											// ready add dot graph
											fprintf2(stderr, "dot graph target loc start ");
											debug_gimple_stmt2(user_tmp->ret_stmt);
											warning_at2(gimple_location_safe(user_tmp->ret_stmt), 0, "use location");
											fprintf2(stderr, "dot graph target loc en1\n\n");

											fprintf2(stderr, "dot graph target line desc form basic block %d: ", gimple_bb(user_tmp->ret_stmt)->index);
											debug_gimple_stmt2(user_tmp->ret_stmt);
											warning_at2(gimple_location_safe(user_tmp->ret_stmt), 0, "use location");
											fprintf2(stderr, "dot graph target line desend\n\n");

											fprintf2(stderr, "dot graph target loc start ");
											debug_gimple_stmt2(user_tmp->ret_stmt);
											warning_at2(gimple_location_safe(user_tmp->ret_stmt), 0, "use location");
											fprintf2(stderr, "dot graph target loc end\n\n");

											fprintf2(stderr, "dot graph target basicblock start ");
											fprintf2(stderr, "from %s basic block %d", (char *)get_name(function_tree), gimple_bb(user_tmp->ret_stmt)->index);
											fprintf2(stderr, "dot graph target basicblock end\n\n");

											if (gimple_block(user_tmp->ret_stmt))
												if (TREE_CODE(BLOCK_SUPERCONTEXT(gimple_block(user_tmp->ret_stmt))) == FUNCTION_DECL && (function_tree != BLOCK_SUPERCONTEXT(gimple_block(user_tmp->ret_stmt))))
												{

													fprintf2(stderr, "dot graph target basicblock star1 ");
													fprintf2(stderr, "from %s basic block %d", (char *)get_name(BLOCK_SUPERCONTEXT(gimple_block(user_tmp->ret_stmt))), gimple_bb(user_tmp->ret_stmt)->index);
													fprintf2(stderr, "dot graph target basicblock end\n\n");
													// lastfucntion = BLOCK_SUPERCONTEXT(gimple_block(user_tmp->ret_stmt));
													// debug2(user_tmp->aptr);
												}
											fprintf2(stderr, "dot graph stmt start ");
											debug2(user_tmp->ret_stmt);
											warning_at2(gimple_location_safe(user_tmp->ret_stmt), 0, "use location");
											fprintf2(stderr, "dot graph stmt end\n\n");
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
											fprintf2(stderr, "dot graph entry %s\n", (char *)get_name(function_tree));
											fprintf2(stderr, "dot graph target entry end\n\n");
										}

										finalstmt = u_stmt;

										debug2(u_stmt);
										warning_at2(gimple_location_safe(u_stmt), 0, "use location");
										now_stmt = u_stmt;
										// ready add dot graph
										if (debugmod)
										{

											fprintf2(stderr, "dot graph target loc start ");
											debug_gimple_stmt2(table_temp->last_stmt);
											warning_at2(gimple_location_safe(table_temp->last_stmt), 0, "use location");
											fprintf2(stderr, "dot graph target loc en1\n\n");

											fprintf2(stderr, "dot graph target line desc form basic block %d: ", gimple_bb(u_stmt)->index);
											debug2(u_stmt);
											warning_at2(gimple_location_safe(u_stmt), 0, "use location");
											fprintf2(stderr, "dot graph target line desend\n\n");

											fprintf2(stderr, "dot graph target loc start ");
											debug_gimple_stmt2(table_temp->last_stmt);
											warning_at2(gimple_location_safe(table_temp->last_stmt), 0, "use location");
											fprintf2(stderr, "dot graph target loc end\n\n");

											fprintf2(stderr, "dot graph target basicblock start ");
											fprintf2(stderr, "from %s basic block %d", (char *)get_name(function_tree), gimple_bb(u_stmt)->index);
											fprintf2(stderr, "dot graph target basicblock en1\n\n");

											struct relate_type relate_type;

											relate_type.relate_funtree = function_tree;

											if (gimple_block(u_stmt))
												if (TREE_CODE(BLOCK_SUPERCONTEXT(gimple_block(u_stmt))) == FUNCTION_DECL && (function_tree != BLOCK_SUPERCONTEXT(gimple_block(u_stmt))))
												{

													fprintf2(stderr, "dot graph target basicblock star1 ");
													fprintf2(stderr, "from %s basic block %d", (char *)get_name(BLOCK_SUPERCONTEXT(gimple_block(u_stmt))), gimple_bb(u_stmt)->index);
													fprintf2(stderr, "dot graph target basicblock end\n\n");

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
													fprintf2(stderr, "dot graph stmt start ");
													debug2(finalstmt);
													debug2(table_temp->last_stmt);
													// warning_at2(gimple_location_safe(table_temp->last_stmt), 0, "use location");
													fprintf2(stderr, "dot graph stmt end\n\n");
												}

												name = get_name(gimple_call_fn(u_stmt));
												if (name != NULL)
													if (!strcmp(name, "free") || !strcmp(name, "xfree") || !strcmp(name, "realloc"))
													{
														fprintf2(stderr, "dot graph stmt start ");
														debug2(u_stmt);
														warning_at2(gimple_location_safe(u_stmt), 0, "use location");

														fprintf2(stderr, "dot graph stmt end\n\n");

														if (freemod)
														{

															fprintf2(stderr, "dot graph target color desc");
															if (!strcmp(name, "realloc"))
															{
																fprintf2(stderr, "red");
															}
															else
																fprintf2(stderr, "green");
															fprintf2(stderr, "dot graph target color desend\n\n");
														}
														if (!strcmp(name, "realloc"))
														{
															fprintf2(stderr, "If realloc() fails, the initial memory block will not be freed() ");
														}
													}
													else
													{

														fprintf2(stderr, "dot graph stmt start ");
														debug2(u_stmt);
														warning_at2(gimple_location_safe(u_stmt), 0, "use location");

														fprintf2(stderr, "dot graph stmt end\n\n");
													}
											}
											else
											{

												if (debugmod)
												{
													fprintf2(stderr, "dot graph stmt start ");
													debug2(u_stmt);
													debug2(table_temp->last_stmt);

													fprintf2(stderr, "dot graph stmt end\n\n");
												}
												fprintf2(stderr, "dot graph stmt start ");
												debug2(u_stmt);
												warning_at2(gimple_location_safe(u_stmt), 0, "use location");
												fprintf2(stderr, "dot graph stmt end\n\n");
											}
										}
									}

									if (user_tmp->target != NULL)
									{
										if (gimple_code(u_stmt) == GIMPLE_CALL)
										{
											if (gimple_call_fndecl(u_stmt))
											{
												name = get_name(gimple_call_fndecl(u_stmt));

												if (name != NULL)
													fprintf2(stderr, "this stmt call this function :%s\n", name);
												else
													fprintf2(stderr, "this stmt call this function :null\n", name);
											}
										}
										if (gimple_code(u_stmt) == GIMPLE_PHI)
										{
											find_phistmt = 1;
											fprintf2(stderr, "this stmt have mutiple branch ---%s-----\n", name);
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

												fprintf2(stderr, "\nThis stmt may duplicate memory requests.\n");
												find_mallocstmt = IS_MALLOC_FUCNTION;
											}
											else
											{

												find_mallocstmt = IS_OTHRER_FUCNTION;
											}

										if (calleetype == FUNCITON_HEAP)
										{
											debug_tree2(function_tree);
										}
										else if (calleetype == FUNCITON_THREAD && threadmod == true)
										{
											if (gimple_code(finalstmt) == GIMPLE_ASSIGN)
											{
												// global variable
												if (!TREE_STATIC(gimple_assign_lhs(finalstmt)) == true)
												{

													fprintf2(stderr, " \n LOCAL VARIBALE  \n");
													debug_gimple_stmt2(finalstmt);
													warning_at2(gimple_location_safe(finalstmt), 0, "use location");
													fprintf2(stderr, " \n LOCAL VARIBALE  \n");
													continue;
												}
												else
												{
													fprintf2(stderr, "\n ================== warring  ================== \n");

													fprintf2(stderr, "\033[40;35m warring thread fucntion detction is on \033[0m\n");
													// sfprintf2(stderr, "function return value related stmt \n");
													fprintf2(stderr, "\033[40;35m Detect GLOBAL VARIBALE \033[0m\n");
													debug_gimple_stmt2(finalstmt);
													warning_at2(gimple_location_safe(finalstmt), 0, "use location");
													fprintf2(stderr, "\033[40;35m thread job function \033[0m\n");
													fprintf2(stderr, "\033[40;35m this stmt possible have Race Condition 。 \033[0m\n");
													fprintf2(stderr, "\n ================== warring  ================== \n");
												}
											}
										}

										if ((gimple_code(u_stmt) == GIMPLE_CALL) && (find_mallocstmt == IS_MALLOC_FUCNTION || find_mallocstmt == IS_OTHRER_FUCNTION))
										{
											// fprintf2(stderr, "\n ================== find ================== \n");
											// debug2(u_stmt);

											find_mallocstmt = IS_MALLOC_FUCNTION;

											// gimple *def_stmt = SSA_NAME_DEF_STMT(user_tmp->target);
											// fprintf2(stderr, "NEWX FUCNTIONMWEQMEQWP: \n");
											gimple *def_stmt = SSA_NAME_DEF_STMT(user_tmp->target);

											const char *name;
											name = get_name(gimple_call_fn(u_stmt));

											if (name != NULL)
												if (!strcmp(name, "free") || !strcmp(name, "xfree") || !strcmp(name, "realloc"))
												{

													free_type.stmt = u_stmt;
													free_array.push_back(free_type);
													fprintf2(stderr, "\n ================== find free stmt ================== \n");
													debug2(u_stmt);
													warning_at2(gimple_location_safe(u_stmt), 0, "use location");
													if (!strcmp(name, "realloc"))
													{
														find_freestmt++;
														fprintf2(stderr, "\033[40;32m    FIND REALLOC STMT count:%d name:%s \033[0m\n", find_freestmt, name);
														fprintf2(stderr, "\033[40;32m    this stmt possiable free memory \033[0m\n", find_freestmt, name);
													}
													else
													{
														find_freestmt++;
														fprintf2(stderr, "\033[40;32m    HAS FREE STMT count:%d name:%s \033[0m\n", find_freestmt, name);
													}
													fprintf2(stderr, "\n ================== find free stmt ================== \n");
												}
												else if (!strcmp(name, "pthread_create"))
												{

													table_temp->last_stmt = u_stmt;
													table_temp->swap_type = FUNCITON_THREAD;

													if (table_temp->swap_type == FUNCITON_THREAD)
													{
														fprintf2(stderr, "\n ================== find pthread stmt ================== \n");
														debug2(u_stmt);
														warning_at2(gimple_location_safe(u_stmt), 0, "use location");
														fprintf2(stderr, "\033[40;32m    FIND PTHREAD_CREATED STMT  \033[0m\n");
														// callerFunArray.pthread_type_num== 0?"CREATE_JOINABLE" : "CREATE_DETACHED");
														// debug_gimple_stmt2(table_temp->last_stmt);
														fprintf2(stderr, "\n ================== pre_pthread_detched ================== \n");
														if (gimple_call_num_args(table_temp->last_stmt) != 0)
														{
															if (is_gimple_assign(table_temp->last_stmt))
																// debug_tree2(TREE_OPERAND(gimple_call_arg(table_temp->last_stmt, 2), 0));
																trace_function_path(gimple_assign_lhs(table_temp->last_stmt), -1, NULL_TREE, &find_pthread_detched);
															else
																trace_function_path(TREE_OPERAND(gimple_call_arg(table_temp->last_stmt, 2), 0), -1, NULL_TREE, &find_pthread_detched);
														}
														fprintf2(stderr, "\n ================== pre_pthread_detched end ================== \n");

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
															fprintf2(stderr, "\n ================== pre_pthread_detched ================== \n");
															trace_function_path(TREE_OPERAND(gimple_call_arg(table_temp->swap_stmt, 2), 0), -1, NULL_TREE, &find_pthread_detched);
															fprintf2(stderr, "\n ================== pre_pthread_detched end ================== \n");
															findtree = gimple_call_arg(table_temp->swap_stmt, 3);
															if (findtree != NULL)
																trace_function_path(TREE_OPERAND(gimple_call_arg(table_temp->swap_stmt, 2), 0), 0, findtree, &find_freestmt);
														}

														if (find_pthread_detched == 0)
														{

															fprintf2(stderr, "\033[40;32m    FIND PTHREAD_CREATED TYPE is %s  \033[0m\n", table_temp->pthread_type == 0 ? "CREATE_JOINABLE" : "CREATE_DETACHED");
															is_pthread_detched = 0;
															if (table_temp->pthread_type == 1)
																is_pthread_detched = 1;
														}
														else
														{

															fprintf2(stderr, "\033[40;32m    FIND PTHREAD_CREATED TYPE is CREATE_DETACHED  \033[0m\n");
															is_pthread_detched = 1;
														}
														fprintf2(stderr, "\n ================== find pthread stmt ================== \n");
													}
												}
												else if (!strcmp(name, "pthread_join"))
												{
													if (table_temp->swap_type == FUNCITON_THREAD)
													{
														fprintf2(stderr, "\033[40;32m    FIND PTHREAD_JOIN \033[0m\n");
														find_pthread_join++;
													}
												}
												else
												{
													int freecount = find_freestmt;
													// fprintf2(stderr, "\n ================== trace ================== \n");
													if (name != NULL)
														if (!strcmp(name, "realloc") ||
															!strcmp(name, "malloc") ||
															!strcmp(name, "xmalloc") ||
															!strcmp(name, "calloc") ||
															!strcmp(name, "xcalloc") ||
															!strcmp(name, "strdup") ||
															!strcmp(name, "xstrdup") ||
															!strcmp(name, "fprintf"))
														{
														}
														else
														{
															find_mallocstmt = IS_MALLOC_FUCNTION;
															fprintf2(stderr, "trace fucntion name:%s \n", name);
															trace_function_path(gimple_call_fndecl(u_stmt), 0, table_temp->target, &find_freestmt);
															fprintf2(stderr, "\n ================== trace ================== \n");
															fprintf2(stderr, "trace fucntion free:%d \n", find_freestmt);
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
								// fprintf2(stderr, "\n========================================================================\n");
							}
						}
						if (samefucntioncheck)
						{
							if (debugmod)
							{
								lastbasicblock = -1;
								if (function_maxbb_collect->get(table_temp->target) != NULL)
								{
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
												// debug_tree2(it_i->relate_funtree);
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
										// debug_tree2(it_i->relate_funtree);
										max_bb = *(function_maxbasicblock_collect)->get(it_i->relate_funtree);
										// fprintf2(stderr, "\n==============================hhhhhhhhhhhhhhhh===%d==%s===============\n", it_i->now_basicblock, get_name(it_i->relate_funtree));
										// fprintf2(stderr, "\n==============================hhhhhhhhhhhhhhhh===%d==%s===============\n", max_bb, get_name(it_i->relate_funtree));
										if (max_bb == (it_i->now_basicblock))
										{
											// fprintf2(stderr, "\n==============================hhhhhhhhhhhhhhhh===%d==%s===============\n", max_bb, get_name(it_i->relate_funtree));
											// fprintf2(stderr, "\n==============================hhhhhhhhhhhhhhhh===============%d=====\n", lastbasicblock);
											gimple *def_stmt = SSA_NAME_DEF_STMT(it_i->relate_tree);
											fprintf2(stderr, "dot graph target basicblock start ");
											fprintf2(stderr, "from %s basic block %d", (char *)get_name(it_i->relate_funtree), gimple_bb(it_i->stmt)->index);
											fprintf2(stderr, "dot graph target basicblock en1\n\n");
											unsigned long x = rand();
											fprintf2(stderr, "dot graph arrow");
											fprintf2(stderr, "subgraph cluster_%lu dot graph subgraph  ", x);
											debug2(it_i->stmt);
											if (TREE_CODE(it_i->relate_tree) == SSA_NAME)
												debug2(def_stmt);

											fprintf2(stderr, "dot graph subgrapend\n\n");
										}
									}
									delete function_maxbb_collect;
								}
							}
						}
					}
					else
					{
						find_mallocstmt = PTABLE_IS_NULL;
					}

					if (relatemod)
						if (debugmod)
						{
							vector<relate_type> relate_type_array;
							function_relate_array fun_array;
							// debug_tree2(table_temp->target);
							if (function_relate_collect->get(table_temp->target) != NULL)
							{
								// fprintf2(stderr, "%s\n",get_name (getFucntionDecl));
								fun_array = *(function_relate_collect->get(table_temp->target));
								relate_type_array = fun_array.relate_type_array;

								// struct relate_type relate_type;
								// relate_type.stmt = now_relatelaststmt;
								// relate_type.relate_tree = mallocStmt_tree;
								// relate_type.relate_funtree = function_tree;
								for (int k = 0; k < relate_type_array.size(); k++)
								{

									unsigned long x = rand();
									fprintf2(stderr, "dot graph start relate for1");
									fprintf2(stderr, "ID : %lu\n", (relate_type_array)[k].now_fucntion);
									fprintf2(stderr, "from %s basic block %d", (char *)get_name((relate_type_array)[k].relate_funtree), gimple_bb((relate_type_array)[k].stmt)->index);
									fprintf2(stderr, "dot graph end relate end\n\n");
									fprintf2(stderr, "subgraph cluster_%lu dot graph subgraph  start ID : %lu stmt(relate) ， Tree ID : %lu : ", x, (relate_type_array)[k].now_stmt, (relate_type_array)[k].now_fucntion);
									debug2((relate_type_array)[k].stmt);
									warning_at2(gimple_location_safe((relate_type_array)[k].stmt), 0, "use location");

									fprintf2(stderr, "dot graph subgrapend\n\n");
								}
							}
						}

					if (Looserulesfree)
					{
						fprintf2(stderr, "\n ================== Start Looserules free Check ================== \n");
						fprintf2(stderr, "\n ================== warring ================== \n");
						fprintf2(stderr, "\033[40;35m   this module possiable false positives \033[0m\n");
						fprintf2(stderr, "\n ================== warring ================== \n");

						if (TREE_CODE(table_temp->target) == SSA_NAME || TREE_CODE(table_temp->target) == MEM_REF)
							if (table_temp->target != NULL_TREE)
							{
								// debug_tree2(table_temp->target);
								// fprintf2(stderr, "\n ================================================================= \n");
								if (function_free_collect->get(function_tree) != NULL)
								{
									function_free_array callerFunArray = *(function_free_collect->get(function_tree));
									vector<free_type> callerRetTypearray = callerFunArray.free_type_array;

									for (int k = 0; k < callerRetTypearray.size(); k++)
									{
										// fprintf2(stderr, "\n ================== free stmt ================== \n");
										// debug_gimple_stmt2((callerRetTypearray)[k].stmt);
										if (ptr_derefs_may_alias_p((callerRetTypearray)[k].free_tree, table_temp->target))
										{

											if (is_gimple_call((callerRetTypearray)[k].stmt))
												name = get_name(gimple_call_fn((callerRetTypearray)[k].stmt));

											if (!strcmp(name, "realloc"))
											{
												// fprintf2(stderr, "\033[40;35m  realloc Use after free error! \033[0m\n");
												fprintf2(stderr, "\033[40;35m <Looserules> find realloc stmt free same pointer \033[0m\n");
												fprintf2(stderr, "*/If realloc() fails, the initial memory block will not be freed() */");
											}
											else
												fprintf2(stderr, "\033[40;35m <Looserules> find free stmt free same pointer \033[0m\n");

											debug_gimple_stmt2((callerRetTypearray)[k].stmt);
											warning_at2(gimple_location_safe((callerRetTypearray)[k].stmt), 0, "use location");
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
					if (prebranchexit)
					{
						fprintf2(stderr, "\n=====================prebranchexit:start================\n");
						if (TREE_CODE(table_temp->target) == SSA_NAME || TREE_CODE(table_temp->target) == MEM_REF)
							if (table_temp->target != NULL_TREE)
							{
								user_tmp = treeGimpleArray->get(table_temp->target);
								// debug_tree2(table_temp->target);

								gimple_array start;
								start.stmt = NULL;
								used_stmt = &start;
								push_cfun(table_temp->node->get_fun());
								calculate_dominance_info(CDI_DOMINATORS);
								if (user_tmp->size > 0)
									FOR_EACH_USE_TABLE(user_tmp, u_stmt)
									{
										// debug_tree2(table_temp->target);
										// fprintf2(stderr, "\n =============================asd================= \n");
										// if (function_free_collect->get(function_tree) != NULL)
										// {
										// 	function_free_array callerFunArray = *(function_free_collect->get(function_tree));
										// 	vector<free_type> callerRetTypearray = callerFunArray.free_type_array;
										for (int k = 0; k < global_ret_type_array.size(); k++)
										{
											// fprintf2(stderr, "\n================qweqwe================================%d=\n",123);
											// debug_gimple_stmt((global_ret_type_array)[k].stmt);
											// debug_gimple_stmt((global_ret_type_array)[k].stmt);
											if ((global_ret_type_array)[k].locfucntion == function_tree)
											{
												basic_block bb;

												FOR_EACH_BB_FN(bb, table_temp->node->get_fun())
												{
													edge e;
													edge_iterator ei;
													FOR_EACH_EDGE(e, ei, bb->succs)
													{

														if (gimple_block(u_stmt))
														{

															if (e->dest->index == gimple_bb(u_stmt)->index || bb->index == gimple_bb(u_stmt)->index)
															{

																if (function_free_collect->get(function_tree) != NULL)
																{
																	function_free_array callerFunArray = *(function_free_collect->get(function_tree));
																	vector<free_type> callerRetTypearray = callerFunArray.free_type_array;

																	if (fDFS->get(table_temp->node)->is_succ(e->dest, gimple_bb((global_ret_type_array)[k].stmt)))
																	{
																		for (int k2 = 0; k2 < callerRetTypearray.size(); k2++)
																		{
																			

																				if (dominated_by_p(CDI_DOMINATORS, gimple_bb((callerRetTypearray)[k2].stmt), gimple_bb(u_stmt)))
																					if (dominated_by_p(CDI_DOMINATORS, gimple_bb((global_ret_type_array)[k].stmt), gimple_bb(u_stmt)) || fDFS->get(table_temp->node)->is_succ(gimple_bb((callerRetTypearray)[k2].stmt), gimple_bb((global_ret_type_array)[k].stmt)))
																					{
																						if (!dominated_by_p(CDI_DOMINATORS, gimple_bb((global_ret_type_array)[k].stmt), gimple_bb((callerRetTypearray)[k2].stmt)))
																						{
																							fprintf2(stderr, "\n======================================================================\n");
																							fprintf2(stderr, "\033[40;31m    branch possiable have return or exit  \033[0m\n");
																							name = get_name(table_temp->node->get_fun()->decl);
																							if (name)
																								fprintf2(stderr, "In fucntion name:%s \n", name);

																							debug_gimple_stmt2(u_stmt);
																							// fprintf2(stderr, "bb index := %d\n", gimple_bb(u_stmt)->index);
																							fprintf2(stderr, "gimple stmt in succ := %d \n", gimple_bb(u_stmt)->index);
																							debug_gimple_stmt2((global_ret_type_array)[k].stmt);
																							warning_at2(gimple_location_safe((global_ret_type_array)[k].stmt), 0, "use location");
																							fprintf2(stderr, "branch in succ := %d have return or exit\n", gimple_bb((global_ret_type_array)[k].stmt)->index);
																							debug_gimple_stmt2((callerRetTypearray)[k2].stmt);
																							warning_at2(gimple_location_safe((callerRetTypearray)[k2].stmt), 0, "use location");
																							fprintf2(stderr, "The path has a release memory in succ := %d \n", gimple_bb((callerRetTypearray)[k2].stmt)->index);
																							fprintf2(stderr, "\n======================================================================\n");
																							if (k + 1 < global_ret_type_array.size())
																							if ((global_ret_type_array)[k+1].locfucntion == function_tree)
																								k++;
																						}
																					}
																		}
																	}
																}
																else if (fDFS->get(table_temp->node)->is_succ(e->dest, gimple_bb((global_ret_type_array)[k].stmt)))
																{
																	// fprintf2(stderr, "\n================qweqwe================================%d=\n", e->dest->index);
																	fprintf2(stderr, "\n======================================================================\n");
																	fprintf2(stderr, "\033[40;31m    branch possiable have return or exit  \033[0m\n");
																	debug_gimple_stmt2(u_stmt);
																	fprintf2(stderr, "bb index := %d\n", gimple_bb(u_stmt)->index);
																	debug_gimple_stmt2((global_ret_type_array)[k].stmt);
																	warning_at2(gimple_location_safe((global_ret_type_array)[k].stmt), 0, "use location");
																	fprintf2(stderr, "gimple stmt in succ := %d \n", gimple_bb(u_stmt)->index);
																	name = get_name(table_temp->node->get_fun()->decl);
																	if (name)
																		fprintf2(stderr, "In fucntion name:%s \n", name);
																	fprintf2(stderr, "branch in succ := %d have return or exit\n", gimple_bb((global_ret_type_array)[k].stmt)->index);

																	fprintf2(stderr, "\n======================================================================\n");
																}
															}
														}
													}
												}
											}
										}
									}
								pop_cfun();
								free_dominance_info(CDI_DOMINATORS);
								fprintf2(stderr, "\n=====================prebranchexit:end================\n");
							}
					}

					if (useafterfree)
					{
						user_tmp = treeGimpleArray->get(table_temp->target);
						// debug_tree2(table_temp->target);

						gimple_array start;
						start.stmt = NULL;
						used_stmt = &start;

						fprintf2(stderr, "\n ================== Start Use after free Check ================== \n");
						if (user_tmp->size > 0)
							FOR_EACH_USE_TABLE(user_tmp, u_stmt)
							{
								// debug_gimple_stmt2(u_stmt);
								// debug_tree2(user_tmp->aptr);
								// fprintf2(stderr, "\n ================== test ============== \n", free_array.size());
								for (int i = 0; i < free_array.size(); i++)
								{
									// fprintf2(stderr, "\n ================== warring ==%d================ \n",free_array.size());
									if (gimple_call_fndecl(free_array.at(i).stmt))
										if (u_stmt != free_array.at(i).stmt)
										{
											if (is_gimple_call(u_stmt))
												if (gimple_call_fndecl(free_array.at(i).stmt) == gimple_call_fndecl(u_stmt))
													continue;
											if ((gimple_call_num_args(free_array.at(i).stmt) != 0))
											{
												tree callarg = gimple_call_arg(free_array.at(i).stmt, 0);
												if (TREE_CODE(callarg) == SSA_NAME)
												{
													gimple *def_stmt = SSA_NAME_DEF_STMT(callarg);

													if (def_stmt)
													{
														if (gimple_code(def_stmt) == GIMPLE_PHI)
														{
															if (gimple_phi_num_args(def_stmt))
																for (int k = 0; k < gimple_phi_num_args(def_stmt); k++)
																{
																	tree arg = gimple_phi_arg_def(def_stmt, k);
																	// debug_tree2(arg);
																	if (arg == user_tmp->target)
																		continue;
																}
															if (gimple_phi_result(def_stmt) == user_tmp->target)

																continue;
														}
													}
												}
												if (is_gimple_call(u_stmt))
													if (gimple_call_fndecl(free_array.at(i).stmt) == gimple_call_fndecl(u_stmt))
														if (callarg == user_tmp->target)
															continue;
												if (gimple_code(u_stmt) == GIMPLE_PHI)
													if (gimple_phi_result(u_stmt) == callarg)
														continue;
											}
											if (Location_b2(free_array.at(i).stmt, u_stmt, function_tree))
											{

												fprintf2(stderr, "\n============================================================\n");
												if (free_array.at(i).Looserulesfree)
													fprintf2(stderr, "\033[40;35m <Looserules> find free stmt free same pointer \033[0m\n");
												debug_gimple_stmt2(free_array.at(i).stmt);
												warning_at2(gimple_location_safe(free_array.at(i).stmt), 0, "free in this location");
												check_bbinfo(table_temp->node, gimple_bb(free_array.at(i).stmt));
												// if (user_tmp->aptr)
												// {
												// 	debug_tree2(user_tmp->aptr);
												// 	name = get_tree_code_name(TREE_CODE(user_tmp->aptr));
												// 	if (name)
												// 		fprintf2(stderr, "\033[40;35m target gimple type: %s \033[0m\n", name);
												// }
												fprintf2(stderr, "\n===== possiable asias =====\n");
												debug_gimple_stmt2(u_stmt);
												warning_at2(gimple_location_safe(u_stmt), 0, "use location");
												check_bbinfo(table_temp->node, gimple_bb(u_stmt));
												fprintf2(stderr, "\n ================== warring ================== \n");
												name = get_name(gimple_call_fn(free_array.at(i).stmt));
												if (!strcmp(name, "realloc"))
												{
													fprintf2(stderr, "\033[40;35m  realloc Use after free error! \033[0m\n");
												}
												else
													fprintf2(stderr, "\033[40;35m    Use after free error! \033[0m\n");
												fprintf2(stderr, "\n ================== warring ================== \n");
											}
										}
								}
							}
						fprintf2(stderr, "\ndot graph END\n");
					}
					fprintf2(stderr, " \n Finish check Pointer Collect  \n");
				}
				else
				{
					find_mallocstmt = PTABLE_IS_NULL;
				}

				if (table_temp->swap_type == FUNCITON_THREAD)
				{
					if (is_pthread_detched == 0)
					{
						if (find_pthread_join == 0)
						{
							fprintf2(stderr, "\n======================================================================\n");
							// fprintf2(stderr, "	no free stmt possible memory leak\n");
							fprintf2(stderr, "\033[40;31m    pthread is JOINABLE but no pthread_join stmt \033[0m\n");
							fprintf2(stderr, "\n======================================================================\n");
						}
						else
						{
							fprintf2(stderr, "\n======================================================================\n");
							// fprintf2(stderr, "	no free stmt possible memory leak\n");
							// fprintf2(stderr, "\033[40;31m    pthread is JOINABLE and no memory leak \033[0m\n");
							fprintf2(stderr, "\033[40;32m    pthread is JOINABLE and no memory leak   \033[0m\n");
							fprintf2(stderr, "\n======================================================================\n");
						}
					}
					else
					{
						fprintf2(stderr, "\n======================================================================\n");
						// fprintf2(stderr, "	no free stmt possible memory leak\n");
						fprintf2(stderr, "\033[40;32m    pthread is DETACHED and no memory leak  \033[0m\n");
						// fprintf2(stderr, "\033[40;31m    pthread is DETACHED and no memory leak  \033[0m\n");
						fprintf2(stderr, "\n======================================================================\n");
					}
				}
				if (ptable_type == IS_MALLOC_FUCNTION && find_mallocstmt == IS_OTHRER_FUCNTION)
					find_mallocstmt = IS_MALLOC_FUCNTION;
				if (Looserulesfree)
				{
					fprintf2(stderr, "\n======================================================================\n");
					// fprintf2(stderr, "	possible double free\n");

					fprintf2(stderr, "\033[40;35m  	Looserules free count:%d \033[0m\n", find_Looserulesfreestmt);
					fprintf2(stderr, "\033[40;31m  	free count:%d \033[0m\n", find_freestmt);
					if (find_Looserulesfreestmt >= 2)
					{
						fprintf2(stderr, "\033[40;31m  	possible double free [Looserules]:%d \033[0m\n", find_Looserulesfreestmt);
					}
					if (find_freestmt >= 2)
					{
						fprintf2(stderr, "\033[40;31m  	possible double free [rigorous]:%d \033[0m\n", find_freestmt);
					}
					fprintf2(stderr, "\n======================================================================\n");
				}
				if ((find_mallocstmt == IS_MALLOC_FUCNTION))
				{
					struct defmalloc_type defmalloc_type;
					defmalloc_type.stmt = table_temp->last_stmt;
					defmalloc_type.malloc_tree = table_temp->target;
					for (int i = 0; i < free_array.size(); i++)
					{
						defmalloc_type.free_array.push_back(free_array[i]);
					}
					// defmalloc_type.free_array = free_array;
					defmalloc_array.push_back(defmalloc_type);

					if (find_freestmt == 0)
					{

						fprintf2(stderr, "\n======================================================================\n");
						// fprintf2(stderr, "	no free stmt possible memory leak\n");
						fprintf2(stderr, "\033[40;31m    no free stmt possible memory leak \033[0m\n");
						fprintf2(stderr, "\n======================================================================\n");
					}
					else if (find_freestmt == 1)
					{

						fprintf2(stderr, "\n======================================================================\n");

						if (find_phistmt == 1)
							fprintf2(stderr, "\033[40;31m   need check branch because multiple direction varible\033[0m\n");
						else
							fprintf2(stderr, "\033[40;32m   No memory leak, need more check!  \033[0m\n");
						// 	fprintf2(stderr, "\033[40;32m    NO memory leak \033[0m\n");
						fprintf2(stderr, "\n======================================================================\n\n");
					}
					else if (find_freestmt >= 2)
					{
						fprintf2(stderr, "\n======================================================================\n");
						// fprintf2(stderr, "	possible double free\n");

						fprintf2(stderr, "\033[40;31m  	possible double free [rigorous]:%d \033[0m\n", find_freestmt);
						fprintf2(stderr, "\n======================================================================\n");
					}
				}
				else if (find_mallocstmt == IS_HEAP_FUCNTION)
				{
					fprintf2(stderr, "\n======================================================================\n");
					// fprintf2(stderr, "	this fucntion return possible has heap-object\n");
					fprintf2(stderr, "\033[40;31m 	this fucntion return possible is heap-object \033[0m\n");
					fprintf2(stderr, "\n======================================================================\n");
				}
				else if (find_mallocstmt == PTABLE_IS_NULL)
				{
					fprintf2(stderr, "\n======================================================================\n");
					// fprintf2(stderr, "	this fucntion return possible has heap-object\n");
					fprintf2(stderr, "\033[40;31m 	Can't find this fucntion in Pointer table \033[0m\n");
					fprintf2(stderr, "\033[40;31m 	Possiable no other stmt relate with this stmt \033[0m\n");
					// debug_gimple_stmt2(table_temp->last_stmt);
					if (empty_body_p(table_temp->last_stmt))
						warning_at2(gimple_location_safe(table_temp->last_stmt), 0, "use location");

					fprintf2(stderr, "\n======================================================================\n");
				}
				else
				{

					fprintf2(stderr, "\n======================================================================\n");
					// fprintf2(stderr, "	this stmt need double check\n");

					fprintf2(stderr, "\033[40;31m 	this stmt need double check \033[0m\n");
					if (find_retheapstmt > 0)
					{
						if (find_freestmt > 2)
						{
							fprintf2(stderr, "\n======================================================================\n");
							// fprintf2(stderr, "	possible double free\n");
							fprintf2(stderr, "\033[40;31m    this function with other function alias that other function return value is heap-object \033[0m\n");
							fprintf2(stderr, "\033[40;31m  	possible double free :%d \033[0m\n", find_freestmt);
							fprintf2(stderr, "\n======================================================================\n");
						}
						else if (find_freestmt == 1)
						{

							fprintf2(stderr, "\n======================================================================\n");
							fprintf2(stderr, "\033[40;31m    this function with other function alias that other function return value is heap-object \033[0m\n");
							if (find_phistmt == 1)
								fprintf2(stderr, "\033[40;31m   need check branch because multiple direction varible\033[0m\n");
							else
								fprintf2(stderr, "\033[40;32m   Maybe you don't have memory leak.... need more checks  \033[0m\n");
							// 	fprintf2(stderr, "\033[40;32m    NO memory leak \033[0m\n");
							fprintf2(stderr, "\n======================================================================\n\n");
						}
						else if (find_freestmt == 0)
						{
							fprintf2(stderr, "\n======================================================================\n");
							fprintf2(stderr, "\033[40;31m    this function with other function alias that other function return value is heap-object \033[0m\n");
							fprintf2(stderr, "\033[40;31m    no free stmt possible memory leak \033[0m\n");
							fprintf2(stderr, "\n======================================================================\n");
						}
					}
				}

				fprintf2(stderr, "\n======================================================================\n");
			}
		// defmemoryleak check experiment
		// if (defmemoryleak)
		// {
		// 	for (int i = 0; i < defmalloc_array.size(); i++)
		// 	{
		// 		for (int j = 0; j < defmalloc_array.size(); j++)
		// 		{
		// 			// if (defmalloc_array.at(i).malloc_tree != defmalloc_array.at(j).malloc_tree)
		// 			// {	fprintf2(stderr, "\033[40;31m  start check tree \033[0m\n");
		// 			// 	if (ptr_derefs_may_alias_p(defmalloc_array.at(i).malloc_tree, defmalloc_array.at(j).malloc_tree))
		// 			// 	{
		// 			// 		debug2(defmalloc_array.at(i).malloc_tree);
		// 			// 		debug2(defmalloc_array.at(j).malloc_tree);
		// 			// 		fprintf2(stderr, "\033[40;31m  same alias gimple tree \033[0m\n");
		// 			// 	}
		// 			// }
		// 			if (i != j)
		// 			{
		// 				if (SSA_NAME_VAR(defmalloc_array.at(i).malloc_tree) == SSA_NAME_VAR(defmalloc_array.at(j).malloc_tree))
		// 					if (Location_b3(defmalloc_array.at(j).stmt, defmalloc_array.at(i).stmt, function_tree))
		// 					// if (Location_b2(defmalloc_array.at(i).stmt, defmalloc_array.at(j).stmt, function_tree))
		// 					{
		// 						// debug2(defmalloc_array.at(i).free_array.at(k).stmt);
		// 						if (defmalloc_array.at(i).free_array.size())
		// 						{
		// 							int finddefsafe = 0;
		// 							// fprintf2(stderr, "\033[40;31mfuck%d \033[0m\n",	Location_b3(defmalloc_array.at(i).free_array.at(0).stmt, defmalloc_array.at(i).free_array.at(1).stmt, function_tree));
		// 							// 		fprintf2(stderr, "\033[40;31mfuck%d \033[0m\n",	Location_b3(defmalloc_array.at(i).free_array.at(1).stmt, defmalloc_array.at(i).free_array.at(0).stmt, function_tree));

		// 							for (int k = 0; k < defmalloc_array.at(i).free_array.size(); k++)
		// 							{

		// 								// debug2(defmalloc_array.at(i).free_array.at(k).stmt);
		// 								// debug2(defmalloc_array.at(i).free_array.at(k).stmt);
		// 								// if (gimple_bb(defmalloc_array.at(j).stmt)->index == gimple_bb(defmalloc_array.at(i).free_array.at(k).stmt)->index)
		// 								if (!Location_b3(defmalloc_array.at(j).stmt, defmalloc_array.at(i).free_array.at(k).stmt, function_tree))
		// 								{
		// 									// sec malloc after first free stmt
		// 									finddefsafe = 1;
		// 								}
		// 								else
		// 								{
		// 									// sec malloc before first free stmt
		// 									finddefsafe = -1;
		// 									// debug2(defmalloc_array.at(i).free_array.at(k).stmt);
		// 								}
		// 							}

		// 							if (finddefsafe >= 0)
		// 							{
		// 								// use sec malloc stmt ,first malloc stmt no free
		// 								fprintf2(stderr, "\033[40;31m   malloc def-leak warring \033[0m\n");
		// 								debug2(defmalloc_array.at(i).malloc_tree);
		// 								warning_at2(gimple_location_safe(defmalloc_array.at(i).stmt), 0, "use location");
		// 								fprintf2(stderr, "\033[40;31m   memory leak and alloc new memory \033[0m\n");
		// 								debug2(defmalloc_array.at(j).malloc_tree);
		// 								warning_at2(gimple_location_safe(defmalloc_array.at(j).stmt), 0, "use location");
		// 							}
		// 						}
		// 						else
		// 						{
		// 							// first malloc no free stmt
		// 							fprintf2(stderr, "\033[40;31m   malloc def-leak warring \033[0m\n");
		// 							debug2(defmalloc_array.at(i).malloc_tree);
		// 							warning_at2(gimple_location_safe(defmalloc_array.at(i).stmt), 0, "use location");
		// 							fprintf2(stderr, "\033[40;31m   memory leak and alloc new memory \033[0m\n");
		// 							debug2(defmalloc_array.at(j).malloc_tree);
		// 							warning_at2(gimple_location_safe(defmalloc_array.at(j).stmt), 0, "use location");
		// 						}
		// 					}
		// 			}
		// 		}
		// 	}
		// }
	}
}